#include <array>
#include <fstream>
#include <sstream>
#include <iostream>

#include "RaZ/Render/Scene.hpp"

namespace Raz {

namespace {

const std::string extractFileExt(const std::string& fileName) {
  return (fileName.substr(fileName.find_last_of('.') + 1));
}

MeshPtr importOff(std::ifstream& file) {
  MeshPtr mesh = std::make_unique<Mesh>();

  unsigned int vertexCount, faceCount;
  file.ignore(3);
  file >> vertexCount >> faceCount;
  file.ignore(100, '\n');

  mesh->getVbo().getVertices().resize(vertexCount * 3);
  mesh->getEbo().getVerticesIndices().resize(faceCount * 3);

  for (unsigned int vertexIndex = 0; vertexIndex < vertexCount * 3; vertexIndex += 3)
    file >> mesh->getVbo().getVertices()[vertexIndex]
         >> mesh->getVbo().getVertices()[vertexIndex + 1]
         >> mesh->getVbo().getVertices()[vertexIndex + 2];

  for (unsigned int faceIndex = 0; faceIndex < faceCount * 3; faceIndex += 3) {
    file.ignore(2);
    file >> mesh->getEbo().getVerticesIndices()[faceIndex]
         >> mesh->getEbo().getVerticesIndices()[faceIndex + 1]
         >> mesh->getEbo().getVerticesIndices()[faceIndex + 2];
  }

  return mesh;
}

MeshPtr importObj(std::ifstream& file) {
  MeshPtr mesh = std::make_unique<Mesh>();

  while (!file.eof()) {
    std::string type;
    file >> type;

    if (type[0] == 'v') {
      if (type[1] == 'n') { // Normals
        mesh->getVbo().getNormals().resize(mesh->getVbo().getNormals().size() + 3);

        file >> mesh->getVbo().getNormals()[mesh->getVbo().getNormals().size() - 3]
             >> mesh->getVbo().getNormals()[mesh->getVbo().getNormals().size() - 2]
             >> mesh->getVbo().getNormals()[mesh->getVbo().getNormals().size() - 1];
      } else if (type[1] == 't') { // Texcoords
        mesh->getVbo().getTexcoords().resize(mesh->getVbo().getTexcoords().size() + 2);

        file >> mesh->getVbo().getTexcoords()[mesh->getVbo().getTexcoords().size() - 2]
             >> mesh->getVbo().getTexcoords()[mesh->getVbo().getTexcoords().size() - 1];
      } else { // Vertices
        mesh->getVbo().getVertices().resize(mesh->getVbo().getVertices().size() + 3);

        file >> mesh->getVbo().getVertices()[mesh->getVbo().getVertices().size() - 3]
             >> mesh->getVbo().getVertices()[mesh->getVbo().getVertices().size() - 2]
             >> mesh->getVbo().getVertices()[mesh->getVbo().getVertices().size() - 1];
      }
    } else if (type[0] == 'f') { // Faces
      mesh->getEbo().getVerticesIndices().resize(mesh->getEbo().getVerticesIndices().size() + 3);
      mesh->getEbo().getNormalsIndices().resize(mesh->getEbo().getNormalsIndices().size() + 3);
      mesh->getEbo().getTexcoordsIndices().resize(mesh->getEbo().getTexcoordsIndices().size() + 3);

      const char delim = '/';
      std::string index;
      std::array<std::string, 3> vertIndices;

      file >> index;
      for (uint8_t i = 0; i < 3; ++i)
        std::getline(std::stringstream(index), vertIndices[i], delim);

      *(mesh->getEbo().getVerticesIndices().end() - 3) = std::stoul(vertIndices[0]);
      *(mesh->getEbo().getTexcoordsIndices().end() - 3) = std::stoul(vertIndices[1]);
      *(mesh->getEbo().getNormalsIndices().end() - 3) = std::stoul(vertIndices[2]);

      file >> index;
      for (uint8_t i = 0; i < 3; ++i)
        std::getline(std::stringstream(index), vertIndices[i], delim);

      *(mesh->getEbo().getVerticesIndices().end() - 2) = std::stoul(vertIndices[0]);
      *(mesh->getEbo().getTexcoordsIndices().end() - 2) = std::stoul(vertIndices[1]);
      *(mesh->getEbo().getNormalsIndices().end() - 2) = std::stoul(vertIndices[2]);

      file >> index;
      for (uint8_t i = 0; i < 3; ++i)
        std::getline(std::stringstream(index), vertIndices[i], delim);

      *(mesh->getEbo().getVerticesIndices().end() - 1) = std::stoul(vertIndices[0]);
      *(mesh->getEbo().getTexcoordsIndices().end() - 1) = std::stoul(vertIndices[1]);
      *(mesh->getEbo().getNormalsIndices().end() - 1) = std::stoul(vertIndices[2]);
    } else if (type[0] == 'm') { // Import MTL
      //file >> type;
    } else if (type[0] == 'u') { // Use MTL
      //file >> type;
    } else {
      std::getline(file, type); // Skip the rest of the line
    }
  }

  return mesh;
}

} // namespace

void Scene::import(const std::string& fileName, const VertexShader& vertShader, const FragmentShader& fragShader) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  if (file) {
    const std::string format = extractFileExt(fileName);

    if (format == "off" || format == "OFF") {
      m_meshes.emplace_back(importOff(file));
    } else if (format == "obj" || format == "OBJ") {
      m_meshes.emplace_back(importObj(file));
    } else {
      std::cerr << "Error: '" << format << "' format is not supported" << std::endl;
    }

    m_meshes.back()->load(vertShader, fragShader);
  } else {
    std::cerr << "Error: Couldn't open the file '" << fileName << "'" << std::endl;
  }
}

void Scene::render() const {
  for (const auto& mesh : m_meshes)
    mesh->draw();
}

} // namespace Raz
