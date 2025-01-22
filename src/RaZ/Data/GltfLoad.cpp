#include "RaZ/Data/GltfFormat.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "fastgltf/core.hpp"
#include "fastgltf/math.hpp"
#include "fastgltf/tools.hpp"

#include "tracy/Tracy.hpp"

namespace Raz::GltfFormat {

namespace {

Transform loadTransform(const fastgltf::Node& node) {
  const auto* transform = std::get_if<fastgltf::TRS>(&node.transform);

  if (transform == nullptr) // Shouldn't happen with the option that splits a matrix in TRS components
    throw std::invalid_argument("[GltfLoad] Unexpected node transform type.");

  return Transform(Vec3f(transform->translation.x(), transform->translation.y(), transform->translation.z()),
                   Quaternionf(transform->rotation.w(), transform->rotation.x(), transform->rotation.y(), transform->rotation.z()),
                   Vec3f(transform->scale.x(), transform->scale.y(), transform->scale.z()));

}

void computeNodeTransform(const fastgltf::Node& currentNode,
                          const std::optional<Transform>& parentTransform,
                          const std::vector<fastgltf::Node>& nodes,
                          std::vector<std::optional<Transform>>& transforms) {
  ZoneScopedN("[GltfLoad]::computeNodeTransform");

  if (!currentNode.meshIndex.has_value())
    return;

  const std::size_t currentMeshIndex = *currentNode.meshIndex;

  if (currentMeshIndex >= transforms.size()) {
    Logger::error("[GltfLoad] Unexpected node mesh index.");
    return;
  }

  std::optional<Transform>& currentTransform = transforms[currentMeshIndex];

  if (!currentTransform.has_value())
    currentTransform = loadTransform(currentNode);

  if (parentTransform.has_value()) {
    currentTransform->setPosition(parentTransform->getPosition() + parentTransform->getRotation() * (currentTransform->getPosition() * parentTransform->getScale()));
    currentTransform->setRotation((parentTransform->getRotation() * currentTransform->getRotation()).normalize());
    currentTransform->scale(parentTransform->getScale());
  }

  for (const std::size_t childIndex : currentNode.children)
    computeNodeTransform(nodes[childIndex], currentTransform, nodes, transforms);
}

std::vector<std::optional<Transform>> loadTransforms(const std::vector<fastgltf::Node>& nodes, std::size_t meshCount) {
  ZoneScopedN("[GltfLoad]::loadTransforms");

  std::vector<std::optional<Transform>> transforms;
  transforms.resize(meshCount);

  for (const fastgltf::Node& node : nodes)
    computeNodeTransform(node, std::nullopt, nodes, transforms);

  return transforms;
}

template <typename T, typename FuncT>
void loadVertexData(const fastgltf::Asset& asset,
                    const fastgltf::Primitive& primitive,
                    std::string_view attribName,
                    FuncT&& callback) {
  static_assert(std::is_invocable_v<FuncT, T, std::size_t>);

  ZoneScopedN("[GltfLoad]::loadVertexData");

  const auto attribIter = primitive.findAttribute(attribName);

  if (attribIter == primitive.attributes.end())
    return;

  fastgltf::iterateAccessorWithIndex<T>(asset, asset.accessors[attribIter->accessorIndex], std::forward<FuncT>(callback));
}

void loadVertices(const fastgltf::Asset& asset,
                  const fastgltf::Primitive& primitive,
                  const std::optional<Transform>& transform,
                  Submesh& submesh) {
  ZoneScopedN("[GltfLoad]::loadVertices");

  Logger::debug("[GltfLoad] Loading vertices...");

  const auto positionIter = primitive.findAttribute("POSITION");

  if (positionIter == primitive.attributes.end())
    throw std::invalid_argument("Error: Required 'POSITION' attribute not found in the glTF file.");

  const fastgltf::Accessor& positionAccessor = asset.accessors[positionIter->accessorIndex];

  if (!positionAccessor.bufferViewIndex.has_value())
    return;

  std::vector<Vertex>& vertices = submesh.getVertices();
  vertices.resize(positionAccessor.count);

  fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, positionAccessor, [&vertices] (fastgltf::math::fvec3 position, std::size_t vertexIndex) noexcept {
    vertices[vertexIndex].position = Vec3f(position.x(), position.y(), position.z());
  });

  loadVertexData<fastgltf::math::fvec2>(asset, primitive, "TEXCOORD_0", [&vertices] (fastgltf::math::fvec2 uv, std::size_t vertexIndex) noexcept {
    // The texcoords can be outside the [0; 1] range; they're normalized according to the REPEAT mode. This may be subject to change
    // See: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_wrapping
    vertices[vertexIndex].texcoords = Vec2f(uv.x(), uv.y());

    for (float& elt : vertices[vertexIndex].texcoords.getData()) {
      if (elt < -1.f || elt > 1.f) elt = std::fmod(elt, 1.f);
      if (elt < 0.f) elt += 1.f;
    }
  });

  loadVertexData<fastgltf::math::fvec3>(asset, primitive, "NORMAL", [&vertices] (fastgltf::math::fvec3 normal, std::size_t vertexIndex) noexcept {
    vertices[vertexIndex].normal = Vec3f(normal.x(), normal.y(), normal.z());
  });

  const bool hasTangents = (primitive.findAttribute("TANGENT") != primitive.attributes.end());

  if (hasTangents) {
    loadVertexData<fastgltf::math::fvec4>(asset, primitive, "TANGENT", [&vertices] (fastgltf::math::fvec4 tangent, std::size_t vertexIndex) noexcept {
      // The tangent's input W component is either 1 or -1 and represents the handedness
      // See: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
      vertices[vertexIndex].tangent = Vec3f(tangent.x(), tangent.y(), tangent.z()) * tangent.w();
    });
  } else {
    submesh.computeTangents();
  }

  if (transform.has_value()) {
    for (Vertex& vert : submesh.getVertices()) {
      vert.position = transform->getPosition() + transform->getRotation() * (vert.position * transform->getScale());
      vert.normal   = (transform->getRotation() * vert.normal).normalize();
      vert.tangent  = (transform->getRotation() * vert.tangent).normalize();
    }
  }

  Logger::debug("[GltfLoad] Loaded vertices");
}

void loadIndices(const fastgltf::Asset& asset,
                 const fastgltf::Accessor& indicesAccessor,
                 std::vector<unsigned int>& indices) {
  ZoneScopedN("[GltfLoad]::loadIndices");

  Logger::debug("[GltfLoad] Loading indices...");

  if (!indicesAccessor.bufferViewIndex.has_value())
    throw std::invalid_argument("Error: Missing glTF buffer to load indices from.");

  indices.resize(indicesAccessor.count);
  fastgltf::copyFromAccessor<unsigned int>(asset, indicesAccessor, indices.data());

  Logger::debug("[GltfLoad] Loaded indices");
}

std::pair<Mesh, MeshRenderer> loadMeshes(const fastgltf::Asset& asset,
                                         const std::vector<std::optional<Transform>>& transforms) {
  ZoneScopedN("[GltfLoad]::loadMeshes");

  const std::vector<fastgltf::Mesh>& meshes = asset.meshes;

  Logger::debug("[GltfLoad] Loading " + std::to_string(meshes.size()) + " mesh(es)...");

  Mesh loadedMesh;
  MeshRenderer loadedMeshRenderer;

  for (std::size_t meshIndex = 0; meshIndex < meshes.size(); ++meshIndex) {
    for (const fastgltf::Primitive& primitive : meshes[meshIndex].primitives) {
      if (!primitive.indicesAccessor.has_value())
        throw std::invalid_argument("Error: The glTF file requires having indexed geometry.");

      Submesh& submesh = loadedMesh.addSubmesh();
      SubmeshRenderer& submeshRenderer = loadedMeshRenderer.addSubmeshRenderer();

      // Indices must be loaded first as they are needed to compute the tangents if necessary
      loadIndices(asset, asset.accessors[*primitive.indicesAccessor], submesh.getTriangleIndices());
      loadVertices(asset, primitive, transforms[meshIndex], submesh);

      submeshRenderer.load(submesh, (primitive.type == fastgltf::PrimitiveType::Triangles ? RenderMode::TRIANGLE : RenderMode::POINT));
      submeshRenderer.setMaterialIndex(primitive.materialIndex.value_or(0));
    }
  }

  Logger::debug("[GltfLoad] Loaded mesh(es)");

  return { std::move(loadedMesh), std::move(loadedMeshRenderer) };
}

std::vector<std::optional<Image>> loadImages(const std::vector<fastgltf::Image>& images,
                                             const std::vector<fastgltf::Buffer>& buffers,
                                             const std::vector<fastgltf::BufferView>& bufferViews,
                                             const FilePath& rootFilePath) {
  ZoneScopedN("[GltfLoad]::loadImages");

  Logger::debug("[GltfLoad] Loading " + std::to_string(images.size()) + " image(s)...");

  std::vector<std::optional<Image>> loadedImages;
  loadedImages.reserve(images.size());

  static auto loadFailure = [&loadedImages] (const auto&) {
    Logger::error("[GltfLoad] Cannot find a suitable way of loading an image.");
    loadedImages.emplace_back(std::nullopt);
  };

  for (const fastgltf::Image& img : images) {
    std::visit(fastgltf::visitor {
      [&loadedImages, &rootFilePath] (const fastgltf::sources::URI& imgPath) {
        loadedImages.emplace_back(ImageFormat::load(rootFilePath + imgPath.uri.path()));
      },
      [&loadedImages] (const fastgltf::sources::Vector& imgData) {
        const auto* imgBytes = reinterpret_cast<const unsigned char*>(imgData.bytes.data());
        loadedImages.emplace_back(ImageFormat::loadFromData(imgBytes, imgData.bytes.size()));
      },
      [&bufferViews, &buffers, &loadedImages] (const fastgltf::sources::BufferView& bufferViewSource) {
        const fastgltf::BufferView& imgView = bufferViews[bufferViewSource.bufferViewIndex];
        const fastgltf::Buffer& imgBuffer   = buffers[imgView.bufferIndex];

        std::visit(fastgltf::visitor {
          [&loadedImages, &imgView] (const fastgltf::sources::Array& imgData) {
            const auto* imgBytes = reinterpret_cast<const unsigned char*>(imgData.bytes.data());
            loadedImages.emplace_back(ImageFormat::loadFromData(imgBytes + imgView.byteOffset, imgView.byteLength));
          },
          loadFailure
        }, imgBuffer.data);
      },
      loadFailure
    }, img.data);
  }

  Logger::debug("[GltfLoad] Loaded image(s)");

  return loadedImages;
}

Image extractAmbientOcclusionImage(const Image& occlusionImg) {
  Image ambientImg(occlusionImg.getWidth(), occlusionImg.getHeight(), ImageColorspace::GRAY, occlusionImg.getDataType());

  for (std::size_t i = 0; i < occlusionImg.getWidth() * occlusionImg.getHeight(); ++i) {
    const std::size_t finalIndex = i * occlusionImg.getChannelCount();

    // The occlusion is located in the red (1st) channel
    // See: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_material_occlusiontexture
    if (occlusionImg.getDataType() == ImageDataType::BYTE)
      static_cast<uint8_t*>(ambientImg.getDataPtr())[i] = static_cast<const uint8_t*>(occlusionImg.getDataPtr())[finalIndex];
    else
      static_cast<float*>(ambientImg.getDataPtr())[i] = static_cast<const float*>(occlusionImg.getDataPtr())[finalIndex];
  }

  return ambientImg;
}

std::pair<Image, Image> extractMetalnessRoughnessImages(const Image& metalRoughImg) {
  Image metalnessImg(metalRoughImg.getWidth(), metalRoughImg.getHeight(), ImageColorspace::GRAY, metalRoughImg.getDataType());
  Image roughnessImg(metalRoughImg.getWidth(), metalRoughImg.getHeight(), ImageColorspace::GRAY, metalRoughImg.getDataType());

  for (std::size_t i = 0; i < metalRoughImg.getWidth() * metalRoughImg.getHeight(); ++i) {
    const std::size_t finalIndex = i * metalRoughImg.getChannelCount();

    // The metalness & roughness are located respectively in the blue (3rd) & green (2nd) channels
    // See: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_material_pbrmetallicroughness_metallicroughnesstexture
    if (metalRoughImg.getDataType() == ImageDataType::BYTE) {
      static_cast<uint8_t*>(metalnessImg.getDataPtr())[i] = static_cast<const uint8_t*>(metalRoughImg.getDataPtr())[finalIndex + 2];
      static_cast<uint8_t*>(roughnessImg.getDataPtr())[i] = static_cast<const uint8_t*>(metalRoughImg.getDataPtr())[finalIndex + 1];
    } else {
      static_cast<float*>(metalnessImg.getDataPtr())[i] = static_cast<const float*>(metalRoughImg.getDataPtr())[finalIndex + 2];
      static_cast<float*>(roughnessImg.getDataPtr())[i] = static_cast<const float*>(metalRoughImg.getDataPtr())[finalIndex + 1];
    }
  }

  return { std::move(metalnessImg), std::move(roughnessImg) };
}

template <template <typename> typename OptionalT, typename TextureInfoT, typename FuncT>
void loadTexture(const OptionalT<TextureInfoT>& textureInfo,
                 const std::vector<fastgltf::Texture>& textures,
                 const std::vector<std::optional<Image>>& images,
                 const FuncT& callback) {
  static_assert(std::is_base_of_v<fastgltf::TextureInfo, TextureInfoT>);

  ZoneScopedN("[GltfLoad]::loadTexture");

  if (!textureInfo.has_value())
    return;

  const fastgltf::Optional<std::size_t>& imgIndex = textures[textureInfo->textureIndex].imageIndex;

  if (!imgIndex.has_value() || !images[*imgIndex].has_value())
    return;

  callback(*images[*imgIndex]);
}

void loadMaterials(const std::vector<fastgltf::Material>& materials,
                   const std::vector<fastgltf::Texture>& textures,
                   const std::vector<std::optional<Image>>& images,
                   MeshRenderer& meshRenderer) {
  ZoneScopedN("[GltfLoad]::loadMaterials");

  Logger::debug("[GltfLoad] Loading " + std::to_string(materials.size()) + " material(s)...");

  meshRenderer.getMaterials().clear();

  for (const fastgltf::Material& mat : materials) {
    Material& loadedMat = meshRenderer.addMaterial();
    RenderShaderProgram& matProgram = loadedMat.getProgram();

    matProgram.setAttribute(Vec3f(mat.pbrData.baseColorFactor[0],
                                  mat.pbrData.baseColorFactor[1],
                                  mat.pbrData.baseColorFactor[2]), MaterialAttribute::BaseColor);
    matProgram.setAttribute(Vec3f(mat.emissiveFactor[0],
                                  mat.emissiveFactor[1],
                                  mat.emissiveFactor[2]) * mat.emissiveStrength, MaterialAttribute::Emissive);
    matProgram.setAttribute(mat.pbrData.metallicFactor, MaterialAttribute::Metallic);
    matProgram.setAttribute(mat.pbrData.roughnessFactor, MaterialAttribute::Roughness);

    loadTexture(mat.pbrData.baseColorTexture, textures, images, [&matProgram] (const Image& img) {
      matProgram.setTexture(Texture2D::create(img, true, true), MaterialTexture::BaseColor);
    });

    loadTexture(mat.emissiveTexture, textures, images, [&matProgram] (const Image& img) {
      matProgram.setTexture(Texture2D::create(img, true, true), MaterialTexture::Emissive);
    });

    loadTexture(mat.occlusionTexture, textures, images, [&matProgram] (const Image& img) {
      const Image ambientOcclusionImg = extractAmbientOcclusionImage(img);
      matProgram.setTexture(Texture2D::create(ambientOcclusionImg), MaterialTexture::Ambient);
    });

    loadTexture(mat.normalTexture, textures, images, [&matProgram] (const Image& img) {
      matProgram.setTexture(Texture2D::create(img), MaterialTexture::Normal);
    });

    loadTexture(mat.pbrData.metallicRoughnessTexture, textures, images, [&matProgram] (const Image& img) {
      const auto [metalnessImg, roughnessImg] = extractMetalnessRoughnessImages(img);
      matProgram.setTexture(Texture2D::create(metalnessImg), MaterialTexture::Metallic);
      matProgram.setTexture(Texture2D::create(roughnessImg), MaterialTexture::Roughness);
    });

    loadedMat.loadType(MaterialType::COOK_TORRANCE);
  }

  Logger::debug("[GltfLoad] Loaded material(s)");
}

} // namespace

std::pair<Mesh, MeshRenderer> load(const FilePath& filePath) {
  ZoneScopedN("GltfFormat::load");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  Logger::debug("[GltfLoad] Loading glTF file ('" + filePath + "')...");

  if (!FileUtils::isReadable(filePath))
    throw std::invalid_argument("Error: The glTF file '" + filePath + "' either does not exist or cannot be opened.");

  fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath(filePath.getPath());

  if (data.error() != fastgltf::Error::None)
    throw std::invalid_argument("Error: Could not load the glTF file.");

  const FilePath parentPath = filePath.recoverPathToFile();

  fastgltf::Parser parser;

  fastgltf::Expected<fastgltf::Asset> asset = parser.loadGltf(data.get(),
                                                              parentPath.getPath(),
                                                              fastgltf::Options::LoadExternalBuffers | fastgltf::Options::DecomposeNodeMatrices);

  if (asset.error() != fastgltf::Error::None)
    throw std::invalid_argument("Error: Failed to load glTF: " + fastgltf::getErrorMessage(asset.error()));

  const std::vector<std::optional<Transform>> transforms = loadTransforms(asset->nodes, asset->meshes.size());
  auto [mesh, meshRenderer] = loadMeshes(asset.get(), transforms);

  const std::vector<std::optional<Image>> images = loadImages(asset->images, asset->buffers, asset->bufferViews, parentPath);
  loadMaterials(asset->materials, asset->textures, images, meshRenderer);

  Logger::debug("[GltfLoad] Loaded glTF file (" + std::to_string(mesh.getSubmeshes().size()) + " submesh(es), "
                                                + std::to_string(mesh.recoverVertexCount()) + " vertices, "
                                                + std::to_string(mesh.recoverTriangleCount()) + " triangles, "
                                                + std::to_string(meshRenderer.getMaterials().size()) + " material(s))");

  return { std::move(mesh), std::move(meshRenderer) };
}

} // namespace Raz::GltfFormat
