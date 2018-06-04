#include <fstream>

#include "RaZ/Utils/FileUtils.hpp"
#include "RaZ/Utils/MtlLoader.hpp"

namespace Raz {

namespace MtlLoader {

namespace {

TexturePtr loadTexture(const std::string& mtlFilePath, const std::string& textureFileName) {
  static std::unordered_map<std::string, TexturePtr> loadedTextures;

  TexturePtr map {};
  const auto loadedTexturePos = loadedTextures.find(textureFileName);

  if (loadedTexturePos != loadedTextures.cend()) {
    map = loadedTexturePos->second;
  } else {
    const auto texturePath = FileUtils::extractPathToFile(mtlFilePath) + textureFileName;
    map = std::make_shared<Texture>(texturePath);
    loadedTextures.emplace(textureFileName, map);
  }

  return map;
}

} // namespace

void importMtl(const std::string& mtlFilePath,
               std::vector<MaterialPtr>& materials,
               std::unordered_map<std::string, std::size_t>& materialCorrespIndices) {
  std::ifstream file(mtlFilePath, std::ios_base::in | std::ios_base::binary);

  auto standardMaterial = std::make_unique<MaterialStandard>();
  auto cookTorranceMaterial = std::make_unique<MaterialCookTorrance>();

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
        const TexturePtr& map = std::move(loadTexture(mtlFilePath, nextValue));

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
        standardMaterial->setBumpMap(std::move(loadTexture(mtlFilePath, nextValue)));
        isStandardMaterial = true;
      } else if (tag[0] == 'n') {
        if (tag[1] == 'o') {                           // Normal map [norm]
          cookTorranceMaterial->setNormalMap(std::move(loadTexture(mtlFilePath, nextValue)));
        } else if (tag[1] == 'e') {                    // New material [newmtl]
          materialCorrespIndices.emplace(nextValue, materialCorrespIndices.size());

          if (!isStandardMaterial && !isCookTorranceMaterial)
            continue;

          if (isCookTorranceMaterial)
            materials.emplace_back(std::move(cookTorranceMaterial));
          else
            materials.emplace_back(std::move(standardMaterial));

          cookTorranceMaterial = std::make_unique<MaterialCookTorrance>();
          standardMaterial = std::make_unique<MaterialStandard>();

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

} // namespace MtlLoader

} // namespace Raz
