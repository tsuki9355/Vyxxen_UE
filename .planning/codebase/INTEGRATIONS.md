---
last_mapped_commit: 97fba1f2e8fc0676f4f28f5e659f174a01754612
last_mapped_at: 2026-10-07
---
# External Integrations

**Analysis Date:** 2026-10-07

## APIs & External Services

**Application services:**
- No HTTP client, external API SDK, database client, or online subsystem implementation is detected in `Source/Vyxxen/`.
- `Source/Vyxxen/Vyxxen.Build.cs` declares only `Core`, `CoreUObject`, `Engine`, and `InputCore`. Its OnlineSubsystem/Steam instructions are comments, not active integrations.
- `Vyxxen.uproject` explicitly enables only the editor-scoped `ModelingToolsEditorMode` plugin. Engine-default plugins may exist outside this project declaration; their enablement is not exhaustively established here.
- Blueprint service calls cannot be ruled out from filenames: internals of `Content/Units/PlayerShip/BP_vyxxen-ship.uasset` and `Content/Scenery/BP_level1_scenery.uasset` are unverified.

**Development device transfer:**
- Android File Server runtime settings are present in `Config/DefaultEngine.ini`.
  - SDK/Client: engine AndroidFileServer-related configuration; no custom project client appears in `Source/`.
  - Auth: a `SecurityToken` field is present in `Config/DefaultEngine.ini`; its value is deliberately omitted.
  - Settings enable the plugin setting and allow network connection, while selecting `USBOnly`; shipping inclusion and external shipping start are disabled.
  - These are configuration facts only; no server listener, device connection, or Android package was tested.

## Data Storage

**Databases:**
- Not detected in `Source/Vyxxen/Vyxxen.Build.cs` or `Source/Vyxxen/Private/PlayerShip.cpp`; no ORM, connection variable, schema, or persistence layer is declared.

**File Storage:**
- Project-local Unreal asset packages in `Content/`, including `Content/Levels/MainGame.umap`, ship assets under `Content/Units/PlayerShip/`, and scenery under `Content/Scenery/`.
- Generated editor/runtime output uses local `Saved/`, `Intermediate/`, and `Binaries/` directories excluded by `.gitignore`; no cloud object-storage client is detected in `Source/`.
- No project SaveGame implementation is detected in `Source/Vyxxen/`; save behavior inside binary `Content/` assets is unverified.

**Caching:**
- Local `DerivedDataCache/` exists and is excluded by `.gitignore`; remote/shared cache configuration is not established by the project files inspected.
- No application cache service is declared in `Source/Vyxxen/Vyxxen.Build.cs`.

## Authentication & Identity

**Auth Provider:**
- No player login, identity provider, or online account integration is declared in `Vyxxen.uproject` or implemented in `Source/Vyxxen/`.
- The Android file-server authentication field in `Config/DefaultEngine.ini` belongs to development transfer configuration; do not describe it as player authentication.

## Monitoring & Observability

**Error Tracking:**
- No project-integrated error-tracking service is detected in `Vyxxen.uproject` or `Source/Vyxxen/Vyxxen.Build.cs`; engine-provided crash behavior is outside the inspected project implementation.

**Logs:**
- No custom log category or `UE_LOG` call is present in `Source/Vyxxen/`; `Source/Vyxxen/Private/PlayerShip.cpp` contains only actor lifecycle scaffolding.
- `Saved/` is the local generated-output directory excluded by `.gitignore`; this mapping does not inspect potentially sensitive log contents.
- Android File Server logging/stat reporting settings are disabled in `Config/DefaultEngine.ini`.

## CI/CD & Deployment

**Hosting:**
- Native desktop application targeting is configured in `Source/Vyxxen.Target.cs` and `Config/DefaultEngine.ini`; no web hosting, cloud backend, store publishing, or dedicated server destination is declared.
- Windows, Linux, and Mac graphics target settings in `Config/DefaultEngine.ini` are not proof of successful platform packaging.

**CI Pipeline:**
- No `.github/workflows/`, build-pipeline manifest, or project automation script is detected at the repository root. Build definitions live in `Source/Vyxxen.Target.cs`, `Source/VyxxenEditor.Target.cs`, and `Source/Vyxxen/Vyxxen.Build.cs`.
- Generated `Automation_Vyxxen.sln` and `Automation_Vyxxen.slnx` are local IDE artifacts, not evidence of a configured CI provider.
- Git text normalization is configured in `.gitattributes`; Git LFS filters are not declared there despite binary Unreal files under `Content/`.

## Environment Configuration

**Required env vars:**
- No application-defined environment variables detected in `Source/`; engine association is selected by `Vyxxen.uproject` and project behavior by `Config/Default*.ini`.
- Native tooling prerequisites are declared in `.vsconfig`; build-machine environment requirements beyond those declarations are not verified.

**Secrets location:**
- `Config/DefaultEngine.ini` contains a development file-server authentication field. Document only field presence and purpose; do not copy its value.
- No dedicated secret-management implementation is detected in `Source/`; no secret files or user credential stores are inspected.

## Webhooks & Callbacks

**Incoming:**
- No application webhook endpoint or HTTP server is detected in `Source/Vyxxen/`. Android device transfer configuration in `Config/DefaultEngine.ini` is not an application webhook.

**Outgoing:**
- No outbound webhook/API implementation is detected in `Source/Vyxxen/`; native actor callbacks in `Source/Vyxxen/Private/PlayerShip.cpp` are Unreal lifecycle callbacks only.
- Inspect binary Blueprint graphs in Unreal before asserting exhaustive absence of integrations in `Content/`.

---

*Integration audit: 2026-10-07*
