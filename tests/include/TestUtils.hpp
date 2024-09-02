#pragma once

#ifndef RAZ_TESTUTILS_HPP
#define RAZ_TESTUTILS_HPP

#include <string>

namespace Raz { class Window; }

namespace TestUtils {

Raz::Window& getWindow();

bool executeLuaScript(const std::string& code);

} // namespace TestUtils

#endif // RAZ_TESTUTILS_HPP
