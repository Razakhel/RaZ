#include "Catch.hpp"

#include "RaZ/Utils/FilePath.hpp"
#include "RaZ/Utils/FileUtils.hpp"

using namespace std::literals;

namespace {

const Raz::FilePath encodedFilePath(RAZ_TESTS_ROOT "assets/misc/ͳεs†_fílè_测试.τxt");
const Raz::FilePath asciiFilePath(RAZ_TESTS_ROOT "assets/misc/Test_file.txt");

} // namespace

TEST_CASE("FileUtils is readable") {
  CHECK(Raz::FileUtils::isReadable(encodedFilePath));
  CHECK(Raz::FileUtils::isReadable(asciiFilePath));
  CHECK_FALSE(Raz::FileUtils::isReadable("this_file_does_not_exist.txt"));
}

TEST_CASE("FileUtils read file") {
  std::string fileContent = Raz::FileUtils::readFile(encodedFilePath);
  CHECK(fileContent.size() == 22); // This doesn't represent the actual character count due to the encoding
  CHECK(fileContent == "НΣļlõ ωθяŁĐ!\n");

  fileContent = Raz::FileUtils::readFile(asciiFilePath);
  CHECK(fileContent.size() == 13);
  CHECK(fileContent == "Hello world!\n");
}
