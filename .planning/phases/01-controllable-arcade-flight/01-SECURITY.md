---
phase: "01"
slug: "controllable-arcade-flight"
status: verified
threats_open: 0
asvs_level: 1
created: "2026-10-08"
---

# Phase 01 — Security

## Trust Boundaries

| Boundary | Description | Data Crossing |
|----------|-------------|---------------|
| Editor-authored ship settings → movement component | Editable rates, bounds, pose, and Slow values control local runtime movement. | Local gameplay configuration |
| Local player input → movement requests | Enhanced Input actions update local steering, climb, and Slow state. | Local device input |

## Threat Register

| Threat ID | Category | Component | Severity | Disposition | Mitigation | Status |
|-----------|----------|-----------|----------|-------------|------------|--------|
| T-01-01 | Tampering | `UFlightMovementComponent` settings | medium | mitigate | Validate finite non-negative rates, finite ordered bounds, and clamp computed world Z before applying movement. | closed |
| T-01-02 | Tampering | Editable rates and world-Z bounds | medium | mitigate | Same runtime validation and clamping used by the single movement owner. | closed |
| T-01-03 | Denial of Service | Input cancellation and reset lifecycle | medium | mitigate | Completed/Canceled handlers clear their own request; teardown resets all requests before context removal and input disable; focus flush dispatches cancellation. Independent external `DisableInput` while the component remains active still requires manual verification. | open — below high threshold (non-blocking) |
| T-01-SC/P01 | Tampering | External package supply chain | low | accept | Phase uses the existing Unreal engine and built-in Enhanced Input only; no external package installation occurred. | closed |
| T-01-SC/P02 | Tampering | External package supply chain | low | accept | Phase uses the existing Unreal engine and built-in Enhanced Input only; no external package installation occurred. | closed |
| T-01-SC/P03 | Tampering | External package supply chain | low | accept | Phase uses the existing Unreal engine and built-in Enhanced Input only; no external package installation occurred. | closed |

## Accepted Risks Log

| Risk ID | Threat Ref | Rationale | Accepted By | Date |
|---------|------------|-----------|-------------|------|
| AR-01 | T-01-SC/P01 | No third-party dependency was introduced in Plan 01. | Phase plan | 2026-10-08 |
| AR-02 | T-01-SC/P02 | No third-party dependency was introduced in Plan 02. | Phase plan | 2026-10-08 |
| AR-03 | T-01-SC/P03 | No third-party dependency was introduced in Plan 03. | Phase plan | 2026-10-08 |

## Security Audit Trail

| Audit Date | Threats Total | Closed | Open | Run By |
|------------|---------------|--------|------|--------|
| 2026-10-08 | 6 | 5 | 1 non-blocking | `gsd-security-auditor` |

## Sign-Off

- [x] All threats have a disposition.
- [x] Accepted risks are documented.
- [x] `threats_open: 0` confirmed for the configured `high` blocking threshold.
- [x] `status: verified` set in frontmatter.

**Approval:** verified 2026-10-08

