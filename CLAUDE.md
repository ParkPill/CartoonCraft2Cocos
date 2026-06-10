# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

**Windows (primary development platform):**
Open and build `proj.win32/CartoonCraftNV.sln` in Visual Studio. The CMakeLists.txt is a Cocos2d-x template stub and does not reflect the full source list; the VS project is the authoritative build configuration for Win32.

**Android:**
Build via `proj.android/` using Android Studio or the Cocos command-line tools.

**iOS/Mac:**
Build via `proj.ios_mac/` using Xcode.

Resources are loaded from the `Resources/` directory at runtime.

## Architecture

This is a **Cocos2d-x 3.x C++ game** ("Cartoon Craft") — a real-time strategy/action game with single-player campaigns, a map editor, and multiplayer modes. All game source lives in `Classes/`.

### Core singletons (accessed via macros defined in `GameManager.h`)

| Macro | Class | Role |
|-------|-------|------|
| `GM` | `GameManager` | Central hub: audio, IAP, save data, pathfinding, UI helpers, scene transitions |
| `HUD` | `HudLayer` | In-game HUD: command buttons, joystick, inventory UI |
| `EHUD` | `EditorHud` | Editor mode HUD |
| `WORLD` | `HelloWorld` | The active game world layer |
| `BSM` | `BuggyServerManager` | HTTP backend (REST/JSON via rapidjson), server time sync |
| `MM` | `MultiplayManager` | Multiplayer session state |
| `SM` | `ServerManager` | Additional server operations |
| `LM` | `LanguageManager` | Localization; text looked up from CSV language sheets |

### Key classes

- **`HelloWorld`** (`HelloWorldScene.h/cpp`) — The main game stage layer. Owns all active unit arrays (`heroArray`, `enemyArray`, `MovableArray`, etc.), tile map management, collision resolution, combat loop, fog of war, weapon system, and multiplayer frame sync. This is by far the largest class.
- **`Movable`** — Base for every moving entity (heroes, enemies, missiles, coins, items). Holds physics state, animation, unit stats.
- **`EnemyBase`** (in `Enemies.h`) — Extends `Movable`; represents all combat units and buildings (both hero-side and enemy-side).
- **`Title`** (extends `PageBase`) — Title screen; handles chapter select, account registration, server select.
- **`EditorWorld`** — Map/level editor scene; parallels `HelloWorld` for editing tile maps.
- **`AStar`** — A* pathfinding; `GameManager` holds the instance and exposes `getPath()`.
- **`Fog`** — Fog-of-war sprites managed by `HelloWorld::updateFog()`.

### Save data

All persistent data goes through `cocos2d::UserDefault`. Every key is a `#define KEY_*` constant in `GameManager.h`. The convenience macros `UDGetInt`, `UDSetInt`, `UDGetBool`, `UDSetBool`, `UDGetStr`, `UDSetStr` wrap `UserDefault` calls. **Do not use `UDSetDouble` / `setDoubleForKey`** — it is broken on some Android devices; store doubles as ints via `UDSetInt`.

### Data tables

Unit stats, weapon stats, item stats, shop data, etc. are loaded from CSV/plist files into `ValueMap` tables on `HelloWorld` (`unitStatTable`, `weaponStatTable`, etc.) via `GM->loadCSV()`. Access helpers like `getUnitStat()`, `getWeaponStat()`, `getItemStat()` are on `HelloWorld`.

### Platform-specific code

Win32-specific input handling (mouse, edge-scroll, attack-move mode, worker radial menu) is gated with `#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)` blocks throughout `HelloWorldScene.h/cpp`. The static helper `HelloWorld::usesPcRtsControls()` is the runtime gate.

### Design resolution

2368 × 1334 points (landscape). `imageScale = 0.5f` is the default sprite scale applied across the game.

### Useful utility macros (GameManager.h)

- `strmake` → `StringUtils::format` (printf-style string building)
- `SPT_REMOVE_FUNC` — creates a `CallFunc` that removes a sprite from its parent
- `BTN_FROM_REF_AND_DISABLE_FOR_A_SEC` — standard button-debounce pattern
- `SHAKE_ONCE` — one-frame screen shake action
