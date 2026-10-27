# Roadmap: Vyxxen

## Overview

The first implementation cut delivers arcade ship flight and an exactly synchronized route camera in Unreal Editor. Two playable slices follow the movement-to-camera dependency: first control the existing ship with the complete movement policy, then finish fixed camera framing and repeatable saved MainGame startup. Every phase uses MVP mode and includes editor observation; successful compilation alone cannot complete a phase.

**Milestone:** First implementation cut — flight controls and synchronized camera
**Granularity:** coarse

## Phases

- [ ] **Phase 1: Controllable Arcade Flight** - Play automatic forward flight with independent steering/climb, direct tuning, pitch/bank, selective slowdown, and reliable input cleanup.
- [ ] **Phase 2: Synchronized Camera and Repeatable PIE Delivery** - Play through the fixed route camera with exact ship-X synchronization and durable MainGame startup.

## Phase Details

### Phase 1: Controllable Arcade Flight

**Goal**: The player can control the intended continuously advancing ship in PIE with tunable arcade movement, responsive visual pose, and reliable hold/release behavior.
**Mode:** mvp
**Depends on**: Nothing (first phase)
**Requirements**: MOVE-01, MOVE-02, MOVE-03, MOVE-04, TUNE-01, TUNE-02, POSE-01, POSE-02, SLOW-01, SLOW-02, INPUT-01, INPUT-02
**Success Criteria** (what must be TRUE):
  1. In PIE, the intended ship advances automatically along world +X; the player can steer both ways on Y and climb/descend on Z, including simultaneous steering/climb at their independent configured rates.
  2. Editing forward, lateral, and climb speeds on the ship class or movement component changes the corresponding movement rates. Editable minimum/maximum world-Z heights constrain the ship while allowing movement inward from either limit and continued X/Y travel.
  3. Actual climb/descent pitches the ship and steering banks it around its nose; stopping the corresponding motion eases the pose toward level, including pitch at an altitude limit, without rotating the world-axis translation basis.
  4. Holding Slow ramps X/Y rates toward 60% of normal; release restores normal rates through the editable ramp timing, including a reversal during a ramp. Z climb/descent stays at its normal rate throughout.
  5. Steer, Climb, and Slow work through semantic Enhanced Input actions with device bindings in a Mapping Context. Release, cancellation, and control/focus loss clear the respective requests; returning to play leaves no latched steer/climb/Slow, and clearing Slow uses the normal recovery ramp.

**Plans:** 3/3 plans executed

Plans:
**Wave 1**
- [x] 01-01-PLAN.md — Prove automatic forward flight through the existing MainGame ship path.

**Wave 2** *(blocked on Wave 1 completion)*
- [x] 01-02-PLAN.md — Add independent steer/climb, shared device mappings, and measured height limits.

**Wave 3** *(blocked on Wave 2 completion)*
- [x] 01-03-PLAN.md — Add responsive pitch/bank, selective Slow, and full input cleanup.

**UI hint**: no

**Planning constraints:** Inspect the existing ship Blueprint parent, components, graphs, mesh basis, and MainGame possession/view wiring before changing native inheritance or assets. Preserve authored content and choose one controlled-ship path and one movement owner. Reuse the existing Vyxxen module and add its EnhancedInput dependency when native APIs require it. Establish an observation view and an incremental current editor build/load plus observed flight baseline within this phase; exact synchronized-camera acceptance belongs to Phase 2.

**Research focus:** Resolve installed engine/toolchain compatibility, Blueprint/map ownership, effective tuning overrides, and actual PIE input-loss behavior through targeted local inspection. Existing research supplies the movement patterns; it does not validate the binary assets or runtime.

### Phase 2: Synchronized Camera and Repeatable PIE Delivery

**Goal**: The player can repeatedly start MainGame in PIE and fly through a stable authored camera view that shares the ship's actual forward progress exactly.
**Mode:** mvp
**Depends on**: Phase 1
**Requirements**: CAM-01, CAM-02, PLAY-01
**Success Criteria** (what must be TRUE):
  1. The active route camera preserves its initial camera-to-ship X offset through normal movement, Slow ramp-in/hold/recovery/reversal, and simultaneous steering/climb, including a nonzero starting ship X. Same-update transform samples remain within normal floating-point measurement precision with no accumulated drift or frame of delayed following.
  2. The camera's world Y, world Z, and viewing rotation stay at their authored starting values during steering, altitude changes, pitch, and bank; the authored scenery stays stationary.
  3. After a successful current editor build/load and saved asset wiring, normal MainGame PIE startup controls the intended ship and displays the synchronized route camera. Repeated PIE runs and a fresh editor session reproduce the flight controls, input cleanup, tuning, and camera behavior.

**Plans**: TBD
**UI hint**: no

**Planning constraints:** Derive camera X from the ship's actual resulting X and the initial offset; use no independent camera speed accumulation or Slow state. Order the tick that applies camera follow after the tick that commits ship movement and before camera sampling. Verify the active player view as well as the camera actor transform. Save affected native/Blueprint/input/map references and repeat the integrated acceptance checks after reopening the editor. Phase 1 requirements remain owned by Phase 1; their checks here are integration regression checks.

**Research focus:** Use the documented applied-X and tick-dependency pattern. Inspect local camera selection/automatic view management and reproduce timing issues if acceptance exposes them.

## Scope and Verification

Only the 15 approved v1 requirements belong to this milestone. Firing, fuel, combat, hazards, scoring, stage progression, extra HUD, lateral corridor rules, and packaging stay outside the cut. UI hints refer to the existing editor tuning surface and active game view; they do not authorize new UI features.

Plan checks and result verification remain enabled. Parallelize only independent work with clear ownership; binary ship/map authoring and movement-to-camera dependencies require coordination. Preserve unrelated working changes. Runtime acceptance must record a current build/load and observed PIE evidence; inaccessible runtime checks remain pending.

**Coverage:** 15/15 v1 requirements assigned exactly once; 12 to Phase 1 and 3 to Phase 2. No orphaned or duplicate assignments.

## Progress

**Execution order:** Phase 1 → Phase 2

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 1. Controllable Arcade Flight | 3/3 | In Progress | - |
| 2. Synchronized Camera and Repeatable PIE Delivery | 0/TBD | Not started | - |

---
*Created: 2026-10-07 — roadmap prepared for user review; implementation has not started.*
