#include "RaZ/Data/GltfFormat.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <fastgltf/parser.hpp>

namespace Raz::GltfFormat {

namespace {

template <typename T>
void loadVertexData(const fastgltf::Accessor& accessor,
                    const std::vector<fastgltf::Buffer>& buffers,
                    const std::vector<fastgltf::BufferView>& bufferViews,
                    std::vector<Vertex>& vertices,
                    void (*callback)(Vertex&, const T*)) {
  assert("Error: Loading vertex data requires the accessor to reference a buffer view." && accessor.bufferViewIndex.has_value());

  const fastgltf::BufferView& bufferView = bufferViews[*accessor.bufferViewIndex];
  const fastgltf::DataSource& bufferData = buffers[bufferView.bufferIndex].data;

  if (!std::holds_alternative<fastgltf::sources::Vector>(bufferData))
    throw std::runtime_error("Error: Cannot load glTF data from sources other than vectors.");

  const std::size_t dataOffset    = bufferView.byteOffset + accessor.byteOffset;
  const uint8_t* const vertexData = std::get<fastgltf::sources::Vector>(bufferData).bytes.data() + dataOffset;
  const std::size_t dataStride    = bufferView.byteStride.value_or(fastgltf::getElementByteSize(accessor.type, accessor.componentType));

  for (std::size_t vertIndex = 0; vertIndex < vertices.size(); ++vertIndex) {
    const auto* data = reinterpret_cast<const T*>(vertexData + vertIndex * dataStride);
    callback(vertices[vertIndex], data);
  }
}

void loadVertices(const fastgltf::Primitive& primitive,
                  const std::vector<fastgltf::Buffer>& buffers,
                  const std::vector<fastgltf::BufferView>& bufferViews,
                  const std::vector<fastgltf::Accessor>& accessors,
                  std::vector<Vertex>& vertices) {
  Logger::debug("[GltfLoad] Loading vertices...");

  const auto positionIt = primitive.findAttribute("POSITION");

  if (positionIt == primitive.attributes.end())
    throw std::invalid_argument("Error: Required 'POSITION' attribute not found in the glTF file.");

  const fastgltf::Accessor& positionAccessor = accessors[positionIt->second];

  if (!positionAccessor.bufferViewIndex.has_value())
    return;

  vertices.resize(positionAccessor.count);

  loadVertexData<float>(positionAccessor, buffers, bufferViews, vertices, [] (Vertex& vert, const float* data) {
    vert.position = Vec3f(data[0], data[1], data[2]);
  });

  constexpr std::array<std::pair<std::string_view, void (*)(Vertex&, const float*)>, 3> attributes = {{
    { "TEXCOORD_0", [] (Vertex& vert, const float* data) {
      // The texcoords can be outside the [0; 1] range; they're normalized according to the REPEAT mode. This may be subject to change
      //   See: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_wrapping
      vert.texcoords = Vec2f(data[0], data[1]);

      for (float& elt : vert.texcoords.getData()) {
        if (elt < -1.f || elt > 1.f) elt = std::fmod(elt, 1.f);
        if (elt < 0.f) elt += 1.f;
      }
    }},
    { "NORMAL",  [] (Vertex& vert, const float* data) { vert.normal = Vec3f(data[0], data[1], data[2]); } },
    { "TANGENT", [] (Vertex& vert, const float* data) {
      // The tangent's input W component (data[3]) is either 1 or -1 and represents the handedness
      //   See: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes-overview
      vert.tangent = Vec3f(data[0], data[1], data[2]) * data[3];
    }}
  }};

  for (auto&& [attribName, callback] : attributes) {
    const auto attribIter = primitive.findAttribute(attribName);

    if (attribIter == primitive.attributes.end())
      continue;

    const fastgltf::Accessor& attribAccessor = accessors[attribIter->second];

    if (attribAccessor.bufferViewIndex.has_value())
      loadVertexData(attribAccessor, buffers, bufferViews, vertices, callback);
  }

  Logger::debug("[GltfLoad] Loaded vertices");
}

void loadIndices(const fastgltf::Accessor& indicesAccessor,
                 const std::vector<fastgltf::Buffer>& buffers,
                 const std::vector<fastgltf::BufferView>& bufferViews,
                 std::vector<unsigned int>& indices) {
  Logger::debug("[GltfLoad] Loading indices...");

  if (!indicesAccessor.bufferViewIndex.has_value())
    throw std::invalid_argument("Error: Missing glTF buffer to load indices from.");

  indices.resize(indicesAccessor.count);

  const fastgltf::BufferView& indicesView = bufferViews[*indicesAccessor.bufferViewIndex];
  const fastgltf::Buffer& indicesBuffer   = buffers[indicesView.bufferIndex];

  if (!std::holds_alternative<fastgltf::sources::Vector>(indicesBuffer.data))
    throw std::runtime_error("Error: Cannot load glTF data from sources other than vectors.");

  const std::size_t dataOffset     = indicesView.byteOffset + indicesAccessor.byteOffset;
  const uint8_t* const indicesData = std::get<fastgltf::sources::Vector>(indicesBuffer.data).bytes.data() + dataOffset;
  const std::size_t dataSize       = fastgltf::getElementByteSize(indicesAccessor.type, indicesAccessor.componentType);
  const std::size_t dataStride     = indicesView.byteStride.value_or(dataSize);

  for (std::size_t i = 0; i < indices.size(); ++i) {
    // The indices must be of an unsigned integer type, but its size is unspecified
    // See: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#_mesh_primitive_indices
    switch (dataSize) {
      case 1:
        indices[i] = *reinterpret_cast<const uint8_t*>(indicesData + i * dataStride);
        break;

      case 2:
        indices[i] = *reinterpret_cast<const uint16_t*>(indicesData + i * dataStride);
        break;

      case 4:
        indices[i] = *reinterpret_cast<const uint32_t*>(indicesData + i * dataStride);
        break;

      default:
        throw std::invalid_argument("Error: Unexpected indices data size (" + std::to_string(dataSize) + ").");
    }
  }

  Logger::debug("[GltfLoad] Loaded indices");
}

std::pair<Mesh, MeshRenderer> loadMeshes(const std::vector<fastgltf::Mesh>& meshes,
                                         const std::vector<fastgltf::Buffer>& buffers,
                                         const std::vector<fastgltf::BufferView>& bufferViews,
                                         const std::vector<fastgltf::Accessor>& accessors) {
  Logger::debug("[GltfLoad] Loading " + std::to_string(meshes.size()) + " mesh(es)...");

  Mesh loadedMesh;
  MeshRenderer loadedMeshRenderer;

  for (const fastgltf::Mesh& mesh : meshes) {
    for (const fastgltf::Primitive& primitive : mesh.primitives) {
      if (!primitive.indicesAccessor.has_value())
        throw std::invalid_argument("Error: The glTF file requires having indexed geometry.");

      Submesh& submesh = loadedMesh.addSubmesh();
      SubmeshRenderer& submeshRenderer = loadedMeshRenderer.addSubmeshRenderer();

      loadVertices(primitive, buffers, bufferViews, accessors, submesh.getVertices());
      loadIndices(accessors[*primitive.indicesAccessor], buffers, bufferViews, submesh.getTriangleIndices());

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
        loadedImages.emplace_back(ImageFormat::loadFromData(imgData.bytes));
      },
      [&bufferViews, &buffers, &loadedImages] (const fastgltf::sources::BufferView& bufferViewSource) {
        const fastgltf::BufferView& imgView = bufferViews[bufferViewSource.bufferViewIndex];
        const fastgltf::Buffer& imgBuffer   = buffers[imgView.bufferIndex];

        std::visit(fastgltf::visitor {
          [&loadedImages, &imgView] (const fastgltf::sources::Vector& imgData) {
            loadedImages.emplace_back(ImageFormat::loadFromData(imgData.bytes.data() + imgView.byteOffset, imgView.byteLength));
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

void loadMaterials(const std::vector<fastgltf::Material>& materials, const std::vector<std::optional<Image>>& images, MeshRenderer& meshRenderer) {
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
                                  mat.emissiveFactor[2]) * mat.emissiveStrength.value_or(1.f), MaterialAttribute::Emissive);
    matProgram.setAttribute(mat.pbrData.metallicFactor, MaterialAttribute::Metallic);
    matProgram.setAttribute(mat.pbrData.roughnessFactor, MaterialAttribute::Roughness);

    if (mat.pbrData.baseColorTexture && images[mat.pbrData.baseColorTexture->textureIndex])
      matProgram.setTexture(Texture2D::create(*images[mat.pbrData.baseColorTexture->textureIndex]), MaterialTexture::BaseColor);

    if (mat.emissiveTexture && images[mat.emissiveTexture->textureIndex])
      matProgram.setTexture(Texture2D::create(*images[mat.emissiveTexture->textureIndex]), MaterialTexture::Emissive);

    if (mat.occlusionTexture && images[mat.occlusionTexture->textureIndex]) { // Ambient occlusion
      const Image ambientOcclusionImg = extractAmbientOcclusionImage(*images[mat.occlusionTexture->textureIndex]);
      matProgram.setTexture(Texture2D::create(ambientOcclusionImg), MaterialTexture::Ambient);
    }

    if (mat.normalTexture && images[mat.normalTexture->textureIndex])
      matProgram.setTexture(Texture2D::create(*images[mat.normalTexture->textureIndex]), MaterialTexture::Normal);

    if (mat.pbrData.metallicRoughnessTexture && images[mat.pbrData.metallicRoughnessTexture->textureIndex]) {
      const auto [metalnessImg, roughnessImg] = extractMetalnessRoughnessImages(*images[mat.pbrData.metallicRoughnessTexture->textureIndex]);
      matProgram.setTexture(Texture2D::create(metalnessImg), MaterialTexture::Metallic);
      matProgram.setTexture(Texture2D::create(roughnessImg), MaterialTexture::Roughness);
    }

    loadedMat.loadType(MaterialType::COOK_TORRANCE);
  }

  Logger::debug("[GltfLoad] Loaded material(s)");
}

} // namespace

std::pair<Mesh, MeshRenderer> load(const FilePath& filePath) {
  Logger::debug("[GltfLoad] Loading glTF file ('" + filePath + "')...");

  if (!FileUtils::isReadable(filePath))
    throw std::invalid_argument("Error: The glTF file '" + filePath + "' either does not exist or cannot be opened.");

  fastgltf::GltfDataBuffer data;

  if (!data.loadFromFile(filePath.getPath()))
    throw std::invalid_argument("Error: Could not load the glTF file.");

  const FilePath parentPath = filePath.recoverPathToFile();
  fastgltf::Expected<fastgltf::Asset> asset(fastgltf::Error::None);

  fastgltf::Parser parser;

  switch (fastgltf::determineGltfFileType(&data)) {
    case fastgltf::GltfType::glTF:
      asset = parser.loadGLTF(&data, parentPath.getPath(), fastgltf::Options::LoadExternalBuffers);
      break;

    case fastgltf::GltfType::GLB:
      asset = parser.loadBinaryGLTF(&data, parentPath.getPath(), fastgltf::Options::LoadGLBBuffers);
      break;

    default:
      throw std::invalid_argument("Error: Failed to determine glTF container.");
  }

  if (asset.error() != fastgltf::Error::None)
    throw std::invalid_argument("Error: Failed to load glTF: " + fastgltf::getErrorMessage(asset.error()));

  auto [mesh, meshRenderer] = loadMeshes(asset->meshes, asset->buffers, asset->bufferViews, asset->accessors);

  const std::vector<std::optional<Image>> images = loadImages(asset->images, asset->buffers, asset->bufferViews, parentPath);
  loadMaterials(asset->materials, images, meshRenderer);

  Logger::debug("[GltfLoad] Loaded glTF file (" + std::to_string(mesh.getSubmeshes().size()) + " submesh(es), "
                                                + std::to_string(mesh.recoverVertexCount()) + " vertices, "
                                                + std::to_string(mesh.recoverTriangleCount()) + " triangles, "
                                                + std::to_string(meshRenderer.getMaterials().size()) + " material(s))");

  return { std::move(mesh), std::move(meshRenderer) };
}

} // namespace Raz::GltfFormat
