#include "Catch.hpp"

#include "RaZ/Utils/FilePath.hpp"

#include <fstream>

namespace {

const std::string fileName = "ͳεs†_fílè_测试.τxt";
const Raz::FilePath filePath(RAZ_TESTS_ROOT + "assets/misc/"s + fileName);

} // namespace

TEST_CASE("FilePath constructors") {
  const Raz::FilePath pathChar("spêçïàl/fílè/paτh");
  const Raz::FilePath pathString("spêçïàl/fílè/paτh"s);
  const Raz::FilePath pathStringView("spêçïàl/fílè/paτh"sv);

  CHECK(pathChar == pathString);
  CHECK(pathString == pathStringView);
}

TEST_CASE("FilePath open special chars") {
  std::ifstream file(filePath.getPathStr());

  REQUIRE(file.is_open());

  std::string fileContent;
  std::getline(file, fileContent);

  CHECK(fileContent.size() == 21); // This doesn't represent the actual character count due to the encoding
  CHECK(fileContent == "НΣļlõ ωθяŁĐ!");
}

TEST_CASE("FilePath save special chars") {
  const Raz::FilePath path(fileName);

  {
    std::ofstream file(path.getPathStr());
    file << "НΣļlõ ωθяŁĐ!";
  }

  std::ifstream file(path.getPathStr());

  REQUIRE(file.is_open());

  std::string fileContent;
  std::getline(file, fileContent);

  CHECK(fileContent == "НΣļlõ ωθяŁĐ!");
}

TEST_CASE("FilePath concatenation") {
  const Raz::FilePath testPath = "tèstPâth";

  // operator+(char*)
  {
    const char* str = "_chär_";

    std::string concat = str + testPath;
    CHECK(concat == "_chär_tèstPâth");

    concat = testPath + str;
    CHECK(concat == "tèstPâth_chär_");
  }

  // operator+(wchar_t*)
  {
    const wchar_t* str = L"_chär_";

    std::wstring concat = str + testPath;
    CHECK(concat == L"_chär_tèstPâth");

    concat = testPath + str;
    CHECK(concat == L"tèstPâth_chär_");
  }

  // operator+(std::string)
  {
    const std::string str = "_strïng_";

    std::string concat = str + testPath;
    CHECK(concat == "_strïng_tèstPâth");

    concat = testPath + str;
    CHECK(concat == "tèstPâth_strïng_");
  }

  // operator+(std::wstring)
  {
    const std::wstring str = L"_strïng_";

    std::wstring concat = str + testPath;
    CHECK(concat == L"_strïng_tèstPâth");

    concat = testPath + str;
    CHECK(concat == L"tèstPâth_strïng_");
  }

  // operator<<(std::ostream)
  {
    std::stringstream stream;
    stream << testPath;

    CHECK(stream.str() == "tèstPâth");
  }

  // operator<<(std::wostream)
  {
    std::wstringstream stream;
    stream << testPath;

    CHECK(stream.str() == L"tèstPâth");
  }
}

TEST_CASE("FilePath utilities") {
  CHECK(filePath.recoverPathToFile() == (RAZ_TESTS_ROOT + "assets/misc/"s));
  CHECK(filePath.recoverFileName(/* true */) == "ͳεs†_fílè_测试.τxt");
  CHECK(filePath.recoverFileName(false) == "ͳεs†_fílè_测试");
  CHECK(filePath.recoverExtension() == "τxt");

  const std::string absoluteTestPath = "/path/to/fïlè.êxt";
  CHECK(Raz::FilePath::recoverPathToFile(absoluteTestPath) == "/path/to/");
  CHECK(Raz::FilePath::recoverFileName(absoluteTestPath /*, true */) == "fïlè.êxt");
  CHECK(Raz::FilePath::recoverFileName(absoluteTestPath, false) == "fïlè");
  CHECK(Raz::FilePath::recoverExtension(absoluteTestPath) == "êxt");

  const std::wstring testFileNameExt = L"fïlè.êxt";
  CHECK(Raz::FilePath::recoverPathToFile(testFileNameExt) == L"");
  CHECK(Raz::FilePath::recoverFileName(testFileNameExt /*, true */) == L"fïlè.êxt");
  CHECK(Raz::FilePath::recoverFileName(testFileNameExt, false) == L"fïlè");
  CHECK(Raz::FilePath::recoverExtension(testFileNameExt) == L"êxt");

  const std::string_view relativeTestPath = "..\\path\\to\\fïlè.êxt";
  CHECK(Raz::FilePath::recoverPathToFile(relativeTestPath) == "..\\path\\to\\");
  CHECK(Raz::FilePath::recoverFileName(relativeTestPath /*, true */) == "fïlè.êxt");
  CHECK(Raz::FilePath::recoverFileName(relativeTestPath, false) == "fïlè");
  CHECK(Raz::FilePath::recoverExtension(relativeTestPath) == "êxt");

  const std::wstring_view testFileNameNoExt = L"fïlè";
  CHECK(Raz::FilePath::recoverPathToFile(testFileNameNoExt) == L"");
  CHECK(Raz::FilePath::recoverFileName(testFileNameNoExt /*, true */) == L"fïlè");
  CHECK(Raz::FilePath::recoverFileName(testFileNameNoExt, false) == L"fïlè");
  CHECK(Raz::FilePath::recoverExtension(testFileNameNoExt) == L"");

  const std::string mixedSeparatorsTestPath = "\\longer/path\\to/fïlè.êxt";
  CHECK(Raz::FilePath::recoverPathToFile(mixedSeparatorsTestPath) == "\\longer/path\\to/");
  CHECK(Raz::FilePath::recoverFileName(mixedSeparatorsTestPath /*, true */) == "fïlè.êxt");
  CHECK(Raz::FilePath::recoverFileName(mixedSeparatorsTestPath, false) == "fïlè");
  CHECK(Raz::FilePath::recoverExtension(mixedSeparatorsTestPath) == "êxt");
}
