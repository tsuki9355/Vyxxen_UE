# Walking Skeleton — Vyxxen

**Phase:** 1
**Generated:** 2026-10-07

## Capability Proven End-to-End

By the end of Phase 1, opening the authored MainGame level in Unreal Editor PIE starts the intended ship moving forward, and the player can steer and climb with editable movement behavior and readable ship pose.

## Architectural Decisions

| Decision | Choice | Rationale |
|---|---|---|
| Engine and runtime | Reuse the existing Unreal Engine 5.8 project and `Vyxxen` runtime module. | The project already has an Unreal module, startup level, and Editor target. |
| Controlled entity | Inspect the existing MainGame ship Blueprint and runtime control path; retain its existing Pawn/controller route if it is a Pawn, or preserve a non-Pawn AActor parent and enable that actor's input on the confirmed local PlayerController with `EnableInput`. | Blueprint inheritance and map wiring are not established by asset names; possession is not required when the authored ship is an AActor. |
| Movement owner | Use one native `UFlightMovementComponent` attached to the Editor-confirmed MainGame ship actor, whether its preserved parent is a Pawn or another AActor. | One owner keeps forward, lateral, vertical, bound, slowdown, and pose calculations together across either control route. |
| Tuning surface | Expose movement rates, world-Z bounds, pose response, and slowdown timing directly on the ship movement component. | This matches the requested class/component Details-panel workflow without introducing a separate settings asset. |
| Input | Use Enhanced Input actions and one flight Mapping Context; bind device sources to shared semantic actions. | Keyboard and gamepad input then feed the same movement requests without raw-key gameplay checks. |
| Asset integration | Inspect and preserve the existing ship Blueprint parent, authored components, graphs, and MainGame content; use the already-established Pawn path or the enabled AActor input path without reparenting merely to obtain possession. | The binary asset graphs, runtime control path, and instance overrides must be verified in Editor. |
| Delivery and validation | Use current Editor build/load and observed Play In Editor behavior. | The agreed delivery is a playable Unreal Editor experience; no project-owned automated gameplay test target was found. |

## Stack Touched in Phase 1

- [ ] Existing Unreal project scaffold and `Vyxxen` runtime module reused.
- [ ] Native movement component wired to the authored MainGame ship.
- [ ] Enhanced Input actions and Mapping Context wired to movement requests.
- [ ] Current Editor build/load and the complete manual PIE acceptance matrix recorded.

Database, authentication, web routes or UI, and dev-server deployment do not apply to this Unreal game and remain out of scope.

## Out of Scope

- Database, authentication, web routing or UI, and web deployment systems from the generic web-app template.
- Any project systems that are not part of the Phase 1 requirements in `.planning/ROADMAP.md` and `.planning/REQUIREMENTS.md`.

## Subsequent Slice Plan

- Phase 2: follow the second playable slice already recorded in `.planning/ROADMAP.md`, preserving the single ship movement owner established here.
