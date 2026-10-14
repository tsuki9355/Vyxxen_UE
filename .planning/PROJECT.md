# Vyxxen

## What This Is

Vyxxen is a fully 3D Zaxxon-inspired arcade game in Unreal Engine for the Vertex School AI Bootcamp. The first implementation cut delivers ship flight controls and a synchronized route camera, playable in Unreal Editor. The longer-term first-fortress design includes combat, hazards, fuel airpods, rewards, and stage progression; those systems are deferred beyond this cut.

## Core Value

The player can steer and climb a continuously advancing ship while a stable camera moves with exactly the same forward progress.

## Requirements

### Validated

No gameplay capability has been runtime-validated yet. The existing repository contains a native module and editor-authored assets, but their presence is not proof of playable behavior.

### Active

- [ ] The ship travels automatically along world +X, with editable movement speed.
- [ ] The player steers laterally along Y and climbs/descends along Z through Enhanced Input actions.
- [ ] Minimum and maximum flight height are editable directly on the ship class or its movement component, like movement speed, and constrain flight.
- [ ] Steering banks the ship around its nose; climb/descent pitches it, with neutral input returning the pose toward level.
- [ ] Holding Slow ramps forward and lateral movement toward a 40% speed reduction; release restores normal speed. Climb speed is unchanged.
- [ ] Releasing or canceling input clears steering, climb, and slowdown so controls do not stick.
- [ ] The camera follows exactly the ship's forward progress along X, including slowdown, while its Y/Z coordinates and viewing rotation stay fixed.
- [ ] The developer can start Play In Editor and use the flight controls with the synchronized camera.

### Out of Scope

- Firing, projectiles, and aim cursor — the user explicitly deferred firing in the first implementation cut.
- Fuel capacity, drain, refueling, and the slowdown fuel penalty — belong to the later resource/gameplay cut; this cut implements flight slowdown only.
- Enemies, turrets, missiles, hazards, damage, scoring, plane counter, lives, respawning, and stage completion — later fortress systems, outside the approved flight-and-camera milestone.
- Packaged Windows delivery — the user selected Play In Editor as the delivery target for this cut.
- Space combat, multiplayer, replication, aerodynamic simulation, and behavior trees — beyond the first-fortress prototype's design.

## Context

- **Code repository:** `C:/Users/Breno/Documents/Unreal Projects/Vyxxen`.
- **Art/design workspace:** `C:/Users/Breno/My Drive/Vertex/{ Unreal AI }/Games/Vyxxen`.
- **Design reference:** `C:/Users/Breno/My Drive/Vertex/{ Unreal AI }/Games/Vyxxen/Design/Vyxxen_First_Stage.md`.
- **Current map:** `.planning/codebase/` contains seven codebase documents completed on 2026-10-07.
- `README.md` identifies the Zaxxon-like bootcamp game. `Vyxxen.uproject` associates with Unreal Engine 5.8 and declares the `Vyxxen` runtime module.
- `Source/Vyxxen/Public/PlayerShip.h` currently declares an `AActor` scaffold; `Source/Vyxxen/Private/PlayerShip.cpp` has empty lifecycle hooks. Possession, Blueprint inheritance, camera setup, and existing graph behavior remain unverified.
- `Content/Units/PlayerShip/BP_vyxxen-ship.uasset`, `Content/Scenery/BP_level1_scenery.uasset`, and `Content/Levels/MainGame.umap` are existing binary assets. Inspect them in Unreal before changing inheritance or wiring.
- `Config/DefaultInput.ini` selects Enhanced Input classes, while `Source/Vyxxen/Vyxxen.Build.cs` does not yet declare a native Enhanced Input dependency.
- Native source and several asset directories are existing untracked work, and the project descriptor, editor configuration, and main map are modified. Mapping commit `5b742f7` contains planning artifacts only; preserve and incorporate the actual working project.
- Design architecture names and folders are guidance, not implemented classes. Adapt the reference's `Source/ArcadeClone/` example to the existing `Source/Vyxxen/` module.

## Constraints

- **Milestone scope:** Flight controls and X-only route-camera movement — keep later fortress systems out of this first cut.
- **Scene basis:** Nose/forward is +X, lateral offset is Y, altitude is Z; scene axes do not define raw-key bindings.
- **Editable tuning:** Movement speeds and minimum/maximum height must be editable on the ship class or actor component. A separate data-asset or settings system is unnecessary for this cut.
- **Input:** Use Enhanced Input actions and a Mapping Context; no raw key names in native gameplay logic. Independent steer and climb inputs are not normalized together.
- **Slowdown:** Full slowdown reduces forward and lateral speed by 40%, with ramp timing editable. It does not reduce climb speed.
- **Camera:** One forward position feeds the ship and camera; avoid separate accumulated movement that can drift or double-scroll. The camera does not follow lateral movement, altitude, pitch, or bank.
- **World:** Fortress scenery stays where authored. Move the ship and camera rather than the terrain.
- **Implementation:** Follow Unreal actor/component conventions; reuse and inspect existing assets before changing them. The design favors a pawn plus movement component and a placed camera plus route-following component.
- **Verification:** Successful editor build/load and observed Play In Editor behavior establish completion. Generated binaries or asset filenames alone do not.
- **AI configuration:** Use `gpt-6-luna` with `xhigh` reasoning for all work and GSD agents, as explicitly requested by the user.

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| First milestone is flight controls plus synchronized camera | User specified the first implementation cut | Pending implementation |
| Editable speed and altitude bounds on the ship or movement component | User wants to tune these directly | Pending implementation |
| Camera moves only along X with the same forward progress as the ship | User requires exact speed matching and a fixed axis | Pending implementation |
| Defer Fire/projectiles | User explicitly selected flight controls only | Scope confirmed |
| Deliver through Play In Editor | User selected editor playability | Scope confirmed |
| Preserve automatic progress, pitch/bank, and hold-to-slow from the design | Makes the flight controls consistent with the existing first-fortress design | Pending implementation |
| Use GPT-6 Luna Extra High throughout | Explicit user preference | Project model resolution verified |

## Evolution

This document evolves at phase transitions and milestone boundaries.

After each phase transition, move verified requirements to Validated, record decisions and discovered constraints, and revise the description if the implementation changes scope. Move invalidated requirements to Out of Scope with a reason.

After each milestone, review the core value, scope, context, and decisions before selecting the next implementation cut. Deferred fortress systems become active only through explicit milestone scoping.

---
*Last updated: 2026-10-07 after project initialization and first-cut scope approval*
