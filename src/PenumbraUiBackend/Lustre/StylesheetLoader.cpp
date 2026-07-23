#include "PenumbraUiBackend/Lustre/StylesheetLoader.h"

#include "Lustre/Parser.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace PenumbraUiBackend::Lustre {

::Lustre::Stylesheet LoadStylesheetFromFile(const char* Path, const char* LabelForErrors) {
    std::ifstream file(Path);
    if (!file) {
        std::fprintf(stderr, "%s: could not open %s\n", LabelForErrors, Path);
        return ::Lustre::Stylesheet{};
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    // Parser only stores a non-owning std::string_view over Source
    // (Lustre/Parser.h) -- binding it directly to buffer.str()'s temporary
    // would leave the parser holding a dangling view the instant this
    // constructor call's full-expression ends, before Parse() runs. `source`
    // outlives both.
    const std::string     source = buffer.str();
    ::Lustre::Parser      parser(source, Path);
    ::Lustre::ParseResult result = parser.Parse();
    if (!result.Errors.empty()) {
        std::fprintf(stderr, "%s: %s failed to parse:\n", LabelForErrors, Path);
        for (const auto& error : result.Errors) {
            std::fprintf(stderr, "  %s\n", error.Message.c_str());
        }
    }

    // Parser::Parse() always sets Sheet -- even a file with errors gets the
    // (possibly partial) tree back rather than nullopt, so a caller can see
    // every error at once. has_value() is only false if Parser's contract
    // ever changes; Stylesheet{} is the same fallback the no-Sheet-value
    // and can't-open-file paths above already use.
    return result.Sheet.has_value() ? std::move(*result.Sheet) : ::Lustre::Stylesheet{};
}

} // namespace PenumbraUiBackend::Lustre
