# Phase 1: Controllable Arcade Flight - Context

**Gathered:** 2026-10-07
**Status:** Ready for planning

<domain>
## Phase Boundary

Deliver a playable Phase 1 flight-control slice in Unreal Editor PIE. The intended ship advances automatically along world +X while the player steers along Y, climbs or descends along Z, and can combine those inputs independently. Expose movement rates, world-Z height limits, pose tuning, and slowdown timing directly on the ship class or its movement component. Pitch and bank provide readable feedback; Slow ramps forward and lateral speed to 60% while leaving climb speed unchanged. Enhanced Input release, cancellation, and control loss must clear stored requests.

Firing and projectiles remain deferred. Exact X-synchronized camera movement and repeatable camera delivery are owned by Phase 2. Keep scenery stationary and do not add later fortress systems.

</domain>

<decisions>
## Implementation Decisions

### Default device bindings
- **D-01:** Map both WASD and arrow keys to the same semantic steer and climb actions.
- **D-02:** Include gamepad controls in the first cut. The left stick steers and climbs; pushing it forward/up increases altitude, matching W and Up Arrow.
- **D-03:** Slow is hold-to-use, bound to Left Shift on keyboard and LT/L2 on gamepad. Release and cancellation still follow the approved recovery and input-clearing behavior.

### Starting speed and height values
- **D-04:** Choose editable first-pass movement and height values, then tune them in PIE rather than treating them as measured arcade constants.
- **D-05:** Keep forward travel steady while lateral and vertical control feel responsive. Preserve independent rates when steering and climbing together; do not normalize the combined input.
- **D-06:** Set the editable minimum and maximum world-Z heights from the existing level's apparent playable height band. Preserve an authored starting height when one exists; otherwise start near the midpoint of that band.

### Pitch, bank, and slowdown response
- **D-07:** Keep pitch and bank moderate and clearly readable, responding to actual climb/descent and steer respectively.
- **D-08:** Make pitch and bank responsive with a brief smooth ease back toward neutral when their corresponding motion stops.
- **D-09:** Use an editable one-second Slow ramp toward the 60% X/Y speed level and the same one-second ramp back to normal on release. Climb/descent speed remains unchanged.

### the agent's Discretion
Choose concrete initial movement rates, height values within the inspected level band, and pitch/bank limits during PIE tuning to match these preferences. Preserve the authored ship start height if present; only use the band midpoint when no authored start height is available. Keep the values directly editable on the ship class or movement component.

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Scope and acceptance
- `.planning/PROJECT.md` — Approved first-cut scope, core value, and project constraints.
- `.planning/REQUIREMENTS.md` — Locked movement, tuning, pose, Slow, input, and Phase 1 acceptance requirements.
- `.planning/ROADMAP.md` — Phase boundaries, success criteria, planning constraints, and dependency on Phase 2.

### Research and design
- `.planning/research/SUMMARY.md` — Synthesized Unreal and project research.
- `.planning/research/STACK.md` — Engine, module, and Enhanced Input facts.
- `.planning/research/ARCHITECTURE.md` — Recommended movement ownership and update relationships.
- `.planning/research/FEATURES.md` — Feature behaviors and tuning questions from research.
- `.planning/research/PITFALLS.md` — Known implementation and verification pitfalls.
- `../../../My Drive/Vertex/{ Unreal AI }/Games/Vyxxen/Design/Vyxxen_First_Stage.md` — Design reference for axis orientation, flight behavior, pose, Slow, and stationary scenery.

### Existing code context
- `.planning/codebase/STACK.md` — Existing engine/module and input configuration inventory.
- `.planning/codebase/ARCHITECTURE.md` — Native scaffold, map, Blueprint, and integration boundaries.
- `.planning/codebase/CONVENTIONS.md` — Current Unreal C++ file and reflection conventions.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `Source/Vyxxen/Public/PlayerShip.h` and `Source/Vyxxen/Private/PlayerShip.cpp`: an `APlayerShip` lifecycle scaffold that may be extended after verifying the controlled-pawn path.
- `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Levels/MainGame.umap`: existing authored ship and startup-level assets to inspect and preserve before changing inheritance, possession, input, or view wiring.

### Established Patterns
- The project is a single Unreal runtime module under `Source/Vyxxen/`, with reflected public declarations and private implementations.
- `Config/DefaultInput.ini` selects Enhanced Input classes, but the native module currently does not declare an `EnhancedInput` dependency; source gameplay does not yet establish semantic actions or a Mapping Context.
- The `APlayerShip` scaffold currently derives from `AActor`, so it is not by itself proof of a possessable ship. The Blueprint parent and MainGame ownership are unverified.

### Integration Points
- `Config/DefaultEngine.ini` selects `/Game/Levels/MainGame` for startup. Verify the level's ship instance, effective class defaults, and possession/input setup in Unreal Editor before changing them.
- Inspect the ship Blueprint's parent/components/graphs and the map's authored starting height. Use the existing level height band and preserve the authored start when available.
- Put movement rates, height limits, pose limits/response, and Slow timing directly on the ship class or movement component as already approved.

</code_context>

<specifics>
## Specific Ideas

Use both familiar keyboard layouts and a left-stick gamepad mapping. Push forward/up to climb. Keep the motion readable without a slow, floaty feel; Slow should take one second to reach its full effect and one second to recover.

</specifics>

<deferred>
## Deferred Ideas

No new deferred ideas arose during discussion. Fire/projectiles remain out of scope for this cut, and synchronized camera behavior remains assigned to Phase 2.

</deferred>

---

*Phase: 1-Controllable Arcade Flight*
*Context gathered: 2026-10-07*