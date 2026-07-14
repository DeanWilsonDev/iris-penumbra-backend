# iris-penumbra-backend

The Penumbra backend for [Iris](https://github.com/DeanWilsonDev/iris): the code that walks a
parsed, props-resolved `IrisComponent` IR tree and builds a real
[Penumbra](https://github.com/DeanWilsonDev/penumbra-proto) widget tree from it, via Penumbra's
fluent `Builder` API (`Box::Builder`, `Label::Builder`, etc.).

## Why this is a separate repo

Iris's core (preprocessor + IR + runtime library) is backend-agnostic by design — it's meant to
support more than one backend over time (Penumbra now, an Umbra Engine/Nyx backend deferred).
Penumbra is a general-purpose retained-mode widget library with no inherent reason to know Iris
exists either. Neither should have to pull in the other's build just to compile on its own, so
the code that bridges them — this repo — is its own thing: it vendors both `iris` and
`penumbra-proto` as git submodules and depends on both, while neither of them depends on it or
on each other.

## Status

This is Stage 2 of Iris's roadmap (see `iris`'s `docs/iris_handoff.md` §6). The dependency
wiring is in place (`CMakeLists.txt` links an `iris_penumbra_backend` target against both `iris`
and `penumbra`), but the walker itself has no sources yet — it needs `IrisProps`'s concrete
runtime representation decided first (how heterogeneous prop values: strings, ints, event
lambdas, are actually stored on an `IrisComponent` node), which `iris`'s own spec
(`docs/iris_core_spec.md` §2.5) deliberately leaves open rather than inventing an answer as a
side effect of unrelated work.

## Build

```sh
git submodule update --init --recursive
cmake -S . -B build
cmake --build build
```
