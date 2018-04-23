#include <fstream>

#include "RaZ/Utils/MtlLoader.hpp"

namespace Raz {

namespace MtlLoader {

void importMtl(const std::string& fileName,
               std::vector<MaterialPtr>& materials,
               std::unordered_map<std::string, std::size_t>& materialCorrespIndices) {
  std::ifstream file(fileName, std::ios_base::in | std::ios_base::binary);

  auto standardMaterial = std::make_unique<MaterialStandard>();
  auto cookTorranceMaterial = std::make_unique<MaterialCookTorrance>();

  bool isStandardMaterial = false;
  bool isCookTorranceMaterial = false;

  if (file) {
    while (!file.eof()) {
      std::string line;
      std::string nextValue;
      file >> line >> nextValue;

      if (line[0] == 'K') { // Assign properties
        std::string secondValue, thirdValue;
        file >> secondValue >> thirdValue;

        const float red = std::stof(nextValue);
        const float green = std::stof(secondValue);
        const float blue = std::stof(thirdValue);

        if (line[1] == 'a') {
          standardMaterial->setAmbient(red, green, blue);
        } else if (line[1] == 'd') {
          standardMaterial->setDiffuse(red, green, blue);
        } else if (line[1] == 's') {
          standardMaterial->setSpecular(red, green, blue);
        } else if (line[1] == 'e') {
          standardMaterial->setEmissive(red, green, blue);
        }

        isStandardMaterial = true;
      } else if (line[0] == 'm') { // Import texture
        if (line[4] == 'K') {
          if (line[5] == 'a') {
            standardMaterial->loadAmbientMap(nextValue);
            isStandardMaterial = true;
          } else if (line[5] == 'd') {
            standardMaterial->loadDiffuseMap(nextValue);
            cookTorranceMaterial->loadAlbedoMap(nextValue);
          } else if (line[5] == 's') {
            standardMaterial->loadSpecularMap(nextValue);
            isStandardMaterial = true;
          }
        }  else if (line[4] == 'P') {
          if (line[5] == 'm') {
            cookTorranceMaterial->loadMetallicMap(nextValue);
            isCookTorranceMaterial = true;
          } else if (line[5] == 'r') {
            cookTorranceMaterial->loadRoughnessMap(nextValue);
            isCookTorranceMaterial = true;
          }
        } else if (line[4] == 'd') {
          standardMaterial->loadTransparencyMap(nextValue);
          isStandardMaterial = true;
        } else if (line[4] == 'b') {
          standardMaterial->loadBumpMap(nextValue);
          isStandardMaterial = true;
        }
      } else if (line[0] == 'T') {
        if (line[1] == 'r') { // Assign transparency
          standardMaterial->setTransparency(std::stof(nextValue));
          isStandardMaterial = true;
        }/* else if (line[1] == 'f') { // Assign transmission filter

          isStandardMaterial = true;
        }*/
      } else if (line[0] == 'd') { // Assign transparency
        standardMaterial->setTransparency(std::stof(nextValue));
        isStandardMaterial = true;
      } else if (line[0] == 'b') { // Import bump map
        standardMaterial->loadBumpMap(nextValue);
        isStandardMaterial = true;
      } else if (line[0] == 'n') {
        if (line[1] == 'o') { // Import normal map
          cookTorranceMaterial->loadNormalMap(nextValue);
          isCookTorranceMaterial = true;
        } else if (line[1] == 'e') { // Create new material
          materialCorrespIndices.insert({ nextValue, materialCorrespIndices.size() });

          if (!isStandardMaterial && !isCookTorranceMaterial)
            continue;

          if (isCookTorranceMaterial) {
            materials.emplace_back(std::move(cookTorranceMaterial));
            cookTorranceMaterial = std::make_unique<MaterialCookTorrance>();
          } else {
            materials.emplace_back(std::move(standardMaterial));
            standardMaterial = std::make_unique<MaterialStandard>();
          }

          isStandardMaterial = false;
          isCookTorranceMaterial = false;
        }
      } else {
        std::getline(file, line); // Skip the rest of the line
      }
    }
  } else {
    throw std::runtime_error("Error: Couldn't open the file '" + fileName + "'");
  }

  if (isCookTorranceMaterial)
    materials.emplace_back(std::move(cookTorranceMaterial));
  else
    materials.emplace_back(std::move(standardMaterial));
}

} // namespace MtlLoader

} // namespace Raz
