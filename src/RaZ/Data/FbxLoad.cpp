#include "RaZ/Data/FbxFormat.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <fbxsdk.h>

namespace Raz::FbxFormat {

namespace {

void loadMaterials(fbxsdk::FbxScene* scene, std::vector<Material>& materials, const FilePath& filePath) {
  for (int matIndex = 0; matIndex < scene->GetMaterialCount(); ++matIndex) {
    const fbxsdk::FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(matIndex);
    Material material;

    // Recovering properties

    const fbxsdk::FbxPropertyT<fbxsdk::FbxDouble3>& diffuse = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
    if (diffuse.IsValid()) {
      material.getProgram().setAttribute(Vec3f(static_cast<float>(diffuse.Get()[0]),
                                               static_cast<float>(diffuse.Get()[1]),
                                               static_cast<float>(diffuse.Get()[2])), MaterialAttribute::BaseColor);
    }

    const fbxsdk::FbxPropertyT<fbxsdk::FbxDouble3>& emissive = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sEmissive);
    if (emissive.IsValid()) {
      material.getProgram().setAttribute(Vec3f(static_cast<float>(emissive.Get()[0]),
                                               static_cast<float>(emissive.Get()[1]),
                                               static_cast<float>(emissive.Get()[2])), MaterialAttribute::Emissive);
    }

    const fbxsdk::FbxPropertyT<fbxsdk::FbxDouble3>& ambient = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sAmbient);
    if (ambient.IsValid()) {
      material.getProgram().setAttribute(Vec3f(static_cast<float>(ambient.Get()[0]),
                                               static_cast<float>(ambient.Get()[1]),
                                               static_cast<float>(ambient.Get()[2])), MaterialAttribute::Ambient);
    }

    const fbxsdk::FbxPropertyT<fbxsdk::FbxDouble3>& specular = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular);
    if (specular.IsValid()) {
      material.getProgram().setAttribute(Vec3f(static_cast<float>(specular.Get()[0]),
                                               static_cast<float>(specular.Get()[1]),
                                               static_cast<float>(specular.Get()[2])), MaterialAttribute::Specular);
    }

    const fbxsdk::FbxPropertyT<fbxsdk::FbxDouble>& transparency = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sTransparencyFactor);
    if (transparency.IsValid())
      material.getProgram().setAttribute(static_cast<float>(transparency.Get()), MaterialAttribute::Transparency);

    // Recovering textures

    const FilePath texturePath = filePath.recoverPathToFile();

    const auto* diffuseTexture = static_cast<fbxsdk::FbxFileTexture*>(diffuse.GetSrcObject(fbxsdk::FbxCriteria::ObjectType(fbxsdk::FbxFileTexture::ClassId)));
    if (diffuseTexture) {
      const std::string diffuseTexturePath = texturePath + diffuseTexture->GetRelativeFileName();

      try {
        material.getProgram().setTexture(Texture2D::create(ImageFormat::load(diffuseTexturePath, true)), MaterialTexture::BaseColor);
      } catch (...) {
        Logger::error("[FBX] Failed to load diffuse map '" + diffuseTexturePath + "'.");
      }
    }

    const auto* emissiveTexture = static_cast<fbxsdk::FbxFileTexture*>(emissive.GetSrcObject(fbxsdk::FbxCriteria::ObjectType(fbxsdk::FbxFileTexture::ClassId)));
    if (emissiveTexture) {
      const std::string emissiveTexturePath = texturePath + emissiveTexture->GetRelativeFileName();

      try {
        material.getProgram().setTexture(Texture2D::create(ImageFormat::load(emissiveTexturePath, true)), MaterialTexture::Emissive);
      } catch (...) {
        Logger::error("[FBX] Failed to load emissive map '" + emissiveTexturePath + "'.");
      }
    }

    const auto* ambientTexture = static_cast<fbxsdk::FbxFileTexture*>(ambient.GetSrcObject(fbxsdk::FbxCriteria::ObjectType(fbxsdk::FbxFileTexture::ClassId)));
    if (ambientTexture) {
      const std::string ambientTexturePath = texturePath + ambientTexture->GetRelativeFileName();

      try {
        material.getProgram().setTexture(Texture2D::create(ImageFormat::load(ambientTexturePath, true)), MaterialTexture::Ambient);
      } catch (...) {
        Logger::error("[FBX] Failed to load ambient map '" + ambientTexturePath + "'.");
      }
    }

    const auto* specularTexture = static_cast<fbxsdk::FbxFileTexture*>(specular.GetSrcObject(fbxsdk::FbxCriteria::ObjectType(fbxsdk::FbxFileTexture::ClassId)));
    if (specularTexture) {
      const std::string specularTexturePath = texturePath + specularTexture->GetRelativeFileName();

      try {
        material.getProgram().setTexture(Texture2D::create(ImageFormat::load(specularTexturePath, true)), MaterialTexture::Specular);
      } catch (...) {
        Logger::error("[FBX] Failed to load specular map '" + specularTexturePath + "'.");
      }
    }

    // Normal map not yet handled for standard materials
    /*
    const auto normMapProp = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap);
    if (normMapProp.IsValid()) {
      const auto* normalMap = static_cast<fbxsdk::FbxFileTexture*>(normMapProp.GetSrcObject(fbxsdk::FbxCriteria::ObjectType(fbxsdk::FbxFileTexture::ClassId)));
      if (normalMap) {
        const std::string normalMapPath = texturePath + normalMap->GetRelativeFileName();

        try {
          material.getProgram().setTexture(Texture2D::create(ImageFormat::load(normalMapPath, true)), MaterialTexture::Normal);
        } catch (...) {
          Logger::error("[FBX] Failed to load normal map '" + normalMapPath + "'.");
        }
      }
    }
    */

    material.loadType(MaterialType::BLINN_PHONG);
    materials.emplace_back(std::move(material));
  }
}

} // namespace

std::pair<Mesh, MeshRenderer> load(const FilePath& filePath) {
  fbxsdk::FbxManager* manager = fbxsdk::FbxManager::Create();
  manager->SetIOSettings(fbxsdk::FbxIOSettings::Create(manager, IOSROOT));

  fbxsdk::FbxScene* scene = fbxsdk::FbxScene::Create(manager, filePath.recoverFileName().toUtf8().c_str());

  // Importing the contents of the file into the scene
  {
    fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(manager, "");
    importer->Initialize(filePath.toUtf8().c_str(), -1, manager->GetIOSettings());
    importer->Import(scene);
    importer->Destroy();
  }

  // Overriding the coordinates system & scene unit to match what we expect
  // See: https://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_nodes_and_scene_graph_fbx_scenes_scene_axis_and_unit_conversion_html
  fbxsdk::FbxAxisSystem::OpenGL.ConvertScene(scene); // OpenGL basis
  fbxsdk::FbxSystemUnit::m.ConvertScene(scene); // Units in meters

  Mesh mesh;
  MeshRenderer meshRenderer;

  mesh.getSubmeshes().reserve(scene->GetGeometryCount());
  meshRenderer.getSubmeshRenderers().reserve(scene->GetGeometryCount());

  // Recovering geometry
  for (int meshIndex = 0; meshIndex < scene->GetGeometryCount(); ++meshIndex) {
    auto* fbxMesh = static_cast<fbxsdk::FbxMesh*>(scene->GetGeometry(meshIndex));
    Submesh submesh;
    SubmeshRenderer submeshRenderer;

    // Recovering the mesh's global transform
    // See: https://help.autodesk.com/view/FBX/2020/ENU/?guid=FBX_Developer_Help_nodes_and_scene_graph_fbx_nodes_html#transformation-data
    const fbxsdk::FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform();

    ////////////
    // Values //
    ////////////

    std::vector<Vertex>& vertices = submesh.getVertices();

    // Recovering positions
    vertices.resize(fbxMesh->GetControlPointsCount());

    for (int posIndex = 0; posIndex < fbxMesh->GetControlPointsCount(); ++posIndex) {
      const fbxsdk::FbxVector4 pos = globalTransform.MultT(fbxMesh->GetControlPointAt(posIndex));

      vertices[posIndex].position.x() = static_cast<float>(pos[0]);
      vertices[posIndex].position.y() = static_cast<float>(pos[1]);
      vertices[posIndex].position.z() = static_cast<float>(pos[2]);
    }

    // Recovering texture coordinates (UVs)
    fbxMesh->InitTextureUV(fbxMesh->GetControlPointsCount());
    const fbxsdk::FbxGeometryElementUV* meshTexcoords = fbxMesh->GetElementUV();

    if (meshTexcoords) {
      for (int texIndex = 0; texIndex < meshTexcoords->GetDirectArray().GetCount(); ++texIndex) {
        const fbxsdk::FbxVector2& tex = meshTexcoords->GetDirectArray()[texIndex];

        vertices[texIndex].texcoords.x() = static_cast<float>(tex[0]);
        vertices[texIndex].texcoords.y() = static_cast<float>(tex[1]);
      }
    }

    // Recovering normals
    fbxMesh->GenerateNormals(true, true); // Re-generate normals by vertex
    const fbxsdk::FbxGeometryElementNormal* meshNormals = fbxMesh->GetElementNormal();

    if (meshNormals) {
      for (int normIndex = 0; normIndex < meshNormals->GetDirectArray().GetCount(); ++normIndex) {
        const fbxsdk::FbxVector4 norm = globalTransform.Inverse().MultT(meshNormals->GetDirectArray()[normIndex]);

        vertices[normIndex].normal.x() = static_cast<float>(norm[0]);
        vertices[normIndex].normal.y() = static_cast<float>(norm[1]);
        vertices[normIndex].normal.z() = static_cast<float>(norm[2]);
      }
    }

    // Recovering tangents
    // Not working yet, fetching/calculating way too many tangents (around 4x the amount of vertices)
    /*
    fbxMesh->GenerateTangentsData(meshTexcoords->GetName()); // Generate tangents using UVs
    const fbxsdk::FbxGeometryElementTangent* meshTangents = fbxMesh->GetElementTangent();

    if (meshTangents) {
      for (int tanIndex = 0; tanIndex < meshTangents->GetDirectArray().GetCount(); ++tanIndex) {
        const fbxsdk::FbxVector4 tan = globalTransform.MultR(meshTangents->GetDirectArray()[tanIndex]);

        vertices[tanIndex].tangent.x() = static_cast<float>(tan[0]);
        vertices[tanIndex].tangent.y() = static_cast<float>(tan[1]);
        vertices[tanIndex].tangent.z() = static_cast<float>(tan[2]);
      }
    }
    */

    std::vector<unsigned int>& indices = submesh.getTriangleIndices();

    // Process recovered data
    indices.reserve(static_cast<std::size_t>(fbxMesh->GetPolygonCount()) * 3);

    for (int polyIndex = 0; polyIndex < fbxMesh->GetPolygonCount(); ++polyIndex) {
      indices.emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 0));
      indices.emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 1));
      indices.emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 2));

      for (int polyVertIndex = 3; polyVertIndex < fbxMesh->GetPolygonSize(polyIndex); ++polyVertIndex) {
        indices.emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 0));
        indices.emplace_back(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex - 1));
        indices.emplace_back(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex));
      }
    }

    const auto& meshMaterial = fbxMesh->GetElementMaterial();
    if (meshMaterial) {
      if (meshMaterial->GetMappingMode() == FbxLayerElement::EMappingMode::eAllSame)
        // TODO: small hack to avoid segfaulting when mesh count > material count, but clearly wrong; find another way
        submeshRenderer.setMaterialIndex(std::min(meshIndex, scene->GetMaterialCount() - 1));
      else
        Logger::error("[FBX] Materials can't be mapped to anything other than the whole submesh.");
    }

    mesh.addSubmesh(std::move(submesh));
    meshRenderer.addSubmeshRenderer(std::move(submeshRenderer));

    meshRenderer.getSubmeshRenderers().back().load(mesh.getSubmeshes().back());
  }

  loadMaterials(scene, meshRenderer.getMaterials(), filePath);

  scene->Destroy();
  manager->Destroy();

  return { std::move(mesh), std::move(meshRenderer) };
}

} // namespace Raz::FbxFormat
