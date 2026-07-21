# penumbra-ui-backend — Next Steps

> A handoff pointer, not a spec — kept short deliberately. Refreshed at
> the end of each work session; supersedes its own previous contents
> rather than accumulating history (the individual gap/spec docs are the
> durable record).
> Last updated: 2026-07-21.

## What's actually blocking downstream right now

Nothing known. Full build + `penumbra_ui_backend_tests` clean (0 failures).

## Done this session (2026-07-21, third pass): per-state gradient + box-shadow wired end to end

`penumbra` (bumped to `89216b4`) and `lustre` (bumped to `db6d4db`) landed the
two primitives the previous pass was blocked on — `BoxStyle::GradientTopHovered/
GradientBottomHovered/GradientTopPressed/GradientBottomPressed` and
`BoxStyle::ShadowColor/ShadowBlurRadiusLogical` on the `penumbra` side,
`ResolvedStyle::ShadowColor/ShadowBlurRadiusLogical` plus `box-shadow: <color>
<length>` shorthand parsing on the `lustre` side. `.gitmodules`/pins bumped;
`vendor/lustre`'s own nested submodules (`libs/amanuensis`,
`libs/amanuensis/external/cimmerian`) needed `git submodule update --init
--recursive` run from inside `vendor/lustre` after the bump, since a plain
top-level `submodule update --init --recursive` from this repo's root
re-pins `vendor/penumbra`/`vendor/lustre` back to `.gitmodules`' old SHAs
instead of picking up the new ones — bump the submodule's own HEAD first,
*then* init its nested submodules from inside it.

All three items from the previous "incoming ask" are now wired:

1. **`MergeInto` bug (fixed previous pass, `StyleResolution.cpp`)**:
   `BackgroundGradientStart`/`BackgroundGradientEnd`/`MaxWidthLogical`/
   `TextOverflowMode` now merge field-by-field like every other property.
   This pass added `ShadowColor`/`ShadowBlurRadiusLogical` to the same list
   (new fields from the `lustre` bump above) so they don't develop the
   identical latent gap.
2. **Per-state gradient overlays** (`StyleApplier.cpp`'s `Apply()`): `:hover`/
   `:active` `background-gradient-start`/`-end` now write into
   `BoxStyle::GradientTopHovered`/`GradientBottomHovered`/`GradientTopPressed`/
   `GradientBottomPressed`, mirroring the existing `ColorBackgroundHovered`/
   `Pressed` overlay treatment. No `Disabled` variant, per the original ask
   (consumers fall back to a flat disabled color, not a disabled gradient).
3. **`box-shadow`** (`StyleApplier.cpp`'s `ApplyBoxStyle()`): resolved
   `ShadowColor`/`ShadowBlurRadiusLogical` now write into the matching
   `BoxStyle` fields, same pair-presence convention as the gradient fields.

New regression tests in `tests/LustreStyleApplierTests.cpp` (hover/active
gradient overlays reach `Box::Style`, absence leaves zero-alpha default;
box-shadow reaches `Box::Style`, absence leaves zero blur radius) and
`tests/StyleWiringTests.cpp` (gradient survives a global+component merge).
Full build + `penumbra_ui_backend_tests` clean (0 failures).

**What this unblocks**: `pharos-proto`'s `GradientButton::Draw()`
(`gradient_button.cpp:8-43`) is now exactly what a plain `Button` styled via
Lustre's `:hover`/`:active` blocks plus a `box-shadow` property already does.
Deleting `GradientButton` and re-styling its panels via `.lustre` is
`pharos-proto`'s own follow-up, not this repo's — nothing further needed
here.

## Read first

- `docs/pseudo_class_plain_box_decision.md` — the open decision above.
- `docs/build_context_style_mismatch_gap.md` — a previous real
  `pharos-proto`-triggered bug in this same `StyleApplier`/`Walker`
  pairing, same shape of "compiles clean, fails silently downstream" risk
  worth keeping in mind when touching this file.
