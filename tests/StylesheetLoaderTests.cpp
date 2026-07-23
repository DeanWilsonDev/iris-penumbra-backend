#include "PenumbraUiBackend/Lustre/StylesheetLoader.h"

#include <cstdio>
#include <string>
#include <unistd.h>

extern int Failures; // defined in WalkerTests.cpp

namespace {

void Expect(bool Condition, const std::string& Description) {
    if (Condition) {
        std::printf("[PASS] %s\n", Description.c_str());
    } else {
        std::printf("[FAIL] %s\n", Description.c_str());
        ++Failures;
    }
}

using PenumbraUiBackend::Lustre::LoadStylesheetFromFile;

// A real temp file rather than an in-memory stream, since
// LoadStylesheetFromFile's whole job is the disk-reading step itself.
std::string WriteTempLustreFile(const char* Contents) {
    char      path[] = "/tmp/penumbra_ui_backend_stylesheet_loader_test_XXXXXX";
    const int fd = mkstemp(path);
    write(fd, Contents, std::string(Contents).size());
    close(fd);
    return path;
}

void TestValidFileParsesIntoAPopulatedStylesheet() {
    const std::string path = WriteTempLustreFile(".foo { background-color: #FF0000; }");

    const ::Lustre::Stylesheet sheet = LoadStylesheetFromFile(path.c_str(), "TestValidFileParsesIntoAPopulatedStylesheet");

    Expect(!sheet.Rules.empty(), "a valid .lustre file parses into a Stylesheet with at least one rule");

    std::remove(path.c_str());
}

void TestMissingFileReturnsAnEmptyStylesheet() {
    const ::Lustre::Stylesheet sheet =
        LoadStylesheetFromFile("/nonexistent/path/does-not-exist.lustre", "TestMissingFileReturnsAnEmptyStylesheet");

    Expect(sheet.Rules.empty(), "a missing file returns a default-constructed, empty Stylesheet rather than crashing");
}

void TestFileWithParseErrorsStillReturnsWithoutCrashing() {
    // Duplicate-selector rule blocks are one of Parser's own diagnosed
    // syntax errors (Lustre/Parser.h) -- Parser::Parse() hands back the
    // (possibly partial) tree it built regardless, so a caller can report
    // every error at once; LoadStylesheetFromFile mirrors that, it doesn't
    // discard the Sheet just because Errors is non-empty.
    const std::string path =
        WriteTempLustreFile(".foo { background-color: #FF0000; } .foo { background-color: #00FF00; }");

    const ::Lustre::Stylesheet sheet =
        LoadStylesheetFromFile(path.c_str(), "TestFileWithParseErrorsStillReturnsWithoutCrashing");

    Expect(sheet.Rules.size() == 2, "a file with a parse error still returns the rules the parser did manage to build");

    std::remove(path.c_str());
}

} // namespace

void RunStylesheetLoaderTests() {
    TestValidFileParsesIntoAPopulatedStylesheet();
    TestMissingFileReturnsAnEmptyStylesheet();
    TestFileWithParseErrorsStillReturnsWithoutCrashing();
}
