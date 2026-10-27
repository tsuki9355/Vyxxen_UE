---
last_mapped_commit: 97fba1f2e8fc0676f4f28f5e659f174a01754612
last_mapped_at: 2026-10-07
---
# Codebase Concerns

**Analysis Date:** 2026-10-07

Assessment scope: local text source, configuration, Git tracking, and asset filenames/sizes in `Source/`, `Config/`, `Content/`, `Vyxxen.uproject`, `.gitignore`, and `.gitattributes`. Binary internals in `Content/Levels/MainGame.umap` and `Content/**/*.uasset` are unverified. No editor session, build, cook, or gameplay test establishes runtime behavior. Findings below distinguish observed facts from conditional risks.

## Tech Debt

**Local project state is not reproducible from tracked files:**
- Issue: Git reports `Source/`, `Content/Scenery/`, and `Content/Units/` as untracked. `Vyxxen.uproject` is modified and declares a runtime C++ module named `Vyxxen`. Only the main level is tracked under `Content/`.
- Files: `Vyxxen.uproject`, `Source/Vyxxen/Vyxxen.Build.cs`, `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`, `Content/Scenery/BP_level1_scenery.uasset`, `Content/Levels/MainGame.umap`.
- Impact: A checkout of tracked files cannot reproduce the observed local native module or imported asset collection. Missing level references are a risk; actual map references have not been inspected.
- Fix approach: Review and commit the intended source, project descriptor, and gameplay assets together. Preserve existing modifications and verify a clean checkout after generating project files; do not rely on `Binaries/` or `Intermediate/` to supply missing implementation.

**Native ship lifecycle is template scaffolding:**
- Issue: `APlayerShip` derives from `AActor`, enables ticking, and contains constructor, `BeginPlay`, and `Tick` boilerplate. The lifecycle overrides only call their superclass. No native components, input handlers, movement, weapons, health, or game rules are declared.
- Files: `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`.
- Impact: The name alone does not provide a controllable ship implementation. Planning native features without inspecting the existing ship Blueprint could duplicate or conflict with Blueprint behavior.
- Fix approach: Inspect `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` before defining native ownership. Implement reusable logic in the native class where appropriate and expose intentional Blueprint extension points. Establish the possession and controller design before changing the base class.

**Build and play instructions are absent:**
- Issue: `README.md` contains the project title and a one-line description, with no setup, compilation, gameplay controls, packaging, or validation instructions.
- Files: `README.md`, `Vyxxen.uproject`, `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs`, `Config/DefaultInput.ini`.
- Impact: Contributors must infer the engine/toolchain and how to validate changes. Local generated project files are not a substitute for documented setup.
- Fix approach: Document the engine association, required development tools, project-file generation, editor/game build targets, startup map, actual controls, and a minimal play verification procedure after validating them locally.

**Binary asset handling has no repository-specific attributes:**
- Issue: `.gitattributes` only applies automatic text detection. Git reports no configured `filter`, `diff`, or `merge` attribute for `Content/Levels/MainGame.umap` or `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`.
- Files: `.gitattributes`, `Content/Levels/MainGame.umap`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`, `Content/Scenery/BP_level1_scenery.uasset`.
- Impact: Binary map and Blueprint edits cannot be reviewed or combined as normal text changes. Repeated revisions of large texture packages increase ordinary Git history size.
- Fix approach: Establish asset ownership and an Unreal asset review workflow. Evaluate Git LFS and locking for `.uasset` and `.umap` files before tracking repeated large revisions; confirm remote support before changing repository storage policy.

## Known Bugs

**No runtime bug confirmed by this mapping:**
- Symptoms: Not established. The empty native ship implementation is an observed implementation gap, not evidence that the Blueprint ship or level fails at runtime.
- Files: `Source/Vyxxen/Private/PlayerShip.cpp`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`, `Content/Levels/MainGame.umap`.
- Trigger: Not verified; neither Play In Editor nor a packaged executable is exercised.
- Workaround: Not applicable. Reproduce suspected gameplay failures before promoting the conditional risks in this document into confirmed bugs.

## Security Considerations

**Android File Server configuration includes a tracked token field:**
- Risk: A `SecurityToken` field exists in tracked `Config/DefaultEngine.ini`; treating that field as confidential while committing the configuration defeats that expectation. The value is deliberately excluded from this document and tool output.
- Files: `Config/DefaultEngine.ini`.
- Current mitigation: File Server settings specify `ConnectionType=USBOnly`, `bIncludeInShipping=False`, and `bAllowExternalStartInShipping=False`. The configuration also sets `bEnablePlugin=True` and `bAllowNetworkConnection=True`; these flags alone do not establish an active network service.
- Recommendations: Disable unused File Server functionality. If retained, manage its token outside shared defaults and replace any exposed value where secrecy is required. Verify effective settings on an Android development build before making claims about network reachability; Android use is not established by `Vyxxen.uproject`.

## Performance Bottlenecks

**Empty native tick is enabled:**
- Problem: `APlayerShip` requests per-frame updates even though its native `Tick` has no project-specific work.
- Files: `Source/Vyxxen/Private/PlayerShip.cpp`, `Source/Vyxxen/Public/PlayerShip.h`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`.
- Cause: `PrimaryActorTick.bCanEverTick = true` is present in the constructor and the override only invokes `Super::Tick(DeltaTime)`.
- Improvement path: Inspect Blueprint tick dependencies before disabling ticking or removing the override. No actor count or profiler capture establishes a measurable bottleneck; prioritize this only when the actor's actual runtime usage warrants it.

**High rendering feature baseline lacks measured budgets:**
- Problem: Project settings target Desktop/Maximum with DX12 as the Windows default, virtual shadows, mesh distance fields, dynamic GI/reflections, ray tracing, and Substrate enabled. The configuration is observable; actual GPU cost is unmeasured.
- Files: `Config/DefaultEngine.ini`, `Content/Levels/MainGame.umap`, `Content/Scenery/ground-tile/M_ground-tile.uasset`.
- Cause: Multiple renderer features increase the range of hardware and shader paths that must be validated. Their effective use depends on materials, lighting, engine behavior, and device capability.
- Improvement path: Set a target device and frame-time budget, profile the main map, and adjust feature/scalability settings based on measured cost. Do not assume every enabled feature is active or that the level currently misses its budget.

## Fragile Areas

**Single binary startup map:**
- Files: `Config/DefaultEngine.ini`, `Content/Levels/MainGame.umap`, `Content/Scenery/BP_level1_scenery.uasset`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`.
- Why fragile: Both `GameDefaultMap` and `EditorStartupMap` reference `/Game/Levels/MainGame.MainGame`; this is the only map filename detected under `Content/`. Map internals, World Settings overrides, and referenced assets are unverified.
- Safe modification: Coordinate map ownership, keep reference-preserving asset operations in Unreal, and verify startup, dependencies, and cook behavior after changes. Avoid filesystem renames of assets with unknown referencers.
- Test coverage: No source-level automation or test-map filenames are detected in `Source/` and `Content/`; Blueprint-embedded checks are unverified.

**Input classes are configured without visible native wiring:**
- Files: `Config/DefaultInput.ini`, `Source/Vyxxen/Vyxxen.Build.cs`, `Source/Vyxxen/Public/PlayerShip.h`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`.
- Why fragile: The input defaults select Enhanced Input classes, while the native module lists `Core`, `CoreUObject`, `Engine`, and `InputCore` only. The native ship contains no input bindings, and no Input Action or Input Mapping Context asset filenames are detected. These observations do not prove that Blueprint input is absent.
- Safe modification: Inspect Blueprint handling and effective engine plugin availability. When native code uses Enhanced Input APIs, declare the `EnhancedInput` module dependency and wire a verified input/controller path rather than assuming configured default classes provide gameplay bindings.
- Test coverage: Keyboard/gamepad control, possession, mapping activation, and viewport-focus behavior have no detected local automated coverage.

## Scaling Limits

**Binary content history:**
- Current capacity: `Content/` contains 27 local package files totaling 89,471,543 bytes, about 85.3 MiB. The three `power-floor` texture packages total 73,464,097 bytes, about 70.1 MiB.
- Files: `Content/Scenery/ground-tile/power-floor-orm.uasset`, `Content/Scenery/ground-tile/power-floor-diffuse.uasset`, `Content/Scenery/ground-tile/power-floor-normal.uasset`, `.gitattributes`.
- Limit: No hard repository or runtime limit is measured. Frequent large binary revisions can increase clone/storage costs; disk package size does not establish texture resolution, GPU residency, or streaming cost.
- Scaling path: Review texture import settings and runtime residency in Unreal. Choose an asset storage/versioning policy before accumulating revisions and preserve source-art provenance where needed.

## Dependencies at Risk

**Engine and target-rule compatibility is tightly specified:**
- Risk: `Vyxxen.uproject` declares engine association `5.8`, and both native target files select `BuildSettingsVersion.V7`. Compatibility with another engine installation or target-rule API is not established.
- Files: `Vyxxen.uproject`, `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs`, `README.md`.
- Impact: Contributors using a mismatched engine may fail to generate/build the project or may resave binary packages with incompatible versions. No build is run here, so no actual compile error is asserted.
- Migration plan: Use the declared engine/toolchain for reproducible validation. Handle engine upgrades as explicit changes, regenerate project files, validate target rules and assets, and document the supported setup in `README.md`.

**No third-party package vulnerability or abandonment claim established:**
- Risk: No external package lockfile or third-party runtime SDK is detected in the inspected native module. Enabled project plugin metadata identifies `ModelingToolsEditorMode` with an Editor target allowlist.
- Files: `Source/Vyxxen/Vyxxen.Build.cs`, `Vyxxen.uproject`.
- Impact: A dependency security/version audit cannot be inferred from this small local source inventory; engine-supplied components remain outside this mapping's implementation inspection.
- Migration plan: Track explicit module/plugin additions and their compatibility requirements in `Source/Vyxxen/Vyxxen.Build.cs` and `Vyxxen.uproject` as functionality expands.

## Missing Critical Features

**Native player-control and gameplay contracts are undefined:**
- Problem: The only native gameplay class has no movement, collision components, firing, health, scoring, or input ownership. No native custom GameMode, PlayerController, or Pawn class is detected. `Config/DefaultEngine.ini` does not explicitly set `GlobalDefaultGameMode`; map overrides and Blueprint gameplay remain unknown.
- Files: `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Private/PlayerShip.cpp`, `Config/DefaultEngine.ini`, `Content/Levels/MainGame.umap`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`, `README.md`.
- Blocks: Planning a complete native gameplay slice requires an explicit decision about which behavior belongs to Blueprint, the level, or native classes. This is not a confirmed absence of playable Blueprint behavior.

**Repeatable validation and release procedure:**
- Problem: No local CI workflow, test directory, automation source, build helper script, or packaging instructions are detected alongside `Source/`, `Content/`, and `README.md`.
- Files: `README.md`, `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs`, `Vyxxen.uproject`, `Content/Levels/MainGame.umap`.
- Blocks: There is no repository-defined command or procedure proving that code, map dependencies, and packaged startup work together. Add a minimal repeatable build/play/cook validation path before relying on automated phase completion.

## Test Coverage Gaps

**Native module and ship lifecycle:**
- What's not tested: No Unreal automation declarations, native test files, or functional-test implementation are detected. The actor's spawn/lifecycle contract and future gameplay behavior lack visible regression checks.
- Files: `Source/Vyxxen/Private/PlayerShip.cpp`, `Source/Vyxxen/Public/PlayerShip.h`, `Source/Vyxxen/Vyxxen.cpp`, `Source/Vyxxen/Vyxxen.Build.cs`.
- Risk: Compile and behavior regressions can pass a text review without a repository-defined validation gate. Generated `Binaries/` are not evidence that current source compiles.
- Priority: High for a reproducible editor/game build check; add behavioral tests when actual behavior exists rather than testing empty superclass forwarding.

**Blueprint/map integration and packaged startup:**
- What's not tested: Main-map references, Blueprint compilation, controller/ship setup, scenery collisions, and startup in a cooked build. Binary asset internals and any embedded Blueprint tests are unverified.
- Files: `Content/Levels/MainGame.umap`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`, `Content/Scenery/BP_level1_scenery.uasset`, `Config/DefaultEngine.ini`.
- Risk: Editor-only success can conceal missing assets or setup dependencies in another checkout or a packaged build; no such failure is confirmed here.
- Priority: High; establish a smoke test for the main map and review asset dependencies once intended local files are versioned.

**Rendering and input across target devices:**
- What's not tested: Frame-time behavior on target hardware, renderer fallbacks, keyboard/gamepad responsiveness, and focus loss/recovery.
- Files: `Config/DefaultEngine.ini`, `Config/DefaultInput.ini`, `Content/Levels/MainGame.umap`, `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`.
- Risk: Desktop/Maximum rendering defaults and input-class defaults provide configuration without evidence that supported devices meet gameplay requirements.
- Priority: Medium after a playable control path and concrete hardware targets are established.

---

*Concerns audit: 2026-10-07*
