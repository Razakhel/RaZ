#include <fstream>
#include <map>
#include <sstream>

#include "RaZ/Render/Mesh.hpp"
#include "RaZ/Utils/FileUtils.hpp"

namespace Raz {

namespace {

Vec3f computeTangent(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos,
                     const Vec2f& firstTexcoords, const Vec2f& secondTexcoords, const Vec2f& thirdTexcoords) {
  const Vec3f firstEdge = secondPos - firstPos;
  const Vec3f secondEdge = thirdPos - firstPos;

  const Vec2f firstUVDiff = secondTexcoords - firstTexcoords;
  const Vec2f secondUVDiff = thirdTexcoords - firstTexcoords;

  const float inversionFactor = 1.f / (firstUVDiff[0] * secondUVDiff[1] - secondUVDiff[0] * firstUVDiff[1]);

  const Vec3f tangent = (firstEdge * secondUVDiff[1] - secondEdge * firstUVDiff[1]) * inversionFactor;

  return tangent;
}

TexturePtr loadTexture(const std::string& mtlFilePath, const std::string& textureFileName) {
  static std::unordered_map<std::string, TexturePtr> loadedTextures;

  TexturePtr map {};
  const auto loadedTexturePos = loadedTextures.find(textureFileName);

  if (loadedTexturePos != loadedTextures.cend()) {
    map = loadedTexturePos->second;
  } else {
    const auto texturePath = FileUtils::extractPathToFile(mtlFilePath) + textureFileName;
    map = Texture::create(texturePath);
    loadedTextures.emplace(textureFileName, map);
  }

  return map;
}

void importMtl(const std::string& mtlFilePath,
               std::vector<MaterialPtr>& materials,
               std::unordered_map<std::string, std::size_t>& materialCorrespIndices) {
  std::ifstream file(mtlFilePath, std::ios_base::in | std::ios_base::binary);

  auto standardMaterial = MaterialStandard::create();
  auto cookTorranceMaterial = MaterialCookTorrance::create();

  bool isStandardMaterial = false;
  bool isCookTorranceMaterial = false;

  if (file) {
    while (!file.eof()) {
      std::string tag;
      std::string nextValue;
      file >> tag >> nextValue;

      if (tag[0] == 'K') { // Assign properties
        std::string secondValue, thirdValue;
        file >> secondValue >> thirdValue;

        const float red   = std::stof(nextValue);
        const float green = std::stof(secondValue);
        const float blue  = std::stof(thirdValue);

        if (tag[1] == 'a') {                           // Ambient/ambient occlusion factor [Ka]
          standardMaterial->setAmbient(red, green, blue);
        } else if (tag[1] == 'd') {                    // Diffuse/albedo factor [Kd]
          standardMaterial->setDiffuse(red, green, blue);
        } else if (tag[1] == 's') {                    // Specular factor [Ks]
          standardMaterial->setSpecular(red, green, blue);
        } else if (tag[1] == 'e') {                    // Emissive factor [Ke]
          standardMaterial->setEmissive(red, green, blue);
        }

        isStandardMaterial = true;
      } else if (tag[0] == 'm') {                      // Import texture
        const TexturePtr& map = loadTexture(mtlFilePath, nextValue);

        if (tag[4] == 'K') {                           // Standard maps
          if (tag[5] == 'a') {                         // Ambient/ambient occlusion map [map_Ka]
            standardMaterial->setAmbientMap(map);
            cookTorranceMaterial->setAmbientOcclusionMap(map);
          } else if (tag[5] == 'd') {                  // Diffuse/albedo map [map_Kd]
            standardMaterial->setDiffuseMap(map);
            cookTorranceMaterial->setAlbedoMap(map);
          } else if (tag[5] == 's') {                  // Specular map [map_Ks]
            standardMaterial->setSpecularMap(map);
            isStandardMaterial = true;
          } else if (tag[5] == 'e') {                  // Emissive map [map_Ke]
            standardMaterial->setEmissiveMap(map);
          }
        }  else if (tag[4] == 'P') {                   // PBR maps
          if (tag[5] == 'm') {                         // Metallic map [map_Pm]
            cookTorranceMaterial->setMetallicMap(map);
          } else if (tag[5] == 'r') {                  // Roughness map [map_Pr]
            cookTorranceMaterial->setRoughnessMap(map);
          }

          isCookTorranceMaterial = true;
        } else if (tag[4] == 'd') {                    // Transparency map [map_d]
          standardMaterial->setTransparencyMap(map);
          isStandardMaterial = true;
        } else if (tag[4] == 'b') {                    // Bump map [map_bump]
          standardMaterial->setBumpMap(map);
          isStandardMaterial = true;
        }
      } else if (tag[0] == 'd') {                      // Transparency factor
        standardMaterial->setTransparency(std::stof(nextValue));
        isStandardMaterial = true;
      } else if (tag[0] == 'T') {
        if (tag[1] == 'r') {                           // Transparency factor (alias, 1 - d) [Tr]
          standardMaterial->setTransparency(1.f - std::stof(nextValue));
          isStandardMaterial = true;
        }/* else if (line[1] == 'f') {                 // Transmission filter [Tf]

          isStandardMaterial = true;
        }*/
      }  else if (tag[0] == 'b') {                     // Bump map (alias) [bump]
        standardMaterial->setBumpMap(loadTexture(mtlFilePath, nextValue));
        isStandardMaterial = true;
      } else if (tag[0] == 'n') {
        if (tag[1] == 'o') {                           // Normal map [norm]
          cookTorranceMaterial->setNormalMap(loadTexture(mtlFilePath, nextValue));
        } else if (tag[1] == 'e') {                    // New material [newmtl]
          materialCorrespIndices.emplace(nextValue, materialCorrespIndices.size());

          if (!isStandardMaterial && !isCookTorranceMaterial)
            continue;

          if (isCookTorranceMaterial)
            materials.emplace_back(std::move(cookTorranceMaterial));
          else
            materials.emplace_back(std::move(standardMaterial));

          standardMaterial = MaterialStandard::create();
          cookTorranceMaterial = MaterialCookTorrance::create();

          isStandardMaterial = false;
          isCookTorranceMaterial = false;
        }
      } else {
        std::getline(file, tag); // Skip the rest of the line
      }
    }
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + mtlFilePath + "'");
  }

  if (isCookTorranceMaterial)
    materials.emplace_back(std::move(cookTorranceMaterial));
  else
    materials.emplace_back(std::move(standardMaterial));
}

} // namespace

void Mesh::importObj(std::ifstream& file, const std::string& filePath) {
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
      if (line[1] == 'n') { // Normals
        Vec3f normalsTriplet {};

        file >> normalsTriplet[0]
             >> normalsTriplet[1]
             >> normalsTriplet[2];

        normals.push_back(normalsTriplet);
      } else if (line[1] == 't') { // Texcoords
        Vec2f texcoordsTriplet {};

        file >> texcoordsTriplet[0]
             >> texcoordsTriplet[1];

        texcoords.push_back(texcoordsTriplet);
      } else { // Vertices
        Vec3f positionTriplet {};

        file >> positionTriplet[0]
             >> positionTriplet[1]
             >> positionTriplet[2];

        positions.push_back(positionTriplet);
      }
    } else if (line[0] == 'f') { // Faces
      std::getline(file, line);

      const char delim      = '/';
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
        posIndices.back().emplace_back(partIndices[2]);
        posIndices.back().emplace_back(partIndices[0]);
        posIndices.back().emplace_back(partIndices[3]);

        texcoordsIndices.back().emplace_back(partIndices[6]);
        texcoordsIndices.back().emplace_back(partIndices[4]);
        texcoordsIndices.back().emplace_back(partIndices[7]);

        normalsIndices.back().emplace_back(partIndices[10]);
        normalsIndices.back().emplace_back(partIndices[8]);
        normalsIndices.back().emplace_back(partIndices[11]);
      }

      posIndices.back().emplace_back(partIndices[1]);
      posIndices.back().emplace_back(partIndices[0]);
      posIndices.back().emplace_back(partIndices[2]);

      texcoordsIndices.back().emplace_back(partIndices[4 + quadFaces]);
      texcoordsIndices.back().emplace_back(partIndices[3 + quadFaces]);
      texcoordsIndices.back().emplace_back(partIndices[5 + quadFaces]);

      const auto quadStride = static_cast<uint8_t>(quadFaces * 2);

      normalsIndices.back().emplace_back(partIndices[7 + quadStride]);
      normalsIndices.back().emplace_back(partIndices[6 + quadStride]);
      normalsIndices.back().emplace_back(partIndices[8 + quadStride]);
    } else if (line[0] == 'm') {
      std::string mtlFileName;
      file >> mtlFileName;

      const auto mtlFilePath = FileUtils::extractPathToFile(filePath) + mtlFileName;

      importMtl(mtlFilePath, m_materials, materialCorrespIndices);
    } else if (line[0] == 'u') {
      if (!materialCorrespIndices.empty()) {
        std::string materialName;
        file >> materialName;

        const auto correspMaterial = materialCorrespIndices.find(materialName);

        if (correspMaterial == materialCorrespIndices.cend())
          throw std::runtime_error("Error: No corresponding material found with the name '" + materialName + "'");

        m_submeshes.back()->setMaterialIndex(correspMaterial->second);
      }
    } else if (line[0] == 'o' || line[0] == 'g') {
      if (!posIndices.front().empty()) {
        const std::size_t newSize = posIndices.size() + 1;
        posIndices.resize(newSize);
        texcoordsIndices.resize(newSize);
        normalsIndices.resize(newSize);

        addSubmesh(Submesh::create());
      }

      std::getline(file, line);
    } else {
      std::getline(file, line); // Skip the rest of the line
    }
  }

  std::map<std::array<std::size_t, 3>, unsigned int> indicesMap;

  for (std::size_t submeshIndex = 0; submeshIndex < m_submeshes.size(); ++submeshIndex) {
    SubmeshPtr& submesh = m_submeshes[submeshIndex];
    indicesMap.clear();

    for (std::size_t partIndex = 0; partIndex < posIndices[submeshIndex].size(); ++partIndex) {
      // Face (vertices indices triplets), containing position/texcoords/normals
      // vertIndices[i][j] -> vertex i, feature j (j = 0 -> position, j = 1 -> texcoords, j = 2 -> normal)
      std::array<std::array<std::size_t, 3>, 3> vertIndices {};

      // First vertex informations
      int64_t tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[0][0] = (tempIndex < 0 ? tempIndex + positions.size() : tempIndex - 1ul);

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[0][1] = (tempIndex < 0 ? tempIndex + texcoords.size() : tempIndex - 1ul);

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[0][2] = (tempIndex < 0 ? tempIndex + normals.size() : tempIndex - 1ul);

      ++partIndex;

      // Second vertex informations
      tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[1][0] = (tempIndex < 0 ? tempIndex + positions.size() : tempIndex - 1ul);

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[1][1] = (tempIndex < 0 ? tempIndex + texcoords.size() : tempIndex - 1ul);

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[1][2] = (tempIndex < 0 ? tempIndex + normals.size() : tempIndex - 1ul);

      ++partIndex;

      // Third vertex informations
      tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[2][0] = (tempIndex < 0 ? tempIndex + positions.size() : tempIndex - 1ul);

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[2][1] = (tempIndex < 0 ? tempIndex + texcoords.size() : tempIndex - 1ul);

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[2][2] = (tempIndex < 0 ? tempIndex + normals.size() : tempIndex - 1ul);

      const std::array<Vec3f, 3> facePositions = { positions[vertIndices[0][0]],
                                                   positions[vertIndices[1][0]],
                                                   positions[vertIndices[2][0]] };

      Vec3f faceTangent {};
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
          submesh->getVertices()[indexIter->second].tangent += faceTangent; // Adding current tangent to be averaged later
          submesh->getIndices().emplace_back(indexIter->second);
        } else {
          Vertex vert {};

          vert.position  = facePositions[vertPartIndex];
          vert.texcoords = faceTexcoords[vertPartIndex];
          vert.normal    = faceNormals[vertPartIndex];
          vert.tangent   = faceTangent;

          submesh->getIndices().emplace_back(indicesMap.size());
          indicesMap.emplace(vertIndices[vertPartIndex], indicesMap.size());
          submesh->getVertices().push_back(vert);
        }
      }
    }

    // Normalizing tangents to become unit vectors & to be averaged after being accumulated
    for (auto& vertex : submesh->getVertices())
      vertex.tangent = (vertex.tangent - vertex.normal * vertex.tangent.dot(vertex.normal)).normalize();
  }
}

} // namespace Raz
