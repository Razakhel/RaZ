#include <fbxsdk.h>
#include <fstream>
#include <iostream>

#include "RaZ/Render/Model.hpp"
#include "RaZ/Utils/FileUtils.hpp"

namespace Raz {

ModelPtr Model::importFbx(const std::string& filePath) {
  MeshPtr mesh = std::make_unique<Mesh>();

  FbxManager* manager = FbxManager::Create();

  FbxIOSettings* ioSettings = FbxIOSettings::Create(manager, IOSROOT);
  manager->SetIOSettings(ioSettings);

  FbxImporter* importer = FbxImporter::Create(manager, "");

  importer->Initialize(filePath.c_str(), -1, manager->GetIOSettings());

  FbxScene* scene = FbxScene::Create(manager, FileUtils::extractFileNameFromPath(filePath).c_str());

  // Importing the contents of the file into the scene
  importer->Import(scene);
  importer->Destroy();

  // Recovering geometry
  for (int meshIndex = 0; meshIndex < scene->GetGeometryCount(); ++meshIndex) {
    const auto fbxMesh = static_cast<FbxMesh*>(scene->GetGeometry(meshIndex));
    SubmeshPtr submesh = std::make_unique<Submesh>();

    ////////////
    // Values //
    ////////////

    // Recovering positions
    submesh->getVertices().resize(fbxMesh->GetControlPointsCount());

    for (int posIndex = 0; posIndex < fbxMesh->GetControlPointsCount(); ++posIndex) {
      const FbxVector4& pos = fbxMesh->GetControlPointAt(posIndex);

      submesh->getVertices()[posIndex].position[0] = static_cast<float>(pos[0]);
      submesh->getVertices()[posIndex].position[1] = static_cast<float>(pos[1]);
      submesh->getVertices()[posIndex].position[2] = static_cast<float>(pos[2]);
    }

    // Recovering texture coordinates (UVs)
    fbxMesh->InitTextureUV(fbxMesh->GetControlPointsCount());
    const FbxGeometryElementUV* meshTexcoords = fbxMesh->GetElementUV();

    if (meshTexcoords) {
      for (int texIndex = 0; texIndex < meshTexcoords->GetDirectArray().GetCount(); ++texIndex) {
        const FbxVector2& tex = meshTexcoords->GetDirectArray()[texIndex];

        submesh->getVertices()[texIndex].texcoords[0] = static_cast<float>(tex[0]);
        submesh->getVertices()[texIndex].texcoords[1] = static_cast<float>(tex[1]);
      }
    }

    // Recovering normals
    fbxMesh->GenerateNormals(true, true); // Re-generate normals by vertex
    const FbxGeometryElementNormal* meshNormals = fbxMesh->GetElementNormal();

    if (meshNormals) {
      for (int normIndex = 0; normIndex < meshNormals->GetDirectArray().GetCount(); ++normIndex) {
        const FbxVector4& norm = meshNormals->GetDirectArray()[normIndex];

        submesh->getVertices()[normIndex].normal[0] = static_cast<float>(norm[0]);
        submesh->getVertices()[normIndex].normal[1] = static_cast<float>(norm[1]);
        submesh->getVertices()[normIndex].normal[2] = static_cast<float>(norm[2]);
      }
    }

    // Recovering tangents
    // Not working yet, fetching/calculating way too many tangents (around 4x the amount of vertices)
    /*fbxMesh->GenerateTangentsData(meshTexcoords->GetName()); // Generate tangents using UVs
    const FbxGeometryElementTangent* meshTangents = fbxMesh->GetElementTangent();

    if (meshTangents) {
      for (int tanIndex = 0; tanIndex < meshTangents->GetDirectArray().GetCount(); ++tanIndex) {
        const FbxVector4& tan = meshTangents->GetDirectArray()[tanIndex];

        submesh->getVertices()[tanIndex].tangent = static_cast<float>(tan[0]);
        submesh->getVertices()[tanIndex].tangent = static_cast<float>(tan[1]);
        submesh->getVertices()[tanIndex].tangent = static_cast<float>(tan[2]);
      }
    }*/

    // Process recovered data
    submesh->getIndices().reserve(static_cast<std::size_t>(fbxMesh->GetPolygonCount() * 3));

    for (int polyIndex = 0; polyIndex < fbxMesh->GetPolygonCount(); ++polyIndex) {
      submesh->getIndices().emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 0));
      submesh->getIndices().emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 2));
      submesh->getIndices().emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 1));

      for (int polyVertIndex = 3; polyVertIndex < fbxMesh->GetPolygonSize(polyIndex); ++polyVertIndex) {
        submesh->getIndices().emplace_back(fbxMesh->GetPolygonVertex(polyIndex, 0));
        submesh->getIndices().emplace_back(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex));
        submesh->getIndices().emplace_back(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex - 1));
      }
    }

    const auto& meshMaterial = fbxMesh->GetElementMaterial();
    if (meshMaterial) {
      if (meshMaterial->GetMappingMode() == FbxLayerElement::EMappingMode::eAllSame)
        // TODO: small hack to avoid segfaulting when mesh count > material count, but clearly wrong; find another way
        submesh->setMaterialIndex(std::min(meshIndex, scene->GetMaterialCount() - 1));
      else
        std::cerr << "Error: Materials can't be mapped by anything other than the whole submesh" << std::endl;
    }

    mesh->addSubmesh(std::move(submesh));
  }

  // Recovering materials
  for (int matIndex = 0; matIndex < scene->GetMaterialCount(); ++matIndex) {
    const FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(matIndex);
    auto material = std::make_unique<MaterialStandard>();

    const FbxPropertyT<FbxDouble3>& ambient = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
    if (ambient.IsValid())
      material->setAmbient(static_cast<float>(ambient.Get()[0]),
                           static_cast<float>(ambient.Get()[1]),
                           static_cast<float>(ambient.Get()[2]));

    const FbxPropertyT<FbxDouble3>& diffuse = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
    if (diffuse.IsValid())
      material->setDiffuse(static_cast<float>(diffuse.Get()[0]),
                           static_cast<float>(diffuse.Get()[1]),
                           static_cast<float>(diffuse.Get()[2]));

    const FbxPropertyT<FbxDouble3>& specular = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
    if (specular.IsValid())
      material->setSpecular(static_cast<float>(specular.Get()[0]),
                            static_cast<float>(specular.Get()[1]),
                            static_cast<float>(specular.Get()[2]));

    const FbxPropertyT<FbxDouble3>& emissive = fbxMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
    if (emissive.IsValid())
      material->setEmissive(static_cast<float>(emissive.Get()[0]),
                            static_cast<float>(emissive.Get()[1]),
                            static_cast<float>(emissive.Get()[2]));

    const FbxPropertyT<FbxDouble>& transparency = fbxMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
    if (transparency.IsValid())
      material->setTransparency(static_cast<float>(transparency.Get()));

    // Recovering textures
    const std::string texturePath = FileUtils::extractPathToFile(filePath);

    const auto ambientTexture = static_cast<FbxFileTexture*>(ambient.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (ambientTexture)
      material->loadAmbientMap(texturePath + ambientTexture->GetRelativeFileName());

    const auto diffuseTexture = static_cast<FbxFileTexture*>(diffuse.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (diffuseTexture)
      material->loadDiffuseMap(texturePath + diffuseTexture->GetRelativeFileName());

    const auto specularTexture = static_cast<FbxFileTexture*>(specular.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (specularTexture)
      material->loadSpecularMap(texturePath + specularTexture->GetRelativeFileName());

    const auto emissiveTexture = static_cast<FbxFileTexture*>(emissive.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
    if (emissiveTexture)
      material->loadEmissiveMap(texturePath + emissiveTexture->GetRelativeFileName());

    // Normal map not yet handled for standard materials
    /*const auto normMapProp = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
    if (normMapProp.IsValid()) {
      const auto normalMap = static_cast<FbxFileTexture*>(normMapProp.GetSrcObject(FbxCriteria::ObjectType(FbxFileTexture::ClassId)));
      if (normalMap)
        material->loadNormalMap(texturePath + normalMap->GetRelativeFileName());
    }*/

    mesh->addMaterial(std::move(material));
  }

  return std::make_unique<Model>(std::move(mesh));
}

} // namespace Raz
