#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <fbxsdk.h>
#include <fstream>

namespace Raz::FbxFormat {

namespace {

void loadMaterials(FbxScene* scene, std::vector<MaterialPtr>& materials, const FilePath& filePath) {
  for (int matIndex = 0; matIndex < scene->GetMaterialCount(); ++matIndex) {
    const FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(matIndex);
    auto material = MaterialBlinnPhong::create();

    // Recovering properties

    const FbxPropertyT<FbxDouble3>& diffuse = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
    if (diffuse.IsValid()) {
      material->setDiffuse(static_cast<float>(diffuse.Get()[0]),
                           static_cast<float>(diffuse.Get()[1]),
                           static_cast<float>(diffuse.Get()[2]));
    }

    const FbxPropertyT<FbxDouble3>& ambient = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
    if (ambient.IsValid()) {
      material->setAmbient(static_cast<float>(ambient.Get()[0]),
                           static_cast<float>(ambient.Get()[1]),
                           static_cast<float>(ambient.Get()[2]));
    }

    const FbxPropertyT<FbxDouble3>& specular = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
    if (specular.IsValid()) {
      material->setSpecular(static_cast<float>(specular.Get()[0]),
                            static_cast<float>(specular.Get()[1]),
                            static_cast<float>(specular.Get()[2]));
    }

    const FbxPropertyT<FbxDouble3>& emissive = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
    if (emissive.IsValid()) {
      material->setEmissive(static_cast<float>(emissive.Get()[0]),
                            static_cast<float>(emissive.Get()[1]),
                            static_cast<float>(emissive.Get()[2]));
    }

    const FbxPropertyT<FbxDouble>& transparency = fbxMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
    if (transparency.IsValid())
      material->setTransparency(static_cast<float>(transparency.Get()));

    // Recovering textures

    const FilePath texturePath = filePath.recoverPathToFile();

    const auto* diffuseTexture = static_cast<FbxFileTexture*>(diffuse.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (diffuseTexture) {
      const std::string diffuseTexturePath = texturePath + diffuseTexture->GetRelativeFileName();

      try {
        material->loadDiffuseMap(diffuseTexturePath, 0);
      } catch (...) {
        Logger::error("[FBX] Failed to load diffuse map '" + diffuseTexturePath + "'.");
      }
    }

    const auto* ambientTexture = static_cast<FbxFileTexture*>(ambient.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (ambientTexture) {
      const std::string ambientTexturePath = texturePath + ambientTexture->GetRelativeFileName();

      try {
        material->loadAmbientMap(ambientTexturePath, 1);
      } catch (...) {
        Logger::error("[FBX] Failed to load ambient map '" + ambientTexturePath + "'.");
      }
    }

    const auto* specularTexture = static_cast<FbxFileTexture*>(specular.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (specularTexture) {
      const std::string specularTexturePath = texturePath + specularTexture->GetRelativeFileName();

      try {
        material->loadSpecularMap(specularTexturePath, 2);
      } catch (...) {
        Logger::error("[FBX] Failed to load specular map '" + specularTexturePath + "'.");
      }
    }

    const auto* emissiveTexture = static_cast<FbxFileTexture*>(emissive.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (emissiveTexture) {
      const std::string emissiveTexturePath = texturePath + emissiveTexture->GetRelativeFileName();

      try {
        material->loadEmissiveMap(emissiveTexturePath, 3);
      } catch (...) {
        Logger::error("[FBX] Failed to load emissive map '" + emissiveTexturePath + "'.");
      }
    }

    // Normal map not yet handled for standard materials
    /*
    const auto normMapProp = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
    if (normMapProp.IsValid()) {
      const auto* normalMap = static_cast<FbxFileTexture*>(normMapProp.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
      if (normalMap) {
        const std::string normalMapPath = texturePath + normalMap->GetRelativeFileName();

        try {
          material->loadNormalMap(normalMapPath);
        } catch (...) {
          Logger::error("[FBX] Failed to load normal map '" + normalMapPath + "'.");
        }
      }
    }
    */

    materials.emplace_back(std::move(material));
  }
}

} // namespace

std::pair<Mesh, MeshRenderer> load(const FilePath& filePath) {
  FbxManager* manager = FbxManager::Create();
  manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

  FbxScene* scene = FbxScene::Create(manager, filePath.recoverFileName().toUtf8().c_str());

  // Importing the contents of the file into the scene
  {
    FbxImporter* importer = FbxImporter::Create(manager, "");
    importer->Initialize(filePath.toUtf8().c_str(), -1, manager->GetIOSettings());
    importer->Import(scene);
    importer->Destroy();
  }

  Mesh mesh;
  MeshRenderer meshRenderer;

  mesh.getSubmeshes().reserve(scene->GetGeometryCount());
  meshRenderer.getSubmeshRenderers().reserve(scene->GetGeometryCount());

  // Recovering geometry
  for (int meshIndex = 0; meshIndex < scene->GetGeometryCount(); ++meshIndex) {
    auto* fbxMesh = static_cast<FbxMesh*>(scene->GetGeometry(meshIndex));
    Submesh submesh;
    SubmeshRenderer submeshRenderer;

    ////////////
    // Values //
    ////////////

    std::vector<Vertex>& vertices = submesh.getVertices();

    // Recovering positions
    vertices.resize(fbxMesh->GetControlPointsCount());

    for (int posIndex = 0; posIndex < fbxMesh->GetControlPointsCount(); ++posIndex) {
      const FbxVector4& pos = fbxMesh->GetControlPointAt(posIndex);

      // The FBX has a Z-up, but we expect a Y-up: positions', normals' & tangents' components are reordered
      vertices[posIndex].position.x() = static_cast<float>(pos[0]);
      vertices[posIndex].position.y() = static_cast<float>(pos[2]);
      vertices[posIndex].position.z() = static_cast<float>(pos[1]);
    }

    // Recovering texture coordinates (UVs)
    fbxMesh->InitTextureUV(fbxMesh->GetControlPointsCount());
    const FbxGeometryElementUV* meshTexcoords = fbxMesh->GetElementUV();

    if (meshTexcoords) {
      for (int texIndex = 0; texIndex < meshTexcoords->GetDirectArray().GetCount(); ++texIndex) {
        const FbxVector2& tex = meshTexcoords->GetDirectArray()[texIndex];

        vertices[texIndex].texcoords.x() = static_cast<float>(tex[0]);
        vertices[texIndex].texcoords.y() = static_cast<float>(tex[1]);
      }
    }

    // Recovering normals
    fbxMesh->GenerateNormals(true, true); // Re-generate normals by vertex
    const FbxGeometryElementNormal* meshNormals = fbxMesh->GetElementNormal();

    if (meshNormals) {
      for (int normIndex = 0; normIndex < meshNormals->GetDirectArray().GetCount(); ++normIndex) {
        const FbxVector4& norm = meshNormals->GetDirectArray()[normIndex];

        vertices[normIndex].normal.x() = static_cast<float>(norm[0]);
        vertices[normIndex].normal.y() = static_cast<float>(norm[2]);
        vertices[normIndex].normal.z() = static_cast<float>(norm[1]);
      }
    }

    // Recovering tangents
    // Not working yet, fetching/calculating way too many tangents (around 4x the amount of vertices)
    /*
    fbxMesh->GenerateTangentsData(meshTexcoords->GetName()); // Generate tangents using UVs
    const FbxGeometryElementTangent* meshTangents = fbxMesh->GetElementTangent();

    if (meshTangents) {
      for (int tanIndex = 0; tanIndex < meshTangents->GetDirectArray().GetCount(); ++tanIndex) {
        const FbxVector4& tan = meshTangents->GetDirectArray()[tanIndex];

        vertices[tanIndex].tangent.x() = static_cast<float>(tan[0]);
        vertices[tanIndex].tangent.y() = static_cast<float>(tan[2]);
        vertices[tanIndex].tangent.z() = static_cast<float>(tan[1]);
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