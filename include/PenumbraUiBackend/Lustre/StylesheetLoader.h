#pragma once

#include "Lustre/Ast.h"

namespace PenumbraUiBackend::Lustre {

// Reads Path off disk and parses it as a .lustre stylesheet, doing exactly
// the ifstream/ostringstream/Parser::Parse() sequence every consumer of a
// standalone .lustre file used to hand-roll (docs/next_steps.md's "shared
// .lustre-file-loading helper" ask). If Path can't be opened, logs to
// stderr (prefixed with LabelForErrors, so the caller's own log output
// stays attributable) and returns a default-constructed, empty Stylesheet,
// matching every existing call site's own fallback. If it opens but fails
// to parse cleanly, also logs each Lustre::ParseError to stderr -- but
// still returns whatever (possibly partial) Stylesheet Parser::Parse()
// built, matching Parser's own "hand back the tree regardless of errors"
// contract (Lustre/Parser.h).

::Lustre::Stylesheet LoadStylesheetFromFile(const char* Path, const char* LabelForErrors);

} // namespace PenumbraUiBackend::Lustre
