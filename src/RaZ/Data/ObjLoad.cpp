#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Data/ImageFormat.hpp"
#include "RaZ/Data/Mesh.hpp"
#include "RaZ/Data/ObjFormat.hpp"
#include "RaZ/Render/MeshRenderer.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "tracy/Tracy.hpp"

#include <fstream>
#include <map>
#include <sstream>

namespace Raz::ObjFormat {

namespace {

inline Texture2DPtr loadTexture(const FilePath& textureFilePath, const Color& defaultColor, bool shouldUseSrgb = false) {
  ZoneScopedN("[ObjLoad]::loadTexture");
  ZoneTextF("Path: %s", textureFilePath.toUtf8().c_str());

  if (!FileUtils::isReadable(textureFilePath)) {
    Logger::warn("[ObjLoad] Cannot load texture '" + textureFilePath + "'; either the file does not exist or it cannot be opened.");
    return Texture2D::create(defaultColor);
  }

  // Always apply a vertical flip to imported textures, since OpenGL maps them upside down
  return Texture2D::create(ImageFormat::load(textureFilePath, true), true, shouldUseSrgb);
}

inline void loadMtl(const FilePath& mtlFilePath,
                    std::vector<Material>& materials,
                    std::unordered_map<std::string, std::size_t>& materialCorrespIndices) {
  ZoneScopedN("[ObjLoad]::loadMtl");
  ZoneTextF("Path: %s", mtlFilePath.toUtf8().c_str());

  Logger::debug("[ObjLoad] Loading MTL file ('" + mtlFilePath + "')...");

  std::ifstream file(mtlFilePath, std::ios_base::binary);

  if (!file) {
    Logger::error("[ObjLoad] Could not open the MTL file '" + mtlFilePath + "'.");
    materials.emplace_back(MaterialType::COOK_TORRANCE);
    return;
  }

  Material material;
  MaterialType materialType = MaterialType::BLINN_PHONG;

  while (!file.eof()) {
    std::string tag;
    std::string nextValue;
    file >> tag >> nextValue;

    if (tag[0] == 'K') {                 // Standard properties [K*]
      std::string secondValue;
      std::string thirdValue;
      file >> secondValue >> thirdValue;

      const Vec3f values(std::stof(nextValue), std::stof(secondValue), std::stof(thirdValue));

      if (tag[1] == 'd')                 // Diffuse/albedo factor [Kd]
        material.getProgram().setAttribute(values, MaterialAttribute::BaseColor);
      else if (tag[1] == 'e')            // Emissive factor [Ke]
        material.getProgram().setAttribute(values, MaterialAttribute::Emissive);
      else if (tag[1] == 'a')            // Ambient factor [Ka]
        material.getProgram().setAttribute(values, MaterialAttribute::Ambient);
      else if (tag[1] == 's')            // Specular factor [Ks]
        material.getProgram().setAttribute(values, MaterialAttribute::Specular);
    } else if (tag[0] == 'P') {          // PBR properties [P*]
      const float factor = std::stof(nextValue);

      if (tag[1] == 'm') {               // Metallic factor [Pm]
        material.getProgram().setAttribute(factor, MaterialAttribute::Metallic);
      } else if (tag[1] == 'r') {        // Roughness factor [Pr]
        material.getProgram().setAttribute(factor, MaterialAttribute::Roughness);
      } else if (tag[1] == 's') {        // Sheen factors [Ps]
        std::string secondValue;
        std::string thirdValue;
        std::string fourthValue;
        file >> secondValue >> thirdValue >> fourthValue;
        material.getProgram().setAttribute(Vec4f(factor, std::stof(secondValue), std::stof(thirdValue), std::stof(fourthValue)), MaterialAttribute::Sheen);
      }

      materialType = MaterialType::COOK_TORRANCE;
    } else if (tag[0] == 'm') {          // Import texture [map_*]
      const FilePath textureFilePath = mtlFilePath.recoverPathToFile() + nextValue;

      if (tag[4] == 'K') {               // Standard maps [map_K*]
        if (tag[5] == 'd')               // Diffuse/albedo map [map_Kd]
          material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::White, true), MaterialTexture::BaseColor);
        else if (tag[5] == 'e')          // Emissive map [map_Ke]
          material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::White, true), MaterialTexture::Emissive);
        else if (tag[5] == 'a')          // Ambient/ambient occlusion map [map_Ka]
          material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::White, true), MaterialTexture::Ambient);
        else if (tag[5] == 's')          // Specular map [map_Ks]
          material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::White, true), MaterialTexture::Specular);
      } else if (tag[4] == 'P') {        // PBR maps [map_P*]
        if (tag[5] == 'm')               // Metallic map [map_Pm]
          material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::Red), MaterialTexture::Metallic);
        else if (tag[5] == 'r')          // Roughness map [map_Pr]
          material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::Red), MaterialTexture::Roughness);
        else if (tag[5] == 's')          // Sheen map [map_Ps]
          material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::White, true), MaterialTexture::Sheen); // TODO: should be an RGBA texture with an alpha of 1

        materialType = MaterialType::COOK_TORRANCE;
      } else if (tag[4] == 'd') {        // Opacity (dissolve) map [map_d]
        Texture2DPtr map = loadTexture(textureFilePath, ColorPreset::White);
        map->setFilter(TextureFilter::NEAREST, TextureFilter::NEAREST, TextureFilter::NEAREST);
        material.getProgram().setTexture(std::move(map), MaterialTexture::Opacity);
      } else if (tag[4] == 'b') {        // Bump map [map_bump]
        material.getProgram().setTexture(loadTexture(textureFilePath, ColorPreset::White), MaterialTexture::Bump);
      }
    } else if (tag[0] == 'd') {          // Opacity (dissolve) factor [d]
      material.getProgram().setAttribute(std::stof(nextValue), MaterialAttribute::Opacity);
    } else if (tag[0] == 'T') {
      if (tag[1] == 'r')                 // Transparency factor (alias, 1 - d) [Tr]
        material.getProgram().setAttribute(1.f - std::stof(nextValue), MaterialAttribute::Opacity);
    } else if (tag[0] == 'b') {          // Bump map (alias) [bump]
      material.getProgram().setTexture(loadTexture(mtlFilePath.recoverPathToFile() + nextValue, ColorPreset::White), MaterialTexture::Bump);
    } else if (tag[0] == 'n') {
      if (tag[1] == 'o') {               // Normal map [norm]
        material.getProgram().setTexture(loadTexture(mtlFilePath.recoverPathToFile() + nextValue, ColorPreset::MediumBlue), MaterialTexture::Normal);
      } else if (tag[1] == 'e') {        // New material [newmtl]
        materialCorrespIndices.emplace(nextValue, materialCorrespIndices.size());

        if (material.isEmpty())
          continue;

        material.loadType(materialType);
        materials.emplace_back(std::move(material));

        material     = Material();
        materialType = MaterialType::BLINN_PHONG;
      }
    } else {
      std::getline(file, tag); // Skip the rest of the line
    }
  }

  material.loadType(materialType);
  materials.emplace_back(std::move(material));

  Logger::debug("[ObjLoad] Loaded MTL file (" + std::to_string(materials.size()) + " material(s) loaded)");
}

} // namespace

std::pair<Mesh, MeshRenderer> load(const FilePath& filePath) {
  ZoneScopedN("ObjFormat::load");
  ZoneTextF("Path: %s", filePath.toUtf8().c_str());

  Logger::debug("[ObjLoad] Loading OBJ file ('" + filePath + "')...");

  std::ifstream file(filePath, std::ios_base::binary);

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

      // First vertex information
      int64_t tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[0][0] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + posCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[0][1] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + texCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[0][2] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + normCount) : static_cast<std::size_t>(tempIndex - 1));

      ++partIndex;

      // Second vertex information
      tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[1][0] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + posCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[1][1] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + texCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[1][2] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + normCount) : static_cast<std::size_t>(tempIndex - 1));

      ++partIndex;

      // Third vertex information
      tempIndex = posIndices[submeshIndex][partIndex];
      vertIndices[2][0] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + posCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = texcoordsIndices[submeshIndex][partIndex];
      vertIndices[2][1] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + texCount) : static_cast<std::size_t>(tempIndex - 1));

      tempIndex = normalsIndices[submeshIndex][partIndex];
      vertIndices[2][2] = (tempIndex < 0 ? static_cast<std::size_t>(tempIndex + normCount) : static_cast<std::size_t>(tempIndex - 1));

      const std::array<Vec3f, 3> facePositions = { positions[vertIndices[0][0]],
                                                   positions[vertIndices[1][0]],
                                                   positions[vertIndices[2][0]] };

      std::array<Vec2f, 3> faceTexcoords {};
      if (!texcoords.empty()) {
        faceTexcoords[0] = texcoords[vertIndices[0][1]];
        faceTexcoords[1] = texcoords[vertIndices[1][1]];
        faceTexcoords[2] = texcoords[vertIndices[2][1]];
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
          submesh.getTriangleIndices().emplace_back(indexIter->second);
          continue;
        }

        const Vertex vert {
          facePositions[vertPartIndex],
          faceTexcoords[vertPartIndex],
          faceNormals[vertPartIndex]
        };

        submesh.getTriangleIndices().emplace_back(static_cast<unsigned int>(indicesMap.size()));
        indicesMap.emplace(vertIndices[vertPartIndex], static_cast<unsigned int>(indicesMap.size()));
        submesh.getVertices().emplace_back(vert);
      }
    }
  }

  mesh.computeTangents();

  // Creating the mesh renderer from the mesh's data
  meshRenderer.load(mesh);

  Logger::debug("[ObjLoad] Loaded OBJ file (" + std::to_string(mesh.getSubmeshes().size()) + " submesh(es), "
                                              + std::to_string(mesh.recoverVertexCount()) + " vertices, "
                                              + std::to_string(mesh.recoverTriangleCount()) + " triangles, "
                                              + std::to_string(meshRenderer.getMaterials().size()) + " material(s))");

  return { std::move(mesh), std::move(meshRenderer) };
}

} // namespace Raz::ObjFormat
