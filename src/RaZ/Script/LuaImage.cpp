#include "RaZ/Data/Image.hpp"
#include "RaZ/Math/Vector.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerImageTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Image> image = state.new_usertype<Image>("Image",
                                                           sol::constructors<Image(),
                                                                             Image(ImageColorspace),
                                                                             Image(ImageColorspace, ImageDataType),
                                                                             Image(unsigned int, unsigned int, ImageColorspace),
                                                                             Image(unsigned int, unsigned int, ImageColorspace, ImageDataType)>());
    image["getWidth"]          = &Image::getWidth;
    image["getHeight"]         = &Image::getHeight;
    image["getColorspace"]     = &Image::getColorspace;
    image["getDataType"]       = &Image::getDataType;
    image["getChannelCount"]   = &Image::getChannelCount;
    image["isEmpty"]           = &Image::isEmpty;
    image["recoverByteValue"]  = &Image::recoverByteValue;
    image["recoverFloatValue"] = &Image::recoverFloatValue;
    image["recoverBytePixel"]  = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<uint8_t>);
    image["recoverFloatPixel"] = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<float>);
    image["recoverVec2bPixel"] = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<uint8_t, 2>);
    image["recoverVec3bPixel"] = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<uint8_t, 3>);
    image["recoverVec4bPixel"] = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<uint8_t, 4>);
    image["recoverVec2fPixel"] = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<float, 2>);
    image["recoverVec3fPixel"] = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<float, 3>);
    image["recoverVec4fPixel"] = PickOverload<std::size_t, std::size_t>(&Image::recoverPixel<float, 4>);
    image["setByteValue"]      = &Image::setByteValue;
    image["setFloatValue"]     = &Image::setFloatValue;
    image["setBytePixel"]      = PickOverload<std::size_t, std::size_t, uint8_t>(&Image::setPixel<uint8_t>);
    image["setFloatPixel"]     = PickOverload<std::size_t, std::size_t, float>(&Image::setPixel<float>);
    image["setVec2bPixel"]     = PickOverload<std::size_t, std::size_t, const Vec2b&>(&Image::setPixel<uint8_t, 2>);
    image["setVec3bPixel"]     = PickOverload<std::size_t, std::size_t, const Vec3b&>(&Image::setPixel<uint8_t, 3>);
    image["setVec4bPixel"]     = PickOverload<std::size_t, std::size_t, const Vec4b&>(&Image::setPixel<uint8_t, 4>);
    image["setVec2fPixel"]     = PickOverload<std::size_t, std::size_t, const Vec2f&>(&Image::setPixel<float, 2>);
    image["setVec3fPixel"]     = PickOverload<std::size_t, std::size_t, const Vec3f&>(&Image::setPixel<float, 3>);
    image["setVec4fPixel"]     = PickOverload<std::size_t, std::size_t, const Vec4f&>(&Image::setPixel<float, 4>);

    state.new_enum<ImageColorspace>("ImageColorspace", {
      { "GRAY",       ImageColorspace::GRAY },
      { "GRAY_ALPHA", ImageColorspace::GRAY_ALPHA },
      { "RGB",        ImageColorspace::RGB },
      { "RGBA",       ImageColorspace::RGBA },
      { "SRGB",       ImageColorspace::SRGB },
      { "SRGBA",      ImageColorspace::SRGBA }
    });

    state.new_enum<ImageDataType>("ImageDataType", {
      { "BYTE",  ImageDataType::BYTE },
      { "FLOAT", ImageDataType::FLOAT }
    });
  }
}

} // namespace Raz
