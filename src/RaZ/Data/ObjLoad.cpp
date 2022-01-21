#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/Logger.hpp"

#include <fstream>
#include <map>
#include <sstream>

namespace Raz::ObjFormat {

namespace {

constexpr Vec3f computeTangent(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos,
                               const Vec2f& firstTexcoords, const Vec2f& secondTexcoords, const Vec2f& thirdTexcoords) noexcept {
  const Vec3f firstEdge = secondPos - firstPos;
  const Vec3f secondEdge = thirdPos - firstPos;

  const Vec2f firstUVDiff = secondTexcoords - firstTexcoords;
  const Vec2f secondUVDiff = thirdTexcoords - firstTexcoords;

  const float denominator = (firstUVDiff[0] * secondUVDiff[1] - secondUVDiff[0] * firstUVDiff[1]);

  if (denominator == 0.f)
    return Vec3f(0.f);

  return (firstEdge * secondUVDiff[1] - secondEdge * firstUVDiff[1]) / denominator;
}

inline TexturePtr loadTexture(const FilePath& mtlFilePath, const FilePath& textureFilePath, int bindingIndex = 0) {
  // Always apply a vertical flip to imported textures, since OpenGL maps them upside down
  return Texture::create(mtlFilePath.recoverPathToFile() + textureFilePath, bindingIndex, true);
}

inline void loadMtl(const FilePath& mtlFilePath,
                    std::vector<MaterialPtr>& materials,
                    std::unordered_map<std::string, std::size_t>& materialCorrespIndices) {
  Logger::debug("[ObjLoad] Loading MTL file ('" + mtlFilePath + "')...");

  std::ifstream file(mtlFilePath, std::ios_base::in | std::ios_base::binary);

  auto blinnPhongMaterial   = MaterialBlinnPhong::create();
  auto cookTorranceMaterial = MaterialCookTorrance::create();

  auto addLocalMaterial = [&blinnPhongMaterial, &cookTorranceMaterial, &materials] (bool isCookTorrance) {
    if (isCookTorrance) {
      cookTorranceMaterial->getAlbedoMap()->setBindingIndex(0);
      cookTorranceMaterial->getNormalMap()->setBindingIndex(1);
      cookTorranceMaterial->getMetallicMap()->setBindingIndex(2);
      cookTorranceMaterial->getRoughnessMap()->setBindingIndex(3);
      cookTorranceMaterial->getAmbientOcclusionMap()->setBindingIndex(4);

      materials.emplace_back(std::move(cookTorranceMaterial));
    } else {
      blinnPhongMaterial->getDiffuseMap()->setBindingIndex(0);
      blinnPhongMaterial->getAmbientMap()->setBindingIndex(1);
      blinnPhongMaterial->getSpecularMap()->setBindingIndex(2);
      blinnPhongMaterial->getEmissiveMap()->setBindingIndex(3);
      blinnPhongMaterial->getTransparencyMap()->setBindingIndex(4);
      blinnPhongMaterial->getBumpMap()->setBindingIndex(5);

      materials.emplace_back(std::move(blinnPhongMaterial));
    }
  };

  if (!file) {
    Logger::error("[ObjLoad] Couldn't open the MTL file '" + mtlFilePath + "'.");
    addLocalMaterial(true);
    return;
  }

  bool isBlinnPhongMaterial   = false;
  bool isCookTorranceMaterial = false;

  while (!file.eof()) {
    std::string tag;
    std::string nextValue;
    file >> tag >> nextValue;

    if (tag[0] == 'K') {                 // Standard properties
      std::string secondValue;
      std::string thirdValue;
      file >> secondValue >> thirdValue;

      const float red   = std::stof(nextValue);
      const float green = std::stof(secondValue);
      const float blue  = std::stof(thirdValue);

      if (tag[1] == 'a') {               // Ambient/ambient occlusion factor [Ka]
        blinnPhongMaterial->setAmbient(red, green, blue);
      } else if (tag[1] == 'd') {        // Diffuse/albedo factor [Kd]
        blinnPhongMaterial->setDiffuse(red, green, blue);
        cookTorranceMaterial->setBaseColor(red, green, blue);
      } else if (tag[1] == 's') {        // Specular factor [Ks]
        blinnPhongMaterial->setSpecular(red, green, blue);
      } else if (tag[1] == 'e') {        // Emissive factor [Ke]
        blinnPhongMaterial->setEmissive(red, green, blue);
      }

      isBlinnPhongMaterial = true;
    } else if (tag[0] == 'P') {          // PBR properties
      const float factor = std::stof(nextValue);

      if (tag[1] == 'm')                 // Metallic factor [Pm]
        cookTorranceMaterial->setMetallicFactor(factor);
      else if (tag[1] == 'r')            // Roughness factor [Pr]
        cookTorranceMaterial->setRoughnessFactor(factor);

      isCookTorranceMaterial = true;
    } else if (tag[0] == 'm') {          // Import texture
      const TexturePtr map = loadTexture(mtlFilePath, nextValue);

      if (tag[4] == 'K') {               // Standard maps
        if (tag[5] == 'd') {             // Diffuse/albedo map [map_Kd]
          blinnPhongMaterial->setDiffuseMap(map);
          cookTorranceMaterial->setAlbedoMap(map);
        } else if (tag[5] == 'a') {      // Ambient/ambient occlusion map [map_Ka]
          blinnPhongMaterial->setAmbientMap(map);
          cookTorranceMaterial->setAmbientOcclusionMap(map);
        } else if (tag[5] == 's') {      // Specular map [map_Ks]
          blinnPhongMaterial->setSpecularMap(map);
          isBlinnPhongMaterial = true;
        } else if (tag[5] == 'e') {      // Emissive map [map_Ke]
          blinnPhongMaterial->setEmissiveMap(map);
        }
      }  else if (tag[4] == 'P') {       // PBR maps
        if (tag[5] == 'm')               // Metallic map [map_Pm]
          cookTorranceMaterial->setMetallicMap(map);
        else if (tag[5] == 'r')          // Roughness map [map_Pr]
          cookTorranceMaterial->setRoughnessMap(map);

        isCookTorranceMaterial = true;
      } else if (tag[4] == 'd') {        // Transparency map [map_d]
        blinnPhongMaterial->setTransparencyMap(map);
        isBlinnPhongMaterial = true;
      } else if (tag[4] == 'b') {        // Bump map [map_bump]
        blinnPhongMaterial->setBumpMap(map);
        isBlinnPhongMaterial = true;
      }
    } else if (tag[0] == 'd') {          // Transparency factor
      blinnPhongMaterial->setTransparency(std::stof(nextValue));
      isBlinnPhongMaterial = true;
    } else if (tag[0] == 'T') {
      if (tag[1] == 'r') {               // Transparency factor (alias, 1 - d) [Tr]
        blinnPhongMaterial->setTransparency(1.f - std::stof(nextValue));
        isBlinnPhongMaterial = true;
      }/* else if (line[1] == 'f') {     // Transmission filter [Tf]

        isBlinnPhongMaterial = true;
      }*/
    }  else if (tag[0] == 'b') {         // Bump map (alias) [bump]
      blinnPhongMaterial->setBumpMap(loadTexture(mtlFilePath, nextValue, 5));
      isBlinnPhongMaterial = true;
    } else if (tag[0] == 'n') {
      if (tag[1] == 'o') {               // Normal map [norm]
        cookTorranceMaterial->setNormalMap(loadTexture(mtlFilePath, nextValue, 1));
      } else if (tag[1] == 'e') {        // New material [newmtl]
        materialCorrespIndices.emplace(nextValue, materialCorrespIndices.size());

        if (!isBlinnPhongMaterial && !isCookTorranceMaterial)
          continue;

        addLocalMaterial(isCookTorranceMaterial);

        blinnPhongMaterial   = MaterialBlinnPhong::create();
        cookTorranceMaterial = MaterialCookTorrance::create();

        isBlinnPhongMaterial   = false;
        isCookTorranceMaterial = false;
      }
    } else {
      std::getline(file, tag); // Skip the rest of the line
    }
  }

  addLocalMaterial(isCookTorranceMaterial);

  Logger::debug("[ObjLoad] Loaded MTL file (" + std::to_string(materials.size()) + " material(s) loaded)");
}

} // namespace

std::pair<Mesh, MeshRenderer> load(const FilePath& filePath) {
  Logger::debug("[ObjLoad] Loading OBJ file ('" + filePath + "')...");

  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Couldn't open the OBJ file '" + filePath + '\'');

  Mesh mesh;
  MeshRenderer meshRenderer;

  mesh.addSubmesh();
  meshRenderer.addSubmeshRenderer();

  std::unordered_map<std::string, std::size_t> materialCorrespIndices;

  std::vector<Vec3f> positions;
  std::vector<Vec2f> texcoords;
  std::vector<Vec3f> normals;

  std::vector<std::vector<int64_t>> posIndices(1);
  std::vector<std::vector<int64_t>> texcoordsIndices(1);
  std::vector<std::vector<int64_t>> normalsIndices(1);

  while (!file.eof()) {
    std::string line;
    file >> line;

    if (line[0] == 'v') {
      if (line[1] == 'n') { // Normal
        Vec3f normalTriplet;

        file >> normalTriplet.x()
             >> normalTriplet.y()
             >> normalTriplet.z();

        normals.push_back(normalTriplet);
      } else if (line[1] == 't') { // Texcoords
        Vec2f texcoordsTriplet;

        file >> texcoordsTriplet.x()
             >> texcoordsTriplet.y();

        texcoords.push_back(texcoordsTriplet);
      } else { // Position
        Vec3f positionTriplet;

        file >> positionTriplet.x()
             >> positionTriplet.y()
             >> positionTriplet.z();

        positions.push_back(positionTriplet);
      }
    } else if (line[0] == 'f') { // Faces
      std::getline(file, line);

      constexpr char delim  = '/';
      const auto nbVertices = static_cast<uint16_t>(std::count(line.cbegin(), line.cend(), ' '));
      const auto nbParts    = static_cast<uint8_t>(std::count(line.cbegin(), line.cend(), delim) / nbVertices + 1);
      const bool quadFaces  = (nbVertices == 4);

      std::stringstream indicesStream(line);
      std::vector<int64_t> partIndices(nbParts * nbVertices);
      std::string vertex;

      for (std::size_t vertIndex = 0; vertIndex < nbVertices; ++vertIndex) {
        indicesStream >> vertex;

        std::stringstream vertParts(vertex);
        std::string part;
        uint8_t partIndex = 0;

        while (std::getline(vertParts, part, delim)) {
          if (!part.empty())
            partIndices[partIndex * nbParts + vertIndex + (partIndex * quadFaces)] = std::stol(part);

          ++partIndex;
        }
      }

      if (quadFaces) {
        posIndices.back().emplace_back(partIndices[0]);
        posIndices.back().emplace_back(partIndices[2]);
        posIndices.back().emplace_back(partIndices[3]);

        texcoordsIndices.back().emplace_back(partIndices[4]);
        texcoordsIndices.back().emplace_back(partIndices[6]);
        texcoordsIndices.back().emplace_back(partIndices[7]);

        normalsIndices.back().emplace_back(partIndices[8]);
        normalsIndices.back().emplace_back(partIndices[10]);
        normalsIndices.back().emplace_back(partIndices[11]);
      }

      posIndices.back().emplace_back(partIndices[0]);
      posIndices.back().emplace_back(partIndices[1]);
      posIndices.back().emplace_back(partIndices[2]);

      texcoordsIndices.back().emplace_back(partIndices[3 + quadFaces]);
      texcoordsIndices.back().emplace_back(partIndices[4 + quadFaces]);
      texcoordsIndices.back().emplace_back(partIndices[5 + quadFaces]);

      const auto quadStride = static_cast<uint8_t>(quadFaces * 2);

      normalsIndices.back().emplace_back(partIndices[6 + quadStride]);
      normalsIndices.back().emplace_back(partIndices[7 + quadStride]);
      normalsIndices.back().emplace_back(partIndices[8 + quadStride]);
    } else if (line[0] == 'm') { // Material import (mtllib)
      std::string mtlFileName;
      file >> mtlFileName;

      const std::string mtlFilePath = filePath.recoverPathToFile() + mtlFileName;
      loadMtl(mtlFilePath, meshRenderer.getMaterials(), materialCorrespIndices);
    } else if (line[0] == 'u') { // Material usage (usemtl)
      if (materialCorrespIndices.empty())
        continue;

      std::string materialName;
      file >> materialName;

      const auto correspMaterial = materialCorrespIndices.find(materialName);

      if (correspMaterial == materialCorrespIndices.cend())
        Logger::error("[ObjLoad] No corresponding material found with the name '" + materialName + "'.");
      else
        meshRenderer.getSubmeshRenderers().back().setMaterialIndex(correspMaterial->second);
    } else if (line[0] == 'o' || line[0] == 'g') {
      if (!posIndices.front().empty()) {
        const std::size_t newSize = posIndices.size() + 1;
        posIndices.resize(newSize);
        texcoordsIndices.resize(newSize);
        normalsIndices.resize(newSize);

        mesh.addSubmesh();
        meshRenderer.addSubmeshRenderer().setMaterialIndex(std::numeric_limits<std::size_t>::max());
      }

      std::getline(file, line);
    } else {
      std::getline(file, line); // Skip the rest of the line
    }
  }

  const auto posCount  = static_cast<int64_t>(positions.size());
  const auto texCount  = static_cast<int64_t>(texcoords.size());
  const auto normCount = static_cast<int64_t>(normals.size());

  std::map<std::array<std::size_t, 3>, unsigned int> indicesMap;

  for (std::size_t submeshIndex = 0; submeshIndex < mesh.getSubmeshes().size(); ++submeshIndex) {
    Submesh& submesh = mesh.getSubmeshes()[submeshIndex];
    indicesMap.clear();

    for (std::size_t partIndex = 0; partIndex < posIndices[submeshIndex].size(); ++partIndex) {
      // Face (vertices indices triplets), containing position/texcoords/normals
      // vertIndices[i][j] -> vertex i, feature j (j = 0 -> position, j = 1 -> texcoords, j = 2 -> normal)
      std::array<std::array<std::size_t, 3>, 3> vertIndices {};

      // First vertex informations
      int64_t tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[0][0] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + posCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[0][1] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + texCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[0][2] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + normCount) : static_cast<std::size_t>(tempIndex - 1));

      ++partIndex;

      // Second vertex informations
      tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[1][0] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + posCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[1][1] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + texCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[1][2] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + normCount) : static_cast<std::size_t>(tempIndex - 1));

      ++partIndex;

      // Third vertex informations
      tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[2][0] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + posCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[2][1] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + texCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[2][2] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + normCount) : static_cast<std::size_t>(tempIndex - 1));

      const std::array<Vec3f, 3> facePositions = { positions[vertIndices[0][0]],
                                                   positions[vertIndices[1][0]],
                                                   positions[vertIndices[2][0]] };

      Vec3f faceTangent;
      std::array<Vec2f, 3> faceTexcoords {};
      if (!texcoords.empty()) {
        faceTexcoords[0] = texcoords[vertIndices[0][1]];
        faceTexcoords[1] = texcoords[vertIndices[1][1]];
        faceTexcoords[2] = texcoords[vertIndices[2][1]];

        faceTangent = computeTangent(facePositions[0], facePositions[1], facePositions[2],
                                     faceTexcoords[0], faceTexcoords[1], faceTexcoords[2]);
      }

      std::array<Vec3f, 3> faceNormals {};
      if (!normals.empty()) {
        faceNormals[0] = normals[vertIndices[0][2]];
        faceNormals[1] = normals[vertIndices[1][2]];
        faceNormals[2] = normals[vertIndices[2][2]];
      }

      for (uint8_t vertPartIndex = 0; vertPartIndex < 3; ++vertPartIndex) {
        const auto indexIter = indicesMap.find(vertIndices[vertPartIndex]);

        if (indexIter != indicesMap.cend()) {
          submesh.getVertices()[indexIter->second].tangent += faceTangent; // Adding current tangent to be averaged later
          submesh.getTriangleIndices().emplace_back(indexIter->second);
        } else {
          Vertex vert {};

          vert.position  = facePositions[vertPartIndex];
          vert.texcoords = faceTexcoords[vertPartIndex];
          vert.normal    = faceNormals[vertPartIndex];
          vert.tangent   = faceTangent;

          submesh.getTriangleIndices().emplace_back(static_cast<unsigned int>(indicesMap.size()));
          indicesMap.emplace(vertIndices[vertPartIndex], static_cast<unsigned int>(indicesMap.size()));
          submesh.getVertices().emplace_back(vert);
        }
      }
    }

    // Normalizing tangents to become unit vectors & to be averaged after being accumulated
    for (Vertex& vertex : submesh.getVertices()) {
      // If the tangent is null, don't normalize it to avoid NaNs
      if (vertex.tangent.strictlyEquals(Vec3f(0.f)))
        continue;

      vertex.tangent = (vertex.tangent - vertex.normal * vertex.tangent.dot(vertex.normal)).normalize();
    }
  }

  // Creating the mesh renderer from the mesh's data
  meshRenderer.load(mesh);

  Logger::debug("[ObjLoad] Loaded OBJ file (" + std::to_string(mesh.getSubmeshes().size()) + " submesh(es), "
                                              + std::to_string(mesh.recoverVertexCount()) + " vertices, "
                                              + std::to_string(mesh.recoverTriangleCount()) + " triangles)");

  return { std::move(mesh), std::move(meshRenderer) };
}

} // namespace Raz::ObjFormat
