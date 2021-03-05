#include "RaZ/Utils/BvhFormat.hpp"
#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/StrUtils.hpp"

#include <fstream>
#include <unordered_map>

namespace Raz {

namespace {

void loadJoint(std::ifstream& file, std::unordered_map<std::string, SkeletonJoint&>& joints, Skeleton& skeleton, SkeletonJoint& parentJoint) {
  std::string token;
  file >> token;

  if (token.front() == 'E') { // "End Site" declaration, stopping recursion
    std::getline(file, token); // "Site"
    std::getline(file, token); // Opening scope '{'

    file >> token;
    if (token.front() != 'O') // "OFFSET"
      throw std::invalid_argument("Error: Invalid BVH joint offset");

    Vec3f offset;
    file >> offset.x() >> offset.y() >> offset.z();
    std::getline(file, token);

    // TODO: use the last offset?

    file >> token;
    if (token.front() != '}')
      throw std::invalid_argument("Error: Invalid BVH joint closing scope");
    std::getline(file, token);

    return;
  }

  if (token.front() != 'J') // "JOINT"
    throw std::invalid_argument("Error: Invalid BVH joint declaration");

  file >> token;
  SkeletonJoint& currentJoint = joints.emplace(token, skeleton.addNode()).first->second;
  currentJoint.addParents(parentJoint);

  std::getline(file, token);

  file >> token;
  if (token.front() != '{')
    throw std::invalid_argument("Error: Invalid BVH joint opening scope");
  std::getline(file, token);

  file >> token;
  if (token.front() != 'O') // "OFFSET"
    throw std::invalid_argument("Error: Invalid BVH joint offset");

  Vec3f jointOffset;
  file >> jointOffset.x() >> jointOffset.y() >> jointOffset.z();
  currentJoint.setTranslation(jointOffset);
  std::getline(file, token);

  file >> token;
  if (token.front() != 'C') // "CHANNELS"
    throw std::invalid_argument("Error: Invalid BVH joint channels");

  file >> token;
  if (token.front() != '3')
    throw std::invalid_argument("Error: Invalid BVH joint channel count");
  std::getline(file, token);

  loadJoint(file, joints, skeleton, currentJoint);

  file >> token;
  if (token.front() != '}')
    throw std::invalid_argument("Error: Invalid BVH joint closing scope");
  std::getline(file, token);

  while (!file.eof()) {
    file >> std::ws; // Discarding all leading white spaces

    if (file.peek() == '}') // Reaching the end of scope, no other joint is declared on this level
      break;

    loadJoint(file, joints, skeleton, currentJoint);
  }
}

void loadSkeleton(std::ifstream& file, Skeleton& skeleton) {
  std::string token;

  file >> token;
  if (token != "HIERARCHY")
    throw std::invalid_argument("Error: Invalid BVH header");

  file >> token;
  if (token != "ROOT")
    throw std::invalid_argument("Error: Invalid BVH root joint");

  std::unordered_map<std::string, SkeletonJoint&> joints;

  file >> token;
  SkeletonJoint& rootJoint = joints.emplace(token, skeleton.addNode()).first->second;

  std::getline(file, token); // Skipping the rest of the line

  file >> token;
  if (token.front() != '{')
    throw std::invalid_argument("Error: Invalid BVH root joint opening scope");
  std::getline(file, token);

  file >> token;
  if (token.front() != 'O') // "OFFSET"
    throw std::invalid_argument("Error: Invalid BVH root joint offset");

  Vec3f rootPos;
  file >> rootPos.x() >> rootPos.y() >> rootPos.z();
  rootJoint.setTranslation(rootPos);
  std::getline(file, token);

  file >> token;
  if (token.front() != 'C') // "CHANNELS"
    throw std::invalid_argument("Error: Invalid BVH root joint channels");

  file >> token;
  if (token.front() != '6')
    throw std::invalid_argument("Error: Invalid BVH root joint channel count");
  std::getline(file, token);

  loadJoint(file, joints, skeleton, rootJoint);
  std::getline(file, token); // Root joint's closing scope
}

void loadAnimation(std::ifstream& file, Animation& animation, std::size_t jointCount) {
  std::string token;

  file >> token;
  if (token != "MOTION")
    throw std::invalid_argument("Error: Invalid BVH animation declaration");

  file >> token;
  if (token != "Frames:")
    throw std::invalid_argument("Error: Invalid BVH animation frame count declaration");

  file >> token;
  const int frameCount = std::stoi(token);
  if (frameCount <= 0)
    throw std::invalid_argument("Error: Invalid BVH animation frame count");
  std::getline(file, token);

  std::getline(file, token, ':');
  if (token != "Frame Time")
    throw std::invalid_argument("Error: Invalid BVH animation frame time declaration");
  file.ignore(1);

  file >> token;
  const float frameTime = std::stof(token);
  if (frameTime <= 0.f)
    throw std::invalid_argument("Error: Invalid BVH animation frame time");
  std::getline(file, token);

  for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
    Keyframe& keyframe = animation.addKeyframe();
    keyframe.setKeyTime(frameTime);

    // Loading the root transform
    {
      Vec3f rootPosition;

      file >> token;
      rootPosition.x() = std::stof(token);

      file >> token;
      rootPosition.y() = std::stof(token);

      file >> token;
      rootPosition.z() = std::stof(token);

      file >> token;
      Quaternionf rootRotation(Degreesf(std::stof(token)), Axis::Z);

      file >> token;
      rootRotation *= Quaternionf(Degreesf(std::stof(token)), Axis::X);

      file >> token;
      rootRotation *= Quaternionf(Degreesf(std::stof(token)), Axis::Y);

      keyframe.addJointTransform(rootRotation, rootPosition);
    }

    for (std::size_t transformIndex = 1; transformIndex < jointCount; ++transformIndex) {
      file >> token;
      Quaternionf rotation(Degreesf(std::stof(token)), Axis::Z);

      file >> token;
      rotation = Quaternionf(Degreesf(std::stof(token)), Axis::X) * rotation;

      file >> token;
      rotation = Quaternionf(Degreesf(std::stof(token)), Axis::Y) * rotation;

      keyframe.addJointTransform(rotation);
    }
  }
}

} // namespace

void BvhFormat::import(const FilePath& filePath) {
  std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  if (!file)
    throw std::invalid_argument("Error: Couldn't open the BVH file '" + filePath + "'");

  const std::string format = StrUtils::toLowercaseCopy(filePath.recoverExtension().toUtf8());

  if (format != "bvh")
    throw std::invalid_argument("Error: '" + filePath + "' doesn't have a .bvh extension");

  loadSkeleton(file, skeleton);
  loadAnimation(file, animation, skeleton.getNodeCount());
}

} // namespace Raz
