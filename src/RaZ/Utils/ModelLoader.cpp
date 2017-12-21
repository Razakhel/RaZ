#include <map>
#include <fstream>
#include <sstream>

#include "RaZ/Render/Model.hpp"
#include "RaZ/Utils/ModelLoader.hpp"

namespace Raz {

namespace {

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

ModelPtr importObj(std::ifstream& file) {
  MeshPtr mesh = std::make_shared<Mesh>();
  SubmeshPtr currentSubmesh = std::make_unique<Submesh>();
  Material material;

  std::vector<Vec3f> positions;
  std::vector<Vec2f> texcoords;
  std::vector<Vec3f> normals;

  std::vector<int64_t> posIndices;
  std::vector<int64_t> texcoordsIndices;
  std::vector<int64_t> normalsIndices;

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
        posIndices.emplace_back(partIndices[2]);
        posIndices.emplace_back(partIndices[0]);
        posIndices.emplace_back(partIndices[3]);

        texcoordsIndices.emplace_back(partIndices[6]);
        texcoordsIndices.emplace_back(partIndices[4]);
        texcoordsIndices.emplace_back(partIndices[7]);

        normalsIndices.emplace_back(partIndices[10]);
        normalsIndices.emplace_back(partIndices[8]);
        normalsIndices.emplace_back(partIndices[11]);
      }

      posIndices.emplace_back(partIndices[1]);
      posIndices.emplace_back(partIndices[0]);
      posIndices.emplace_back(partIndices[2]);

      texcoordsIndices.emplace_back(partIndices[4 + quadFaces]);
      texcoordsIndices.emplace_back(partIndices[3 + quadFaces]);
      texcoordsIndices.emplace_back(partIndices[5 + quadFaces]);

      const auto quadStride = static_cast<uint8_t>(quadFaces * 2);

      normalsIndices.emplace_back(partIndices[7 + quadStride]);
      normalsIndices.emplace_back(partIndices[6 + quadStride]);
      normalsIndices.emplace_back(partIndices[8 + quadStride]);
    //} else if (line[0] == 'm') {
      // Import MTL
    //} else if (line[0] == 'u') {
      // Use MTL
    } else if (line[0] == 'o') {
      mesh->addSubmesh(std::move(currentSubmesh));
      currentSubmesh = std::make_unique<Submesh>();
    //} else if (line[0] == 'g') {
      // Create/use group
    //} else if (line[0] == 's') {
      // Enable/disable smooth shading
    } else {
      std::getline(file, line); // Skip the rest of the line
    }
  }

  std::map<std::array<std::size_t, 3>, unsigned int> indicesMap;

  for (std::size_t partIndex = 0; partIndex < posIndices.size(); ++partIndex) {
    const int64_t tempPosIndex = posIndices[partIndex];
    const std::size_t posIndex = (tempPosIndex < 0 ? tempPosIndex + positions.size() : tempPosIndex - 1ul);

    const int64_t tempTexIndex = texcoordsIndices[partIndex];
    const std::size_t texIndex = (tempTexIndex < 0 ? tempTexIndex + texcoords.size() : tempTexIndex - 1ul);

    const int64_t tempNormIndex = normalsIndices[partIndex];
    const std::size_t normIndex = (tempNormIndex < 0 ? tempNormIndex + normals.size() : tempNormIndex - 1ul);

    const std::array<std::size_t, 3> vertIndices = { posIndex, texIndex, normIndex };

    const auto indexIter = indicesMap.find(vertIndices);
    if (indexIter != indicesMap.cend()) {
      currentSubmesh->getEbo().getIndices().emplace_back(indexIter->second);
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

      currentSubmesh->getEbo().getIndices().emplace_back(indicesMap.size());
      indicesMap.emplace(vertIndices, indicesMap.size());
      currentSubmesh->getVbo().getVertices().push_back(vert);
    }
  }

  mesh->addSubmesh(std::move(currentSubmesh));
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
