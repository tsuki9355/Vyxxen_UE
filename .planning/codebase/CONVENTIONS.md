---
last_mapped_commit: 97fba1f2e8fc0676f4f28f5e659f174a01754612
last_mapped_at: 2026-10-07
---
# Coding Conventions

**Analysis Date:** 2026-10-07

## Naming Patterns

**Files:**
- Use matching PascalCase C++ class stems, with declarations in `Source/Vyxxen/Public/PlayerShip.h` and definitions in `Source/Vyxxen/Private/PlayerShip.cpp`.
- Keep Unreal build rule suffixes: `Source/Vyxxen/Vyxxen.Build.cs`, `Source/Vyxxen.Target.cs`, and `Source/VyxxenEditor.Target.cs`.
- Asset names mix prefixes and hyphenated imported names: `Content/Scenery/BP_level1_scenery.uasset`, `Content/Scenery/ground-tile/M_ground-tile.uasset`, and `Content/Units/PlayerShip/vyxxen-ship.uasset`. No uniform asset naming validator is detected.

**Functions:**
- Use PascalCase Unreal lifecycle names and explicit overrides, as in `BeginPlay()` and `Tick(float DeltaTime)` in `Source/Vyxxen/Public/PlayerShip.h`.

**Variables:**
- Match the visible Unreal API style: `DeltaTime` and `PrimaryActorTick.bCanEverTick` in `Source/Vyxxen/Private/PlayerShip.cpp`.
- No project-owned member variable naming pattern can be established from the scaffold in `Source/Vyxxen/Public/PlayerShip.h`.

**Types:**
- Use the Unreal actor prefix and module export macro: `class VYXXEN_API APlayerShip : public AActor` in `Source/Vyxxen/Public/PlayerShip.h`.
- Retain reflection declarations `UCLASS()` and `GENERATED_BODY()` in reflected headers such as `Source/Vyxxen/Public/PlayerShip.h`.

## Code Style

**Formatting:**
- Follow the visible tab indentation and opening braces on separate lines in `Source/Vyxxen/Private/PlayerShip.cpp` and `Source/Vyxxen/Vyxxen.Build.cs`.
- Whitespace is not fully normalized: the scaffold contains trailing tabs, extra blank lines, and mixed spacing in `Source/Vyxxen/Public/PlayerShip.h` and `Source/Vyxxen/Vyxxen.cpp`.
- No dedicated formatter or editor style configuration is detected alongside `Vyxxen.uproject`; `.gitattributes` sets `* text=auto`.

**Linting:**
- No dedicated lint configuration is detected alongside `Vyxxen.uproject` or in `Source/`.
- `Source/Vyxxen.Target.cs` and `Source/VyxxenEditor.Target.cs` select `BuildSettingsVersion.V7`; do not interpret this as evidence of a passing build or an enforced project lint gate.

## Import Organization

**Order:**
1. In headers, include foundational Unreal types first: `CoreMinimal.h`, then `GameFramework/Actor.h` in `Source/Vyxxen/Public/PlayerShip.h`.
2. Keep the generated reflection header last: `PlayerShip.generated.h` in `Source/Vyxxen/Public/PlayerShip.h`.
3. In implementations, include the corresponding header first; `Source/Vyxxen/Vyxxen.cpp` then includes `Modules/ModuleManager.h`.

**Path Aliases:**
- No application path aliases are detected. Includes use module-visible header names such as `PlayerShip.h` in `Source/Vyxxen/Private/PlayerShip.cpp`.
- Declare module dependencies in `Source/Vyxxen/Vyxxen.Build.cs`; the current public dependencies are `Core`, `CoreUObject`, `Engine`, and `InputCore`.

## Error Handling

**Patterns:**
- No project error-handling, assertions, `ensure`, `check`, or exception pattern is detected in `Source/Vyxxen/`.
- `Source/Vyxxen/Private/PlayerShip.cpp` only sets ticking and delegates lifecycle methods to the base class; it supplies no substantive failure path to copy.

## Logging

**Framework:** No project logging category or log statements detected in `Source/Vyxxen/`.

**Patterns:**
- `Source/Vyxxen/Vyxxen.cpp` registers the default game module and does not define a shared logger. Establish any future category explicitly rather than assuming one exists.

## Comments

**When to Comment:**
- The comments in `Source/Vyxxen/Public/PlayerShip.h` and `Source/Vyxxen/Private/PlayerShip.cpp` explain generated lifecycle scaffolding; no project-specific rationale convention is established.
- `Source/Vyxxen/Vyxxen.Build.cs` includes commented suggestions for optional Slate and online subsystem dependencies; these are instructions in comments, not active dependencies.

**JSDoc/TSDoc:**
- Not applicable to the native sources under `Source/Vyxxen/`; no custom API documentation markup is detected there.

## Function Design

**Size:** The functions in `Source/Vyxxen/Private/PlayerShip.cpp` contain only one executable statement each. No numerical function-size rule is configured.

**Parameters:** Preserve engine signatures such as `Tick(float DeltaTime)` in `Source/Vyxxen/Public/PlayerShip.h`.

**Return Values:** Existing lifecycle hooks return `void`; no custom result/error abstraction exists in `Source/Vyxxen/Public/PlayerShip.h`.

## Module Design

**Exports:**
- Export reflected actor classes with `VYXXEN_API` as in `Source/Vyxxen/Public/PlayerShip.h`.
- Keep public declarations in `Source/Vyxxen/Public/` and implementations in `Source/Vyxxen/Private/`.
- `Source/Vyxxen/Vyxxen.cpp` uses `IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Vyxxen, "Vyxxen")`; `Vyxxen.uproject` declares a single runtime module.

**Barrel Files:**
- No barrel pattern is present. `Source/Vyxxen/Vyxxen.h` is a minimal module header that includes `CoreMinimal.h`.

**Evidence boundaries:**
- No repository-local skill folders or `AGENTS.md` are detected alongside `Vyxxen.uproject`; conventions here derive from the inspected files.
- The current working-tree `Source/` is untracked; describe its present contents without assuming they are available in a clean checkout.
- Binary Blueprint internals, parent classes, and editor wiring in `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Levels/MainGame.umap` are unverified.

---

*Convention analysis: 2026-10-07*
