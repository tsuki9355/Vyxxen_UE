---
last_mapped_commit: 97fba1f2e8fc0676f4f28f5e659f174a01754612
last_mapped_at: 2026-10-07
---
# Technology Stack

**Analysis Date:** 2026-10-07

## Languages

**Primary:**
- Unreal C++ gameplay module in `Source/Vyxxen/`; `Source/Vyxxen/Public/PlayerShip.h` declares a reflected `AActor`, and `Source/Vyxxen/Private/PlayerShip.cpp` implements constructor, BeginPlay, and Tick scaffolding.
- Blueprint assets are present at `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Scenery/BP_level1_scenery.uasset`. Binary graph contents, inheritance, and executable behavior are unverified.
- C++ language standard is not explicitly pinned in `Source/Vyxxen/Vyxxen.Build.cs` or either `Source/*.Target.cs`; use the selected engine/toolchain defaults until verified.

**Secondary:**
- C# UnrealBuildTool declarations in `Source/Vyxxen/Vyxxen.Build.cs`, `Source/Vyxxen.Target.cs`, and `Source/VyxxenEditor.Target.cs`; no standalone C# application is declared.
- Unreal INI configuration in `Config/DefaultEngine.ini`, `Config/DefaultGame.ini`, `Config/DefaultInput.ini`, and `Config/DefaultEditor.ini`.

## Runtime

**Environment:**
- Unreal Engine association `5.8` in `Vyxxen.uproject`; exact installed engine revision is not established by that association.
- One runtime module, `Vyxxen`, loads at `Default` phase in `Vyxxen.uproject`; registration uses `IMPLEMENT_PRIMARY_GAME_MODULE` in `Source/Vyxxen/Vyxxen.cpp`.
- Local Win64 editor module artifacts exist in `Binaries/Win64/`; presence does not prove they match current source or run successfully.

**Package Manager:**
- UnrealBuildTool resolves engine module dependencies through `Source/Vyxxen/Vyxxen.Build.cs`; no application package manager or lockfile is detected in the repository root.
- Engine/plugin association is declared in `Vyxxen.uproject`, without an independent dependency lockfile.

## Frameworks

**Core:**
- Unreal Engine 5.8 association: actors, reflection, rendering, assets, and editor integration (`Vyxxen.uproject`, `Source/Vyxxen/Public/PlayerShip.h`).
- Enhanced Input classes are configured as default player input and input component in `Config/DefaultInput.ini`. `EnhancedInput` is not an explicit native dependency in `Source/Vyxxen/Vyxxen.Build.cs`.
- CommonUI settings exist in `Config/DefaultEngine.ini`; explicit plugin enablement and native usage are not demonstrated by `Vyxxen.uproject` or `Source/Vyxxen/Vyxxen.Build.cs`.

**Testing:**
- No project-authored test framework, automation tests, or test target detected in `Source/`; `Source/Vyxxen.Target.cs` and `Source/VyxxenEditor.Target.cs` define Game and Editor targets only.
- Engine test infrastructure availability is not evidence of project tests; binary content in `Content/` remains uninspected.

**Build/Dev:**
- UnrealBuildTool targets select `BuildSettingsVersion.V7` in `Source/Vyxxen.Target.cs` and `Source/VyxxenEditor.Target.cs`.
- Explicit/shared precompiled headers are selected in `Source/Vyxxen/Vyxxen.Build.cs`.
- `.vsconfig` requests Visual Studio native game/desktop workloads, Unreal IDE/debugger components, MSVC 14.50.18.0 components, LLVM Clang, Windows SDK 22621, and .NET Framework 4.6.2 targeting pack. These are declared component requirements, not an installation inventory.
- Generated IDE solutions exist at `Vyxxen.sln`, `Vyxxen.slnx`, `Automation_Vyxxen.sln`, and `Automation_Vyxxen.slnx`; use `Source/` declarations as build configuration authority.

## Key Dependencies

**Critical:**
- `Core`, `CoreUObject`, `Engine`, and `InputCore` are public native module dependencies in `Source/Vyxxen/Vyxxen.Build.cs`; versions follow the selected Unreal engine.
- `Engine` is also listed under the runtime module's additional dependencies in `Vyxxen.uproject`.
- `Content/Levels/MainGame.umap` is the configured startup/game map in `Config/DefaultEngine.ini`; world contents and map-level class overrides are unverified.

**Infrastructure:**
- `ModelingToolsEditorMode` is explicitly enabled for Editor targets only in `Vyxxen.uproject`.
- `Slate`, `SlateCore`, `OnlineSubsystem`, and Steam are mentioned only in comments in `Source/Vyxxen/Vyxxen.Build.cs`; do not treat them as active declared dependencies.
- No project `Plugins/` directory or separate third-party SDK manifest is detected alongside `Vyxxen.uproject`.

## Configuration

**Environment:**
- Configure project behavior through `Config/Default*.ini` and engine/plugin selection through `Vyxxen.uproject`; no application environment-variable contract is detected in `Source/`.
- `Config/DefaultEngine.ini` selects desktop/maximum graphics, DX12 with SM6 on Windows, Vulkan SM6 for Linux, and Metal SM6 for Mac. These settings do not verify builds on those platforms.
- `Config/DefaultEngine.ini` enables virtual shadows, mesh distance fields, ray tracing, Substrate, and GI/reflection method value 1; static lighting is disabled. Runtime rendering and hardware compatibility are not tested by this map.
- `Config/DefaultInput.ini` configures device axes, mouse capture, and Enhanced Input defaults; source gameplay does not implement input bindings in `Source/Vyxxen/Private/PlayerShip.cpp`.
- `Config/DefaultEditor.ini` holds asset preview profiles; treat these as editor configuration, not gameplay state.
- `Config/DefaultEngine.ini` contains an Android development file-server authentication field; omit its value from documentation and logs.

**Build:**
- Use `Source/Vyxxen.Target.cs` for Game target configuration, `Source/VyxxenEditor.Target.cs` for Editor target configuration, and `Source/Vyxxen/Vyxxen.Build.cs` for module dependencies.
- `.gitignore` excludes `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, and generated `.sln` files; `.gitattributes` requests automatic text normalization and does not configure Git LFS.

## Platform Requirements

**Development:**
- Use an Unreal installation compatible with the `5.8` association in `Vyxxen.uproject` and a compatible native toolchain matching `.vsconfig` requirements.
- Windows editor build artifacts are present at `Binaries/Win64/UnrealEditor-Vyxxen.dll`; no build or editor launch is performed for this analysis.
- No repository project skills are detected under `.codex/skills/` or `.agents/skills/`, and `.planning/config.json` is absent.

**Production:**
- Native desktop game is the configured target (`Source/Vyxxen.Target.cs`, `Config/DefaultEngine.ini`). Shipping packaging, distribution/store destination, dedicated server, and deployment pipeline are not detected.
- Validate packaging and binary asset references in Unreal before assuming `Content/Levels/MainGame.umap` is a shippable playable experience.

---

*Stack analysis: 2026-10-07*
