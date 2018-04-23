#include <map>
#include <fstream>
#include <sstream>

#include "RaZ/Render/Model.hpp"
#include "RaZ/Utils/MtlLoader.hpp"
#include "RaZ/Utils/ModelLoader.hpp"

namespace Raz {

namespace {

std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

Vec3f computeTangent(const Vec3f& firstPos, const Vec3f& secondPos, const Vec3f& thirdPos,
                     const Vec2f& firstTexcoords, const Vec2f& secondTexcoords, const Vec2f& thirdTexcoords) {
  const Vec3f firstEdge  = secondPos - firstPos;
  const Vec3f secondEdge = thirdPos - firstPos;

  const Vec2f firstUVDiff  = secondTexcoords - firstTexcoords;
  const Vec2f secondUVDiff = thirdTexcoords - firstTexcoords;

  const float inversionFactor = 1.f / (firstUVDiff[0] * secondUVDiff[1] - secondUVDiff[0] * firstUVDiff[1]);

  const Vec3f tangent = (firstEdge * secondUVDiff[1] - secondEdge * firstUVDiff[1]) * inversionFactor;

  return tangent;
}

ModelPtr importObj(std::ifstream& file) {
  MeshPtr mesh = std::make_unique<Mesh>();
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

      const char delim = '/';
      const auto nbVertices = static_cast<uint16_t>(std::count(line.cbegin(), line.cend(), ' '));
      const auto nbParts = static_cast<uint8_t>(std::count(line.cbegin(), line.cend(), delim) / nbVertices + 1);
      const bool quadFaces = (nbVertices == 4);

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
      std::string materialName;
      file >> materialName;

      MtlLoader::importMtl(materialName, mesh->getMaterials(), materialCorrespIndices);
    } else if (line[0] == 'u') {
      std::string materialName;
      file >> materialName;

      mesh->getSubmeshes().back()->setMaterialIndex(materialCorrespIndices.find(materialName)->second);
    } else if (line[0] == 'o' || line[0] == 'g') {
      if (!posIndices.front().empty()) {
        const std::size_t newSize = posIndices.size() + 1;
        posIndices.resize(newSize);
        texcoordsIndices.resize(newSize);
        normalsIndices.resize(newSize);

        mesh->addSubmesh(std::make_unique<Submesh>());
      }

      std::getline(file, line);
    } else {
      std::getline(file, line); // Skip the rest of the line
    }
  }

  std::map<std::array<std::size_t, 3>, unsigned int> indicesMap;

  for (std::size_t submeshIndex = 0; submeshIndex < mesh->getSubmeshes().size(); ++submeshIndex) {
    SubmeshPtr& submesh = mesh->getSubmeshes()[submeshIndex];
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

  return std::make_unique<Model>(std::move(mesh));
}

ModelPtr importOff(std::ifstream& file) {
  MeshPtr mesh = std::make_unique<Mesh>();
  mesh->getSubmeshes().emplace_back(std::make_unique<Submesh>());

  std::size_t vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  mesh->getSubmeshes().front()->getVertices().resize(vertexCount * 3);

  for (std::size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    file >> mesh->getSubmeshes().front()->getVertices()[vertexIndex].position[0]
         >> mesh->getSubmeshes().front()->getVertices()[vertexIndex].position[1]
         >> mesh->getSubmeshes().front()->getVertices()[vertexIndex].position[2];

  for (std::size_t faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
    uint16_t partCount {};
    file >> partCount;

    mesh->getSubmeshes().front()->getIndices().reserve(mesh->getSubmeshes().front()->getIndices().size() + partCount);

    std::vector<std::size_t> indices(partCount);
    file >> indices[0] >> indices[1] >> indices[2];

    mesh->getSubmeshes().front()->getIndices().emplace_back(indices[0]);
    mesh->getSubmeshes().front()->getIndices().emplace_back(indices[1]);
    mesh->getSubmeshes().front()->getIndices().emplace_back(indices[2]);

    for (uint16_t partIndex = 3; partIndex < partCount; ++partIndex) {
      file >> indices[partIndex];

      mesh->getSubmeshes().front()->getIndices().emplace_back(indices[0]);
      mesh->getSubmeshes().front()->getIndices().emplace_back(indices[partIndex - 1]);
      mesh->getSubmeshes().front()->getIndices().emplace_back(indices[partIndex]);
    }
  }

  return std::make_unique<Model>(std::move(mesh));
}

} // namespace

ModelPtr ModelLoader::importModel(const std::string& fileName) {
  ModelPtr model;
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "obj" || format == "OBJ")
      model = std::move(importObj(file));
    else if (format == "off" || format == "OFF")
      model = std::move(importOff(file));
    else
      throw std::runtime_error("Error: '" + format + "' format is not supported");
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + fileName + "'");
  }

  return model;
}

} // namespace Raz
