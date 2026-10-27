# Phase 1: Controllable Arcade Flight - Pattern Map

**Mapped:** 2026-10-07
**Files analyzed:** 8 proposed implementation areas
**Analogs found:** 3 / 8 (two partial native lifecycle references and one exact module-config analog; binary assets are inspection targets only)

## File Classification

| New/Modified File | Role | Data Flow | Closest Analog | Match Quality |
|-------------------|------|-----------|----------------|---------------|
| `Source/Vyxxen/Public/PlayerShip.h` | controller / Pawn declaration | event-driven input and transform state | `Source/Vyxxen/Public/PlayerShip.h` | partial: tracked actor scaffold, not a Pawn |
| `Source/Vyxxen/Private/PlayerShip.cpp` | controller / Pawn behavior | event-driven input and transform | `Source/Vyxxen/Private/PlayerShip.cpp` | partial: lifecycle/tick only |
| `Source/Vyxxen/Public/FlightMovementComponent.h` | component | event-driven requests to transform | `Source/Vyxxen/Public/PlayerShip.h` | no component analog; actor declaration is only a weak Unreal C++ convention reference |
| `Source/Vyxxen/Private/FlightMovementComponent.cpp` | component | event-driven requests to transform | `Source/Vyxxen/Private/PlayerShip.cpp` | no component analog; actor lifecycle is only a weak reference |
| `Source/Vyxxen/Vyxxen.Build.cs` | config | build-time dependency resolution | `Source/Vyxxen/Vyxxen.Build.cs` | exact for module dependency edits |
| `Content/Input/<Steer, Climb, Slow actions and flight Mapping Context>` | config / authored input data | event-driven device-to-action mapping | None found in checked-in project content | none |
| `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` (inspect/edit only if needed) | config / authored actor integration | event-driven possession and input setup | Same asset; internal graph not readable here | asset exists; behavior unverified |
| `Content/Levels/MainGame.umap` (inspect/edit only if needed) | config / authored level integration | event-driven startup and possession | Same map; internal actors not readable here | asset exists; behavior unverified |

The movement component and `Content/Input/` path are proposed by `01-RESEARCH.md`, not observed existing files. The Blueprint and map are tracked binary assets; do not infer their parent classes, graphs, component tree, possession, input context, view, effective defaults, start height, or scenery height band from their names. Open them in Unreal Editor before selecting the controlled Pawn path or changing asset wiring. The research proposes one controlled Pawn with one movement owner, but the existing native `APlayerShip` currently derives from `AActor`; keep any inheritance/asset plan conditional on editor inspection. Camera movement belongs to Phase 2, so no camera file is assigned here.

## Pattern Assignments

### `Source/Vyxxen/Public/PlayerShip.h` (controller / Pawn declaration, event-driven)

**Analog:** `Source/Vyxxen/Public/PlayerShip.h` (tracked; `git ls-files` verified)

**Reflection, export, and lifecycle declaration** (lines 5-24):

```cpp
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerShip.generated.h"

UCLASS()
class VYXXEN_API APlayerShip : public AActor
{
	GENERATED_BODY()

public:
	APlayerShip();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
```

Copy the module export/reflection and public-header conventions, and retain generated-header ordering. This is not a possessable-Pawn pattern: the current base class is `AActor`, and no Enhanced Input action declarations, movement properties, components, or possession hooks exist here. First inspect the Blueprint/map path; then either evolve the correct controlled type or preserve its existing parent and add the approved movement owner without breaking authored assets. If Enhanced Input types appear in this public header, make the module dependency visible at the required public scope; research recommends keeping those includes private when the public interface can avoid exposing them.

### `Source/Vyxxen/Private/PlayerShip.cpp` (controller / Pawn behavior, event-driven)

**Analog:** `Source/Vyxxen/Private/PlayerShip.cpp` (tracked; `git ls-files` verified)

**Include and lifecycle pattern** (lines 4-26):

```cpp
#include "PlayerShip.h"

APlayerShip::APlayerShip()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
```

Use the matching class-stem include and call the superclass from lifecycle overrides. This file is otherwise an empty scaffold: there is no input binding, tick-time movement, cleanup, smoothing, validation, or error-handling implementation to copy. Keep input handlers focused on semantic requests and put axis movement, clamping, Slow interpolation, and actual-motion pose signals in the single selected movement owner, as the research recommends. Do not rotate the movement basis to implement pitch/bank.

### `Source/Vyxxen/Public/FlightMovementComponent.h` and `Source/Vyxxen/Private/FlightMovementComponent.cpp` (component, event-driven transform)

**Analog:** None. The current project has no native ActorComponent or movement-component implementation. `PlayerShip.h` / `.cpp` above provide only the module's reflected-header and lifecycle conventions, not a component pattern.

**Implementation guidance from research:** Build one component on the editor-confirmed controlled Pawn. Keep editable forward/steer/climb rates, world-Z bounds, pose limits/response, and Slow duration directly on the ship or this component. Apply independent world +X/Y/Z rates; do not normalize combined steer/climb. Clamp resulting world Z to the inspected height interval and derive pitch from actual clamped vertical motion. Keep visual pose on a mesh/pivot hierarchy so it does not redirect translation. Maintain one Slow alpha that moves from its current value toward the held/released target over the editable one-second duration; scale X/Y to 60% at full Slow and leave Z unchanged. Validate finite, nonnegative rates and a valid min/max interval as the phase research's narrow configuration guard. These are phase recommendations, not patterns already implemented in this codebase.

### `Source/Vyxxen/Vyxxen.Build.cs` (config, build-time dependencies)

**Analog:** `Source/Vyxxen/Vyxxen.Build.cs` (tracked; `git ls-files` verified)

**Module dependency declaration** (lines 9-13):

```csharp
PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

PrivateDependencyModuleNames.AddRange(new string[] { });
```

Add the `EnhancedInput` module dependency here if native source includes or calls its types. Match dependency visibility to whether those types appear in public declarations; research says private visibility is preferred when public headers do not expose them. The `.ini` class selections below do not replace the C++ module dependency.

### `Content/Input/<Steer, Climb, Slow actions and flight Mapping Context>` (config, event-driven mapping)

**Analog:** None found in checked-in `Content/`; the inventory contains no `Content/Input/` assets.

Author semantic Steer and Climb axis actions plus a Slow bool action, then map both WASD and arrow keys, left-stick X/Y (push forward/up to climb), and Left Shift / LT-L2 to those actions in one Mapping Context. Use mapping modifiers for sign changes and tune analog dead zone in the Mapping Context. Bind Triggered values to update requests and clear each request on both Completed and Canceled; also clear all cached requests when possession/control or the active context ends. This follows locked decisions D-01–D-03 and the input lifecycle recommendation in `01-RESEARCH.md`; there is no project asset pattern to copy. Inspect existing binary content in Editor before creating duplicates.

### `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Levels/MainGame.umap` (authored integration, conditional edits)

**Analog:** The same tracked assets exist, but binary graphs are not represented by inspectable source excerpts. Their tracked status was verified with `git ls-files`.

Inspect the ship Blueprint's parent, components, graphs, class defaults, and overrides; inspect MainGame's GameMode/default Pawn, placed ship and auto-possession, active input context, authored starting Z, and apparent playable height band. Preserve the current parent/wiring where possible. Configure the min/max interval from the observed band and preserve authored start Z, using its midpoint only when no authored start exists. Change binary assets only where Editor inspection establishes the required integration. No internal asset facts are asserted by this map.

## Shared Patterns

### Unreal native declarations and lifecycle

**Source:** `Source/Vyxxen/Public/PlayerShip.h:5-24`, `Source/Vyxxen/Private/PlayerShip.cpp:4-26`
**Apply to:** Native ship and component declarations/implementations.

Use `VYXXEN_API`, `UCLASS()`/`GENERATED_BODY()` for reflected types, keep public headers under `Public/` and implementations under `Private/`, put the generated header last among includes, match class/file stems, and call `Super` from lifecycle overrides. The current code does not establish field naming, component construction, or Enhanced Input conventions.

### Enhanced Input is configured at the project boundary

**Source:** `Config/DefaultInput.ini:70,79-80` (tracked; `git ls-files` verified)

```ini
bShouldFlushPressedKeysOnViewportFocusLost=True
DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput
DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent
```

These settings select Enhanced Input classes and request key flushing when the viewport loses focus; they do not define the three semantic actions, mapping context, input bindings, or clearing of game-owned cached values. Follow the phase lifecycle guidance to handle Completed and Canceled and reset cached input state on unpossess/context removal/teardown, then verify focus-loss recovery in PIE for keyboard and gamepad.

### Module dependencies

**Source:** `Source/Vyxxen/Vyxxen.Build.cs:9-13`
**Apply to:** Native files that use Enhanced Input APIs.

Declare engine module dependencies in the module rules and choose public/private visibility according to exposed header types. There is no separate package manager or code-level import convention.

### Manual verification and absent patterns

The project has no project-owned movement test framework/target and no comparable validation helper. Use the research validation matrix as manual Editor build/load and PIE acceptance; do not invent a test command or assume that generated binaries prove the project builds. No project-owned pattern was found for authentication, logging, exceptions, centralized errors, or input cleanup. This phase is local movement and requires none of the former; input cleanup must be implemented and verified explicitly.

## No Analog Found

| File / area | Role | Data Flow | Reason |
|--------------|------|-----------|--------|
| `Source/Vyxxen/Public/FlightMovementComponent.h` / `Private/FlightMovementComponent.cpp` | component | event-driven transform | No ActorComponent or movement component exists in native project source. |
| `Content/Input/<actions and Mapping Context>` | config | event-driven mapping | No input asset directory/files were found in checked-in content. |
| `BP_vyxxen-ship.uasset` / `MainGame.umap` internals | authored integration | event-driven possession/configuration | Binary assets cannot provide source excerpts here; inspect them in Unreal Editor. |

## Metadata

**Analog search scope:** `Source/`, `Config/`, `Content/`, and the Phase 1 research/context artifacts.
**Files scanned:** 8 native/config source files, project content inventory, and the two phase artifacts.
**Tracked-source gate:** Every named on-disk analog was verified with `git ls-files -- <path>`; no ignored mirror paths are used.
**Pattern extraction date:** 2026-10-07
