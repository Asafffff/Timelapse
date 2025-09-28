# Crash analysis

## Summary
- Investigated why the forked Timelapse build crashes MapleStory when injected while the upstream build does not.
- Identified that the form resources cannot be located at runtime because `MainForm.resx` was moved under `Application/MainForm/`, changing the embedded resource name so it no longer matches `Timelapse::MainForm`.
- The crash happens when `ComponentResourceManager` tries to resolve resources during `InitializeComponent` and raises `MissingManifestResourceException`, which aborts the process during DLL injection.

## Key findings
- `MainForm` still lives in the `Timelapse` namespace (`Timelapse/Application/MainForm/MainForm.h`), so `ComponentResourceManager` looks for a manifest named `Timelapse.MainForm.resources` when the form is instantiated (`Timelapse/Application/MainForm/Designer.cpp`).
- Moving `MainForm.resx` into the nested `Application/MainForm/` folder without updating its logical name causes MSBuild to embed it as `Timelapse.Application.MainForm.MainForm.resources` instead of `Timelapse.MainForm.resources`, so the lookup fails.
- The original repository keeps `MainForm.resx` next to the form source, so the manifest name matches and the resource lookup succeeds.

## Suggested fix
- Either move `MainForm.resx` back beside the form, or set an explicit `<LogicalName>` (`Timelapse.MainForm.resources`) for `Application/MainForm/MainForm.resx` in `Timelapse.vcxproj` so that the embedded resource name matches the form type.
