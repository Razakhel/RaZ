#include <map>
#include <fstream>
#include <sstream>

#include "RaZ/Render/Model.hpp"
#include "RaZ/Utils/MtlLoader.hpp"
#include "RaZ/Utils/ModelLoader.hpp"

namespace Raz {

namespace {

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

ModelPtr importObj(std::ifstream& file) {
  MeshPtr mesh = std::make_shared<Mesh>();
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
    indicesMap.clear();

    for (std::size_t partIndex = 0; partIndex < posIndices[submeshIndex].size(); ++partIndex) {
      const int64_t tempPosIndex = posIndices[submeshIndex][partIndex];
      const std::size_t posIndex = (tempPosIndex < 0 ? tempPosIndex + positions.size() : tempPosIndex - 1ul);

      const int64_t tempTexIndex = texcoordsIndices[submeshIndex][partIndex];
      const std::size_t texIndex = (tempTexIndex < 0 ? tempTexIndex + texcoords.size() : tempTexIndex - 1ul);

      const int64_t tempNormIndex = normalsIndices[submeshIndex][partIndex];
      const std::size_t normIndex = (tempNormIndex < 0 ? tempNormIndex + normals.size() : tempNormIndex - 1ul);

      const std::array<std::size_t, 3> vertIndices = { posIndex, texIndex, normIndex };

      const auto indexIter = indicesMap.find(vertIndices);
      if (indexIter != indicesMap.cend()) {
        mesh->getSubmeshes()[submeshIndex]->getEbo().getIndices().emplace_back(indexIter->second);
      } else {
        Vertex vert {};

        vert.positions[0] = positions[posIndex][0];
        vert.positions[1] = positions[posIndex][1];
        vert.positions[2] = positions[posIndex][2];

        if (!texcoords.empty()) {
          vert.texcoords[0] = texcoords[texIndex][0];
          vert.texcoords[1] = texcoords[texIndex][1];
        }

        if (!normals.empty()) {
          vert.normals[0] = normals[normIndex][0];
          vert.normals[1] = normals[normIndex][1];
          vert.normals[2] = normals[normIndex][2];
        }

        mesh->getSubmeshes()[submeshIndex]->getEbo().getIndices().emplace_back(indicesMap.size());
        indicesMap.emplace(vertIndices, indicesMap.size());
        mesh->getSubmeshes()[submeshIndex]->getVbo().getVertices().push_back(vert);
      }
    }
  }

  return std::make_unique<Model>(std::move(mesh));
}

ModelPtr importOff(std::ifstream& file) {
  MeshPtr mesh = std::make_shared<Mesh>();
  mesh->getSubmeshes().emplace_back(std::make_unique<Submesh>());

  std::size_t vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  mesh->getSubmeshes().front()->getVbo().getVertices().resize(vertexCount * 3);
  mesh->getSubmeshes().front()->getEbo().getIndices().resize(faceCount * 3);

  for (std::size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    file >> mesh->getSubmeshes().front()->getVbo().getVertices()[vertexIndex].positions[0]
         >> mesh->getSubmeshes().front()->getVbo().getVertices()[vertexIndex].positions[1]
         >> mesh->getSubmeshes().front()->getVbo().getVertices()[vertexIndex].positions[2];

  for (std::size_t faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
    file.ignore(2);
    file >> mesh->getSubmeshes().front()->getEbo().getIndices()[faceIndex]
         >> mesh->getSubmeshes().front()->getEbo().getIndices()[faceIndex + 1]
         >> mesh->getSubmeshes().front()->getEbo().getIndices()[faceIndex + 2];
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

    model->load();
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + fileName + "'");
  }

  return model;
}

} // namespace Raz
