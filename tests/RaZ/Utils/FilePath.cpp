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
