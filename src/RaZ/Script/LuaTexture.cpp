#include "RaZ/Data/Color.hpp"
#include "RaZ/Data/Image.hpp"
#include "RaZ/Render/Texture.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerTextureTypes() {
  sol::state& state = getState();

  {
    sol::usertype<Texture> texture = state.new_usertype<Texture>("Texture", sol::no_constructor);
    texture["getIndex"]      = &Texture::getIndex;
    texture["getColorspace"] = &Texture::getColorspace;
    texture["getDataType"]   = &Texture::getDataType;
    texture["bind"]          = &Texture::bind;
    texture["unbind"]        = &Texture::unbind;
    texture["setFilter"]     = sol::overload(PickOverload<TextureFilter>(&Texture::setFilter),
                                             PickOverload<TextureFilter, TextureFilter>(&Texture::setFilter),
                                             PickOverload<TextureFilter, TextureFilter, TextureFilter>(&Texture::setFilter));
    texture["setWrapping"]   = &Texture::setWrapping;
    texture["setColorspace"] = sol::overload(PickOverload<TextureColorspace>(&Texture::setColorspace),
                                             PickOverload<TextureColorspace, TextureDataType>(&Texture::setColorspace));
  }

#if !defined(USE_OPENGL_ES)
  {
    sol::usertype<Texture1D> texture1D = state.new_usertype<Texture1D>("Texture1D",
                                                                       sol::constructors<Texture1D(TextureColorspace),
                                                                                         Texture1D(TextureColorspace, TextureDataType),
                                                                                         Texture1D(unsigned int, TextureColorspace),
                                                                                         Texture1D(unsigned int, TextureColorspace, TextureDataType),
                                                                                         Texture1D(const Color&),
                                                                                         Texture1D(const Color&, unsigned int)>(),
                                                                       sol::base_classes, sol::bases<Texture>());
    texture1D["getWidth"] = &Texture1D::getWidth;
    texture1D["create"]   = sol::overload(&Texture1D::create<TextureColorspace>,
                                          &Texture1D::create<TextureColorspace, TextureDataType>,
                                          &Texture1D::create<unsigned int, TextureColorspace>,
                                          &Texture1D::create<unsigned int, TextureColorspace, TextureDataType>,
                                          &Texture1D::create<const Color&>,
                                          &Texture1D::create<const Color&, unsigned int>);
    texture1D["resize"]   = &Texture1D::resize;
    texture1D["fill"]     = &Texture1D::fill;
  }
#endif

  {
    sol::usertype<Texture2D> texture2D = state.new_usertype<Texture2D>("Texture2D",
                                                                       sol::constructors<Texture2D(TextureColorspace),
                                                                                         Texture2D(TextureColorspace, TextureDataType),
                                                                                         Texture2D(unsigned int, unsigned int, TextureColorspace),
                                                                                         Texture2D(unsigned int, unsigned int, TextureColorspace,
                                                                                                   TextureDataType),
                                                                                         Texture2D(const Image&),
                                                                                         Texture2D(const Image&, bool),
                                                                                         Texture2D(const Color&),
                                                                                         Texture2D(const Color&, unsigned int),
                                                                                         Texture2D(const Color&, unsigned int, unsigned int)>(),
                                                                       sol::base_classes, sol::bases<Texture>());
    texture2D["getWidth"]     = &Texture2D::getWidth;
    texture2D["getHeight"]    = &Texture2D::getHeight;
    texture2D["create"]       = sol::overload(&Texture2D::create<TextureColorspace>,
                                              &Texture2D::create<TextureColorspace, TextureDataType>,
                                              &Texture2D::create<unsigned int, unsigned int, TextureColorspace>,
                                              &Texture2D::create<unsigned int, unsigned int, TextureColorspace, TextureDataType>,
                                              &Texture2D::create<const Image&>,
                                              &Texture2D::create<const Image&, bool>,
                                              &Texture2D::create<const Color&>,
                                              &Texture2D::create<const Color&, unsigned int>,
                                              &Texture2D::create<const Color&, unsigned int, unsigned int>);
    texture2D["resize"]       = &Texture2D::resize;
    texture2D["load"]         = sol::overload([] (Texture2D& t, const Image& img) { t.load(img); },
                                              PickOverload<const Image&, bool>(&Texture2D::load));
    texture2D["fill"]         = &Texture2D::fill;
#if !defined(USE_OPENGL_ES)
    texture2D["recoverImage"] = &Texture2D::recoverImage;
#endif
  }

  {
    sol::usertype<Texture3D> texture3D = state.new_usertype<Texture3D>("Texture3D",
                                                                       sol::constructors<Texture3D(TextureColorspace),
                                                                                         Texture3D(TextureColorspace, TextureDataType),
                                                                                         Texture3D(unsigned int, unsigned int, unsigned int, TextureColorspace),
                                                                                         Texture3D(unsigned int, unsigned int, unsigned int, TextureColorspace,
                                                                                                   TextureDataType),
                                                                                         Texture3D(const Color&),
                                                                                         Texture3D(const Color&, unsigned int),
                                                                                         Texture3D(const Color&, unsigned int, unsigned int),
                                                                                         Texture3D(const Color&, unsigned int, unsigned int, unsigned int)>(),
                                                                       sol::base_classes, sol::bases<Texture>());
    texture3D["getWidth"]  = &Texture3D::getWidth;
    texture3D["getHeight"] = &Texture3D::getHeight;
    texture3D["getDepth"]  = &Texture3D::getDepth;
    texture3D["create"]    = sol::overload(&Texture3D::create<TextureColorspace>,
                                           &Texture3D::create<TextureColorspace, TextureDataType>,
                                           &Texture3D::create<unsigned int, unsigned int, unsigned int, TextureColorspace>,
                                           &Texture3D::create<unsigned int, unsigned int, unsigned int, TextureColorspace, TextureDataType>,
                                           &Texture3D::create<const Color&>,
                                           &Texture3D::create<const Color&, unsigned int>,
                                           &Texture3D::create<const Color&, unsigned int, unsigned int>,
                                           &Texture3D::create<const Color&, unsigned int, unsigned int, unsigned int>);
    texture3D["resize"]    = &Texture3D::resize;
    texture3D["fill"]      = &Texture3D::fill;
  }

  state.new_enum<TextureColorspace>("TextureColorspace", {
    { "INVALID", TextureColorspace::INVALID },
    { "GRAY",    TextureColorspace::GRAY },
    { "RG",      TextureColorspace::RG },
    { "RGB",     TextureColorspace::RGB },
    { "RGBA",    TextureColorspace::RGBA },
    { "SRGB",    TextureColorspace::SRGB },
    { "SRGBA",   TextureColorspace::SRGBA },
    { "DEPTH",   TextureColorspace::DEPTH }
  });

  state.new_enum<TextureDataType>("TextureDataType", {
    { "BYTE",    TextureDataType::BYTE },
    { "FLOAT16", TextureDataType::FLOAT16 },
    { "FLOAT32", TextureDataType::FLOAT32 }
  });

  state.new_enum<TextureFilter>("TextureFilter", {
    { "NEAREST", TextureFilter::NEAREST },
    { "LINEAR",  TextureFilter::LINEAR }
  });

  state.new_enum<TextureWrapping>("TextureWrapping", {
    { "REPEAT", TextureWrapping::REPEAT },
    { "CLAMP",  TextureWrapping::CLAMP }
  });
}

} // namespace Raz
