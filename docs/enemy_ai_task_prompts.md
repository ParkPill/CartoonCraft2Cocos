# Enemy AI Fix — Task Prompts (Cartoon Craft, Cocos2d-x 3.x)

Hand these to the coding AI **one at a time, in order**. Tasks 1–4 are bug fixes and are prerequisites for the later balance/design work. Wait for each task's verification to pass before moving to the next.

> **Shared context block** — paste this at the top of every prompt if you start a fresh session:
>
> ```
> Project context: This is an RTS game in C++ on Cocos2d-x 3.x. All gameplay
> code is in Classes/GameScene.cpp (class GameScene, ~16,500 lines). The
> single-player enemy AI is a rule-based loop: GameScene::updateEnemyAI()
> runs once per second and calls enemyAIUpdateFood(), enemyAIManageWorkers(),
> enemyAIManageShips(), enemyAITrainUnits() (internally gated to 30s), and
> enemyAICheckBuildings() (gated to 60s). The AI has its own resource pools
> (enemyGold, enemyLumber, enemyOil, enemyFoodInUse, enemyFoodMax) and builds
> instantly via buildTheBuilding() with isFree=true. Two races share this
> code: Human (HQ=UNIT_CASTLE, worker=UNIT_WORKER) and Orc (HQ=UNIT_ORC_HQ,
> renamed to UNIT_CASTLE on completion in setAfterBuildingProcess(),
> worker=UNIT_GOBLIN_WORKER). Diagnostic log() calls already exist at most
> failure branches. Do not change player-side behavior.
> ```

---

## TASK 1 — Replace distance-based building ownership with explicit HQ ownership (BUG, highest priority)

```
TASK: Fix a radius-mismatch bug in enemyAICheckBuildings() that causes
duplicate buildings and permanently failed tech prerequisites.

PROBLEM:
- enemyAIFindBuildTile() searches for placement tiles using Chebyshev
  distance (square ring, radius up to 22 tiles), and
  enemyAIFindWaterBuildTile() searches up to radius 30.
- But countNear() in enemyAICheckBuildings() counts existing buildings
  using Euclidean distance (circle, 22 * TILE_SIZE).
- A building placed in the "corner" of the square search area (e.g.
  dx=16, dy=16 → Euclidean ~22.6 tiles) is outside the circle, so
  countNear() never sees it. Result: the AI rebuilds the same building
  every 60s cycle forever, and prerequisite checks (e.g. Factory
  requires Lumbermill) can fail forever if the Lumbermill sits in a
  corner. Shipyards are worst: placed up to radius 30 but counted only
  within 22.

REQUIRED CHANGES:
1. Add an owner reference to AI-built buildings: when
   enemyAICheckBuildings() builds a building for a given HQ, store a
   pointer or unique ID of that HQ on the new building (add a member
   like `EnemyBase* ownerHQ` or `int ownerHQId` to the unit class —
   pick whichever fits the existing codebase conventions; if raw
   pointers risk dangling, prefer an ID and a lookup).
2. Rewrite countNear(type, hq) to count buildings whose owner is that
   HQ (and are alive), instead of using any distance check.
3. Apply the same ownership model to the prerequisite check
   (def.prereqType) and the food-building cap
   (kEAIMaxFoodBuildingsPerHQ).
4. Decide and implement what happens to owned buildings when their HQ
   is destroyed: simplest acceptable answer is to reassign them to the
   nearest surviving HQ on the next enemyAICheckBuildings() pass, or
   leave them ownerless (they just won't count toward any HQ's caps).
   Document the choice in a comment.
5. The HQ-construction branch (no-HQ case) and the oil extractor built
   in enemyAIManageShips() should also set an owner (nearest HQ, or
   null if none).

VERIFICATION:
- Run a Debug build for at least 10 build cycles (10+ minutes) on a
  land map. Confirm via the existing enemyAI[build] log lines that no
  building type other than the food building is ever built more than
  once per HQ.
- Confirm Factory/Airport (Human) or Troll House/Temple (Orc) get built
  after their prerequisite exists, even when the prerequisite was
  placed far from the HQ.
- On a water map, confirm exactly one Shipyard per HQ.
```

---

## TASK 2 — Fix the AI dead-stop when all enemy buildings are destroyed (BUG)

```
TASK: Fix updateEnemyAI() so the AI can rebuild its HQ after losing all
buildings.

PROBLEM:
- updateEnemyAI() early-returns if no enemy building with energy > 0
  exists (the hasEnemyBuilding gate).
- But enemyAICheckBuildings() contains a dedicated branch that builds a
  new HQ when workers exist but no HQ does. Because of the gate, this
  branch is unreachable in exactly the situation it was written for:
  if every enemy building is destroyed but workers survive, the AI
  permanently stops.

REQUIRED CHANGES:
1. Change the gate condition in updateEnemyAI() to: run the AI if there
   is at least one living enemy building OR at least one living enemy
   worker (UNIT_WORKER or UNIT_GOBLIN_WORKER with isEnemy == true).
2. Make sure the "no HQ → build HQ near a worker" branch in
   enemyAICheckBuildings() then actually executes and is not blocked by
   anything else that assumes a building exists (check
   enemyAIUpdateFood(), enemyAIManageWorkers(), enemyAIManageShips(),
   enemyAITrainUnits() all tolerate a zero-building state without
   crashing or asserting).
3. Keep the existing one-time "AI will not run" log but update its
   wording to reflect the new condition (no buildings AND no workers).

VERIFICATION:
- In a test game, destroy every enemy building (use a debug command or
  cheat if available) while leaving at least one enemy worker alive.
  Within the next 60s build cycle, confirm via the enemyAI[build] log
  that a new HQ is placed (subject to the AI having enough
  enemyGold/enemyLumber), and that the base then resumes normal
  worker/production behavior.
- Also verify the true dead state (no buildings, no workers) still
  cleanly stops the AI with the log message and no crash.
```

---

## TASK 3 — Complete the oil economy (BUG / missing logic)

```
TASK: The enemy AI's oil economy is incomplete: oil costs are never
checked or paid, refineries are never built, and the oil-ship cap can
collapse to zero. Fix all three.

PROBLEM DETAILS:
A) Every AI cost check uses only getGoldPriceForUnit() and
   getLumberPriceForUnit(). If any unit or building has an oil cost
   (check GameManager for an oil-price getter, e.g. BattleShips
   typically cost oil), the AI either builds it without paying oil or
   stockpiles enemyOil with no consumer.
B) Neither kHumanBuildList nor kOrcBuildList contains
   UNIT_HUMAN_OIL_REFINERY / UNIT_ORC_OIL_REFINERY (or Foundry). First
   investigate in Movable::moveNew() / setOilShipToExtractor() where an
   enemy oil ship deposits its cargo (what returningPlace it needs). If
   a refinery (or the shipyard/HQ) is required as the drop-off, the AI
   currently can never bank oil at all on some maps.
C) enemyAIUnitCap() caps oil ships at oilSpotCount *
   kEAIShipsPerExtractor, counting live UNIT_OIL_SPOT entries in
   mutualArray. Verify whether placing an extractor on a spot removes
   or kills the spot. If it does, oilSpotCount drops as extractors are
   built and the cap wrongly goes to 0 while working extractors exist.

REQUIRED CHANGES:
1. Add oil-cost handling to every AI production/build path
   (enemyAICheckBuildings, enemyAITrainUnits, enemyAIManageShips' 
   extractor build): check enemyOil >= oilCost and call
   addEnemyOil(-oilCost) on success. If no oil-price getter exists in
   GameManager, add one mirroring the gold/lumber getters.
2. Determine the oil drop-off requirement. If a refinery is needed,
   add it to both build lists (with the shipyard as prereqType and
   gated on mapHasWater), and make sure enemy oil ships get a valid
   returningPlace for oil.
3. Fix the oil-ship cap basis: count live, completed enemy oil
   extractors instead of raw oil spots if spots are consumed by
   extraction — cap = (extractors + unclaimed spots) *
   kEAIShipsPerExtractor is acceptable.

VERIFICATION:
- On a water map with 2+ oil spots, run 10+ minutes and confirm via
  logs/debug display that enemyOil increases over time, oil ships stay
  at the intended cap, and any oil-costing unit correctly waits until
  enemyOil is sufficient and deducts it when trained.
```

---

## TASK 4 — Harden returningPlace / currentOilExtractor lifetime (BUG, crash risk)

```
TASK: Prevent dangling pointers and stranded gatherers when a building
referenced by a worker or oil ship is destroyed.

PROBLEM:
- Enemy workers store returningPlace (deposit target) and oil ships
  store currentOilExtractor. If that building is destroyed and
  released mid-trip, the unit holds a dangling pointer →
  potential crash in Movable::moveNew()'s intersectsRect check, or at
  best a worker frozen forever carrying resources that are never
  credited to enemyGold/enemyLumber.
- Additionally, enemyAIManageWorkers() sets returningPlace =
  getNearestLumberTank(...) for ALL idle workers before deciding
  whether to send them to gold or wood. Verify setWorkerToMine()
  overwrites returningPlace with the correct gold drop-off; if it does
  not, gold workers may deposit at a lumber mill or nowhere.

REQUIRED CHANGES:
1. Find the central unit-death/removal path for buildings (wherever
   enemy buildings are removed from enemyArray and released). Add a
   sweep there: for every living unit, if unit->returningPlace ==
   dyingBuilding or unit->currentOilExtractor == dyingBuilding, set
   that pointer to nullptr and reset the unit's gather state enough
   that it becomes idle (unitAct = UNIT_ACT_NONE or equivalent), so
   the next enemyAIManageWorkers()/enemyAIManageShips() tick reassigns
   it.
2. In Movable::moveNew(), add null-guards before every dereference of
   returningPlace / currentOilExtractor if any are missing.
3. Verify and, if needed, fix the returningPlace assignment for gold
   workers (correct drop-off per resource type).

VERIFICATION:
- Test: let enemy workers gather, then destroy their drop-off building
  (HQ or lumber mill) while a worker is mid-trip carrying resources.
  Confirm no crash, and confirm the worker is reassigned within a few
  seconds (visible as it resuming gathering).
- Same test for an oil ship's extractor.
```

---

## TASK 5 — Food/resource prioritization and army size (BALANCE, biggest gameplay impact)

```
TASK: The enemy AI's army is permanently tiny because food is the real
bottleneck and workers win the food race. Rebalance.

PROBLEM:
- Current max food: Human = HQ 6 + 4 Farms * 7 = 34; Orc = HQ 6 + 4
  Barbecues * 6 = 30. With 10 workers eating ~10 food, only ~20-24
  food remains for the entire army. The per-unit-type caps (10 each)
  are unreachable; the theoretical 100 food ceiling is irrelevant.
- Worker training (HQ) and combat training (Barracks etc.) draw from
  the same enemyGold/enemyLumber/food pool with no prioritization;
  the HQ usually iterates first in enemyArray, so workers
  consistently win contested resources.

REQUIRED CHANGES:
1. Define a target army food budget as a named constant (suggest
   kEAIArmyFoodBudget = 50) and derive the food-building count from
   it: raise kEAIMaxFoodBuildingsPerHQ so that
   HQ food + farms/barbecues >= worker food (10) + kEAIArmyFoodBudget.
   (Human: ~8 Farms; Orc: ~9 Barbecues. Compute, don't hardcode
   blindly — read the actual per-building food values.)
2. Add worker/army prioritization in enemyAITrainUnits():
   - Define kEAITargetWorkers (keep 10 per HQ).
   - While workers < 6 per HQ: workers may use all resources (economy
     bootstrapping).
   - Once workers >= 6 per HQ: reserve headroom for combat — the HQ
     may only train another worker if (enemyFoodMax - enemyFoodInUse)
     > some margin (suggest 8) AND enemyGold > some reserve (suggest
     300). Combat buildings are exempt from the reserve.
3. Keep the enemyFoodMax hard cap at 100 but confirm the new farm
   counts don't exceed it.

VERIFICATION:
- Full test game, 15+ minutes: log enemyFoodInUse/enemyFoodMax once
  per minute. Confirm the AI reaches and sustains a combat force of
  30+ food worth of army units (visible in logs / on the map), and
  that worker count stabilizes at the target instead of consuming all
  food.
```

---

## TASK 6 — Attack wave system (DESIGN, makes the AI feel threatening)

```
TASK: Enemy combat units currently trickle toward the player one at a
time the moment they spawn (updateUnitMoveNew gives every idle unit an
independent findTargetHero + forceAttack). Replace this with a
rally-and-wave system.

REQUIRED BEHAVIOR:
1. Rally: newly trained enemy combat units (everything except workers,
   oil ships, shuttles) should move to a rally point near their HQ
   (e.g. a fixed offset from the HQ toward the map center, or toward
   the nearest player building if known) and hold there. Holding units
   must still defend themselves: keep the existing revenge/aggro
   behavior when attacked or when a player unit comes within their
   sight range, but do NOT let them chase across the map from the
   rally state.
2. Wave trigger: track the total food value of units holding at the
   rally. When it reaches a threshold, release all of them as one
   attack wave using the existing forceAttack/findTargetHero flow.
   Make the threshold scale over time: first wave at ~8 food, then
   +4 food per subsequent wave, capped at ~24 (tune as named
   constants: kEAIFirstWaveFood, kEAIWaveFoodStep, kEAIMaxWaveFood).
3. Defense override: if any enemy building takes damage, immediately
   release all rallied units to attack the nearest player unit near
   that building (base defense beats wave timing).
4. Implementation constraints:
   - Add a unit state/flag (e.g. isRallying) rather than a new global
     system; integrate with the existing unitAct state machine so
     updateUnitMoveNew()'s idle-target-search skips rallying units.
   - Drive the wave logic from updateEnemyAI() (1s tick), not
     per-frame.
   - Multiplayer (isMultiplay) must be completely unaffected.

VERIFICATION:
- Test game: confirm units visibly gather at the rally point, then
  attack together in groups; confirm attacking the enemy base pulls
  the rallied group into defense; confirm no unit is permanently stuck
  in the rally state (add a safety timeout of e.g. 180s that releases
  a wave regardless of size).
```

---

## TASK 7 — Round-robin training, race caching, cleanup (QUALITY)

```
TASK: Three smaller improvements to the enemy AI. Apply all in one pass.

1. ROUND-ROBIN UNIT TRAINING:
   In enemyAITrainUnits(), each production building currently always
   tries its unit list from index 0 and breaks after the first
   attempt, so a Barracks trains 10 Swordmen before the first Archer.
   Change to round-robin: store a per-building "next unit index"
   (add an int member on the unit/building class, or a
   std::map<EnemyBase*, int> keyed by building), start each tick's
   attempt at that index, advance it after every successful training,
   and wrap around. Skip entries that are at cap or unaffordable and
   try the next, instead of giving up for the tick.

2. CACHE THE ENEMY RACE:
   isOrc is currently re-derived every tick by scanning enemyArray for
   any isOrcTypeUnit() match. This has a real bug: completed
   UNIT_ORC_HQ is renamed to UNIT_CASTLE in setAfterBuildingProcess(),
   so an Orc base whose only living unit is its finished HQ evaluates
   as Human for that tick (wrong build list / worker type). Fix:
   detect the race ONCE inside updateEnemyAI()'s one-time init block
   (enemyAIInitialized) and store it in a new member (e.g. int
   enemyAIRace or bool enemyAIIsOrc). Replace all three per-tick scans
   in enemyAICheckBuildings/enemyAITrainUnits/enemyAIManageShips with
   the cached value. Note: at init time the map's starting enemy units
   are already spawned, so the scan-once approach is safe; if the
   enemy could ever start with zero units, fall back to re-detecting
   on the first tick where enemyArray is non-empty.

3. CLEANUP:
   - In setAfterBuildingProcess(), the condition
     `unit->unitType == UNIT_ORC_HQ` inside the startProductSchedule()
     check is dead code (the type was just rewritten to UNIT_CASTLE
     one line earlier). Remove it, with a comment explaining the
     rename.
   - In enemyAIManageWorkers(), the guard
     `if ((int)nearWorkers.size() > kEAIMaxWorkersPerHQ) break;` is
     off-by-one (allows 11) and nearWorkers never grows inside the
     loop, so it's a static check. Move it to a proper condition:
     compute assignedCount = nearWorkers.size() before the loop and
     stop assigning when assignedCount >= kEAIMaxWorkersPerHQ,
     incrementing it per assignment.

VERIFICATION:
- Confirm via logs that a Barracks alternates Swordman/Archer (or
  OrcAxe/OrcSpear) instead of massing one type first.
- Confirm an Orc AI game behaves identically before and after (same
  build list used) including after all goblin workers are killed.
- Confirm worker count per HQ never exceeds kEAIMaxWorkersPerHQ.
```

---

## TASK 8 (OPTIONAL / LATER) — Expansion bases

```
TASK: Add expansion logic so the AI doesn't economically die when its
starting gold mine depletes. PREREQUISITE: Task 1 (HQ ownership) must
be complete, since multiple HQs share the single enemyGold/enemyLumber
pool and building caps must be per-HQ for this to work.

REQUIRED BEHAVIOR:
1. Depletion detection: in updateEnemyAI() (low frequency is fine,
   e.g. piggyback on the 60s build tick), for each HQ check whether
   any living UNIT_MINE exists within a working radius (reuse the
   28-tile worker radius). If not, and enemy resources afford an HQ,
   find the nearest living mine that has no enemy HQ within that
   radius and no player building within a safety radius.
2. Build a new HQ near that mine using the existing
   enemyAIFindBuildTile(..., avoidMines=true) + buildTheBuilding()
   instant-build pattern, then let the existing per-HQ worker/build
   logic populate it naturally.
3. Cap total simultaneous enemy HQs (suggest kEAIMaxHQs = 3).
4. Watch out: enemyAIManageWorkers() assigns workers to the globally
   nearest mine from the worker's position — verify workers at the
   new HQ don't walk back to the old depleted area, and that the food
   math (Task 5) still holds with multiple HQs contributing +6 each.

VERIFICATION:
- Test map where the starting mine has low gold: confirm the AI
  builds a second HQ near a fresh mine after depletion and resumes
  income (enemyGold rising in logs).
```

---

## How to run this process

- Give the coding AI **one task per session**, always with the shared context block.
- Require it to end every task with: (a) a diff-style summary of changed functions, (b) confirmation that each VERIFICATION item was actually tested (with the relevant log output pasted), and (c) a list of any assumptions it had to make about code it couldn't see.
- If the AI reports it cannot find a function or member mentioned in the prompt, have it paste the closest matching code back to you before it invents a replacement.
