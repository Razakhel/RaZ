#include "Catch.hpp"

#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"

using namespace std::literals;

namespace {

const Raz::FilePath filePath(RAZ_TESTS_ROOT "assets/misc/ͳεs†_fílè_测试.τxt");

} // namespace

TEST_CASE("FileUtils read file") {
  std::string fileContent = Raz::FileUtils::readFile(filePath);
  CHECK(fileContent.size() == 22); // This doesn't represent the actual character count due to the encoding
  CHECK(fileContent == "НΣļlõ ωθяŁĐ!\n");

  fileContent = Raz::FileUtils::readFile(RAZ_TESTS_ROOT "assets/shaders/basic.tese");
  CHECK(fileContent.size() == 105);
  CHECK(fileContent == "#version 400 core\n"
                       "\n"
                       "layout(triangles, equal_spacing, ccw) in;\n"
                       "\n"
                       "void main() {\n"
                       "  gl_Position = vec4(0.0);\n"
                       "}\n");
}
