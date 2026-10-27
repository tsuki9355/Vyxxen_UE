# Phase 1: Controllable Arcade Flight - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-10-07
**Phase:** 1-Controllable Arcade Flight
**Areas discussed:** Default device bindings, starting speed and height values, pitch, bank, and slowdown response

---

## Default device bindings

### Keyboard layout
| Option | Description | Selected |
|--------|-------------|----------|
| Both WASD and arrow keys | Either set triggers the same actions. | ✓ |
| WASD only | Use only the WASD layout. | |
| Arrow keys only | Use only the arrow-key layout. | |

**User's choice:** Both WASD and arrow keys trigger the same actions.
**Notes:** The player can choose either familiar keyboard layout.

### Gamepad
| Option | Description | Selected |
|--------|-------------|----------|
| Keyboard and gamepad | Add left-stick steering and climbing. | ✓ |
| Keyboard only | Keep the first cut to desktop keys. | |

**User's choice:** Include left-stick gamepad controls alongside keyboard.
**Notes:** None.

### Gamepad climb direction
| Option | Description | Selected |
|--------|-------------|----------|
| Push forward/up to climb | Matches W and Up Arrow. | ✓ |
| Push forward/up to descend | Inverted flight-style controls. | |

**User's choice:** Push forward/up to climb.
**Notes:** Matches the keyboard direction.

### Slow binding
| Option | Description | Selected |
|--------|-------------|----------|
| Left Shift and LT/L2 | Keyboard modifier and gamepad trigger. | ✓ |
| Spacebar and RT/R2 | Alternate keyboard and gamepad controls. | |
| Left Shift only | Keyboard control without a gamepad binding. | |

**User's choice:** Hold Left Shift or LT/L2 for Slow.
**Notes:** Slow remains hold-to-use.

---

## Starting speed and height values

### Initial values
| Option | Description | Selected |
|--------|-------------|----------|
| Editable first-pass values tuned in PIE | Adjust the initial values during playtesting. | ✓ |
| Exact numeric targets | User supplies the values. | |
| Specific reference or recording | Match measured source values. | |

**User's choice:** Set editable first-pass values and tune them in PIE.
**Notes:** Treat initial rates and heights as provisional rather than measured arcade constants.

### Rate feel
| Option | Description | Selected |
|--------|-------------|----------|
| Steady forward pace with responsive Y/Z controls | Forward travel is steady while steer and climb feel responsive. | ✓ |
| Similar pacing on all three rates | Keep X, Y, and Z rates similarly paced. | |
| Slow forward pace | Favor precise maneuvering. | |

**User's choice:** Steady forward pace; Y/Z controls feel responsive.
**Notes:** Combined steer and climb retain independent rates.

### Height band
| Option | Description | Selected |
|--------|-------------|----------|
| Broad altitude range | Clearly demonstrate climb and descent. | |
| Tight altitude range | Keep the ship close to the authored route. | |
| Existing level height band | Use the level's apparent playable height band. | ✓ |

**User's choice:** Base the height range on the level's existing height band.
**Notes:** Minimum and maximum remain editable world-Z limits.

### Starting height
| Option | Description | Selected |
|--------|-------------|----------|
| Preserve authored height; otherwise band midpoint | Keep the current authored start height when present; otherwise use the band's midpoint. | ✓ |
| Center in the band | Recenter the ship even if this changes its authored start height. | |
| Near the lower end | Start closer to the lower limit. | |

**User's choice:** Preserve the authored start height; if none exists, use the band midpoint.
**Notes:** Inspect the level and ship start before changing placement.

---

## Pitch, bank, and slowdown response

### Pitch and bank amount
| Option | Description | Selected |
|--------|-------------|----------|
| Moderate and clearly readable | Clearly show motion without an exaggerated tilt. | ✓ |
| Subtle | Use little visual tilt. | |
| Strong arcade-style tilt | Make the pitch and bank more pronounced. | |

**User's choice:** Moderate and clearly readable pitch and bank.
**Notes:** Actual climb/descent drives pitch; steering drives bank.

### Pose response
| Option | Description | Selected |
|--------|-------------|----------|
| Responsive with a brief smooth ease | Controls feel prompt while the pose settles smoothly. | ✓ |
| Sharp and quick | Little easing. | |
| Slow and floaty | Long, gentle response. | |

**User's choice:** Responsive with a brief smooth ease.
**Notes:** Pose returns toward level when the corresponding motion stops.

### Slow ramp-in
| Option | Description | Selected |
|--------|-------------|----------|
| About 0.5 seconds | Quick ramp. | |
| About 1 second | Noticeable but responsive ramp. | ✓ |
| About 2 seconds | Deliberate ramp. | |

**User's choice:** About one second to reach full Slow effect.
**Notes:** Full Slow reduces forward and lateral speed to 60%; climb speed remains unchanged.

### Slow recovery
| Option | Description | Selected |
|--------|-------------|----------|
| Same one-second ramp | Recover at the ramp-in pace. | ✓ |
| About half a second | Recover faster. | |
| Different recovery time | User supplies another value. | |

**User's choice:** Use the same one-second ramp on release.
**Notes:** Releasing or canceling clears the request and uses the recovery ramp.

---

## the agent's Discretion

Choose concrete initial movement rates, height values within the inspected level band, and pitch/bank limits during PIE tuning to match the choices above. Preserve the authored ship start height when present.

## Deferred Ideas

No new deferred ideas. Firing/projectiles remain outside this cut, and exact synchronized camera movement remains Phase 2.