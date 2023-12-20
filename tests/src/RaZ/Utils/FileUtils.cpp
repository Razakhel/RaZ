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

TEST_CASE("FileUtils read file to array") {
  {
    const std::vector<unsigned char> fileContent = Raz::FileUtils::readFileToArray(encodedFilePath);
    CHECK(fileContent.size() == 22);
    CHECK(fileContent == std::vector<unsigned char>({ 0xD0, 0x9D, 0xCE, 0xA3, 0xC4, 0xBC, 'l', 0xC3, 0xB5, ' ',
                                                  //   \     /     \     /     \     /     |    \     /
                                                  //      Н           Σ           ļ        l       õ
                                                      0xCF, 0x89, 0xCE, 0xB8, 0xD1, 0x8F, 0xC5, 0x81, 0xC4, 0x90, '!', '\n' }));
                                                  //   \     /     \     /     \     /     \     /     \     /     |    \/
                                                  //      ω           θ           я           Ł           Đ        !    \n
  }

  {
    const std::vector<unsigned char> fileContent = Raz::FileUtils::readFileToArray(asciiFilePath);
    CHECK(fileContent.size() == 13);
    CHECK(fileContent == std::vector<unsigned char>({ 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!', '\n' }));
  }
}

TEST_CASE("FileUtils read file to string") {
  {
    const std::string fileContent = Raz::FileUtils::readFileToString(encodedFilePath);
    CHECK(fileContent.size() == 22); // This doesn't represent the actual character count due to the encoding
    CHECK(fileContent == "НΣļlõ ωθяŁĐ!\n");
  }

  {
    const std::string fileContent = Raz::FileUtils::readFileToString(asciiFilePath);
    CHECK(fileContent.size() == 13);
    CHECK(fileContent == "Hello world!\n");
  }
}
