#include "MapEditor.h"
#include "GameManager.h"
#include "LanguageManager.h"
#include "ui/UIScale9Sprite.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <editor-support/spine/spine-cocos2dx.h>
#include <editor-support/spine/SkeletonAnimation.h>
#include <algorithm>
#include <chrono>
#include <cstring>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commdlg.h>
#include "platform/desktop/CCGLViewImpl-desktop.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#include "MacFileDialog.h"
#endif

USING_NS_CC;
using namespace cocos2d::ui;

namespace {
    const int MAP_TILE_SIZE = 50;
    const int TILESET_COLUMNS = 10;
    const int BASE_GRASS_TILE_ID = 0;
    const int FULL_SOIL_TILE_ID = 41;
    const int WATER_TILE_ID_OFFSET = 110; // water block sits 11 rows below the soil block

    const int MIN_MAP_SIZE = 5;
    const int MAX_MAP_SIZE = 200;

    const char* const SAVE_FILE_NAME = "mapeditor_save.json";
    const int SAVE_FORMAT_VERSION = 1;

    const char* const BTN_SKIN_NORMAL = "uiBoxSmall.png";
    const char* const BTN_SKIN_SELECTED = "uiBoxSmallBlue.png";
    // btnGray.png/btnGreen.png are 267x236 with a thick hand-drawn border.
    // 24pt insets (vs. the art's full ~50pt border) are sized so the editor's
    // many ~48pt-tall buttons can render the corner/border art undistorted —
    // a 9-slice needs (top+bottom inset) <= button height to avoid squashing.
    const Rect BTN_SKIN_CAP_INSETS(24, 24, 219, 188);

    // HelloWorld::createUnit renders units/trees at native sprite size (scaleX=1,
    // no extra reduction) — e.g. tree2_0.png is 75x75 against a 50px tile, deliberately
    // overlapping neighbors for a full canopy look. Match that here so the editor
    // preview matches actual in-game density instead of looking sparser.
    const float OBJECT_SPRITE_SCALE = 1.0f;

    struct ObjectTypeDef {
        const char* name;
        // Regular units: a sprite frame name in CartoonCraftNV.plist. Heroes
        // (isHero=true) repurpose this same field to hold the Spine skeleton's
        // base filename instead (e.g. "orc" for Resources/spine/orc.json +
        // orc.atlas) - see GameManager::getHeroSpine, which this mirrors.
        const char* spriteFrame;
        int race;
        // Mirrors GameManager::getUnitMaxHP(), the actual hardcoded source of
        // truth HelloWorldScene uses at runtime — there is no CSV/data-table
        // backing unit stats in this game. -1 = not a real unit, no HP.
        // Always -1 for heroes: their HP/ATT scale with Level (PlacedObject::level)
        // via GM->getUnitHP/getUnitATT, not a flat value.
        int baseHp;
        // Extra line shown in the palette hover tooltip, for entries whose
        // role isn't obvious from the name/icon alone. Empty = tooltip just
        // shows the name and base HP.
        const char* tooltip;
        // Naval units: placeable on water tiles only. Aggregates can omit a
        // member with a default initializer, so existing entries above don't
        // need editing.
        bool requiresWater = false;
        // Spine-animated gacha hero instead of a regular sprite-based unit.
        bool isHero = false;
        // Only meaningful when isHero: overrides the skeleton's default skin
        // (the 3 "crazy" werewolf/bear/lion variants reuse a base skeleton
        // with a different skin instead of having their own). Empty = default skin.
        const char* spineSkin = "";
        // True for the "crazy" werewolf/bear/lion heroes: their crazy_* skin
        // was never actually exported (crazy_bear/crazy_lion don't exist as
        // skins in bear.json/lion.json - renders with zero attachments, i.e.
        // invisible; crazywerewolf's split-off skeleton file was abandoned).
        // Kept as real roster entries rather than deleted, because every
        // entry after them is saved by array index in map JSON (see
        // kEditorTypeToUnit in GameScene.cpp) - existing maps already
        // reference indices past this point (Resources/stage0_0.json,
        // stage1_1.json both place "Camera Start"). Deleting these 3 slots
        // would silently shift every later index and corrupt those maps, so
        // they're excluded from the palette (rebuildObjectPaletteRow /
        // cycleHeroPage) instead of removed from the table.
        bool hiddenFromPalette = false;
    };
    // Roster + sprite names copied from HelloWorld::getSpriteNameForUnit /
    // GameManager::getUnitName (Movable.h UNIT_* constants). Non-placeable
    // markers (start/event points, missiles, triggers) are intentionally
    // excluded. Gacha-style RPG heroes (UNIT_HERO_*) ARE included, in their
    // own RACE_HERO tab — see ObjectTypeDef's isHero/spineSkin comments.
    const char* const kHeroTooltip = "Gacha hero - HP/ATT scale with Level (see Level field)";
    const ObjectTypeDef kObjectTypes[] = {
        // Human
        {"Worker", "workerAxeStand0.png", MapEditor::RACE_HUMAN, 60, "Gathers resources, builds and repairs buildings"},
        {"Swordsman", "swordmanStand0.png", MapEditor::RACE_HUMAN, 150, ""},
        {"Archer", "archerStand0.png", MapEditor::RACE_HUMAN, 90, ""},
        {"Catapult", "catapult0.png", MapEditor::RACE_HUMAN, 400, ""},
        {"Helicopter", "helicopter0.png", MapEditor::RACE_HUMAN, 70, ""},
        {"Castle", "castle.png", MapEditor::RACE_HUMAN, 4000, ""},
        {"Farm", "farm.png", MapEditor::RACE_HUMAN, 700, ""},
        {"Barracks", "barracks.png", MapEditor::RACE_HUMAN, 1400, ""},
        {"Lumber Mill", "lumberMill.png", MapEditor::RACE_HUMAN, 1000, ""},
        {"Watcher Tower", "watcherTower.png", MapEditor::RACE_HUMAN, 500, ""},
        {"Factory", "factory.png", MapEditor::RACE_HUMAN, 1500, ""},
        {"Airport", "airport.png", MapEditor::RACE_HUMAN, 2000, ""},
        {"Human Ship", "humanShuttle.png", MapEditor::RACE_HUMAN, 100,
            "Transport - carries units, no attack. Water tiles only, occupies 1 tile, can't share a tile with another ship", true},
        // Orc
        {"Orc Axe", "orcAxeStand0.png", MapEditor::RACE_ORC, 180, ""},
        {"Orc Spear", "orcSpearStand0.png", MapEditor::RACE_ORC, 120, ""},
        {"Goblin", "goblinStand0.png", MapEditor::RACE_ORC, 80, ""},
        {"Goblin Bomb", "goblinBombStand0.png", MapEditor::RACE_ORC, 80, ""},
        {"Goblin Worker", "goblinAxeStand0.png", MapEditor::RACE_ORC, 60, "Gathers resources, builds and repairs buildings"},
        {"Troll", "trollStand0.png", MapEditor::RACE_ORC, 580, ""},
        {"Wizard", "wizardStand0.png", MapEditor::RACE_ORC, 40, ""},
        {"Orc HQ", "hq.png", MapEditor::RACE_ORC, 2000, ""},
        {"Orc Bunker", "bunker.png", MapEditor::RACE_ORC, 800, ""},
        // GameManager::getUnitMaxHP() has no explicit case for these three —
        // they fall through to its generic default (60), so that's what they
        // actually have in-game today.
        {"Orc Barracks", "axeport.png", MapEditor::RACE_ORC, 60, ""},
        {"Troll House", "statueHouse.png", MapEditor::RACE_ORC, 60, ""},
        {"Temple", "alter.png", MapEditor::RACE_ORC, 60, ""},
        {"Barbecue", "barbecue.png", MapEditor::RACE_ORC, 400, ""},
        {"Orc Ship", "orcShuttle.png", MapEditor::RACE_ORC, 100,
            "Transport - carries units, no attack. Water tiles only, occupies 1 tile, can't share a tile with another ship", true},
        // Undead
        {"Zombie Orc", "zombieOrc0.png", MapEditor::RACE_UNDEAD, 150, ""},
        {"Zombie Swordsman", "zombieHuman0.png", MapEditor::RACE_UNDEAD, 120, ""},
        {"Zombie Castle", "castle.png", MapEditor::RACE_UNDEAD, 4000, ""},
        {"Zombie HQ", "hq.png", MapEditor::RACE_UNDEAD, 2000, ""},
        // Neutral
        {"Tree", "tree2_0.png", MapEditor::RACE_NEUTRAL, 20, "Neutral resource - workers chop it for wood"},
        {"Mine", "mine.png", MapEditor::RACE_NEUTRAL, 50000, "Neutral resource node"},
        // Not a gameplay unit — a placeable location marker. Trigger actions can
        // target a flag (or any other placed object) by id instead of typing raw
        // tile coordinates; see TriggerAction::targetObjectId.
        // spot0.png is a loose file in Resources/ (not a packed sheet frame) -
        // createIconButton falls back to file loading, and spawnObjectAt
        // swaps in the animated version via GM->createSpotFlagSprite().
        {"Flag", "spot0.png", MapEditor::RACE_NEUTRAL, -1, "Trigger target marker only - not a unit, no HP. Edit it to show/hide the flag art in-game"},
        // Heroes — Spine-animated gacha units (Movable.h UNIT_HERO_*). HP/ATT
        // scale with Level instead of a flat baseHp; see ObjectTypeDef comments
        // and GameManager::getSpineFileName/getHeroSpine, which this mirrors.
        // "Hero " prefix disambiguates from same-named regular units (Archer, Wizard).
        {"Hero Orc", "orc", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Goblin", "goblin", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Spearman", "spearMan", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Lizardman", "lizard", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Archer", "archer", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        // werewolf/bear/lion skeletons have no "default" skin - their single
        // skin must be set by name (mirrors GameManager::getHeroSpine). The
        // werewolf/crazywerewolf file-split experiment was abandoned; werewolf
        // is back to its original file with skin named "werewolf".
        {"Hero Werewolf", "werewolf", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "werewolf"},
        {"Hero Monk", "monk", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Fighter", "fighter", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Bear", "bear", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "bear"},
        {"Hero Healer", "healer", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Knight", "knight", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Elf Swordsman", "elfSwordMan", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Assassin", "assassin", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Lion", "lion", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "lion"},
        {"Hero Wizard", "wizard", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Tanker", "tanker", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Skeleton", "skeleton", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Reaper", "necromancer", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Ent", "ent", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Salamander", "salamander", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Undine", "undine", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        // The 3 "crazy" variants reuse a base skeleton with a different skin -
        // one that was never actually exported, so they render with zero
        // attachments (invisible). Hidden from the palette; see
        // ObjectTypeDef::hiddenFromPalette for why these stay in the table
        // instead of being deleted outright.
        {"Hero Crazy Werewolf", "werewolf", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "crazy_werewolf", true},
        {"Hero Crazy Bear", "bear", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "crazy_bear", true},
        {"Hero Crazy Lion", "lion", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "crazy_lion", true},
        {"Hero Lady Werewolf", "werewolfFemale", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Lady Lion", "femaleLion", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Lady Bear", "ladybear", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Santa", "santa", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Rudolph", "rudolph", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Santa Dog", "santadog", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Penguin", "penguin", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Cat in Boots", "catinboots", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Mole", "mole", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Toy Mouse", "robotMouse", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Savage Archer", "savageArcher", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Bat Monster", "batmonster", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Meat", "green_meat", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Parasite", "parasite", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Watermelon", "watermelon", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Baby Mino", "minobaby", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Mino", "mino", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Kerberos", "kerberos", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Lamia", "lamia", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Chunja", "chunja", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Golem", "golem", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        // indices 79-81: ships (appended after heroes)
        {"Human Battle Ship", "humanShip.png", MapEditor::RACE_HUMAN, 400,
            "Ranged combat ship - attacks with cannonballs. Water only.", true},
        {"Orc Battle Ship", "orcShip.png", MapEditor::RACE_ORC, 400,
            "Ranged combat ship - attacks with cannonballs. Water only.", true},
        {"Human Dreadnought", "humanBattleShip.png", MapEditor::RACE_HUMAN, 600,
            "Heavy warship - slow, high HP, fires explosive battleCanonBall with splash damage. Water only.", true},
        // index 82: camera start marker
        {"Camera Start", "blueFlag0.png", MapEditor::RACE_NEUTRAL, -1,
            "Sets the initial camera position when the map is loaded. Place one per map."},
        // indices 83-84: oil system
        {"Oil Spot", "oilSpot.png", MapEditor::RACE_NEUTRAL, 50000,
            "Oil deposit on water. Place here for Oil Ship to build Oil Extractor.", true},
        {"Human Oil Ship", "humanOilShip.png", MapEditor::RACE_HUMAN, 150,
            "Builder ship - builds Oil Extractor on oil spots. Water only.", true},
        // index 85: Orc Oil Ship. Fills the one slot where kEditorTypeToUnit
        // (GameScene.cpp) already had UNIT_ORC_OIL_SHIP but this roster had no
        // matching entry - the two tables index-align by position, so this must
        // stay before the appended buildings below.
        {"Orc Oil Ship", "orcOilShip.png", MapEditor::RACE_ORC, 150,
            "Builder ship - builds Oil Extractor on oil spots. Water only.", true},
        // indices 86-93: buildings that are normally worker/ship-constructed
        // in-game (shipyards, oil extractors/refineries, foundries) plus their
        // Orc counterparts. Added so the editor can place them and so the
        // Building Count trigger condition can target them by type. HP mirrors
        // GameManager::getUnitMaxHP() (Human Foundry/Oil Refinery fall through
        // to its 60 default); requiresWater matches GameScene's
        // isWaterBuildingType() (shipyards, extractors, refineries are on water;
        // foundries are land buildings).
        {"Human Shipyard", "humanShipyard.png", MapEditor::RACE_HUMAN, 2800,
            "Builds and repairs ships. Water only.", true},
        {"Orc Shipyard", "orcShipyard.png", MapEditor::RACE_ORC, 2800,
            "Builds and repairs ships. Water only.", true},
        {"Human Oil Extractor", "humanOilExtractor.png", MapEditor::RACE_HUMAN, 1500,
            "Extracts oil from an oil spot. Water only.", true},
        {"Orc Oil Extractor", "orcOilExtractor.png", MapEditor::RACE_ORC, 1500,
            "Extracts oil from an oil spot. Water only.", true},
        {"Human Foundry", "humanFoundary.png", MapEditor::RACE_HUMAN, 60,
            "Processes ore into metal for advanced units."},
        {"Orc Foundry", "orcFoundary.png", MapEditor::RACE_ORC, 1500,
            "Processes ore into metal for advanced units."},
        {"Human Oil Refinery", "humanOilRefinery.png", MapEditor::RACE_HUMAN, 60,
            "Refines oil into fuel. Water only.", true},
        {"Orc Oil Refinery", "orcOilRefinery.png", MapEditor::RACE_ORC, 1500,
            "Refines oil into fuel. Water only.", true},
        // index 94: Orc Battle Ship (UNIT_ORC_BATTLE_SHIP) - the Orc counterpart
        // of "Human Dreadnought" (UNIT_HUMAN_BATTLE_SHIP) above. The editor had
        // no slot for it even though it's a normal buildable Orc Shipyard unit.
        // Appended at the end (index-aligned with kEditorTypeToUnit in
        // GameScene.cpp) so existing maps' saved indices don't shift. Named
        // "Dreadnought" to match its human twin and avoid colliding with the
        // "Orc Battle Ship" label already used above for UNIT_ORC_SHIP. HP
        // mirrors GameManager::getUnitMaxHP() = 800.
        {"Orc Dreadnought", "orcBattleShip.png", MapEditor::RACE_ORC, 800,
            "Heavy warship - slow, high HP, fires explosive battleCanonBall with splash damage. Water only.", true},
    };
    const int kObjectTypeCount = sizeof(kObjectTypes) / sizeof(kObjectTypes[0]);
    const int kRaceCount = 5;
    const char* const kRaceNames[kRaceCount] = {"Human", "Orc", "Undead", "Neutral", "Heroes"};
    const int kHeroesPerPage = 12;

    const int kSideCount = 4;
    const char* const kSideNames[kSideCount] = {"Ally", "Enemy", "Neutral", "Event"};
    const char* const kSideDescriptions[kSideCount] = {
        "Ally", "Enemy",
        "Neutral - joins whoever approaches it",
        "Neutral - only changes hands via a trigger/event"
    };

    const char* const kConditionTypeNames[] = {"Always", "Elapsed Time", "Switch", "Unit Count", "Resource", "Unit Arrives", "Building Count", "Unit Dies"};
    const char* const kTriggerActionTypeNames[] = {
        "Display Message", "Create Unit", "Remove Unit", "Set Switch",
        "Victory", "Defeat", "Wait", "Center Camera", "Talk", "Reveal Fog",
        "Order Attack", "Move Unit", "Lock Control", "Teleport Unit"
    };
    const char* const kComparisonNames[] = {"At Least", "At Most", "Exactly"};
    const char* const kSwitchStateNames[] = {"Set", "Cleared"};
    const char* const kSwitchActionNames[] = {"Set", "Clear", "Toggle"};
    const char* const kResourceKindNames[] = {"Gold", "Tree", "Oil"};

    // Cyclically advances value by dir, wrapping within [minInclusive, maxInclusive].
    int cyclicAdd(int value, int dir, int minInclusive, int maxInclusive) {
        int range = maxInclusive - minInclusive + 1;
        int rel = ((value - minInclusive) + dir) % range;
        if (rel < 0) {
            rel += range;
        }
        return rel + minInclusive;
    }

    // "Any" + every roster entry, for unit-type cycle rows. -1 means "Any".
    std::string unitTypeCycleName(int unitTypeIndex) {
        if (unitTypeIndex < 0) {
            return "Any";
        }
        return kObjectTypes[unitTypeIndex].name;
    }

    // Which roster entries are buildings, mirroring GameManager::isThisBuilding()
    // (the runtime source of truth for BuildingCount). The editor stays free of
    // any GM dependency, so this name list is kept in sync by hand; it filters
    // the Building Count trigger condition's Unit Type dropdown to buildings.
    // Names must match kObjectTypes[].name exactly (same pattern as the existing
    // strcmp(def.name, "Flag") checks). Shipyards/oil extractors/foundries are
    // buildings at runtime too but aren't placeable in the editor, so they have
    // no roster entry and are simply absent here.
    bool isBuildingObjectType(int typeIndex) {
        if (typeIndex < 0 || typeIndex >= kObjectTypeCount) {
            return false;
        }
        static const char* const kBuildingNames[] = {
            "Castle", "Farm", "Barracks", "Lumber Mill", "Watcher Tower",
            "Factory", "Airport", "Orc HQ", "Orc Bunker", "Orc Barracks",
            "Troll House", "Temple", "Barbecue", "Zombie Castle", "Zombie HQ",
            "Mine", "Oil Spot",
            // Shipyards, oil extractors/refineries, foundries (+ Orc variants).
            "Human Shipyard", "Orc Shipyard", "Human Oil Extractor",
            "Orc Oil Extractor", "Human Foundry", "Orc Foundry",
            "Human Oil Refinery", "Orc Oil Refinery",
        };
        const char* name = kObjectTypes[typeIndex].name;
        for (const char* b : kBuildingNames) {
            if (strcmp(name, b) == 0) {
                return true;
            }
        }
        return false;
    }

    // The subset of isBuildingObjectType() a worker/oil ship can actually
    // construct in-game - what the Map Settings restriction panel offers.
    // Zombie buildings, Mine, and Oil Spot are placeable-only props that are
    // never player-built, so banning them would be meaningless.
    bool isConstructibleBuildingType(int typeIndex) {
        if (!isBuildingObjectType(typeIndex)) {
            return false;
        }
        static const char* const kNonConstructible[] = {
            "Zombie Castle", "Zombie HQ", "Mine", "Oil Spot",
        };
        const char* name = kObjectTypes[typeIndex].name;
        for (const char* b : kNonConstructible) {
            if (strcmp(name, b) == 0) {
                return false;
            }
        }
        return true;
    }

    // The subset of units a Castle/HQ/Barracks/etc. can produce in-game - what
    // the Map Settings "Units" restriction tab offers. Explicitly listed (like
    // the building names above) so Undead/hero/prop entries are excluded and
    // adding a new roster entry never silently becomes restrictable.
    bool isTrainableUnitType(int typeIndex) {
        if (typeIndex < 0 || typeIndex >= kObjectTypeCount) {
            return false;
        }
        static const char* const kTrainableNames[] = {
            "Worker", "Swordsman", "Archer", "Catapult", "Helicopter", "Human Ship",
            "Orc Axe", "Orc Spear", "Goblin", "Goblin Bomb", "Goblin Worker", "Troll",
            "Wizard", "Orc Ship",
            "Human Battle Ship", "Orc Battle Ship", "Human Dreadnought", "Orc Dreadnought",
            "Human Oil Ship", "Orc Oil Ship",
        };
        const char* name = kObjectTypes[typeIndex].name;
        for (const char* u : kTrainableNames) {
            if (strcmp(name, u) == 0) {
                return true;
            }
        }
        return false;
    }

    // Corner-terrain tile table, copied from the "soil" terrain definition already
    // authored in Resources/tilemap/*.tmx (see <tileset> terrain="TL,TR,BL,BR").
    // The "water" terrain block in the same tileset uses the identical layout,
    // shifted down by 11 rows (WATER_TILE_ID_OFFSET), so one table serves both.
    struct CornerTileEntry {
        int tileId;
        bool tl, tr, bl, br;
    };
    const CornerTileEntry kCornerTiles[] = {
        {30, false, false, false, true},
        {31, false, false, true,  true},
        {32, false, false, true,  false},
        {33, true,  true,  true,  false},
        {34, true,  true,  false, true},
        {40, false, true,  false, true},
        {41, true,  true,  true,  true},
        {42, true,  false, true,  false},
        {43, true,  false, true,  true},
        {44, false, true,  true,  true},
        {50, false, true,  false, false},
        {51, true,  true,  false, false},
        {52, true,  false, false, false},
    };

    Scale9Sprite* makeButtonSkin(const char* file, const Size& size) {
        Scale9Sprite* skin = Scale9Sprite::create(file);
        skin->setCapInsets(BTN_SKIN_CAP_INSETS);
        skin->setContentSize(size);
        return skin;
    }

    // The MenuItemSprite "selected" skin only shows transiently while pressed.
    // To also show a *persistent* selection (current brush / current race tab /
    // current unit), callers tint outBg via setColor() — see highlightGroup().
    MenuItemSprite* createTextButton(const std::string& text, float width, float height,
                                      const ccMenuCallback& callback, Scale9Sprite** outBg = nullptr) {
        Scale9Sprite* normal = makeButtonSkin(BTN_SKIN_NORMAL, Size(width, height));
        Scale9Sprite* selected = makeButtonSkin(BTN_SKIN_SELECTED, Size(width, height));

        Label* lblNormal = LM->getLocalizedLabel(text.c_str(), Color4B::BLACK, 24);
        lblNormal->setPosition(Vec2(width / 2, height / 2));
        normal->addChild(lblNormal);

        Label* lblSelected = LM->getLocalizedLabel(text.c_str(), Color4B::GREEN, 24);
        lblSelected->setPosition(Vec2(width / 2, height / 2));
        selected->addChild(lblSelected);

        if (outBg) {
            *outBg = normal;
        }
        return MenuItemSprite::create(normal, selected, callback);
    }

    MenuItemSprite* createIconButton(const std::string& spriteFrameName, float size,
                                      const ccMenuCallback& callback, Scale9Sprite** outBg = nullptr) {
        Scale9Sprite* normal = makeButtonSkin(BTN_SKIN_NORMAL, Size(size, size));
        Scale9Sprite* selected = makeButtonSkin(BTN_SKIN_SELECTED, Size(size, size));

        // Most icons come from the packed sheets, but a few (the Flag marker's
        // spot0.png) are loose files in Resources/ - fall back to file loading
        // when the name isn't a cached sprite frame.
        bool isSheetFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName) != nullptr;
        Sprite* iconNormal = isSheetFrame ? Sprite::createWithSpriteFrameName(spriteFrameName)
                                          : Sprite::create(spriteFrameName);
        Sprite* iconSelected = isSheetFrame ? Sprite::createWithSpriteFrameName(spriteFrameName)
                                            : Sprite::create(spriteFrameName);
        float maxIconSize = size * 0.7f;
        float iconScale = std::min(maxIconSize / iconNormal->getContentSize().width,
                                    maxIconSize / iconNormal->getContentSize().height);
        iconNormal->setScale(iconScale);
        iconSelected->setScale(iconScale);
        iconNormal->setPosition(Vec2(size / 2, size / 2));
        iconSelected->setPosition(Vec2(size / 2, size / 2));
        normal->addChild(iconNormal);
        selected->addChild(iconSelected);

        if (outBg) {
            *outBg = normal;
        }
        return MenuItemSprite::create(normal, selected, callback);
    }

    // Mirrors GameManager::getHeroSpine: a live, idle-looping Spine skeleton
    // for a hero entry (palette icon or placed-object preview), instead of a
    // flat sprite. spineSkin may be "" (use the skeleton's own default skin).
    spine::SkeletonAnimation* createHeroSpine(const std::string& spineBase, const std::string& spineSkin) {
        spine::SkeletonAnimation* spt = spine::SkeletonAnimation::createWithJsonFile(
            StringUtils::format("%s.json", spineBase.c_str()), StringUtils::format("%s.atlas", spineBase.c_str()), 1);
        bool skinApplied = false;
        if (!spineSkin.empty()) {
            skinApplied = spt->setSkin(spineSkin);
        }
        if (!skinApplied) {
            // Skeletons exported without a "default" skin (werewolf/bear/lion)
            // render nothing until a skin is attached, and setSkin with a name
            // the skeleton doesn't have (the crazy_* variants - those skins
            // aren't in the current exports) is a silent no-op. Either way,
            // attach the skeleton's first named skin so the unit is visible.
            spSkeleton* skeleton = spt->getSkeleton();
            if (!skeleton->data->defaultSkin && skeleton->data->skinsCount > 0) {
                spt->setSkin(skeleton->data->skins[0]->name);
            }
        }
        spt->setAnimation(0, "idle", true);
        // Force one pose update so getBoundingBox() (used by callers to fit the
        // icon into a button) isn't measuring a not-yet-posed skeleton.
        spt->update(0.0f);
        return spt;
    }

    MenuItemSprite* createHeroIconButton(const std::string& spineBase, const std::string& spineSkin, float size,
                                          const ccMenuCallback& callback, Scale9Sprite** outBg = nullptr) {
        Scale9Sprite* normal = makeButtonSkin(BTN_SKIN_NORMAL, Size(size, size));
        Scale9Sprite* selected = makeButtonSkin(BTN_SKIN_SELECTED, Size(size, size));

        float maxIconSize = size * 0.7f;
        spine::SkeletonAnimation* iconNormal = createHeroSpine(spineBase, spineSkin);
        spine::SkeletonAnimation* iconSelected = createHeroSpine(spineBase, spineSkin);
        // Real in-game hero scale is 0.5 (Movable.cpp) - use that as a ceiling,
        // shrinking further only if the skeleton's native bounds don't fit the
        // palette button. Falls back to 0.5 outright if the bounding box comes
        // back empty (e.g. an attachment/animation lookup issue) rather than
        // risking a divide-by-near-zero blowup.
        float iconScale = 0.5f;
        Size boxSize = iconNormal->getBoundingBox().size;
        if (boxSize.width > 0 && boxSize.height > 0) {
            iconScale = std::min(iconScale, std::min(maxIconSize / boxSize.width, maxIconSize / boxSize.height));
        }
        iconNormal->setScale(iconScale);
        iconSelected->setScale(iconScale);
        iconNormal->setPosition(Vec2(size / 2, size / 2));
        iconSelected->setPosition(Vec2(size / 2, size / 2));
        normal->addChild(iconNormal);
        selected->addChild(iconSelected);

        if (outBg) {
            *outBg = normal;
        }
        return MenuItemSprite::create(normal, selected, callback);
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    Vec2 win32MouseWorldPos(EventMouse* evt) {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 viewPos = evt->getLocationInView();
        return Director::getInstance()->convertToGL(Vec2(viewPos.x, visibleSize.height - viewPos.y));
    }

    HWND win32Hwnd() {
        auto* glview = dynamic_cast<GLViewImpl*>(Director::getInstance()->getOpenGLView());
        return glview ? glview->getWin32Window() : nullptr;
    }

    // Native "Save As" dialog. Returns the chosen path, or "" if the user cancelled.
    std::string win32ShowSaveDialog(const std::string& defaultName) {
        char fileBuf[MAX_PATH] = {0};
        strncpy(fileBuf, defaultName.c_str(), MAX_PATH - 1);

        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = win32Hwnd();
        ofn.lpstrFilter = "Map files (*.json)\0*.json\0All files\0*.*\0";
        ofn.lpstrFile = fileBuf;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrDefExt = "json";
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        if (!GetSaveFileNameA(&ofn)) {
            return "";
        }
        return std::string(fileBuf);
    }

    // Native "Open" dialog. Returns the chosen path, or "" if the user cancelled.
    std::string win32ShowOpenDialog() {
        char fileBuf[MAX_PATH] = {0};

        OPENFILENAMEA ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = win32Hwnd();
        ofn.lpstrFilter = "Map files (*.json)\0*.json\0All files\0*.*\0";
        ofn.lpstrFile = fileBuf;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        if (!GetOpenFileNameA(&ofn)) {
            return "";
        }
        return std::string(fileBuf);
    }
#endif
}

Scene* MapEditor::scene() {
    Scene* scene = Scene::create();
    scene->addChild(MapEditor::create());
    return scene;
}

bool MapEditor::init() {
    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    mapRoot = Node::create();
    mapRoot->setPosition(Vec2(80, visibleSize.height - 420));
    this->addChild(mapRoot);

    buildMap(mapWidth, mapHeight);

    setupUI();
    setupObjectPalette();
    setupUnitPropertiesBar();
    setupPaletteTooltip();
    setupMouseCoordLabel();
    setupTriggerToolsRow();
    setupMoveToolsRow();
    setupMainTabs();
    setupModalDimmer();
    setupNewMapPanel();
    setupResizeMapPanel();
    setupMapSettingsPanel();
    setupEditUnitPanel();
    setupTriggerPanel();
    setupConditionEditPanel();
    setupActionEditPanel();
    setupInput();

    // test now — auto-load stage0_0.json on startup for quick iteration
    {
        std::string autoLoadPath = FileUtils::getInstance()->fullPathForFilename("stage0_0.json"); // test now
        if (!autoLoadPath.empty() && FileUtils::getInstance()->isFileExist(autoLoadPath)) {
            std::string json = FileUtils::getInstance()->getStringFromFile(autoLoadPath);
            if (deserialize(json)) {
                currentFilePath = autoLoadPath;
                isDirty = false;
            }
        }
    }

    return true;
}

int MapEditor::defaultHpForType(int typeIndex) const {
    if (typeIndex < 0 || typeIndex >= kObjectTypeCount) {
        return -1;
    }
    return kObjectTypes[typeIndex].baseHp;
}

void MapEditor::buildMap(int width, int height) {
    mapWidth = std::max(MIN_MAP_SIZE, std::min(MAX_MAP_SIZE, width));
    mapHeight = std::max(MIN_MAP_SIZE, std::min(MAX_MAP_SIZE, height));

    terrainGrid.assign(mapWidth * mapHeight, TERRAIN_GRASS);

    if (objectRoot) {
        objectRoot->removeFromParent();
        objectRoot = nullptr;
    }
    placedObjects.clear();
    clearSelection();
    nextObjectId = 1;
    triggers.clear();
    selectedTriggerIndex = -1;
    disabledBuildings.clear();
    disabledUnits.clear();
    currentFilePath.clear();
    isDirty = false;
    clearUndoHistory();

    rebuildTileVisuals();

    objectRoot = Node::create();
    mapRoot->addChild(objectRoot, 1);
}

void MapEditor::rebuildTileVisuals() {
    if (tileBatch) {
        tileBatch->removeFromParent();
        tileBatch = nullptr;
    }
    tileSprites.assign(mapWidth * mapHeight, nullptr);

    // Plain Node, not SpriteBatchNode: a SpriteBatchNode child only gets its quad
    // re-pushed to the GPU atlas when its transform is marked dirty, which a
    // texture-rect-only change (repainting a tile) never triggers on its own.
    Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("50TileSet.png");
    tileBatch = Node::create();
    mapRoot->addChild(tileBatch, 0);

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            Sprite* spt = Sprite::createWithTexture(texture, rectForTileId(BASE_GRASS_TILE_ID));
            spt->setAnchorPoint(Vec2(0.5f, 0.5f));
            spt->setPosition(Vec2(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2.0f,
                                   -(y * MAP_TILE_SIZE) - MAP_TILE_SIZE / 2.0f));
            tileBatch->addChild(spt);
            tileSprites[y * mapWidth + x] = spt;
        }
    }
}

void MapEditor::resizeMap(int newWidth, int newHeight) {
    newWidth = std::max(MIN_MAP_SIZE, std::min(MAX_MAP_SIZE, newWidth));
    newHeight = std::max(MIN_MAP_SIZE, std::min(MAX_MAP_SIZE, newHeight));
    if (newWidth == mapWidth && newHeight == mapHeight) {
        return;
    }

    std::vector<int> newTerrainGrid(newWidth * newHeight, TERRAIN_GRASS);
    int copyWidth = std::min(mapWidth, newWidth);
    int copyHeight = std::min(mapHeight, newHeight);
    for (int y = 0; y < copyHeight; y++) {
        for (int x = 0; x < copyWidth; x++) {
            newTerrainGrid[y * newWidth + x] = terrainGrid[y * mapWidth + x];
        }
    }

    // Objects that fall outside the new bounds can't be kept; drop them (and
    // their sprites) rather than clamping their position into the map.
    for (int i = static_cast<int>(placedObjects.size()) - 1; i >= 0; i--) {
        const PlacedObject& obj = placedObjects[i];
        if (obj.tileX >= newWidth || obj.tileY >= newHeight) {
            if (obj.sprite) {
                obj.sprite->removeFromParent();
            }
            placedObjects.erase(placedObjects.begin() + i);
        }
    }
    clearSelection();
    clearUndoHistory();

    mapWidth = newWidth;
    mapHeight = newHeight;
    terrainGrid = std::move(newTerrainGrid);

    rebuildTileVisuals();
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            refreshTile(x, y);
        }
    }

    isDirty = true;
}

int MapEditor::terrainAt(int x, int y) const {
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
        return TERRAIN_GRASS;
    }
    return terrainGrid[y * mapWidth + x];
}

int MapEditor::cornerTerrain(int cx, int cy) const {
    // corner (cx,cy) is touched by up to 4 cells: the ones diagonally around it.
    int neighborX[4] = {cx - 1, cx, cx - 1, cx};
    int neighborY[4] = {cy - 1, cy - 1, cy, cy};
    int best = TERRAIN_GRASS;
    for (int i = 0; i < 4; i++) {
        int t = terrainAt(neighborX[i], neighborY[i]);
        if (t == TERRAIN_WATER) {
            return TERRAIN_WATER; // highest priority, water cuts into everything else
        }
        if (t == TERRAIN_SOIL) {
            best = TERRAIN_SOIL;
        }
    }
    return best;
}

int MapEditor::pickTileIdForCell(int x, int y) const {
    int tl = cornerTerrain(x, y);
    int tr = cornerTerrain(x + 1, y);
    int bl = cornerTerrain(x, y + 1);
    int br = cornerTerrain(x + 1, y + 1);

    int renderTerrain = terrainAt(x, y);
    if (renderTerrain == TERRAIN_GRASS) {
        if (tl == TERRAIN_WATER || tr == TERRAIN_WATER || bl == TERRAIN_WATER || br == TERRAIN_WATER) {
            renderTerrain = TERRAIN_WATER;
        } else if (tl == TERRAIN_SOIL || tr == TERRAIN_SOIL || bl == TERRAIN_SOIL || br == TERRAIN_SOIL) {
            renderTerrain = TERRAIN_SOIL;
        } else {
            return BASE_GRASS_TILE_ID;
        }
    }

    bool btl = (tl == renderTerrain);
    bool btr = (tr == renderTerrain);
    bool bbl = (bl == renderTerrain);
    bool bbr = (br == renderTerrain);

    int soilTileId = FULL_SOIL_TILE_ID;
    for (const CornerTileEntry& entry : kCornerTiles) {
        if (entry.tl == btl && entry.tr == btr && entry.bl == bbl && entry.br == bbr) {
            soilTileId = entry.tileId;
            break;
        }
    }

    return renderTerrain == TERRAIN_WATER ? soilTileId + WATER_TILE_ID_OFFSET : soilTileId;
}

Rect MapEditor::rectForTileId(int tileId) const {
    int col = tileId % TILESET_COLUMNS;
    int row = tileId / TILESET_COLUMNS;
    return Rect(static_cast<float>(col * MAP_TILE_SIZE), static_cast<float>(row * MAP_TILE_SIZE),
                static_cast<float>(MAP_TILE_SIZE), static_cast<float>(MAP_TILE_SIZE));
}

void MapEditor::refreshTile(int x, int y) {
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
        return;
    }
    Sprite* spt = tileSprites[y * mapWidth + x];
    spt->setTextureRect(rectForTileId(pickTileIdForCell(x, y)));
}

void MapEditor::refreshAround(int x, int y) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            refreshTile(x + dx, y + dy);
        }
    }
}

void MapEditor::paintCell(int x, int y, int terrain) {
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
        return;
    }
    int idx = y * mapWidth + x;
    if (terrainGrid[idx] == terrain) {
        return;
    }
    if (strokeActive && currentStroke.type == ACTION_TERRAIN) {
        TerrainCellChange change;
        change.x = x;
        change.y = y;
        change.oldValue = terrainGrid[idx];
        change.newValue = terrain;
        currentStroke.terrainChanges.push_back(change);
    }
    terrainGrid[idx] = terrain;
    refreshAround(x, y);
}

void MapEditor::applyMapZoom(float newScale, Vec2 anchorGl) {
    newScale = std::max(kMapScaleMin, std::min(kMapScaleMax, newScale));
    if (fabsf(newScale - mapScale) < 0.0001f) return;
    Vec2 localPos = (anchorGl - mapRoot->getPosition()) / mapScale;
    mapScale = newScale;
    mapRoot->setScale(mapScale);
    mapRoot->setPosition(anchorGl - localPos * mapScale);
}

bool MapEditor::worldToCell(const Vec2& worldPos, int& outX, int& outY) const {
    Vec2 local = mapRoot->convertToNodeSpace(worldPos);
    outX = static_cast<int>(floorf(local.x / MAP_TILE_SIZE));
    outY = static_cast<int>(floorf(-local.y / MAP_TILE_SIZE));
    return outX >= 0 && outY >= 0 && outX < mapWidth && outY < mapHeight;
}

void MapEditor::paintAtWorldPos(const Vec2& worldPos) {
    int x, y;
    if (worldToCell(worldPos, x, y)) {
        paintCell(x, y, currentBrush);
    }
}

int MapEditor::findObjectAt(int x, int y) const {
    for (size_t i = 0; i < placedObjects.size(); i++) {
        if (placedObjects[i].tileX == x && placedObjects[i].tileY == y) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int MapEditor::findObjectAtWorldPos(const Vec2& worldPos) const {
    Vec2 local = mapRoot->convertToNodeSpace(worldPos);
    int x = static_cast<int>(floorf(local.x / MAP_TILE_SIZE));
    int y = static_cast<int>(floorf(-local.y / MAP_TILE_SIZE));
    int idx = findObjectAt(x, y);
    if (idx >= 0) {
        return idx;
    }
    // Sprites render at native size and deliberately overlap into neighboring
    // tiles (see spawnObjectAt) — when the click lands on that overlap instead
    // of the unit's own home tile, fall back to a bounding-box hit test so the
    // visible art is still clickable rather than registering as empty space.
    for (size_t i = 0; i < placedObjects.size(); i++) {
        if (placedObjects[i].sprite->getBoundingBox().containsPoint(local)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void MapEditor::removeObjectAtIndex(int index) {
    if (index < 0 || index >= static_cast<int>(placedObjects.size())) {
        return;
    }
    placedObjects[index].sprite->removeFromParent();
    placedObjects.erase(placedObjects.begin() + index);
}

Color3B MapEditor::colorForSide(int side) const {
    switch (side) {
        case SIDE_ENEMY: return Color3B(255, 140, 140);
        case SIDE_NEUTRAL_APPROACH: return Color3B(255, 230, 120);
        case SIDE_NEUTRAL_EVENT: return Color3B(190, 150, 255);
        default: return Color3B::WHITE; // SIDE_ALLY
    }
}

Node* MapEditor::spawnObjectAt(int x, int y, int typeIndex, int side, int hp, int id, int level, bool visible,
                               const std::string& alias) {
    Vec2 pos = Vec2(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2.0f, -(y * MAP_TILE_SIZE) - MAP_TILE_SIZE / 2.0f);
    const ObjectTypeDef& def = kObjectTypes[typeIndex];
    Node* spt;
    if (def.isHero) {
        spine::SkeletonAnimation* hero = createHeroSpine(def.spriteFrame, def.spineSkin);
        hero->setScale(0.5f); // matches real in-game hero scale, Movable.cpp
        spt = hero;
    } else if (strcmp(def.name, "Flag") == 0) {
        Sprite* regular = GM->createSpotFlagSprite();
        regular->setScale(OBJECT_SPRITE_SCALE);
        spt = regular;
    } else {
        Sprite* regular = Sprite::createWithSpriteFrameName(def.spriteFrame);
        regular->setScale(OBJECT_SPRITE_SCALE);
        spt = regular;
    }
    spt->setPosition(pos);
    spt->setColor(colorForSide(side));
    // Runtime-hidden flags stay visible in the editor (they must remain
    // clickable/editable) but render dimmed so the setting is obvious.
    spt->setOpacity(visible ? 255 : 110);
    objectRoot->addChild(spt);

    if (id < 0) {
        id = nextObjectId++;
    } else if (id >= nextObjectId) {
        nextObjectId = id + 1;
    }

    PlacedObject obj;
    obj.typeIndex = typeIndex;
    obj.tileX = x;
    obj.tileY = y;
    obj.sprite = spt;
    obj.side = side;
    obj.hp = hp;
    obj.id = id;
    obj.level = std::max(1, level);
    obj.visible = visible;
    obj.alias = alias;
    placedObjects.push_back(obj);
    return spt;
}

void MapEditor::placeObjectAt(int x, int y) {
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
        return;
    }
    if (findObjectAt(x, y) >= 0) {
        return;
    }
    if (currentObjectType < 0 || currentObjectType >= kObjectTypeCount) {
        return;
    }
    if (kObjectTypes[currentObjectType].requiresWater && terrainAt(x, y) != TERRAIN_WATER) {
        setStatus(StringUtils::format("%s can only be placed on water", kObjectTypes[currentObjectType].name));
        return;
    }

    int hp = parsedHpOverride();
    int level = kObjectTypes[currentObjectType].isHero && tfLevel ? std::max(1, atoi(tfLevel->getString().c_str())) : 1;
    spawnObjectAt(x, y, currentObjectType, currentSide, hp, -1, level);

    if (strokeActive && currentStroke.type == ACTION_OBJECT) {
        ObjectCellChange change;
        change.x = x;
        change.y = y;
        change.hadObject = false;
        change.oldType = -1;
        change.oldSide = -1;
        change.oldHp = -1;
        change.oldLevel = 1;
        change.hasObject = true;
        change.newType = currentObjectType;
        change.newSide = currentSide;
        change.newHp = hp;
        change.newLevel = level;
        currentStroke.objectChanges.push_back(change);
    }
}

void MapEditor::eraseObjectAt(const Vec2& worldPos) {
    int idx = findObjectAtWorldPos(worldPos);
    if (idx < 0) {
        return;
    }
    int x = placedObjects[idx].tileX;
    int y = placedObjects[idx].tileY;
    int oldType = placedObjects[idx].typeIndex;
    int oldSide = placedObjects[idx].side;
    int oldHp = placedObjects[idx].hp;
    int oldLevel = placedObjects[idx].level;
    bool oldVisible = placedObjects[idx].visible;
    std::string oldAlias = placedObjects[idx].alias;
    removeObjectAtIndex(idx);

    if (strokeActive && currentStroke.type == ACTION_OBJECT) {
        ObjectCellChange change;
        change.x = x;
        change.y = y;
        change.hadObject = true;
        change.oldType = oldType;
        change.oldSide = oldSide;
        change.oldHp = oldHp;
        change.oldLevel = oldLevel;
        change.oldVisible = oldVisible;
        change.oldAlias = oldAlias;
        change.hasObject = false;
        change.newType = -1;
        change.newSide = -1;
        change.newHp = -1;
        change.newLevel = 1;
        currentStroke.objectChanges.push_back(change);
    }
}

void MapEditor::objectToolAtWorldPos(const Vec2& worldPos) {
    if (dragEraseObjects) {
        eraseObjectAt(worldPos);
        return;
    }
    int x, y;
    if (!worldToCell(worldPos, x, y)) {
        return;
    }
    placeObjectAt(x, y);
}

void MapEditor::clearAllObjects() {
    for (PlacedObject& obj : placedObjects) {
        obj.sprite->removeFromParent();
    }
    placedObjects.clear();
}

bool MapEditor::isValidMoveTarget(int objectIndex, int x, int y) const {
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
        return false;
    }
    int occupant = findObjectAt(x, y);
    if (occupant >= 0 && occupant != objectIndex) {
        return false;
    }
    const PlacedObject& obj = placedObjects[objectIndex];
    if (kObjectTypes[obj.typeIndex].requiresWater && terrainAt(x, y) != TERRAIN_WATER) {
        return false;
    }
    return true;
}

bool MapEditor::isValidGroupMoveTarget(int objectIndex, int x, int y,
                                       const std::vector<int>& group) const {
    if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
        return false;
    }
    int occupant = findObjectAt(x, y);
    if (occupant >= 0 && occupant != objectIndex) {
        // A cell being vacated by another member of the same drag counts as
        // free - every member shifts by the same tile delta, so two members
        // can never land on the same cell.
        if (std::find(group.begin(), group.end(), occupant) == group.end()) {
            return false;
        }
    }
    const PlacedObject& obj = placedObjects[objectIndex];
    if (kObjectTypes[obj.typeIndex].requiresWater && terrainAt(x, y) != TERRAIN_WATER) {
        return false;
    }
    return true;
}

bool MapEditor::findNearestFreeCell(int typeIndex, int startX, int startY,
                                    const std::vector<std::pair<int, int>>& claimedCells,
                                    int& outX, int& outY) const {
    // Chebyshev-ring spiral outward from the start cell; the first ring where
    // a cell is in bounds, unoccupied, unclaimed by an earlier copy in the
    // same operation, and terrain-legal for the type wins.
    int maxRadius = std::max(mapWidth, mapHeight);
    for (int r = 0; r <= maxRadius; r++) {
        for (int dy = -r; dy <= r; dy++) {
            for (int dx = -r; dx <= r; dx++) {
                if (std::max(abs(dx), abs(dy)) != r) {
                    continue;
                }
                int x = startX + dx;
                int y = startY + dy;
                if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
                    continue;
                }
                if (findObjectAt(x, y) >= 0) {
                    continue;
                }
                if (std::find(claimedCells.begin(), claimedCells.end(),
                              std::make_pair(x, y)) != claimedCells.end()) {
                    continue;
                }
                if (kObjectTypes[typeIndex].requiresWater && terrainAt(x, y) != TERRAIN_WATER) {
                    continue;
                }
                outX = x;
                outY = y;
                return true;
            }
        }
    }
    return false;
}

bool MapEditor::spawnCopyNear(const ClipboardEntry& entry, int nearX, int nearY,
                              std::vector<std::pair<int, int>>& claimedCells,
                              std::vector<int>& outNewIds) {
    int tx, ty;
    if (!findNearestFreeCell(entry.typeIndex, nearX, nearY, claimedCells, tx, ty)) {
        return false;
    }
    spawnObjectAt(tx, ty, entry.typeIndex, entry.side, entry.hp, -1, entry.level, entry.visible, entry.alias);
    const PlacedObject& copy = placedObjects.back();

    if (strokeActive && currentStroke.type == ACTION_OBJECT) {
        ObjectCellChange change;
        change.x = tx;
        change.y = ty;
        change.hadObject = false;
        change.oldType = -1;
        change.oldSide = -1;
        change.oldHp = -1;
        change.oldLevel = 1;
        change.hasObject = true;
        change.newType = entry.typeIndex;
        change.newSide = entry.side;
        change.newHp = entry.hp;
        change.newLevel = entry.level;
        change.newVisible = entry.visible;
        change.newAlias = entry.alias;
        change.id = copy.id; // redo respawns the copy under the same id
        currentStroke.objectChanges.push_back(change);
    }
    claimedCells.push_back({tx, ty});
    outNewIds.push_back(copy.id);
    return true;
}

void MapEditor::duplicateSelection() {
    std::vector<int> sourceIndices;
    for (int id : selectedObjectIds) {
        int idx = objectIndexById(id);
        if (idx >= 0) {
            sourceIndices.push_back(idx);
        }
    }
    if (sourceIndices.empty()) {
        setStatus("Select a unit to duplicate");
        return;
    }

    beginStroke(ACTION_OBJECT);
    std::vector<std::pair<int, int>> claimedCells;
    std::vector<int> newIds;
    int failed = 0;
    for (int idx : sourceIndices) {
        // By-value copy: spawnCopyNear appends to placedObjects, which can
        // reallocate and would dangle a reference.
        const PlacedObject src = placedObjects[idx];
        ClipboardEntry entry;
        entry.typeIndex = src.typeIndex;
        entry.side = src.side;
        entry.hp = src.hp;
        entry.level = src.level;
        entry.visible = src.visible;
        entry.alias = src.alias;
        entry.offsetX = 0;
        entry.offsetY = 0;
        if (!spawnCopyNear(entry, src.tileX, src.tileY, claimedCells, newIds)) {
            failed++;
        }
    }
    endStroke();

    if (newIds.empty()) {
        setStatus("No free space to duplicate into");
        return;
    }
    selectedObjectIds = newIds;
    refreshSelectionOverlay();
    if (failed > 0) {
        setStatus(StringUtils::format("Duplicated %d unit(s), %d had no free space",
                                      static_cast<int>(newIds.size()), failed));
    } else {
        setStatus(StringUtils::format("Duplicated %d unit(s)", static_cast<int>(newIds.size())));
    }
}

void MapEditor::deleteSelection() {
    // A drag in progress would hold a placedObjects index that the removals
    // below invalidate - let go first (same concern as undo/redo).
    cancelMoveDrag();

    std::vector<int> sourceIndices;
    for (int id : selectedObjectIds) {
        int idx = objectIndexById(id);
        if (idx >= 0) {
            sourceIndices.push_back(idx);
        }
    }
    if (sourceIndices.empty()) {
        setStatus("Select a unit to delete");
        return;
    }

    beginStroke(ACTION_OBJECT);
    // Remove from the highest index down so earlier removals don't shift the
    // indices still waiting in the list.
    std::sort(sourceIndices.begin(), sourceIndices.end());
    for (auto it = sourceIndices.rbegin(); it != sourceIndices.rend(); ++it) {
        const PlacedObject& obj = placedObjects[*it];
        ObjectCellChange change;
        change.x = obj.tileX;
        change.y = obj.tileY;
        change.hadObject = true;
        change.oldType = obj.typeIndex;
        change.oldSide = obj.side;
        change.oldHp = obj.hp;
        change.oldLevel = obj.level;
        change.oldVisible = obj.visible;
        change.oldAlias = obj.alias;
        change.hasObject = false;
        change.newType = -1;
        change.newSide = -1;
        change.newHp = -1;
        change.newLevel = 1;
        change.id = obj.id; // undo respawns under the same id, keeping trigger refs valid
        currentStroke.objectChanges.push_back(change);
        removeObjectAtIndex(*it);
    }
    endStroke();

    int count = static_cast<int>(sourceIndices.size());
    clearSelection();
    setStatus(StringUtils::format("Deleted %d unit(s)", count));
}

void MapEditor::copySelectionToClipboard() {
    std::vector<int> sourceIndices;
    for (int id : selectedObjectIds) {
        int idx = objectIndexById(id);
        if (idx >= 0) {
            sourceIndices.push_back(idx);
        }
    }
    if (sourceIndices.empty()) {
        setStatus("Select a unit to copy (Ctrl+C)");
        return;
    }

    // Offsets are stored relative to the selection's bounding-box center so a
    // paste centers the formation on the cursor cell.
    int minX = placedObjects[sourceIndices[0]].tileX, maxX = minX;
    int minY = placedObjects[sourceIndices[0]].tileY, maxY = minY;
    for (int idx : sourceIndices) {
        minX = std::min(minX, placedObjects[idx].tileX);
        maxX = std::max(maxX, placedObjects[idx].tileX);
        minY = std::min(minY, placedObjects[idx].tileY);
        maxY = std::max(maxY, placedObjects[idx].tileY);
    }
    int anchorX = (minX + maxX) / 2;
    int anchorY = (minY + maxY) / 2;

    clipboardEntries.clear();
    for (int idx : sourceIndices) {
        const PlacedObject& src = placedObjects[idx];
        ClipboardEntry entry;
        entry.typeIndex = src.typeIndex;
        entry.side = src.side;
        entry.hp = src.hp;
        entry.level = src.level;
        entry.visible = src.visible;
        entry.alias = src.alias;
        entry.offsetX = src.tileX - anchorX;
        entry.offsetY = src.tileY - anchorY;
        clipboardEntries.push_back(entry);
    }
    setStatus(StringUtils::format("Copied %d unit(s) - Ctrl+V to paste at the cursor",
                                  static_cast<int>(clipboardEntries.size())));
}

void MapEditor::pasteClipboardAtCursor() {
    if (clipboardEntries.empty()) {
        setStatus("Clipboard empty - Ctrl+C a selection first");
        return;
    }

    // Anchor on the map cell under the mouse cursor, clamped into the map so
    // pasting still works when the cursor sits over the toolbar or off-map.
    Vec2 anchorWorld = hasMouseWorldPos ? lastMouseWorldPos
                                        : Vec2(Director::getInstance()->getVisibleSize() / 2);
    Vec2 local = mapRoot->convertToNodeSpace(anchorWorld);
    int anchorX = static_cast<int>(floorf(local.x / MAP_TILE_SIZE));
    int anchorY = static_cast<int>(floorf(-local.y / MAP_TILE_SIZE));
    anchorX = std::max(0, std::min(anchorX, mapWidth - 1));
    anchorY = std::max(0, std::min(anchorY, mapHeight - 1));

    beginStroke(ACTION_OBJECT);
    std::vector<std::pair<int, int>> claimedCells;
    std::vector<int> newIds;
    int failed = 0;
    for (const ClipboardEntry& entry : clipboardEntries) {
        if (!spawnCopyNear(entry, anchorX + entry.offsetX, anchorY + entry.offsetY,
                           claimedCells, newIds)) {
            failed++;
        }
    }
    endStroke();

    if (newIds.empty()) {
        setStatus("No free space to paste into");
        return;
    }
    selectedObjectIds = newIds;
    refreshSelectionOverlay();
    if (failed > 0) {
        setStatus(StringUtils::format("Pasted %d unit(s), %d had no free space",
                                      static_cast<int>(newIds.size()), failed));
    } else {
        setStatus(StringUtils::format("Pasted %d unit(s)", static_cast<int>(newIds.size())));
    }
}

int MapEditor::objectIndexById(int id) const {
    for (size_t i = 0; i < placedObjects.size(); i++) {
        if (placedObjects[i].id == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool MapEditor::isObjectIdSelected(int id) const {
    return std::find(selectedObjectIds.begin(), selectedObjectIds.end(), id) != selectedObjectIds.end();
}

void MapEditor::clearSelection() {
    selectedObjectIds.clear();
    if (selectionOverlay) {
        selectionOverlay->clear();
    }
}

void MapEditor::refreshSelectionOverlay() {
    if (!selectionOverlay) {
        selectionOverlay = DrawNode::create();
        mapRoot->addChild(selectionOverlay, 2); // above objectRoot (z=1)
    }
    selectionOverlay->clear();
    // Draw a box around each still-existing selected object; prune ids whose
    // object is gone (erased in the Units tab, removed by undo, ...).
    std::vector<int> alive;
    alive.reserve(selectedObjectIds.size());
    for (int id : selectedObjectIds) {
        int idx = objectIndexById(id);
        if (idx < 0) {
            continue;
        }
        alive.push_back(id);
        const PlacedObject& obj = placedObjects[idx];
        Rect bb = obj.sprite->getBoundingBox();
        if (bb.size.width < 8 || bb.size.height < 8) {
            // Hero spine nodes can report a degenerate box - fall back to the
            // sprite position so the highlight still lands on the unit even
            // mid-drag (tileX/tileY only update on drop).
            Vec2 p = obj.sprite->getPosition();
            bb = Rect(p.x - MAP_TILE_SIZE / 2.0f, p.y - MAP_TILE_SIZE / 2.0f,
                      MAP_TILE_SIZE, MAP_TILE_SIZE);
        }
        selectionOverlay->drawRect(bb.origin, bb.origin + bb.size,
                                   Color4F(0.25f, 1.0f, 0.35f, 0.9f));
    }
    if (alive.size() != selectedObjectIds.size()) {
        selectedObjectIds = alive;
    }
}

void MapEditor::beginBandSelect(const Vec2& worldPos, bool additive) {
    isBandSelecting = true;
    bandSelectAdditive = additive;
    bandSelectStartPos = worldPos;
    if (!bandSelectRect) {
        bandSelectRect = DrawNode::create();
        this->addChild(bandSelectRect, 9); // below the toolbar UI (z >= 10)
    }
    bandSelectRect->clear();
    bandSelectRect->setVisible(true);
}

void MapEditor::updateBandSelect(const Vec2& worldPos) {
    if (!isBandSelecting || !bandSelectRect) {
        return;
    }
    Vec2 lo(std::min(bandSelectStartPos.x, worldPos.x), std::min(bandSelectStartPos.y, worldPos.y));
    Vec2 hi(std::max(bandSelectStartPos.x, worldPos.x), std::max(bandSelectStartPos.y, worldPos.y));
    bandSelectRect->clear();
    bandSelectRect->drawSolidRect(lo, hi, Color4F(0.25f, 1.0f, 0.35f, 0.10f));
    bandSelectRect->drawRect(lo, hi, Color4F(0.25f, 1.0f, 0.35f, 0.9f));
}

void MapEditor::endBandSelect(const Vec2& worldPos) {
    if (!isBandSelecting) {
        return;
    }
    isBandSelecting = false;
    if (bandSelectRect) {
        bandSelectRect->clear();
        bandSelectRect->setVisible(false);
    }
    // A no-drag click on empty ground clears the selection (unless Shift-held,
    // which means "I'm adding, don't wipe what I have").
    if (worldPos.distance(bandSelectStartPos) < 8.0f) {
        if (!bandSelectAdditive) {
            clearSelection();
        }
        return;
    }
    Vec2 a = mapRoot->convertToNodeSpace(bandSelectStartPos);
    Vec2 b = mapRoot->convertToNodeSpace(worldPos);
    Rect band(std::min(a.x, b.x), std::min(a.y, b.y), fabsf(b.x - a.x), fabsf(b.y - a.y));
    if (!bandSelectAdditive) {
        selectedObjectIds.clear();
    }
    for (const PlacedObject& obj : placedObjects) {
        if (!band.intersectsRect(obj.sprite->getBoundingBox())) {
            continue;
        }
        if (!isObjectIdSelected(obj.id)) {
            selectedObjectIds.push_back(obj.id);
        }
    }
    refreshSelectionOverlay();
    setStatus(StringUtils::format("%d unit(s) selected", static_cast<int>(selectedObjectIds.size())));
}

void MapEditor::selectAllOfSameType(int objectIndex) {
    if (objectIndex < 0 || objectIndex >= static_cast<int>(placedObjects.size())) {
        return;
    }
    const PlacedObject& ref = placedObjects[objectIndex];
    selectedObjectIds.clear();
    for (const PlacedObject& obj : placedObjects) {
        if (obj.typeIndex == ref.typeIndex && obj.side == ref.side) {
            selectedObjectIds.push_back(obj.id);
        }
    }
    refreshSelectionOverlay();
    setStatus(StringUtils::format("Selected all %s: %d unit(s)",
                                  kObjectTypes[ref.typeIndex].name,
                                  static_cast<int>(selectedObjectIds.size())));
}

bool MapEditor::handleSelectionClick(int objectIndex) {
    int id = placedObjects[objectIndex].id;
    long long nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    bool isDoubleClick = (id == lastClickedObjectId) && (nowMs - lastObjectClickMs < 400);
    lastObjectClickMs = nowMs;
    lastClickedObjectId = id;

    if (isDoubleClick) {
        selectAllOfSameType(objectIndex);
        // Reset so a triple-click reads as a fresh click on a selected unit
        // (which opens the batch editor) instead of another double-click.
        lastClickedObjectId = -1;
        movePressOpensPanel = false;
        return true;
    }
    if (shiftDown) {
        // Shift+click toggles membership, RTS-style.
        auto it = std::find(selectedObjectIds.begin(), selectedObjectIds.end(), id);
        if (it != selectedObjectIds.end()) {
            selectedObjectIds.erase(it);
        } else {
            selectedObjectIds.push_back(id);
        }
        refreshSelectionOverlay();
        setStatus(StringUtils::format("%d unit(s) selected", static_cast<int>(selectedObjectIds.size())));
        movePressOpensPanel = false;
        return true;
    }
    if (!isObjectIdSelected(id)) {
        // First click on an unselected unit only selects it; clicking it
        // again (or any other selected unit) opens the property editor.
        selectedObjectIds.assign(1, id);
        refreshSelectionOverlay();
        setStatus(StringUtils::format("%s (ID %d) selected - click again to edit",
                                      kObjectTypes[placedObjects[objectIndex].typeIndex].name, id));
        movePressOpensPanel = false;
    } else {
        movePressOpensPanel = true;
    }
    return false;
}

void MapEditor::beginMoveDrag(const Vec2& worldPos) {
    movingObjectIndex = findObjectAtWorldPos(worldPos);
    moveDragStarted = false;
    moveDragStartPos = worldPos;
    groupDragIndices.clear();
    groupDragOrigSpritePositions.clear();
    if (movingObjectIndex >= 0) {
        moveDragOrigSpritePos = placedObjects[movingObjectIndex].sprite->getPosition();
        // Grabbing a unit that is part of a multi-selection drags the whole
        // selection together; grabbing anything else drags just that unit.
        if (selectedObjectIds.size() > 1 &&
            isObjectIdSelected(placedObjects[movingObjectIndex].id)) {
            for (int id : selectedObjectIds) {
                int idx = objectIndexById(id);
                if (idx >= 0) {
                    groupDragIndices.push_back(idx);
                    groupDragOrigSpritePositions.push_back(placedObjects[idx].sprite->getPosition());
                }
            }
        }
    }
}

void MapEditor::updateMoveDrag(const Vec2& worldPos) {
    if (movingObjectIndex < 0) {
        return;
    }
    // Don't commit to a drag until the cursor clearly travels — a plain click
    // must survive small hand jitter so it still opens the property editor.
    constexpr float kDragThreshold = 12.0f;
    if (!moveDragStarted && worldPos.distance(moveDragStartPos) < kDragThreshold) {
        return;
    }
    moveDragStarted = true;

    PlacedObject& obj = placedObjects[movingObjectIndex];
    int x, y;
    bool inMap = worldToCell(worldPos, x, y);

    if (!groupDragIndices.empty()) {
        // Group preview: every member keeps its original tile offset from the
        // grabbed unit; per-member red tint for cells that would reject the
        // drop. tileX/tileY stay untouched until the drop commits, so the
        // occupancy checks below still see the original layout.
        if (inMap) {
            int dTx = x - obj.tileX;
            int dTy = y - obj.tileY;
            Vec2 spriteDelta(dTx * MAP_TILE_SIZE, -dTy * MAP_TILE_SIZE);
            for (size_t i = 0; i < groupDragIndices.size(); i++) {
                PlacedObject& member = placedObjects[groupDragIndices[i]];
                member.sprite->setPosition(groupDragOrigSpritePositions[i] + spriteDelta);
                bool ok = isValidGroupMoveTarget(groupDragIndices[i],
                                                 member.tileX + dTx, member.tileY + dTy,
                                                 groupDragIndices);
                member.sprite->setColor(ok ? colorForSide(member.side) : Color3B(255, 60, 60));
            }
        } else {
            Vec2 freeDelta = mapRoot->convertToNodeSpace(worldPos) - moveDragOrigSpritePos;
            for (size_t i = 0; i < groupDragIndices.size(); i++) {
                PlacedObject& member = placedObjects[groupDragIndices[i]];
                member.sprite->setPosition(groupDragOrigSpritePositions[i] + freeDelta);
                member.sprite->setColor(Color3B(255, 60, 60));
            }
        }
        refreshSelectionOverlay();
        return;
    }

    if (inMap && isValidMoveTarget(movingObjectIndex, x, y)) {
        // Legal drop cell: preview by snapping to its center, normal tint.
        obj.sprite->setPosition(Vec2(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2.0f,
                                     -(y * MAP_TILE_SIZE) - MAP_TILE_SIZE / 2.0f));
        obj.sprite->setColor(colorForSide(obj.side));
    } else {
        // Illegal (out of bounds / occupied / wrong terrain): follow the cursor
        // and tint red so the user sees the drop would be rejected.
        obj.sprite->setPosition(mapRoot->convertToNodeSpace(worldPos));
        obj.sprite->setColor(Color3B(255, 60, 60));
    }
    refreshSelectionOverlay();
}

void MapEditor::endMoveDrag(const Vec2& worldPos) {
    int idx = movingObjectIndex;
    movingObjectIndex = -1;
    std::vector<int> group = groupDragIndices;
    std::vector<Vec2> groupOrig = groupDragOrigSpritePositions;
    groupDragIndices.clear();
    groupDragOrigSpritePositions.clear();
    if (idx < 0) {
        return;
    }

    if (!moveDragStarted) {
        // Plain click: inspect/edit, same as the Trigger tab. Only opens when
        // the pressed unit was already selected (movePressOpensPanel; the
        // touch path always sets it, keeping the old tap-to-edit behavior).
        if (movePressOpensPanel) {
            if (selectedObjectIds.size() > 1 && isObjectIdSelected(placedObjects[idx].id)) {
                openEditUnitPanelForSelection();
            } else {
                openEditUnitPanel(idx);
            }
        }
        return;
    }
    moveDragStarted = false;

    if (!group.empty()) {
        // ---- group drop ----
        auto revertAll = [&]() {
            for (size_t i = 0; i < group.size(); i++) {
                PlacedObject& member = placedObjects[group[i]];
                member.sprite->setPosition(groupOrig[i]);
                member.sprite->setColor(colorForSide(member.side));
            }
            refreshSelectionOverlay();
        };
        PlacedObject& anchor = placedObjects[idx];
        int x, y;
        if (!worldToCell(worldPos, x, y) || (x == anchor.tileX && y == anchor.tileY)) {
            revertAll();
            return;
        }
        int dTx = x - anchor.tileX;
        int dTy = y - anchor.tileY;
        for (int gi : group) {
            const PlacedObject& member = placedObjects[gi];
            if (!isValidGroupMoveTarget(gi, member.tileX + dTx, member.tileY + dTy, group)) {
                revertAll();
                setStatus("Cannot move there");
                return;
            }
        }

        // One undo step for the whole formation: all vacates first, then all
        // arrivals. Together with the id-guard in applyObjectChange this
        // replays safely even when one member's destination is another
        // member's source cell.
        beginStroke(ACTION_OBJECT);
        for (int gi : group) {
            const PlacedObject& member = placedObjects[gi];
            ObjectCellChange from;
            from.x = member.tileX;
            from.y = member.tileY;
            from.hadObject = true;
            from.oldType = member.typeIndex;
            from.oldSide = member.side;
            from.oldHp = member.hp;
            from.oldLevel = member.level;
            from.oldVisible = member.visible;
            from.oldAlias = member.alias;
            from.hasObject = false;
            from.newType = -1;
            from.newSide = -1;
            from.newHp = -1;
            from.newLevel = 1;
            from.id = member.id;
            currentStroke.objectChanges.push_back(from);
        }
        for (int gi : group) {
            const PlacedObject& member = placedObjects[gi];
            ObjectCellChange to;
            to.x = member.tileX + dTx;
            to.y = member.tileY + dTy;
            to.hadObject = false;
            to.oldType = -1;
            to.oldSide = -1;
            to.oldHp = -1;
            to.oldLevel = 1;
            to.hasObject = true;
            to.newType = member.typeIndex;
            to.newSide = member.side;
            to.newHp = member.hp;
            to.newLevel = member.level;
            to.newVisible = member.visible;
            to.newAlias = member.alias;
            to.id = member.id;
            currentStroke.objectChanges.push_back(to);
        }
        endStroke();

        for (int gi : group) {
            PlacedObject& member = placedObjects[gi];
            member.tileX += dTx;
            member.tileY += dTy;
            member.sprite->setPosition(Vec2(member.tileX * MAP_TILE_SIZE + MAP_TILE_SIZE / 2.0f,
                                            -(member.tileY * MAP_TILE_SIZE) - MAP_TILE_SIZE / 2.0f));
            member.sprite->setColor(colorForSide(member.side));
        }
        refreshSelectionOverlay();
        setStatus(StringUtils::format("Moved %d units", static_cast<int>(group.size())));
        return;
    }

    PlacedObject& obj = placedObjects[idx];
    obj.sprite->setColor(colorForSide(obj.side));

    int x, y;
    bool inMap = worldToCell(worldPos, x, y);
    if (!inMap || (x == obj.tileX && y == obj.tileY)) {
        obj.sprite->setPosition(moveDragOrigSpritePos);
        refreshSelectionOverlay();
        return;
    }
    if (!isValidMoveTarget(idx, x, y)) {
        obj.sprite->setPosition(moveDragOrigSpritePos);
        refreshSelectionOverlay();
        setStatus("Cannot move there");
        return;
    }

    // Record the move as one undo step: the source cell loses the object and
    // the destination gains it, both carrying the object's id so undo/redo
    // preserves identity (trigger targetObjectId references stay valid).
    beginStroke(ACTION_OBJECT);
    ObjectCellChange from;
    from.x = obj.tileX;
    from.y = obj.tileY;
    from.hadObject = true;
    from.oldType = obj.typeIndex;
    from.oldSide = obj.side;
    from.oldHp = obj.hp;
    from.oldLevel = obj.level;
    from.oldVisible = obj.visible;
    from.oldAlias = obj.alias;
    from.hasObject = false;
    from.newType = -1;
    from.newSide = -1;
    from.newHp = -1;
    from.newLevel = 1;
    from.id = obj.id;
    currentStroke.objectChanges.push_back(from);
    ObjectCellChange to;
    to.x = x;
    to.y = y;
    to.hadObject = false;
    to.oldType = -1;
    to.oldSide = -1;
    to.oldHp = -1;
    to.oldLevel = 1;
    to.hasObject = true;
    to.newType = obj.typeIndex;
    to.newSide = obj.side;
    to.newHp = obj.hp;
    to.newLevel = obj.level;
    to.newVisible = obj.visible;
    to.newAlias = obj.alias;
    to.id = obj.id;
    currentStroke.objectChanges.push_back(to);
    endStroke();

    obj.tileX = x;
    obj.tileY = y;
    obj.sprite->setPosition(Vec2(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2.0f,
                                 -(y * MAP_TILE_SIZE) - MAP_TILE_SIZE / 2.0f));
    refreshSelectionOverlay();
    setStatus(StringUtils::format("Moved %s to (%d, %d)", kObjectTypes[obj.typeIndex].name, x, y));
}

void MapEditor::cancelMoveDrag() {
    if (movingObjectIndex < 0) {
        groupDragIndices.clear();
        groupDragOrigSpritePositions.clear();
        return;
    }
    if (!groupDragIndices.empty()) {
        for (size_t i = 0; i < groupDragIndices.size(); i++) {
            PlacedObject& member = placedObjects[groupDragIndices[i]];
            member.sprite->setPosition(groupDragOrigSpritePositions[i]);
            member.sprite->setColor(colorForSide(member.side));
        }
    } else {
        PlacedObject& obj = placedObjects[movingObjectIndex];
        obj.sprite->setPosition(moveDragOrigSpritePos);
        obj.sprite->setColor(colorForSide(obj.side));
    }
    movingObjectIndex = -1;
    moveDragStarted = false;
    groupDragIndices.clear();
    groupDragOrigSpritePositions.clear();
    refreshSelectionOverlay();
}

void MapEditor::beginStroke(ActionType type) {
    currentStroke = UndoAction();
    currentStroke.type = type;
    strokeActive = true;
}

void MapEditor::endStroke() {
    if (!strokeActive) {
        return;
    }
    strokeActive = false;
    if (currentStroke.terrainChanges.empty() && currentStroke.objectChanges.empty()) {
        return;
    }
    undoStack.push_back(currentStroke);
    isDirty = true;
    const size_t maxUndo = 100;
    if (undoStack.size() > maxUndo) {
        undoStack.erase(undoStack.begin());
    }
    redoStack.clear();
}

void MapEditor::applyTerrainChange(const TerrainCellChange& change, bool useNew) {
    terrainGrid[change.y * mapWidth + change.x] = useNew ? change.newValue : change.oldValue;
    refreshAround(change.x, change.y);
}

void MapEditor::applyObjectChange(const ObjectCellChange& change, bool useNew) {
    bool shouldHave = useNew ? change.hasObject : change.hadObject;
    int type = useNew ? change.newType : change.oldType;
    int side = useNew ? change.newSide : change.oldSide;
    int hp = useNew ? change.newHp : change.oldHp;
    int level = useNew ? change.newLevel : change.oldLevel;
    bool visible = useNew ? change.newVisible : change.oldVisible;
    const std::string& alias = useNew ? change.newAlias : change.oldAlias;
    int idx = findObjectAt(change.x, change.y);

    if (shouldHave) {
        if (idx >= 0) {
            removeObjectAtIndex(idx);
        }
        spawnObjectAt(change.x, change.y, type, side, hp, change.id, level, visible, alias);
    } else if (idx >= 0) {
        // Identity guard for replayed group moves: when one member's source is
        // another member's destination, this cell can transiently hold a
        // *different* unit than the one this change recorded - removing it
        // here would delete a unit an earlier change just respawned. Changes
        // without an id (paint/erase strokes) keep the old cell-based behavior.
        if (change.id < 0 || placedObjects[idx].id == change.id) {
            removeObjectAtIndex(idx);
        }
    }
}

void MapEditor::clearUndoHistory() {
    undoStack.clear();
    redoStack.clear();
    strokeActive = false;
}

void MapEditor::undo() {
    if (undoStack.empty()) {
        return;
    }
    // Undo re-spawns objects and shifts placedObjects indices, so a move drag
    // in progress (Ctrl+Z while the mouse button is held) must let go first.
    cancelMoveDrag();
    UndoAction action = undoStack.back();
    undoStack.pop_back();

    if (action.type == ACTION_TERRAIN) {
        for (const TerrainCellChange& change : action.terrainChanges) {
            applyTerrainChange(change, false);
        }
    } else {
        for (const ObjectCellChange& change : action.objectChanges) {
            applyObjectChange(change, false);
        }
    }

    redoStack.push_back(action);
    isDirty = true;
    refreshSelectionOverlay(); // selected objects may have moved or vanished
    setStatus("Undo");
}

void MapEditor::redo() {
    if (redoStack.empty()) {
        return;
    }
    cancelMoveDrag(); // same index-invalidation concern as undo()

    UndoAction action = redoStack.back();
    redoStack.pop_back();

    if (action.type == ACTION_TERRAIN) {
        for (const TerrainCellChange& change : action.terrainChanges) {
            applyTerrainChange(change, true);
        }
    } else {
        for (const ObjectCellChange& change : action.objectChanges) {
            applyObjectChange(change, true);
        }
    }

    undoStack.push_back(action);
    isDirty = true;
    refreshSelectionOverlay(); // selected objects may have moved or vanished
    setStatus("Redo");
}

void MapEditor::selectBrush(int terrain) {
    currentBrush = terrain;
    currentTool = TOOL_TERRAIN;
    highlightGroup(terrainButtonBgs, currentBrush);
    updateToolLabel();
}

void MapEditor::selectObjectTool(int typeIndex) {
    if (tfHp) {
        // Only follow the new unit's default HP if the field is still showing
        // the previous unit's default (untouched) — a manually typed override
        // should survive switching the palette selection.
        std::string text = tfHp->getString();
        int oldDefault = defaultHpForType(currentObjectType);
        bool untouched = text.empty() || text == "default" ||
            (oldDefault > 0 && atoi(text.c_str()) == oldDefault);
        if (untouched) {
            int newDefault = defaultHpForType(typeIndex);
            tfHp->setString(newDefault > 0 ? StringUtils::format("%d", newDefault) : "default");
        }
    }

    currentObjectType = typeIndex;
    currentTool = TOOL_OBJECT;

    if (rowLevel) {
        rowLevel->setVisible(kObjectTypes[typeIndex].isHero);
    }

    int slot = -1;
    for (size_t i = 0; i < currentRaceUnitIndices.size(); i++) {
        if (currentRaceUnitIndices[i] == typeIndex) {
            slot = static_cast<int>(i);
            break;
        }
    }
    highlightGroup(objectButtonBgs, slot);
    updateToolLabel();
}

void MapEditor::updateToolLabel() {
    if (!toolLabel) {
        return;
    }
    if (currentTool == TOOL_TERRAIN) {
        const char* names[] = {"Grass", "Dirt", "Water"};
        toolLabel->setString(StringUtils::format("Terrain: %s  (1/2/3 keys, right-drag to pan)", names[currentBrush]));
    } else {
        toolLabel->setString(StringUtils::format(
            "Placing: %s  (drag to place, drag from an existing one to erase, Ctrl+Click one to edit Side/HP)",
            kObjectTypes[currentObjectType].name));
    }
}

bool MapEditor::isAnyModalOpen() const {
    return isNewMapPanelOpen || isResizeMapPanelOpen || isMapSettingsPanelOpen || isEditUnitPanelOpen ||
           isTriggerPanelOpen || isConditionEditPanelOpen || isActionEditPanelOpen;
}

void MapEditor::setStatus(const std::string& text) {
    if (statusLabel) {
        statusLabel->setString(text);
    }
}

void MapEditor::setupUI() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    terrainToolsRow = Node::create();
    this->addChild(terrainToolsRow, 10);

    // terrain brush row
    const float terrainBtnW = 130;
    const float terrainBtnH = 60;
    Scale9Sprite* bgGrass = nullptr;
    Scale9Sprite* bgSoil = nullptr;
    Scale9Sprite* bgWater = nullptr;
    MenuItemSprite* itemGrass = createTextButton("Grass", terrainBtnW, terrainBtnH, [this](Ref*) { selectBrush(TERRAIN_GRASS); }, &bgGrass);
    MenuItemSprite* itemSoil = createTextButton("Dirt", terrainBtnW, terrainBtnH, [this](Ref*) { selectBrush(TERRAIN_SOIL); }, &bgSoil);
    MenuItemSprite* itemWater = createTextButton("Water", terrainBtnW, terrainBtnH, [this](Ref*) { selectBrush(TERRAIN_WATER); }, &bgWater);
    terrainButtonBgs = {bgGrass, bgSoil, bgWater};

    itemGrass->setPosition(Vec2(0, 0));
    itemSoil->setPosition(Vec2(145, 0));
    itemWater->setPosition(Vec2(290, 0));

    Menu* terrainMenu = Menu::create(itemGrass, itemSoil, itemWater, nullptr);
    terrainMenu->setPosition(Vec2(150, visibleSize.height - 105));
    terrainToolsRow->addChild(terrainMenu);

    toolLabel = LM->getLocalizedLabel("", Color4B::WHITE, 24);
    toolLabel->setAnchorPoint(Vec2(0, 1));
    toolLabel->setPosition(Vec2(20, visibleSize.height - 165));
    terrainToolsRow->addChild(toolLabel);
    highlightGroup(terrainButtonBgs, currentBrush);
    updateToolLabel();

    // file row — always visible regardless of which tab is active
    const float fileBtnW = 110;
    const float fileBtnH = 56;
    MenuItemSprite* itemNew = createTextButton("New", fileBtnW, fileBtnH, [this](Ref*) { showNewMapPanel(); });
    MenuItemSprite* itemSave = createTextButton("Save", fileBtnW, fileBtnH, [this](Ref*) { saveMap(); });
    MenuItemSprite* itemSaveAs = createTextButton("Save As", fileBtnW + 30, fileBtnH, [this](Ref*) { saveMapAs(); });
    MenuItemSprite* itemLoad = createTextButton("Load", fileBtnW, fileBtnH, [this](Ref*) { loadMap(); });
    MenuItemSprite* itemResize = createTextButton("Resize", fileBtnW, fileBtnH, [this](Ref*) { showResizeMapPanel(); });
    MenuItemSprite* itemRestrict = createTextButton("Restrict", fileBtnW + 20, fileBtnH, [this](Ref*) { showMapSettingsPanel(); });
    MenuItemSprite* itemRun = createTextButton("Run", fileBtnW, fileBtnH, [this](Ref*) { runMap(); });
    itemRun->getNormalImage()->setColor(Color3B(120, 220, 120));

    itemNew->setPosition(Vec2(0, 0));
    itemSave->setPosition(Vec2(120, 0));
    itemSaveAs->setPosition(Vec2(240, 0));
    itemLoad->setPosition(Vec2(360, 0));
    itemResize->setPosition(Vec2(480, 0));
    itemRestrict->setPosition(Vec2(610, 0));
    itemRun->setPosition(Vec2(740, 0));

    Menu* fileMenu = Menu::create(itemNew, itemSave, itemSaveAs, itemLoad, itemResize, itemRestrict, itemRun, nullptr);
    fileMenu->setPosition(Vec2(120, visibleSize.height - 285));
    this->addChild(fileMenu, 10);

    statusLabel = LM->getLocalizedLabel("", Color4B::WHITE, 22);
    statusLabel->setAnchorPoint(Vec2(0, 1));
    statusLabel->setPosition(Vec2(20, visibleSize.height - 325));
    this->addChild(statusLabel, 10);
}

void MapEditor::setupObjectPalette() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    unitToolsRow = Node::create();
    this->addChild(unitToolsRow, 10);

    // race tabs
    const float tabW = 110;
    const float tabH = 50;
    Vector<MenuItem*> tabItems;
    raceTabBgs.assign(kRaceCount, nullptr);
    for (int r = 0; r < kRaceCount; r++) {
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(kRaceNames[r], tabW, tabH, [this, r](Ref*) { selectRace(r); }, &bg);
        item->setPosition(Vec2(r * (tabW + 10), 0));
        tabItems.pushBack(item);
        raceTabBgs[r] = bg;
    }
    Menu* raceMenu = Menu::createWithArray(tabItems);
    raceMenu->setPosition(Vec2(80, visibleSize.height - 105));
    unitToolsRow->addChild(raceMenu);

    // dynamic unit icon row, rebuilt per race tab
    objectPaletteRow = Node::create();
    objectPaletteRow->setPosition(Vec2(170, visibleSize.height - 165));
    unitToolsRow->addChild(objectPaletteRow);

    Label* paletteLabel = LM->getLocalizedLabel("Objects:", Color4B::WHITE, 22);
    paletteLabel->setAnchorPoint(Vec2(0, 0.5f));
    paletteLabel->setPosition(Vec2(20, visibleSize.height - 165));
    unitToolsRow->addChild(paletteLabel);

    // Only relevant for the Heroes tab (45 entries don't fit one unscrolled
    // row) - hidden for every other race. Sits to the right of the icon row.
    heroPageRow = addCycleRow(unitToolsRow, "Page:", 1140, visibleSize.height - 165, &lblHeroPage,
                              [this](int dir) { cycleHeroPage(dir); });

    highlightGroup(raceTabBgs, currentRaceTab);
    rebuildObjectPaletteRow();
}

void MapEditor::rebuildObjectPaletteRow() {
    objectPaletteRow->removeAllChildren();
    objectButtonBgs.clear();
    currentRaceUnitIndices.clear();

    const float btnSize = 70;
    const float spacing = 80;

    std::vector<int> raceIndices;
    for (int i = 0; i < kObjectTypeCount; i++) {
        if (kObjectTypes[i].race == currentRaceTab && !kObjectTypes[i].hiddenFromPalette) {
            raceIndices.push_back(i);
        }
    }

    int startIdx = 0;
    int endIdx = static_cast<int>(raceIndices.size());
    int pageCount = 1;
    if (currentRaceTab == RACE_HERO) {
        pageCount = std::max(1, (static_cast<int>(raceIndices.size()) + kHeroesPerPage - 1) / kHeroesPerPage);
        currentHeroPage = std::max(0, std::min(currentHeroPage, pageCount - 1));
        startIdx = currentHeroPage * kHeroesPerPage;
        endIdx = std::min(startIdx + kHeroesPerPage, static_cast<int>(raceIndices.size()));
    }

    Vector<MenuItem*> items;
    for (int k = startIdx; k < endIdx; k++) {
        int i = raceIndices[k];
        int slot = static_cast<int>(currentRaceUnitIndices.size());
        currentRaceUnitIndices.push_back(i);

        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item;
        if (kObjectTypes[i].isHero) {
            item = createHeroIconButton(kObjectTypes[i].spriteFrame, kObjectTypes[i].spineSkin, btnSize,
                                         [this, i](Ref*) { selectObjectTool(i); }, &bg);
        } else {
            item = createIconButton(kObjectTypes[i].spriteFrame, btnSize,
                                     [this, i](Ref*) { selectObjectTool(i); }, &bg);
        }
        item->setPosition(Vec2(slot * spacing, 0));
        items.pushBack(item);
        objectButtonBgs.push_back(bg);
    }

    Menu* objectMenu = Menu::createWithArray(items);
    objectMenu->setPosition(Vec2::ZERO);
    objectPaletteRow->addChild(objectMenu);

    if (heroPageRow) {
        heroPageRow->setVisible(currentRaceTab == RACE_HERO);
    }
    if (lblHeroPage) {
        lblHeroPage->setString(StringUtils::format("%d / %d", currentHeroPage + 1, pageCount));
    }
}

void MapEditor::cycleHeroPage(int dir) {
    int heroCount = 0;
    for (int i = 0; i < kObjectTypeCount; i++) {
        if (kObjectTypes[i].race == RACE_HERO && !kObjectTypes[i].hiddenFromPalette) {
            heroCount++;
        }
    }
    int pageCount = std::max(1, (heroCount + kHeroesPerPage - 1) / kHeroesPerPage);
    currentHeroPage = cyclicAdd(currentHeroPage, dir, 0, pageCount - 1);
    rebuildObjectPaletteRow();
}

void MapEditor::selectRace(int race) {
    currentRaceTab = race;
    currentHeroPage = 0;
    highlightGroup(raceTabBgs, currentRaceTab);
    rebuildObjectPaletteRow();
    // switching tabs implies "I want to place a unit from this race" — arm the first one
    if (!currentRaceUnitIndices.empty()) {
        selectObjectTool(currentRaceUnitIndices[0]);
    }
}

void MapEditor::highlightGroup(std::vector<Scale9Sprite*>& group, int activeIndex) {
    for (size_t i = 0; i < group.size(); i++) {
        if (!group[i]) {
            continue;
        }
        group[i]->setColor(static_cast<int>(i) == activeIndex ? Color3B(255, 221, 120) : Color3B::WHITE);
    }
}

int MapEditor::parsedHpOverride() const {
    if (!tfHp) {
        return -1;
    }
    std::string text = tfHp->getString();
    if (text.empty() || text == "default") {
        return -1;
    }
    int value = atoi(text.c_str());
    if (value <= 0) {
        return -1;
    }
    // Field just shows the selected unit's base HP (not a manual override) —
    // keep the -1 sentinel so future balance changes still apply.
    if (value == defaultHpForType(currentObjectType)) {
        return -1;
    }
    return value;
}

void MapEditor::selectSide(int side) {
    currentSide = side;
    highlightGroup(sideButtonBgs, currentSide);
    setStatus(kSideDescriptions[side]);
}

void MapEditor::setupUnitPropertiesBar() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    const float rowY = visibleSize.height - 225;

    Label* sideLabel = LM->getLocalizedLabel("Side:", Color4B::WHITE, 22);
    sideLabel->setAnchorPoint(Vec2(0, 0.5f));
    sideLabel->setPosition(Vec2(20, rowY));
    unitToolsRow->addChild(sideLabel);

    const float sideBtnW = 110;
    const float sideBtnH = 44;
    Vector<MenuItem*> sideItems;
    sideButtonBgs.assign(kSideCount, nullptr);
    for (int s = 0; s < kSideCount; s++) {
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(kSideNames[s], sideBtnW, sideBtnH, [this, s](Ref*) { selectSide(s); }, &bg);
        item->setPosition(Vec2(s * (sideBtnW + 10), 0));
        sideItems.pushBack(item);
        sideButtonBgs[s] = bg;
    }
    Menu* sideMenu = Menu::createWithArray(sideItems);
    sideMenu->setPosition(Vec2(200, rowY));
    unitToolsRow->addChild(sideMenu);
    highlightGroup(sideButtonBgs, currentSide);

    Label* hpLabel = LM->getLocalizedLabel("HP:", Color4B::WHITE, 22);
    hpLabel->setAnchorPoint(Vec2(0, 0.5f));
    hpLabel->setPosition(Vec2(650, rowY));
    unitToolsRow->addChild(hpLabel);

    DrawNode* hpBg = DrawNode::create();
    hpBg->drawSolidRect(Vec2(700, rowY - 20), Vec2(810, rowY + 20), Color4F(1, 1, 1, 0.18f));
    unitToolsRow->addChild(hpBg);

    int initialDefaultHp = defaultHpForType(currentObjectType);
    tfHp = TextField::create(initialDefaultHp > 0 ? StringUtils::format("%d", initialDefaultHp) : "default", "Arial", 22);
    tfHp->setAnchorPoint(Vec2(0, 0.5f));
    tfHp->setPosition(Vec2(710, rowY));
    tfHp->setTouchAreaEnabled(true);
    tfHp->setTouchSize(Size(100, 40));
    tfHp->setMaxLength(10);
    tfHp->setMaxLengthEnabled(true);
    unitToolsRow->addChild(tfHp);
    watchTextFieldFocus(tfHp);

    MenuItemSprite* itemDefaultHp = createTextButton("Default HP", 160, sideBtnH, [this](Ref*) {
        int baseHp = defaultHpForType(currentObjectType);
        tfHp->setString(baseHp > 0 ? StringUtils::format("%d", baseHp) : "default");
        setStatus("New placements will use the unit's default HP");
    });
    itemDefaultHp->setPosition(Vec2(900, rowY));
    Menu* hpMenu = Menu::create(itemDefaultHp, nullptr);
    hpMenu->setPosition(Vec2::ZERO);
    unitToolsRow->addChild(hpMenu);

    // Heroes only: HP/ATT scale with Level instead of a flat value, so the HP
    // field above is irrelevant to them - this replaces it when isHero.
    rowLevel = Node::create();
    unitToolsRow->addChild(rowLevel);

    Label* levelLabel = LM->getLocalizedLabel("Level:", Color4B::WHITE, 22);
    levelLabel->setAnchorPoint(Vec2(0, 0.5f));
    levelLabel->setPosition(Vec2(1060, rowY));
    rowLevel->addChild(levelLabel);

    DrawNode* levelBg = DrawNode::create();
    levelBg->drawSolidRect(Vec2(1130, rowY - 20), Vec2(1210, rowY + 20), Color4F(1, 1, 1, 0.18f));
    rowLevel->addChild(levelBg);

    tfLevel = TextField::create("1", "Arial", 22);
    tfLevel->setAnchorPoint(Vec2(0, 0.5f));
    tfLevel->setPosition(Vec2(1140, rowY));
    tfLevel->setTouchAreaEnabled(true);
    tfLevel->setTouchSize(Size(70, 40));
    tfLevel->setMaxLength(3);
    tfLevel->setMaxLengthEnabled(true);
    rowLevel->addChild(tfLevel);
    watchTextFieldFocus(tfLevel);

    rowLevel->setVisible(kObjectTypes[currentObjectType].isHero);
}

void MapEditor::setupMouseCoordLabel() {
    mouseCoordLabel = LM->getLocalizedLabel("", Color4B::WHITE, 20);
    mouseCoordLabel->setAnchorPoint(Vec2(0, 0));
    mouseCoordLabel->setPosition(Vec2(10, 10));
    this->addChild(mouseCoordLabel, 50);
}

void MapEditor::updateMouseCoordLabel(const Vec2& worldPos) {
    if (!mouseCoordLabel) {
        return;
    }
    int x, y;
    worldToCell(worldPos, x, y);
    mouseCoordLabel->setString(StringUtils::format("(%d, %d)", x, y));
}

void MapEditor::setupPaletteTooltip() {
    paletteTooltip = Node::create();
    paletteTooltip->setVisible(false);
    this->addChild(paletteTooltip, 50);

    paletteTooltipBg = DrawNode::create();
    paletteTooltip->addChild(paletteTooltipBg);

    paletteTooltipLabel = LM->getLocalizedLabel("", Color4B::WHITE, 20);
    paletteTooltipLabel->setAnchorPoint(Vec2(0, 1));
    paletteTooltip->addChild(paletteTooltipLabel);
}

std::string MapEditor::paletteTooltipText(int typeIndex) const {
    if (typeIndex < 0 || typeIndex >= kObjectTypeCount) {
        return "";
    }
    const ObjectTypeDef& def = kObjectTypes[typeIndex];
    std::string text = def.name;
    int baseHp = defaultHpForType(typeIndex);
    if (baseHp > 0) {
        text += StringUtils::format("\nBase HP: %d", baseHp);
    }
    if (def.tooltip[0] != '\0') {
        text += "\n";
        text += def.tooltip;
    }
    return text;
}

// objectButtonBgs[slot]'s bounding box is already in its parent's coordinate
// space (cocos2d convention), so we only need one more hop up the chain to
// reach world space.
Rect MapEditor::paletteSlotWorldRect(int slot) const {
    if (slot < 0 || slot >= static_cast<int>(objectButtonBgs.size()) || !objectButtonBgs[slot]) {
        return Rect::ZERO;
    }
    Node* bg = objectButtonBgs[slot];
    Node* parent = bg->getParent();
    if (!parent) {
        return Rect::ZERO;
    }
    Rect localBox = bg->getBoundingBox();
    Vec2 worldOrigin = parent->convertToWorldSpace(localBox.origin);
    Vec2 worldTopRight = parent->convertToWorldSpace(Vec2(localBox.getMaxX(), localBox.getMaxY()));
    return Rect(worldOrigin.x, worldOrigin.y, worldTopRight.x - worldOrigin.x, worldTopRight.y - worldOrigin.y);
}

void MapEditor::updateHoverTooltip(const Vec2& worldPos) {
    if (!paletteTooltip) {
        return;
    }
    // Hide while any drag/paint/pan interaction is in flight — a tooltip
    // popping up mid-stroke is distracting and can sit on stale data.
    if (isAnyModalOpen() || isPainting || isPanning || isBandSelecting || movingObjectIndex >= 0) {
        paletteTooltip->setVisible(false);
        return;
    }
    if (currentMainTab == TAB_UNITS) {
        for (size_t slot = 0; slot < currentRaceUnitIndices.size(); slot++) {
            Rect rect = paletteSlotWorldRect(static_cast<int>(slot));
            if (!rect.containsPoint(worldPos)) {
                continue;
            }
            showHoverTooltip(paletteTooltipText(currentRaceUnitIndices[slot]),
                             Vec2(rect.origin.x, rect.getMaxY()));
            return;
        }
    }
    int objectIndex = findObjectAtWorldPos(worldPos);
    if (objectIndex >= 0) {
        const PlacedObject& obj = placedObjects[objectIndex];
        std::string text = StringUtils::format("%s\nID: %d", kObjectTypes[obj.typeIndex].name, obj.id);
        if (!obj.alias.empty()) {
            text += "\nAlias: " + obj.alias;
        }
        Rect box = obj.sprite->getBoundingBox(); // mapRoot space
        Vec2 anchor = mapRoot->convertToWorldSpace(Vec2(box.getMinX(), box.getMaxY()));
        showHoverTooltip(text, anchor);
        return;
    }
    paletteTooltip->setVisible(false);
}

void MapEditor::showHoverTooltip(const std::string& text, const Vec2& anchorWorldPos) {
    paletteTooltipLabel->setString(text);

    const float paddingX = 12;
    const float paddingY = 10;
    Size textSize = paletteTooltipLabel->getContentSize();
    Size bgSize(textSize.width + paddingX * 2, textSize.height + paddingY * 2);

    paletteTooltipBg->clear();
    paletteTooltipBg->drawSolidRect(Vec2::ZERO, Vec2(bgSize.width, bgSize.height), Color4F(0, 0, 0, 0.85f));
    paletteTooltipLabel->setPosition(Vec2(paddingX, bgSize.height - paddingY));

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 pos(anchorWorldPos.x, anchorWorldPos.y + 8);
    pos.x = std::max(4.0f, std::min(pos.x, visibleSize.width - bgSize.width - 4.0f));
    pos.y = std::max(4.0f, std::min(pos.y, visibleSize.height - bgSize.height - 4.0f));

    paletteTooltip->setPosition(pos);
    paletteTooltip->setVisible(true);
}

void MapEditor::setupTriggerToolsRow() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    triggerToolsRow = Node::create();
    this->addChild(triggerToolsRow, 10);

    MenuItemSprite* itemOpenTriggers = createTextButton("Open Trigger Editor", 260, 56, [this](Ref*) { openTriggerPanel(); });
    itemOpenTriggers->setPosition(Vec2(150, visibleSize.height - 105));
    Menu* openMenu = Menu::create(itemOpenTriggers, nullptr);
    openMenu->setPosition(Vec2::ZERO);
    triggerToolsRow->addChild(openMenu);

    Label* hintLabel = LM->getLocalizedLabel(
        "Click a unit or flag on the map to inspect/edit it (id, side, HP). This tab does not paint or place.",
        Color4B::WHITE, 20);
    hintLabel->setAnchorPoint(Vec2(0, 0.5f));
    hintLabel->setPosition(Vec2(20, visibleSize.height - 165));
    triggerToolsRow->addChild(hintLabel);
}

void MapEditor::setupMoveToolsRow() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    moveToolsRow = Node::create();
    this->addChild(moveToolsRow, 10);

    MenuItemSprite* itemDuplicate = createTextButton("Duplicate", 200, 56, [this](Ref*) { duplicateSelection(); });
    itemDuplicate->setPosition(Vec2(120, visibleSize.height - 105));
    MenuItemSprite* itemDelete = createTextButton("Delete", 160, 56, [this](Ref*) { deleteSelection(); });
    itemDelete->setPosition(Vec2(320, visibleSize.height - 105));
    Menu* dupMenu = Menu::create(itemDuplicate, itemDelete, nullptr);
    dupMenu->setPosition(Vec2::ZERO);
    moveToolsRow->addChild(dupMenu);

    Label* hintLabel = LM->getLocalizedLabel(
        "Drag a placed unit to move it; a plain click opens its property editor. Duplicate (Ctrl+C, Ctrl+V) copies the selection; Delete (Del) removes it.",
        Color4B::WHITE, 20);
    hintLabel->setAnchorPoint(Vec2(0, 0.5f));
    hintLabel->setPosition(Vec2(20, visibleSize.height - 165));
    moveToolsRow->addChild(hintLabel);
}

void MapEditor::setupMainTabs() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const char* const kMainTabNames[] = {"Terrain", "Units", "Trigger", "Move"};
    const int kMainTabCount = 4;
    const float tabW = 150;
    const float tabH = 56;

    Vector<MenuItem*> tabItems;
    mainTabBgs.assign(kMainTabCount, nullptr);
    for (int t = 0; t < kMainTabCount; t++) {
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(kMainTabNames[t], tabW, tabH, [this, t](Ref*) { selectMainTab(t); }, &bg);
        item->setPosition(Vec2(t * (tabW + 10), 0));
        tabItems.pushBack(item);
        mainTabBgs[t] = bg;
    }
    Menu* mainTabMenu = Menu::createWithArray(tabItems);
    mainTabMenu->setPosition(Vec2(100, visibleSize.height - 45));
    this->addChild(mainTabMenu, 10);

    selectMainTab(TAB_TERRAIN);
}

void MapEditor::selectMainTab(int tab) {
    currentMainTab = tab;
    highlightGroup(mainTabBgs, tab);
    terrainToolsRow->setVisible(tab == TAB_TERRAIN);
    unitToolsRow->setVisible(tab == TAB_UNITS);
    triggerToolsRow->setVisible(tab == TAB_TRIGGER);
    moveToolsRow->setVisible(tab == TAB_MOVE);
    if (paletteTooltip) {
        paletteTooltip->setVisible(false);
    }

    // Switching into Terrain/Units implies "I want to use this tool" — arm it,
    // matching the existing per-race tab behavior. Trigger and Move have no
    // default tool to arm (they act on already-placed units, not the palette).
    if (tab == TAB_TERRAIN) {
        selectBrush(currentBrush);
    } else if (tab == TAB_UNITS) {
        selectRace(currentRaceTab);
    }

    // The multi-selection only means something in the Trigger/Move tabs (it
    // survives switching between those two); drop it elsewhere so the overlay
    // doesn't linger over terrain painting.
    if (tab != TAB_TRIGGER && tab != TAB_MOVE) {
        clearSelection();
    }
    isBandSelecting = false;
    if (bandSelectRect) {
        bandSelectRect->clear();
        bandSelectRect->setVisible(false);
    }
}

void MapEditor::setupModalDimmer() {
    // No explicit width/height: LayerColor::create(color) auto-sizes to
    // Director::getWinSize(). An explicitly-sized LayerColor much smaller
    // than that does not reliably render on this engine build, so we lean on
    // the auto-sized form, which is proven to work, instead.
    modalDimmer = LayerColor::create(Color4B(0, 0, 0, 160));
    modalDimmer->setVisible(false);
    this->addChild(modalDimmer, 15); // above toolbar/grid (z=10), below modal panels (z=20)

    // Block touch/click events from reaching toolbar Menu buttons behind the dimmer.
    // SceneGraphPriority puts this listener above all z<15 nodes, so it fires first
    // and swallows the event. Checking isVisible() ensures no swallowing when hidden,
    // so normal operation is unaffected. Any future popup that calls
    // setModalDimmerVisible(true) automatically benefits from this blocking.
    auto* blocker = EventListenerTouchOneByOne::create();
    blocker->setSwallowTouches(true);
    blocker->onTouchBegan = [this](Touch*, Event*) -> bool {
        return modalDimmer->isVisible();
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(blocker, modalDimmer);

    // Second blocker between triggerPanel (z=20) and its child edit panels (z=25).
    // Same pattern: swallows touches only when visible.
    triggerSubDimmer = LayerColor::create(Color4B(0, 0, 0, 120));
    triggerSubDimmer->setVisible(false);
    this->addChild(triggerSubDimmer, 22);

    auto* subBlocker = EventListenerTouchOneByOne::create();
    subBlocker->setSwallowTouches(true);
    subBlocker->onTouchBegan = [this](Touch*, Event*) -> bool {
        return triggerSubDimmer->isVisible();
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(subBlocker, triggerSubDimmer);
}

void MapEditor::setModalDimmerVisible(bool visible) {
    if (modalDimmer) {
        modalDimmer->setVisible(visible);
    }
}

void MapEditor::setTriggerSubDimmerVisible(bool visible) {
    if (triggerSubDimmer) {
        triggerSubDimmer->setVisible(visible);
    }
}

void MapEditor::setupNewMapPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const float panelWidth = 420;
    const float panelHeight = 240;

    newMapPanel = Node::create();
    newMapPanel->setPosition(Vec2(60, visibleSize.height - 170 - panelHeight));
    newMapPanel->setVisible(false);
    this->addChild(newMapPanel, 20);

    DrawNode* panelBg = DrawNode::create();
    panelBg->drawSolidRect(Vec2(0, 0), Vec2(panelWidth, panelHeight), Color4F(0.08f, 0.08f, 0.08f, 0.9f));
    newMapPanel->addChild(panelBg);

    Label* title = LM->getLocalizedLabel("New Map", Color4B::WHITE, 28);
    title->setPosition(Vec2(panelWidth / 2, panelHeight - 30));
    newMapPanel->addChild(title);

    const float fieldBoxWidth = 160;
    const float fieldBoxHeight = 44;
    const float fieldBoxX = 140;

    Label* lblWidth = LM->getLocalizedLabel("Width:", Color4B::WHITE, 24);
    lblWidth->setAnchorPoint(Vec2(0, 0.5f));
    lblWidth->setPosition(Vec2(40, panelHeight - 90));
    newMapPanel->addChild(lblWidth);

    DrawNode* tfWidthBg = DrawNode::create();
    tfWidthBg->drawSolidRect(Vec2(fieldBoxX, panelHeight - 90 - fieldBoxHeight / 2),
                              Vec2(fieldBoxX + fieldBoxWidth, panelHeight - 90 + fieldBoxHeight / 2),
                              Color4F(1, 1, 1, 0.18f));
    newMapPanel->addChild(tfWidthBg);

    tfWidth = TextField::create("40", "Arial", 24);
    tfWidth->setAnchorPoint(Vec2(0, 0.5f));
    tfWidth->setPosition(Vec2(fieldBoxX + 14, panelHeight - 90));
    tfWidth->setTouchAreaEnabled(true);
    tfWidth->setTouchSize(Size(fieldBoxWidth, fieldBoxHeight));
    tfWidth->setMaxLength(3);
    tfWidth->setMaxLengthEnabled(true);
    newMapPanel->addChild(tfWidth);
    watchTextFieldFocus(tfWidth);

    Label* lblHeight = LM->getLocalizedLabel("Height:", Color4B::WHITE, 24);
    lblHeight->setAnchorPoint(Vec2(0, 0.5f));
    lblHeight->setPosition(Vec2(40, panelHeight - 140));
    newMapPanel->addChild(lblHeight);

    DrawNode* tfHeightBg = DrawNode::create();
    tfHeightBg->drawSolidRect(Vec2(fieldBoxX, panelHeight - 140 - fieldBoxHeight / 2),
                               Vec2(fieldBoxX + fieldBoxWidth, panelHeight - 140 + fieldBoxHeight / 2),
                               Color4F(1, 1, 1, 0.18f));
    newMapPanel->addChild(tfHeightBg);

    tfHeight = TextField::create("40", "Arial", 24);
    tfHeight->setAnchorPoint(Vec2(0, 0.5f));
    tfHeight->setPosition(Vec2(fieldBoxX + 14, panelHeight - 140));
    tfHeight->setTouchAreaEnabled(true);
    tfHeight->setTouchSize(Size(fieldBoxWidth, fieldBoxHeight));
    tfHeight->setMaxLength(3);
    tfHeight->setMaxLengthEnabled(true);
    newMapPanel->addChild(tfHeight);
    watchTextFieldFocus(tfHeight);

    const float confirmBtnW = 150;
    const float confirmBtnH = 60;
    MenuItemSprite* itemCreate = createTextButton("Create", confirmBtnW, confirmBtnH, [this](Ref*) { onConfirmNewMap(); });
    MenuItemSprite* itemCancel = createTextButton("Cancel", confirmBtnW, confirmBtnH, [this](Ref*) { hideNewMapPanel(); });

    itemCreate->setPosition(Vec2(125, 35));
    itemCancel->setPosition(Vec2(295, 35));
    Menu* confirmMenu = Menu::create(itemCreate, itemCancel, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    newMapPanel->addChild(confirmMenu);
}

void MapEditor::showNewMapPanel() {
    tfWidth->setString(StringUtils::format("%d", mapWidth));
    tfHeight->setString(StringUtils::format("%d", mapHeight));
    newMapPanel->setVisible(true);
    isNewMapPanelOpen = true;
    setModalDimmerVisible(true);
}

void MapEditor::hideNewMapPanel() {
    newMapPanel->setVisible(false);
    isNewMapPanelOpen = false;
    setModalDimmerVisible(false);
}

void MapEditor::onConfirmNewMap() {
    int width = atoi(tfWidth->getString().c_str());
    int height = atoi(tfHeight->getString().c_str());
    if (width <= 0 || height <= 0) {
        setStatus("Invalid size");
        return;
    }
    buildMap(width, height);
    addDefaultWinTrigger();
    hideNewMapPanel();
    setStatus(StringUtils::format("New map %dx%d", mapWidth, mapHeight));
}

void MapEditor::setupResizeMapPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const float panelWidth = 420;
    const float panelHeight = 240;

    resizeMapPanel = Node::create();
    resizeMapPanel->setPosition(Vec2(60, visibleSize.height - 170 - panelHeight));
    resizeMapPanel->setVisible(false);
    this->addChild(resizeMapPanel, 20);

    DrawNode* panelBg = DrawNode::create();
    panelBg->drawSolidRect(Vec2(0, 0), Vec2(panelWidth, panelHeight), Color4F(0.08f, 0.08f, 0.08f, 0.9f));
    resizeMapPanel->addChild(panelBg);

    Label* title = LM->getLocalizedLabel("Resize Map", Color4B::WHITE, 28);
    title->setPosition(Vec2(panelWidth / 2, panelHeight - 30));
    resizeMapPanel->addChild(title);

    const float fieldBoxWidth = 160;
    const float fieldBoxHeight = 44;
    const float fieldBoxX = 140;

    Label* lblWidth = LM->getLocalizedLabel("Width:", Color4B::WHITE, 24);
    lblWidth->setAnchorPoint(Vec2(0, 0.5f));
    lblWidth->setPosition(Vec2(40, panelHeight - 90));
    resizeMapPanel->addChild(lblWidth);

    DrawNode* tfWidthBg = DrawNode::create();
    tfWidthBg->drawSolidRect(Vec2(fieldBoxX, panelHeight - 90 - fieldBoxHeight / 2),
                              Vec2(fieldBoxX + fieldBoxWidth, panelHeight - 90 + fieldBoxHeight / 2),
                              Color4F(1, 1, 1, 0.18f));
    resizeMapPanel->addChild(tfWidthBg);

    tfResizeWidth = TextField::create("40", "Arial", 24);
    tfResizeWidth->setAnchorPoint(Vec2(0, 0.5f));
    tfResizeWidth->setPosition(Vec2(fieldBoxX + 14, panelHeight - 90));
    tfResizeWidth->setTouchAreaEnabled(true);
    tfResizeWidth->setTouchSize(Size(fieldBoxWidth, fieldBoxHeight));
    tfResizeWidth->setMaxLength(3);
    tfResizeWidth->setMaxLengthEnabled(true);
    resizeMapPanel->addChild(tfResizeWidth);
    watchTextFieldFocus(tfResizeWidth);

    Label* lblHeight = LM->getLocalizedLabel("Height:", Color4B::WHITE, 24);
    lblHeight->setAnchorPoint(Vec2(0, 0.5f));
    lblHeight->setPosition(Vec2(40, panelHeight - 140));
    resizeMapPanel->addChild(lblHeight);

    DrawNode* tfHeightBg = DrawNode::create();
    tfHeightBg->drawSolidRect(Vec2(fieldBoxX, panelHeight - 140 - fieldBoxHeight / 2),
                               Vec2(fieldBoxX + fieldBoxWidth, panelHeight - 140 + fieldBoxHeight / 2),
                               Color4F(1, 1, 1, 0.18f));
    resizeMapPanel->addChild(tfHeightBg);

    tfResizeHeight = TextField::create("40", "Arial", 24);
    tfResizeHeight->setAnchorPoint(Vec2(0, 0.5f));
    tfResizeHeight->setPosition(Vec2(fieldBoxX + 14, panelHeight - 140));
    tfResizeHeight->setTouchAreaEnabled(true);
    tfResizeHeight->setTouchSize(Size(fieldBoxWidth, fieldBoxHeight));
    tfResizeHeight->setMaxLength(3);
    tfResizeHeight->setMaxLengthEnabled(true);
    resizeMapPanel->addChild(tfResizeHeight);
    watchTextFieldFocus(tfResizeHeight);

    const float confirmBtnW = 150;
    const float confirmBtnH = 60;
    MenuItemSprite* itemResize = createTextButton("Resize", confirmBtnW, confirmBtnH, [this](Ref*) { onConfirmResizeMap(); });
    MenuItemSprite* itemCancel = createTextButton("Cancel", confirmBtnW, confirmBtnH, [this](Ref*) { hideResizeMapPanel(); });

    itemResize->setPosition(Vec2(125, 35));
    itemCancel->setPosition(Vec2(295, 35));
    Menu* confirmMenu = Menu::create(itemResize, itemCancel, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    resizeMapPanel->addChild(confirmMenu);
}

void MapEditor::showResizeMapPanel() {
    tfResizeWidth->setString(StringUtils::format("%d", mapWidth));
    tfResizeHeight->setString(StringUtils::format("%d", mapHeight));
    resizeMapPanel->setVisible(true);
    isResizeMapPanelOpen = true;
    setModalDimmerVisible(true);
}

void MapEditor::hideResizeMapPanel() {
    resizeMapPanel->setVisible(false);
    isResizeMapPanelOpen = false;
    setModalDimmerVisible(false);
}

void MapEditor::onConfirmResizeMap() {
    int width = atoi(tfResizeWidth->getString().c_str());
    int height = atoi(tfResizeHeight->getString().c_str());
    if (width <= 0 || height <= 0) {
        setStatus("Invalid size");
        return;
    }
    resizeMap(width, height);
    hideResizeMapPanel();
    setStatus(StringUtils::format("Resized to %dx%d", mapWidth, mapHeight));
}

void MapEditor::setupMapSettingsPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    mapSettingsBuildingIndices.clear();
    mapSettingsUnitIndices.clear();
    for (int i = 0; i < kObjectTypeCount; i++) {
        if (isConstructibleBuildingType(i)) {
            mapSettingsBuildingIndices.push_back(i);
        } else if (isTrainableUnitType(i)) {
            mapSettingsUnitIndices.push_back(i);
        }
    }

    const int columns = 3;
    const float rowSpacing = 58;
    // Size the panel to whichever tab needs more rows so both fit.
    int maxCount = std::max(static_cast<int>(mapSettingsBuildingIndices.size()),
                            static_cast<int>(mapSettingsUnitIndices.size()));
    const int rows = (maxCount + columns - 1) / columns;

    const float panelWidth = 20 + columns * 320;
    // Room for title, tab row, hint, the grid, and the close button.
    const float panelHeight = 200 + rows * rowSpacing + 70;

    mapSettingsPanel = Node::create();
    mapSettingsPanel->setPosition(Vec2((visibleSize.width - panelWidth) / 2,
                                       (visibleSize.height - panelHeight) / 2));
    mapSettingsPanel->setVisible(false);
    this->addChild(mapSettingsPanel, 20);

    DrawNode* panelBg = DrawNode::create();
    panelBg->drawSolidRect(Vec2(0, 0), Vec2(panelWidth, panelHeight), Color4F(0.08f, 0.08f, 0.08f, 0.95f));
    mapSettingsPanel->addChild(panelBg);

    Label* title = LM->getLocalizedLabel("Production Restrictions", Color4B::WHITE, 28);
    title->setPosition(Vec2(panelWidth / 2, panelHeight - 30));
    mapSettingsPanel->addChild(title);

    // Buildings / Units tab row.
    const float tabW = 200;
    const float tabH = 46;
    const char* const tabNames[] = {"Buildings", "Units"};
    mapSettingsTabBgs.assign(2, nullptr);
    Vector<MenuItem*> tabItems;
    for (int t = 0; t < 2; t++) {
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(tabNames[t], tabW, tabH,
                                                 [this, t](Ref*) { selectMapSettingsTab(t); }, &bg);
        item->setPosition(Vec2(panelWidth / 2 + (t == 0 ? -tabW / 2 - 6 : tabW / 2 + 6), panelHeight - 80));
        tabItems.pushBack(item);
        mapSettingsTabBgs[t] = bg;
    }
    Menu* tabMenu = Menu::createWithArray(tabItems);
    tabMenu->setPosition(Vec2::ZERO);
    mapSettingsPanel->addChild(tabMenu);

    Label* hint = LM->getLocalizedLabel(
        "Click an item to ban it on this map. Red = cannot be produced, by any side including the AI.",
        Color4B(200, 200, 200, 255), 20);
    hint->setPosition(Vec2(panelWidth / 2, panelHeight - 118));
    mapSettingsPanel->addChild(hint);

    const float gridTopY = panelHeight - 150;

    mapSettingsBuildingContainer = Node::create();
    mapSettingsPanel->addChild(mapSettingsBuildingContainer);
    buildMapSettingsGrid(mapSettingsBuildingContainer, mapSettingsBuildingIndices,
                         mapSettingsBuildingBgs, panelWidth, gridTopY);

    mapSettingsUnitContainer = Node::create();
    mapSettingsPanel->addChild(mapSettingsUnitContainer);
    buildMapSettingsGrid(mapSettingsUnitContainer, mapSettingsUnitIndices,
                         mapSettingsUnitBgs, panelWidth, gridTopY);

    MenuItemSprite* itemClose = createTextButton("Close", 150, 56, [this](Ref*) { hideMapSettingsPanel(); });
    itemClose->setPosition(Vec2(panelWidth / 2, 45));
    Menu* closeMenu = Menu::create(itemClose, nullptr);
    closeMenu->setPosition(Vec2::ZERO);
    mapSettingsPanel->addChild(closeMenu);

    selectMapSettingsTab(0);
}

void MapEditor::buildMapSettingsGrid(Node* container, const std::vector<int>& typeIndices,
                                     std::vector<Scale9Sprite*>& outBgs,
                                     float panelWidth, float topY) {
    const int columns = 3;
    const float btnW = 300;
    const float btnH = 48;
    const float colSpacing = 320;
    const float rowSpacing = 58;

    Vector<MenuItem*> items;
    outBgs.assign(typeIndices.size(), nullptr);
    for (size_t slot = 0; slot < typeIndices.size(); slot++) {
        int typeIndex = typeIndices[slot];
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(kObjectTypes[typeIndex].name, btnW, btnH,
                                                 [this, typeIndex](Ref*) { toggleDisabledType(typeIndex); }, &bg);
        float col = static_cast<float>(slot % columns);
        float row = static_cast<float>(slot / columns);
        item->setPosition(Vec2(20 + col * colSpacing + btnW / 2,
                               topY - row * rowSpacing - btnH / 2));
        items.pushBack(item);
        outBgs[slot] = bg;
    }
    if (!items.empty()) {
        Menu* menu = Menu::createWithArray(items);
        menu->setPosition(Vec2::ZERO);
        container->addChild(menu);
    }
}

void MapEditor::showMapSettingsPanel() {
    refreshMapSettingsButtons();
    mapSettingsPanel->setVisible(true);
    isMapSettingsPanelOpen = true;
    setModalDimmerVisible(true);
}

void MapEditor::hideMapSettingsPanel() {
    mapSettingsPanel->setVisible(false);
    isMapSettingsPanelOpen = false;
    setModalDimmerVisible(false);
}

void MapEditor::selectMapSettingsTab(int tab) {
    mapSettingsTab = tab;
    if (mapSettingsBuildingContainer) {
        mapSettingsBuildingContainer->setVisible(tab == 0);
    }
    if (mapSettingsUnitContainer) {
        mapSettingsUnitContainer->setVisible(tab == 1);
    }
    for (int t = 0; t < static_cast<int>(mapSettingsTabBgs.size()); t++) {
        if (mapSettingsTabBgs[t]) {
            mapSettingsTabBgs[t]->setColor(t == tab ? Color3B(255, 221, 120) : Color3B::WHITE);
        }
    }
}

void MapEditor::toggleDisabledType(int typeIndex) {
    std::set<int>& target = isConstructibleBuildingType(typeIndex) ? disabledBuildings : disabledUnits;
    if (target.count(typeIndex)) {
        target.erase(typeIndex);
    } else {
        target.insert(typeIndex);
    }
    isDirty = true;
    refreshMapSettingsButtons();
}

void MapEditor::refreshMapSettingsButtons() {
    for (size_t slot = 0; slot < mapSettingsBuildingIndices.size(); slot++) {
        Scale9Sprite* bg = mapSettingsBuildingBgs[slot];
        if (bg) {
            bool banned = disabledBuildings.count(mapSettingsBuildingIndices[slot]) > 0;
            bg->setColor(banned ? Color3B(235, 110, 110) : Color3B::WHITE);
        }
    }
    for (size_t slot = 0; slot < mapSettingsUnitIndices.size(); slot++) {
        Scale9Sprite* bg = mapSettingsUnitBgs[slot];
        if (bg) {
            bool banned = disabledUnits.count(mapSettingsUnitIndices[slot]) > 0;
            bg->setColor(banned ? Color3B(235, 110, 110) : Color3B::WHITE);
        }
    }
}

void MapEditor::addDefaultWinTrigger() {
    Trigger t;
    t.name = "Win";
    t.preserve = true;
    t.sides[SIDE_ALLY] = true;

    TriggerCondition c;
    c.type = COND_BUILDING_COUNT;
    c.unitSide = SIDE_ENEMY;
    c.comparison = CMP_AT_MOST;
    c.amount = 0;
    t.conditions.push_back(c);

    TriggerAction a;
    a.type = TACT_VICTORY;
    t.actions.push_back(a);

    triggers.push_back(t);
}

void MapEditor::setupEditUnitPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const float panelWidth = 620;
    const float panelHeight = 320;

    editUnitPanel = Node::create();
    // NOTE: centering via (visibleSize.width|height - panelSize) / 2 reliably
    // misplaces panels near the bottom-right corner on this engine build (same
    // issue hit with the New Map panel) — stick to the edge-relative offset
    // pattern that's proven to render correctly everywhere else in this class.
    editUnitPanel->setPosition(Vec2(220, visibleSize.height - 660));
    editUnitPanel->setVisible(false);
    this->addChild(editUnitPanel, 20);

    // verticalScrollBackground.png: a hand-drawn scroll with rolled top/bottom
    // edges. 9-sliced so the rolls stay crisp while the body stretches. The
    // unstretched margins render at native pixel size regardless of panel
    // size, so they need to stay small relative to our (much smaller than
    // the 1433x1174 source) panel or the rolled ends swallow the whole thing.
    Scale9Sprite* panelBg = Scale9Sprite::create("verticalScrollBackground.png");
    panelBg->setCapInsets(Rect(50, 60, 1333, 1054));
    panelBg->setContentSize(Size(panelWidth, panelHeight));
    panelBg->setPosition(Vec2(panelWidth / 2, panelHeight / 2));
    editUnitPanel->addChild(panelBg);

    const Color3B panelTextColor(60, 40, 25); // dark brown, readable on the parchment

    editUnitTitle = LM->getLocalizedLabel("Edit Unit", Color4B(panelTextColor.r, panelTextColor.g, panelTextColor.b, 255), 28);
    editUnitTitle->setPosition(Vec2(panelWidth / 2, panelHeight - 40));
    editUnitPanel->addChild(editUnitTitle);

    Label* sideLabel = LM->getLocalizedLabel("Side:", Color4B(panelTextColor.r, panelTextColor.g, panelTextColor.b, 255), 22);
    sideLabel->setAnchorPoint(Vec2(0, 0.5f));
    sideLabel->setPosition(Vec2(20, panelHeight - 100));
    editUnitPanel->addChild(sideLabel);

    const float sideBtnW = 110;
    const float sideBtnH = 44;
    Vector<MenuItem*> sideItems;
    editSideButtonBgs.assign(kSideCount, nullptr);
    for (int s = 0; s < kSideCount; s++) {
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(kSideNames[s], sideBtnW, sideBtnH, [this, s](Ref*) { selectEditSide(s); }, &bg);
        item->setPosition(Vec2(s * (sideBtnW + 8), 0));
        sideItems.pushBack(item);
        editSideButtonBgs[s] = bg;
    }
    Menu* sideMenu = Menu::createWithArray(sideItems);
    sideMenu->setPosition(Vec2(110, panelHeight - 100));
    editUnitPanel->addChild(sideMenu);

    rowEditHp = Node::create();
    editUnitPanel->addChild(rowEditHp);

    Label* hpLabel = LM->getLocalizedLabel("HP:", Color4B(panelTextColor.r, panelTextColor.g, panelTextColor.b, 255), 22);
    hpLabel->setAnchorPoint(Vec2(0, 0.5f));
    hpLabel->setPosition(Vec2(40, panelHeight - 165));
    rowEditHp->addChild(hpLabel);

    DrawNode* hpBg = DrawNode::create();
    hpBg->drawSolidRect(Vec2(140, panelHeight - 187), Vec2(320, panelHeight - 143), Color4F(0, 0, 0, 0.12f));
    rowEditHp->addChild(hpBg);

    tfEditHp = TextField::create("default", "Arial", 22);
    tfEditHp->setAnchorPoint(Vec2(0, 0.5f));
    tfEditHp->setPosition(Vec2(150, panelHeight - 165));
    tfEditHp->setTouchAreaEnabled(true);
    tfEditHp->setTouchSize(Size(170, 44));
    tfEditHp->setMaxLength(10);
    tfEditHp->setMaxLengthEnabled(true);
    tfEditHp->setTextColor(Color4B(60, 40, 25, 255));
    rowEditHp->addChild(tfEditHp);
    watchTextFieldFocus(tfEditHp);

    // Heroes only: shown in place of HP, since their HP/ATT scale with Level.
    rowEditLevel = Node::create();
    editUnitPanel->addChild(rowEditLevel);

    Label* editLevelLabel = LM->getLocalizedLabel("Level:", Color4B(panelTextColor.r, panelTextColor.g, panelTextColor.b, 255), 22);
    editLevelLabel->setAnchorPoint(Vec2(0, 0.5f));
    editLevelLabel->setPosition(Vec2(360, panelHeight - 165));
    rowEditLevel->addChild(editLevelLabel);

    DrawNode* editLevelBg = DrawNode::create();
    editLevelBg->drawSolidRect(Vec2(450, panelHeight - 187), Vec2(560, panelHeight - 143), Color4F(0, 0, 0, 0.12f));
    rowEditLevel->addChild(editLevelBg);

    tfEditLevel = TextField::create("1", "Arial", 22);
    tfEditLevel->setAnchorPoint(Vec2(0, 0.5f));
    tfEditLevel->setPosition(Vec2(460, panelHeight - 165));
    tfEditLevel->setTouchAreaEnabled(true);
    tfEditLevel->setTouchSize(Size(100, 44));
    tfEditLevel->setMaxLength(3);
    tfEditLevel->setMaxLengthEnabled(true);
    tfEditLevel->setTextColor(Color4B(60, 40, 25, 255));
    rowEditLevel->addChild(tfEditLevel);
    watchTextFieldFocus(tfEditLevel);

    // Flags only: shown in place of HP (a Flag has none). Controls whether
    // the flag art is rendered at runtime; hidden flags remain trigger targets.
    rowEditVisible = Node::create();
    editUnitPanel->addChild(rowEditVisible);

    Label* visibleLabel = LM->getLocalizedLabel("Visible:", Color4B(panelTextColor.r, panelTextColor.g, panelTextColor.b, 255), 22);
    visibleLabel->setAnchorPoint(Vec2(0, 0.5f));
    visibleLabel->setPosition(Vec2(40, panelHeight - 165));
    rowEditVisible->addChild(visibleLabel);

    const char* kVisibleNames[] = {"Show", "Hide"};
    Vector<MenuItem*> visibleItems;
    editVisibleButtonBgs.assign(2, nullptr);
    for (int v = 0; v < 2; v++) {
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(kVisibleNames[v], sideBtnW, sideBtnH, [this, v](Ref*) { selectEditVisible(v); }, &bg);
        item->setPosition(Vec2(v * (sideBtnW + 8), 0));
        visibleItems.pushBack(item);
        editVisibleButtonBgs[v] = bg;
    }
    Menu* visibleMenu = Menu::createWithArray(visibleItems);
    visibleMenu->setPosition(Vec2(200, panelHeight - 165));
    rowEditVisible->addChild(visibleMenu);

    // Optional language key used as the unit's display name at runtime.
    rowEditAlias = Node::create();
    editUnitPanel->addChild(rowEditAlias);

    Label* aliasLabel = LM->getLocalizedLabel("Alias:", Color4B(panelTextColor.r, panelTextColor.g, panelTextColor.b, 255), 22);
    aliasLabel->setAnchorPoint(Vec2(0, 0.5f));
    aliasLabel->setPosition(Vec2(40, panelHeight - 220));
    rowEditAlias->addChild(aliasLabel);

    DrawNode* aliasBg = DrawNode::create();
    aliasBg->drawSolidRect(Vec2(140, panelHeight - 242), Vec2(560, panelHeight - 198), Color4F(0, 0, 0, 0.12f));
    rowEditAlias->addChild(aliasBg);

    tfEditAlias = TextField::create("none", "Arial", 22);
    tfEditAlias->setAnchorPoint(Vec2(0, 0.5f));
    tfEditAlias->setPosition(Vec2(150, panelHeight - 220));
    tfEditAlias->setTouchAreaEnabled(true);
    tfEditAlias->setTouchSize(Size(420, 44));
    tfEditAlias->setMaxLength(40);
    tfEditAlias->setMaxLengthEnabled(true);
    tfEditAlias->setTextColor(Color4B(60, 40, 25, 255));
    rowEditAlias->addChild(tfEditAlias);
    watchTextFieldFocus(tfEditAlias);

    const float confirmBtnW = 150;
    const float confirmBtnH = 56;
    MenuItemSprite* itemApply = createTextButton("Apply", confirmBtnW, confirmBtnH, [this](Ref*) { onConfirmEditUnit(); });
    MenuItemSprite* itemCancel = createTextButton("Cancel", confirmBtnW, confirmBtnH, [this](Ref*) { closeEditUnitPanel(); });
    itemApply->setPosition(Vec2(panelWidth / 2 - 90, 40));
    itemCancel->setPosition(Vec2(panelWidth / 2 + 90, 40));
    Menu* confirmMenu = Menu::create(itemApply, itemCancel, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    editUnitPanel->addChild(confirmMenu);
}

void MapEditor::openEditUnitPanel(int objectIndex) {
    if (objectIndex < 0 || objectIndex >= static_cast<int>(placedObjects.size())) {
        return;
    }
    editingObjectIndex = objectIndex;
    editingObjectIds.assign(1, placedObjects[objectIndex].id);
    const PlacedObject& obj = placedObjects[objectIndex];

    editUnitTitle->setString(StringUtils::format("Edit Unit: %s (ID %d)", kObjectTypes[obj.typeIndex].name, obj.id));
    selectEditSide(obj.side);
    if (obj.hp > 0) {
        tfEditHp->setString(StringUtils::format("%d", obj.hp));
    } else {
        int baseHp = defaultHpForType(obj.typeIndex);
        tfEditHp->setString(baseHp > 0 ? StringUtils::format("%d", baseHp) : "default");
    }

    bool isHero = kObjectTypes[obj.typeIndex].isHero;
    bool isFlag = strcmp(kObjectTypes[obj.typeIndex].name, "Flag") == 0;
    rowEditHp->setVisible(!isHero && !isFlag);
    rowEditLevel->setVisible(isHero);
    rowEditVisible->setVisible(isFlag);
    selectEditVisible(obj.visible ? 0 : 1);
    tfEditLevel->setString(StringUtils::format("%d", obj.level));
    rowEditAlias->setVisible(true);
    tfEditAlias->setString(obj.alias);

    editUnitPanel->setVisible(true);
    isEditUnitPanelOpen = true;
    setModalDimmerVisible(true);
}

void MapEditor::closeEditUnitPanel() {
    editUnitPanel->setVisible(false);
    isEditUnitPanelOpen = false;
    editingObjectIndex = -1;
    editingObjectIds.clear();
    setModalDimmerVisible(false);
}

void MapEditor::openEditUnitPanelForSelection() {
    // Prune the selection to objects that still exist before deciding
    // between the classic single editor and the batch editor.
    std::vector<int> ids;
    for (int id : selectedObjectIds) {
        if (objectIndexById(id) >= 0) {
            ids.push_back(id);
        }
    }
    if (ids.empty()) {
        return;
    }
    if (ids.size() == 1) {
        openEditUnitPanel(objectIndexById(ids[0]));
        return;
    }

    editingObjectIds = ids;
    editingObjectIndex = objectIndexById(ids[0]);

    bool anyHero = false;
    bool anyNonHero = false;
    for (int id : ids) {
        const PlacedObject& obj = placedObjects[objectIndexById(id)];
        if (kObjectTypes[obj.typeIndex].isHero) {
            anyHero = true;
        } else {
            anyNonHero = true;
        }
    }

    editUnitTitle->setString(StringUtils::format("Edit %d Units", static_cast<int>(ids.size())));
    selectEditSide(placedObjects[editingObjectIndex].side);
    // Batch sentinel: a field left as "keep" (or empty / non-numeric) means
    // "don't touch that property on any unit"; a typed value applies to all.
    tfEditHp->setString("keep");
    tfEditLevel->setString("");
    tfEditLevel->setPlaceHolder("keep");
    rowEditHp->setVisible(anyNonHero);
    rowEditLevel->setVisible(anyHero);
    // Flag visibility is a single-object edit only - no batch "keep" semantics.
    rowEditVisible->setVisible(false);
    // Alias is a per-unit identifier - batch-applying one would assign the
    // same language key to every selected unit, which is never wanted.
    rowEditAlias->setVisible(false);

    editUnitPanel->setVisible(true);
    isEditUnitPanelOpen = true;
    setModalDimmerVisible(true);
}

void MapEditor::selectEditSide(int side) {
    editPanelSide = side;
    highlightGroup(editSideButtonBgs, editPanelSide);
}

void MapEditor::selectEditVisible(int visibleIdx) {
    editPanelVisible = visibleIdx;
    highlightGroup(editVisibleButtonBgs, editPanelVisible);
}

void MapEditor::onConfirmEditUnit() {
    // Resolve the edited ids back to live objects (units can vanish between
    // opening the panel and Apply only via code paths that close it first,
    // but stay defensive - ids are the source of truth, not indices).
    std::vector<int> indices;
    for (int id : editingObjectIds) {
        int idx = objectIndexById(id);
        if (idx >= 0) {
            indices.push_back(idx);
        }
    }
    if (indices.empty()) {
        closeEditUnitPanel();
        return;
    }
    bool batch = indices.size() > 1;

    std::string hpText = tfEditHp->getString();
    // Batch mode: "keep"/empty leaves each unit's HP alone. Single mode keeps
    // the original semantics where an emptied field resets to the default.
    bool applyHp = !batch || (!hpText.empty() && hpText != "keep");
    int hpParsed = (hpText.empty() || hpText == "default" || hpText == "keep") ? -1 : atoi(hpText.c_str());
    if (hpParsed <= 0) {
        hpParsed = -1;
    }

    int levelParsed = atoi(tfEditLevel->getString().c_str());
    bool applyLevel = !batch || levelParsed >= 1; // "keep"/empty parses to 0

    std::string aliasText = tfEditAlias->getString();
    // Stray whitespace in a language key would silently break the lookup.
    aliasText.erase(0, aliasText.find_first_not_of(" \t"));
    aliasText.erase(aliasText.find_last_not_of(" \t") + 1);

    for (int idx : indices) {
        PlacedObject& obj = placedObjects[idx];
        obj.side = editPanelSide;
        if (!batch) {
            obj.alias = aliasText;
        }
        if (!batch && strcmp(kObjectTypes[obj.typeIndex].name, "Flag") == 0) {
            obj.visible = (editPanelVisible == 0);
            obj.sprite->setOpacity(obj.visible ? 255 : 110);
        }
        if (kObjectTypes[obj.typeIndex].isHero) {
            if (applyLevel) {
                obj.level = std::max(1, levelParsed);
            }
        } else if (applyHp) {
            int hp = hpParsed;
            // Untouched, the field just displays this unit's base HP (not an
            // explicit override) — keep storing the -1 sentinel so future
            // balance changes to the unit's base HP still apply to it.
            if (hp == defaultHpForType(obj.typeIndex)) {
                hp = -1;
            }
            obj.hp = hp;
        }
        obj.sprite->setColor(colorForSide(obj.side));
    }
    isDirty = true;
    if (batch) {
        setStatus(StringUtils::format("Updated %d units", static_cast<int>(indices.size())));
    } else {
        const PlacedObject& obj = placedObjects[indices[0]];
        setStatus(StringUtils::format("Updated %s at (%d,%d)", kObjectTypes[obj.typeIndex].name, obj.tileX, obj.tileY));
    }
    closeEditUnitPanel();
}

std::string MapEditor::serialize() const {
    std::string rawBytes;
    rawBytes.reserve(terrainGrid.size());
    for (int v : terrainGrid) {
        rawBytes.push_back(static_cast<char>(v));
    }
    std::string compressed = GameManager::getInstance()->compress_string(rawBytes);

    char* encoded = nullptr;
    int encodedLen = base64Encode(reinterpret_cast<const unsigned char*>(compressed.data()),
                                   static_cast<unsigned int>(compressed.size()), &encoded);
    std::string base64Data(encoded, encodedLen);
    free(encoded);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.Key("version");
    writer.Int(SAVE_FORMAT_VERSION);
    writer.Key("width");
    writer.Int(mapWidth);
    writer.Key("height");
    writer.Int(mapHeight);
    writer.Key("tileData");
    writer.String(base64Data.c_str(), static_cast<rapidjson::SizeType>(base64Data.size()));
    writer.Key("objects");
    writer.StartArray();
    for (const PlacedObject& obj : placedObjects) {
        writer.StartObject();
        writer.Key("type");
        writer.Int(obj.typeIndex);
        writer.Key("x");
        writer.Int(obj.tileX);
        writer.Key("y");
        writer.Int(obj.tileY);
        writer.Key("side");
        writer.Int(obj.side);
        writer.Key("hp");
        writer.Int(obj.hp);
        writer.Key("id");
        writer.Int(obj.id);
        writer.Key("level");
        writer.Int(obj.level);
        // Written only when hidden - absent means visible, so files saved
        // before this key existed load unchanged.
        if (!obj.visible) {
            writer.Key("visible");
            writer.Bool(false);
        }
        // Written only when set - most units have no alias, and skipping the
        // empty ones keeps the file from growing.
        if (!obj.alias.empty()) {
            writer.Key("alias");
            writer.String(obj.alias.c_str(), static_cast<rapidjson::SizeType>(obj.alias.size()));
        }
        writer.EndObject();
    }
    writer.EndArray();

    writer.Key("triggers");
    writer.StartArray();
    for (const Trigger& t : triggers) {
        writer.StartObject();
        writer.Key("name");
        writer.String(t.name.c_str(), static_cast<rapidjson::SizeType>(t.name.size()));
        writer.Key("preserve");
        writer.Bool(t.preserve);
        writer.Key("sides");
        writer.StartArray();
        for (int s = 0; s < kTriggerSideCount; s++) {
            writer.Bool(t.sides[s]);
        }
        writer.EndArray();
        writer.Key("conditions");
        writer.StartArray();
        for (const TriggerCondition& c : t.conditions) {
            writer.StartObject();
            writer.Key("type");
            writer.Int(c.type);
            writer.Key("elapsedSeconds");
            writer.Int(c.elapsedSeconds);
            writer.Key("switchIndex");
            writer.Int(c.switchIndex);
            writer.Key("switchState");
            writer.Int(c.switchState);
            writer.Key("unitSide");
            writer.Int(c.unitSide);
            writer.Key("unitTypeIndex");
            writer.Int(c.unitTypeIndex);
            writer.Key("comparison");
            writer.Int(c.comparison);
            writer.Key("amount");
            writer.Int(c.amount);
            writer.Key("resourceKind");
            writer.Int(c.resourceKind);
            writer.Key("targetObjectId");
            writer.Int(c.targetObjectId);
            // Written only when set - absent means "Any", so files saved
            // before this key existed load unchanged.
            if (c.sourceObjectId >= 0) {
                writer.Key("sourceObjectId");
                writer.Int(c.sourceObjectId);
            }
            writer.Key("isRepeat");
            writer.Bool(c.isRepeat);
            writer.EndObject();
        }
        writer.EndArray();
        writer.Key("actions");
        writer.StartArray();
        for (const TriggerAction& a : t.actions) {
            writer.StartObject();
            writer.Key("type");
            writer.Int(a.type);
            writer.Key("message");
            writer.String(a.message.c_str(), static_cast<rapidjson::SizeType>(a.message.size()));
            writer.Key("unitSide");
            writer.Int(a.unitSide);
            writer.Key("unitTypeIndex");
            writer.Int(a.unitTypeIndex);
            writer.Key("tileX");
            writer.Int(a.tileX);
            writer.Key("tileY");
            writer.Int(a.tileY);
            writer.Key("targetObjectId");
            writer.Int(a.targetObjectId);
            writer.Key("sourceObjectId");
            writer.Int(a.sourceObjectId);
            writer.Key("count");
            writer.Int(a.count);
            writer.Key("switchIndex");
            writer.Int(a.switchIndex);
            writer.Key("switchAction");
            writer.Int(a.switchAction);
            writer.Key("waitSeconds");
            writer.Double(a.waitSeconds);
            writer.Key("talkSeconds");
            writer.Double(a.talkSeconds);
            writer.Key("visionEnabled");
            writer.Bool(a.visionEnabled);
            writer.Key("controlLocked");
            writer.Bool(a.controlLocked);
            // Written only when set - absent means the default "start after
            // the previous action finishes", so files saved before this key
            // existed load unchanged.
            if (a.runWithPrevious) {
                writer.Key("runWithPrevious");
                writer.Bool(true);
            }
            writer.EndObject();
        }
        writer.EndArray();
        writer.EndObject();
    }
    writer.EndArray();

    // Written only when something is banned - absent means "everything
    // buildable", so files saved before these keys existed load unchanged.
    if (!disabledBuildings.empty()) {
        writer.Key("disabledBuildings");
        writer.StartArray();
        for (int typeIndex : disabledBuildings) {
            writer.Int(typeIndex);
        }
        writer.EndArray();
    }
    if (!disabledUnits.empty()) {
        writer.Key("disabledUnits");
        writer.StartArray();
        for (int typeIndex : disabledUnits) {
            writer.Int(typeIndex);
        }
        writer.EndArray();
    }

    writer.EndObject();

    return buffer.GetString();
}

bool MapEditor::deserialize(const std::string& jsonStr) {
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());
    if (doc.HasParseError() || !doc.IsObject()) {
        return false;
    }
    if (!doc.HasMember("width") || !doc.HasMember("height") || !doc.HasMember("tileData")) {
        return false;
    }

    int width = doc["width"].GetInt();
    int height = doc["height"].GetInt();
    std::string base64Data = doc["tileData"].GetString();

    unsigned char* decoded = nullptr;
    int decodedLen = base64Decode(reinterpret_cast<const unsigned char*>(base64Data.c_str()),
                                   static_cast<unsigned int>(base64Data.size()), &decoded);
    if (decodedLen <= 0) {
        free(decoded);
        return false;
    }
    std::string compressed(reinterpret_cast<char*>(decoded), decodedLen);
    free(decoded);

    std::string rawBytes = GameManager::getInstance()->decompress_string(compressed);
    if (static_cast<int>(rawBytes.size()) != width * height) {
        return false;
    }

    buildMap(width, height);
    for (int i = 0; i < mapWidth * mapHeight; i++) {
        terrainGrid[i] = static_cast<unsigned char>(rawBytes[i]);
    }
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            refreshTile(x, y);
        }
    }

    if (doc.HasMember("objects") && doc["objects"].IsArray()) {
        for (auto& item : doc["objects"].GetArray()) {
            if (!item.HasMember("type") || !item.HasMember("x") || !item.HasMember("y")) {
                continue;
            }
            int typeIndex = item["type"].GetInt();
            int x = item["x"].GetInt();
            int y = item["y"].GetInt();
            if (typeIndex < 0 || typeIndex >= kObjectTypeCount) {
                continue;
            }
            if (x < 0 || y < 0 || x >= mapWidth || y >= mapHeight) {
                continue;
            }
            int side = item.HasMember("side") ? item["side"].GetInt() : SIDE_ALLY;
            int hp = item.HasMember("hp") ? item["hp"].GetInt() : -1;
            int id = item.HasMember("id") ? item["id"].GetInt() : -1;
            int level = item.HasMember("level") ? item["level"].GetInt() : 1;
            bool visible = !(item.HasMember("visible") && item["visible"].IsBool()) || item["visible"].GetBool();
            std::string alias = (item.HasMember("alias") && item["alias"].IsString()) ? item["alias"].GetString() : "";
            spawnObjectAt(x, y, typeIndex, side, hp, id, level, visible, alias);
        }
    }

    if (doc.HasMember("triggers") && doc["triggers"].IsArray()) {
        for (auto& titem : doc["triggers"].GetArray()) {
            Trigger t;
            if (titem.HasMember("name")) {
                t.name = titem["name"].GetString();
            }
            if (titem.HasMember("preserve")) {
                t.preserve = titem["preserve"].GetBool();
            }
            if (titem.HasMember("sides") && titem["sides"].IsArray()) {
                int s = 0;
                for (auto& sv : titem["sides"].GetArray()) {
                    if (s >= kTriggerSideCount) {
                        break;
                    }
                    t.sides[s] = sv.GetBool();
                    s++;
                }
            }
            if (titem.HasMember("conditions") && titem["conditions"].IsArray()) {
                for (auto& citem : titem["conditions"].GetArray()) {
                    TriggerCondition c;
                    if (citem.HasMember("type")) {
                        c.type = static_cast<TriggerConditionType>(citem["type"].GetInt());
                    }
                    if (citem.HasMember("elapsedSeconds")) {
                        c.elapsedSeconds = citem["elapsedSeconds"].GetInt();
                    }
                    if (citem.HasMember("switchIndex")) {
                        c.switchIndex = citem["switchIndex"].GetInt();
                    }
                    if (citem.HasMember("switchState")) {
                        c.switchState = static_cast<TriggerSwitchState>(citem["switchState"].GetInt());
                    }
                    if (citem.HasMember("unitSide")) {
                        c.unitSide = citem["unitSide"].GetInt();
                    }
                    if (citem.HasMember("unitTypeIndex")) {
                        c.unitTypeIndex = citem["unitTypeIndex"].GetInt();
                    }
                    if (citem.HasMember("comparison")) {
                        c.comparison = static_cast<TriggerComparison>(citem["comparison"].GetInt());
                    }
                    if (citem.HasMember("amount")) {
                        c.amount = citem["amount"].GetInt();
                    }
                    if (citem.HasMember("resourceKind")) {
                        c.resourceKind = static_cast<TriggerResourceKind>(citem["resourceKind"].GetInt());
                    }
                    if (citem.HasMember("targetObjectId")) {
                        c.targetObjectId = citem["targetObjectId"].GetInt();
                    }
                    if (citem.HasMember("sourceObjectId")) {
                        c.sourceObjectId = citem["sourceObjectId"].GetInt();
                    }
                    if (citem.HasMember("isRepeat")) {
                        c.isRepeat = citem["isRepeat"].GetBool();
                    }
                    t.conditions.push_back(c);
                }
            }
            if (titem.HasMember("actions") && titem["actions"].IsArray()) {
                for (auto& aitem : titem["actions"].GetArray()) {
                    TriggerAction a;
                    if (aitem.HasMember("type")) {
                        a.type = static_cast<TriggerActionType>(aitem["type"].GetInt());
                    }
                    if (aitem.HasMember("message")) {
                        a.message = aitem["message"].GetString();
                    }
                    if (aitem.HasMember("unitSide")) {
                        a.unitSide = aitem["unitSide"].GetInt();
                    }
                    if (aitem.HasMember("unitTypeIndex")) {
                        a.unitTypeIndex = aitem["unitTypeIndex"].GetInt();
                    }
                    if (aitem.HasMember("tileX")) {
                        a.tileX = aitem["tileX"].GetInt();
                    }
                    if (aitem.HasMember("tileY")) {
                        a.tileY = aitem["tileY"].GetInt();
                    }
                    if (aitem.HasMember("targetObjectId")) {
                        a.targetObjectId = aitem["targetObjectId"].GetInt();
                    }
                    if (aitem.HasMember("sourceObjectId")) {
                        a.sourceObjectId = aitem["sourceObjectId"].GetInt();
                    }
                    if (aitem.HasMember("count")) {
                        a.count = aitem["count"].GetInt();
                    }
                    if (aitem.HasMember("switchIndex")) {
                        a.switchIndex = aitem["switchIndex"].GetInt();
                    }
                    if (aitem.HasMember("switchAction")) {
                        a.switchAction = static_cast<TriggerSwitchAction>(aitem["switchAction"].GetInt());
                    }
                    if (aitem.HasMember("waitSeconds")) {
                        a.waitSeconds = static_cast<float>(aitem["waitSeconds"].GetDouble());
                    }
                    if (aitem.HasMember("talkSeconds")) {
                        a.talkSeconds = static_cast<float>(aitem["talkSeconds"].GetDouble());
                    }
                    if (aitem.HasMember("visionEnabled")) {
                        a.visionEnabled = aitem["visionEnabled"].GetBool();
                    }
                    if (aitem.HasMember("controlLocked")) {
                        a.controlLocked = aitem["controlLocked"].GetBool();
                    }
                    if (aitem.HasMember("runWithPrevious")) {
                        a.runWithPrevious = aitem["runWithPrevious"].GetBool();
                    }
                    t.actions.push_back(a);
                }
            }
            triggers.push_back(t);
        }
    }

    // buildMap() above already reset disabledBuildings/disabledUnits for maps
    // saved before these keys existed.
    if (doc.HasMember("disabledBuildings") && doc["disabledBuildings"].IsArray()) {
        for (auto& ditem : doc["disabledBuildings"].GetArray()) {
            if (!ditem.IsInt()) {
                continue;
            }
            int typeIndex = ditem.GetInt();
            if (isConstructibleBuildingType(typeIndex)) {
                disabledBuildings.insert(typeIndex);
            }
        }
    }
    if (doc.HasMember("disabledUnits") && doc["disabledUnits"].IsArray()) {
        for (auto& ditem : doc["disabledUnits"].GetArray()) {
            if (!ditem.IsInt()) {
                continue;
            }
            int typeIndex = ditem.GetInt();
            if (isTrainableUnitType(typeIndex)) {
                disabledUnits.insert(typeIndex);
            }
        }
    }
    refreshMapSettingsButtons();

    if (triggerPanel) {
        refreshTriggerList();
        refreshTriggerDetail();
    }

    return true;
}

void MapEditor::saveMap() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    // A never-saved map has no path yet, so "Save" behaves like "Save As" the
    // first time — same convention as a typical document editor.
    if (currentFilePath.empty()) {
        saveMapAs();
        return;
    }
    std::string json = serialize();
    bool ok = FileUtils::getInstance()->writeStringToFile(json, currentFilePath);
    if (ok) isDirty = false;
    setStatus(ok ? StringUtils::format("Saved %s", currentFilePath.c_str()) : "Save failed");
#else
    std::string json = serialize();
    std::string path = FileUtils::getInstance()->getWritablePath() + SAVE_FILE_NAME;
    bool ok = FileUtils::getInstance()->writeStringToFile(json, path);
    if (ok) isDirty = false;
    setStatus(ok ? StringUtils::format("Saved (%d bytes)", static_cast<int>(json.size())) : "Save failed");
#endif
}

void MapEditor::saveMapAs() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    std::string path = win32ShowSaveDialog("map.json");
    if (path.empty()) {
        setStatus("Save cancelled");
        return;
    }
    std::string json = serialize();
    bool ok = FileUtils::getInstance()->writeStringToFile(json, path);
    if (ok) {
        currentFilePath = path;
        isDirty = false;
    }
    setStatus(ok ? StringUtils::format("Saved %s", path.c_str()) : "Save failed");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    std::string path = macShowSaveDialog("map.json");
    if (path.empty()) {
        setStatus("Save cancelled");
        return;
    }
    std::string json = serialize();
    bool ok = FileUtils::getInstance()->writeStringToFile(json, path);
    if (ok) {
        currentFilePath = path;
        isDirty = false;
    }
    setStatus(ok ? StringUtils::format("Saved %s", path.c_str()) : "Save failed");
#else
    saveMap();
#endif
}

void MapEditor::loadMap() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    std::string path = win32ShowOpenDialog();
    if (path.empty()) {
        setStatus("Load cancelled");
        return;
    }
    std::string json = FileUtils::getInstance()->getStringFromFile(path);
    bool ok = deserialize(json);
    if (ok) {
        currentFilePath = path;
        isDirty = false;
    }
    setStatus(ok ? StringUtils::format("Loaded %s", path.c_str()) : "Load failed (corrupt file)");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    std::string path = macShowOpenDialog();
    if (path.empty()) {
        setStatus("Load cancelled");
        return;
    }
    std::string json = FileUtils::getInstance()->getStringFromFile(path);
    bool ok = deserialize(json);
    if (ok) {
        currentFilePath = path;
        isDirty = false;
    }
    setStatus(ok ? StringUtils::format("Loaded %s", path.c_str()) : "Load failed (corrupt file)");
#else
    std::string path = FileUtils::getInstance()->getWritablePath() + SAVE_FILE_NAME;
    if (!FileUtils::getInstance()->isFileExist(path)) {
        setStatus("No save file found");
        return;
    }
    std::string json = FileUtils::getInstance()->getStringFromFile(path);
    bool ok = deserialize(json);
    if (ok) isDirty = false;
    setStatus(ok ? StringUtils::format("Loaded %dx%d", mapWidth, mapHeight) : "Load failed (corrupt file)");
#endif
}

void MapEditor::watchTextFieldFocus(TextField* tf) {
    tf->addEventListener([this](Ref*, TextField::EventType type) {
        if (type == TextField::EventType::ATTACH_WITH_IME) {
            isTextFieldFocused = true;
        } else if (type == TextField::EventType::DETACH_WITH_IME) {
            isTextFieldFocused = false;
        }
    });
}

// Title label + "<"/">" buttons + a value label the caller fills in. Returns the
// row Node (for show/hide per condition/action type) with outValueLabel set to
// the label to update with the current cycled value's text.
Node* MapEditor::addCycleRow(Node* parent, const std::string& title, float x, float y,
                              Label** outValueLabel, const std::function<void(int)>& onCycle) {
    Node* row = Node::create();
    row->setPosition(Vec2(x, y));
    parent->addChild(row);

    Label* titleLbl = LM->getLocalizedLabel(title.c_str(), Color4B::WHITE, 22);
    titleLbl->setAnchorPoint(Vec2(0, 0.5f));
    titleLbl->setPosition(Vec2(0, 0));
    row->addChild(titleLbl);

    const float arrowSize = 34;
    // Taller click target than the 34pt glyph box; rows are spaced 50pt apart
    // so 44pt still clears neighboring rows. Width must stay = arrowSize: the
    // horizontal layout below (and the target-overlay math in
    // setupActionEditPanel) is derived from it.
    const float arrowTouchH = 44;
    const float valueWidth = 260;
    const float leftX = 180;
    const float valueX = leftX + arrowSize / 2 + 10 + valueWidth / 2;
    const float rightX = valueX + valueWidth / 2 + 10 + arrowSize / 2;

    MenuItemSprite* leftBtn = createTextButton("<", arrowSize, arrowTouchH, [onCycle](Ref*) { onCycle(-1); });
    leftBtn->setPosition(Vec2(leftX, 0));
    MenuItemSprite* rightBtn = createTextButton(">", arrowSize, arrowTouchH, [onCycle](Ref*) { onCycle(1); });
    rightBtn->setPosition(Vec2(rightX, 0));
    Menu* menu = Menu::create(leftBtn, rightBtn, nullptr);
    menu->setPosition(Vec2::ZERO);
    row->addChild(menu);

    Label* valueLbl = LM->getLocalizedLabel("", Color4B::WHITE, 22);
    valueLbl->setPosition(Vec2(valueX, 0));
    row->addChild(valueLbl);

    if (outValueLabel) {
        *outValueLabel = valueLbl;
    }
    return row;
}

// Title label + a boxed TextField. Returns the row Node, with outField set to
// the TextField to read/write the value through.
Node* MapEditor::addTextFieldRow(Node* parent, const std::string& title, float x, float y,
                                  float fieldWidth, const std::string& initialValue, TextField** outField) {
    Node* row = Node::create();
    row->setPosition(Vec2(x, y));
    parent->addChild(row);

    Label* titleLbl = LM->getLocalizedLabel(title.c_str(), Color4B::WHITE, 22);
    titleLbl->setAnchorPoint(Vec2(0, 0.5f));
    titleLbl->setPosition(Vec2(0, 0));
    row->addChild(titleLbl);

    const float fieldX = 180;
    const float fieldHeight = 40;

    DrawNode* bg = DrawNode::create();
    bg->drawSolidRect(Vec2(fieldX, -fieldHeight / 2), Vec2(fieldX + fieldWidth, fieldHeight / 2), Color4F(1, 1, 1, 0.18f));
    row->addChild(bg);

    TextField* tf = TextField::create(initialValue, "Arial", 22);
    tf->setAnchorPoint(Vec2(0, 0.5f));
    tf->setPosition(Vec2(fieldX + 10, 0));
    tf->setTouchAreaEnabled(true);
    tf->setTouchSize(Size(fieldWidth, fieldHeight));
    tf->setMaxLength(40);
    tf->setMaxLengthEnabled(true);
    row->addChild(tf);
    watchTextFieldFocus(tf);

    if (outField) {
        *outField = tf;
    }
    return row;
}

// Attaches a transparent-but-clickable overlay over a cycle-row's value label
// so that clicking the text opens a scrollable dropdown (e.g. "At:"/"To:" or
// "Unit Type:" rows) instead of only stepping one value at a time via the
// arrows. The normal skin is opacity-0 (invisible, showing the label
// beneath), the selected skin flashes as press feedback.
void MapEditor::addDropdownOverlay(Node* row, const std::function<void()>& onOpen) {
    const float arrowSz = 34.0f, valW = 260.0f, leftX = 180.0f;
    const float valX = leftX + arrowSz / 2 + 10 + valW / 2;
    Scale9Sprite* n = makeButtonSkin(BTN_SKIN_NORMAL, Size(valW, arrowSz + 4));
    n->setOpacity(0);
    Scale9Sprite* s = makeButtonSkin(BTN_SKIN_SELECTED, Size(valW, arrowSz + 4));
    MenuItemSprite* btn = MenuItemSprite::create(n, s, [this, onOpen](Ref*) {
        this->scheduleOnce([onOpen](float){ onOpen(); }, 0.0f, "dd_open");
    });
    btn->setPosition(Vec2(valX, 0));
    Menu* m = Menu::create(btn, nullptr);
    m->setPosition(Vec2::ZERO);
    row->addChild(m);
}

void MapEditor::setMultiHighlight(std::vector<Scale9Sprite*>& group, const bool* states, int count) {
    for (int i = 0; i < count && i < static_cast<int>(group.size()); i++) {
        if (!group[i]) {
            continue;
        }
        group[i]->setColor(states[i] ? Color3B(255, 221, 120) : Color3B::WHITE);
    }
}

void MapEditor::setupTriggerPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const float panelWidth = visibleSize.width - 80;
    const float panelHeight = visibleSize.height - 80;

    triggerPanel = Node::create();
    triggerPanel->setPosition(Vec2(40, 40));
    triggerPanel->setVisible(false);
    this->addChild(triggerPanel, 20);

    DrawNode* panelBg = DrawNode::create();
    panelBg->drawSolidRect(Vec2(0, 0), Vec2(panelWidth, panelHeight), Color4F(0.08f, 0.08f, 0.08f, 0.95f));
    triggerPanel->addChild(panelBg);

    Label* title = LM->getLocalizedLabel("Trigger Editor", Color4B::WHITE, 28);
    title->setPosition(Vec2(panelWidth / 2, panelHeight - 30));
    triggerPanel->addChild(title);

    MenuItemSprite* itemClose = createTextButton("Close", 110, 50, [this](Ref*) { closeTriggerPanel(); });
    itemClose->setPosition(Vec2(panelWidth - 70, panelHeight - 30));
    Menu* closeMenu = Menu::create(itemClose, nullptr);
    closeMenu->setPosition(Vec2::ZERO);
    triggerPanel->addChild(closeMenu);

    // Left column: trigger list
    const float listX = 20;
    const float listWidth = 300;

    Label* listHeader = LM->getLocalizedLabel("Triggers", Color4B::WHITE, 24);
    listHeader->setAnchorPoint(Vec2(0, 0.5f));
    listHeader->setPosition(Vec2(listX, panelHeight - 80));
    triggerPanel->addChild(listHeader);

    MenuItemSprite* itemAddTrigger = createTextButton("+ Add", 80, 44, [this](Ref*) { addTrigger(); });
    itemAddTrigger->setPosition(Vec2(listX + listWidth - 175, panelHeight - 80));
    MenuItemSprite* itemCopyTrigger = createTextButton("Copy", 80, 44, [this](Ref*) { duplicateSelectedTrigger(); });
    itemCopyTrigger->setPosition(Vec2(listX + listWidth - 90, panelHeight - 80));
    MenuItemSprite* itemDeleteTrigger = createTextButton("Delete", 80, 44, [this](Ref*) { deleteSelectedTrigger(); });
    // 20pt clear of "Copy" so the destructive button isn't flush against it.
    itemDeleteTrigger->setPosition(Vec2(listX + listWidth + 10, panelHeight - 80));
    Menu* listBtnMenu = Menu::create(itemAddTrigger, itemCopyTrigger, itemDeleteTrigger, nullptr);
    listBtnMenu->setPosition(Vec2::ZERO);
    triggerPanel->addChild(listBtnMenu);

    triggerListContainer = Node::create();
    triggerListContainer->setPosition(Vec2(listX, panelHeight - 120));
    triggerPanel->addChild(triggerListContainer);

    // Right column: selected trigger detail
    const float detailX = listX + listWidth + 30;

    Label* nameLbl = LM->getLocalizedLabel("Name:", Color4B::WHITE, 22);
    nameLbl->setAnchorPoint(Vec2(0, 0.5f));
    nameLbl->setPosition(Vec2(detailX, panelHeight - 80));
    triggerPanel->addChild(nameLbl);

    DrawNode* nameBg = DrawNode::create();
    nameBg->drawSolidRect(Vec2(detailX + 100, panelHeight - 80 - 20), Vec2(detailX + 420, panelHeight - 80 + 20),
                           Color4F(1, 1, 1, 0.18f));
    triggerPanel->addChild(nameBg);

    tfTriggerName = TextField::create("Trigger", "Arial", 22);
    tfTriggerName->setAnchorPoint(Vec2(0, 0.5f));
    tfTriggerName->setPosition(Vec2(detailX + 110, panelHeight - 80));
    tfTriggerName->setTouchAreaEnabled(true);
    tfTriggerName->setTouchSize(Size(310, 40));
    tfTriggerName->setMaxLength(40);
    tfTriggerName->setMaxLengthEnabled(true);
    triggerPanel->addChild(tfTriggerName);
    watchTextFieldFocus(tfTriggerName);

    // Side checkboxes + preserve toggle (StarCraft "Players" + "Preserve Trigger")
    Label* sidesLbl = LM->getLocalizedLabel("Applies to:", Color4B::WHITE, 22);
    sidesLbl->setAnchorPoint(Vec2(0, 0.5f));
    sidesLbl->setPosition(Vec2(detailX, panelHeight - 140));
    triggerPanel->addChild(sidesLbl);

    const float sideBtnW = 110;
    const float sideBtnH = 40;
    Vector<MenuItem*> sideItems;
    triggerSideButtonBgs.assign(kTriggerSideCount, nullptr);
    for (int s = 0; s < kTriggerSideCount; s++) {
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(kSideNames[s], sideBtnW, sideBtnH, [this, s](Ref*) { toggleTriggerSide(s); }, &bg);
        item->setPosition(Vec2(detailX + 150 + s * (sideBtnW + 8), panelHeight - 140));
        sideItems.pushBack(item);
        triggerSideButtonBgs[s] = bg;
    }
    Scale9Sprite* preserveBg = nullptr;
    MenuItemSprite* itemPreserve = createTextButton("Preserve", 120, sideBtnH,
                                                      [this](Ref*) { toggleTriggerPreserve(); }, &preserveBg);
    itemPreserve->setPosition(Vec2(detailX + 150 + kTriggerSideCount * (sideBtnW + 8) + 70, panelHeight - 140));
    sideItems.pushBack(itemPreserve);
    triggerPreserveBg = preserveBg;

    Menu* sideMenu = Menu::createWithArray(sideItems);
    sideMenu->setPosition(Vec2::ZERO);
    triggerPanel->addChild(sideMenu);

    // Conditions section
    Label* condHeader = LM->getLocalizedLabel("Conditions", Color4B::WHITE, 24);
    condHeader->setAnchorPoint(Vec2(0, 0.5f));
    condHeader->setPosition(Vec2(detailX, panelHeight - 200));
    triggerPanel->addChild(condHeader);

    MenuItemSprite* itemAddCond = createTextButton("+ Add Condition", 220, 44, [this](Ref*) { openConditionEditPanel(-1); });
    itemAddCond->setPosition(Vec2(detailX + 330, panelHeight - 200));
    Menu* addCondMenu = Menu::create(itemAddCond, nullptr);
    addCondMenu->setPosition(Vec2::ZERO);
    triggerPanel->addChild(addCondMenu);

    conditionListContainer = Node::create();
    conditionListContainer->setPosition(Vec2(detailX, panelHeight - 240));
    triggerPanel->addChild(conditionListContainer);

    // Actions section
    Label* actHeader = LM->getLocalizedLabel("Actions", Color4B::WHITE, 24);
    actHeader->setAnchorPoint(Vec2(0, 0.5f));
    actHeader->setPosition(Vec2(detailX, panelHeight - 520));
    triggerPanel->addChild(actHeader);

    MenuItemSprite* itemAddAct = createTextButton("+ Add Action", 220, 44, [this](Ref*) { openActionEditPanel(-1); });
    itemAddAct->setPosition(Vec2(detailX + 330, panelHeight - 520));
    Menu* addActMenu = Menu::create(itemAddAct, nullptr);
    addActMenu->setPosition(Vec2::ZERO);
    triggerPanel->addChild(addActMenu);

    actionListContainer = Node::create();
    actionListContainer->setPosition(Vec2(detailX, panelHeight - 560));
    triggerPanel->addChild(actionListContainer);
}

void MapEditor::openTriggerPanel() {
    triggerPanel->setVisible(true);
    isTriggerPanelOpen = true;
    setModalDimmerVisible(true);
    if (selectedTriggerIndex < 0 && !triggers.empty()) {
        selectTrigger(0);
    } else {
        refreshTriggerList();
        refreshTriggerDetail();
    }
}

void MapEditor::closeTriggerPanel() {
    if (selectedTriggerIndex >= 0 && selectedTriggerIndex < static_cast<int>(triggers.size()) && tfTriggerName) {
        triggers[selectedTriggerIndex].name = tfTriggerName->getString();
    }
    triggerPanel->setVisible(false);
    isTriggerPanelOpen = false;
    setModalDimmerVisible(false);
    refreshTriggerList();
}

void MapEditor::refreshTriggerList() {
    triggerListContainer->removeAllChildren();

    const float rowH = 44;
    const float btnW = 280;
    Vector<MenuItem*> items;
    for (size_t i = 0; i < triggers.size(); i++) {
        int idx = static_cast<int>(i);
        std::string label = triggers[i].name.empty() ? "(unnamed)" : triggers[i].name;
        Scale9Sprite* bg = nullptr;
        MenuItemSprite* item = createTextButton(label, btnW, rowH - 4, [this, idx](Ref*) { selectTrigger(idx); }, &bg);
        item->setPosition(Vec2(btnW / 2, -static_cast<float>(i) * rowH - rowH / 2));
        items.pushBack(item);
        if (bg && idx == selectedTriggerIndex) {
            bg->setColor(Color3B(255, 221, 120));
        }
    }
    if (!items.empty()) {
        Menu* menu = Menu::createWithArray(items);
        menu->setPosition(Vec2::ZERO);
        triggerListContainer->addChild(menu);
    }
}

void MapEditor::selectTrigger(int index) {
    if (selectedTriggerIndex >= 0 && selectedTriggerIndex < static_cast<int>(triggers.size()) && tfTriggerName) {
        triggers[selectedTriggerIndex].name = tfTriggerName->getString();
    }
    selectedTriggerIndex = (index >= 0 && index < static_cast<int>(triggers.size())) ? index : -1;
    conditionListScroll = 0;
    actionListScroll = 0;
    refreshTriggerList();
    refreshTriggerDetail();
}

void MapEditor::addTrigger() {
    Trigger t;
    t.name = StringUtils::format("Trigger %d", static_cast<int>(triggers.size()) + 1);
    triggers.push_back(t);
    isDirty = true;
    selectTrigger(static_cast<int>(triggers.size()) - 1);
}

void MapEditor::duplicateSelectedTrigger() {
    if (selectedTriggerIndex < 0 || selectedTriggerIndex >= static_cast<int>(triggers.size())) {
        return;
    }
    // Sync any pending name edit into the source trigger before copying it.
    if (tfTriggerName) {
        triggers[selectedTriggerIndex].name = tfTriggerName->getString();
    }
    Trigger copy = triggers[selectedTriggerIndex];
    copy.name += " Copy";
    triggers.insert(triggers.begin() + selectedTriggerIndex + 1, copy);
    isDirty = true;
    selectTrigger(selectedTriggerIndex + 1);
}

void MapEditor::deleteSelectedTrigger() {
    if (selectedTriggerIndex < 0 || selectedTriggerIndex >= static_cast<int>(triggers.size())) {
        return;
    }
    triggers.erase(triggers.begin() + selectedTriggerIndex);
    isDirty = true;
    selectedTriggerIndex = -1;
    if (!triggers.empty()) {
        selectTrigger(0);
    } else {
        refreshTriggerList();
        refreshTriggerDetail();
    }
}

void MapEditor::toggleTriggerSide(int side) {
    if (selectedTriggerIndex < 0) {
        return;
    }
    Trigger& t = triggers[selectedTriggerIndex];
    t.sides[side] = !t.sides[side];
    isDirty = true;
    setMultiHighlight(triggerSideButtonBgs, t.sides, kTriggerSideCount);
}

void MapEditor::toggleTriggerPreserve() {
    if (selectedTriggerIndex < 0) {
        return;
    }
    Trigger& t = triggers[selectedTriggerIndex];
    t.preserve = !t.preserve;
    isDirty = true;
    if (triggerPreserveBg) {
        triggerPreserveBg->setColor(t.preserve ? Color3B(255, 221, 120) : Color3B::WHITE);
    }
}

std::string MapEditor::describeCondition(const TriggerCondition& c) const {
    std::string base;
    switch (c.type) {
        case COND_ALWAYS:
            base = "Always";
            break;
        case COND_ELAPSED_TIME:
            base = StringUtils::format("Elapsed time >= %d sec", c.elapsedSeconds);
            break;
        case COND_SWITCH:
            base = StringUtils::format("Switch %d is %s", c.switchIndex + 1, kSwitchStateNames[c.switchState]);
            break;
        case COND_UNIT_COUNT:
            base = StringUtils::format("%s unit count for %s is %s %d", unitTypeCycleName(c.unitTypeIndex).c_str(),
                                        kSideNames[c.unitSide], kComparisonNames[c.comparison], c.amount);
            break;
        case COND_RESOURCE:
            base = StringUtils::format("%s is %s %d", kResourceKindNames[c.resourceKind], kComparisonNames[c.comparison], c.amount);
            break;
        case COND_UNIT_ARRIVES:
            // A specific placed unit supersedes the Side/Unit Type filter.
            if (c.sourceObjectId >= 0) {
                base = StringUtils::format("%s arrives at %s (range %d)",
                                            describeTargetObject(c.sourceObjectId).c_str(),
                                            c.targetObjectId >= 0 ? describeTargetObject(c.targetObjectId).c_str() : "(no target)",
                                            c.amount);
            } else {
                base = StringUtils::format("%s %s arrives at %s (range %d)", kSideNames[c.unitSide],
                                            unitTypeCycleName(c.unitTypeIndex).c_str(),
                                            c.targetObjectId >= 0 ? describeTargetObject(c.targetObjectId).c_str() : "(no target)",
                                            c.amount);
            }
            break;
        case COND_BUILDING_COUNT: {
            // Player-facing objective text: the "building count format" language
            // template with {0} = the building's (localized) name and {1} = the
            // required number. By design the comparison (at least / at most /
            // exactly) is not surfaced here - only the count is.
            std::string buildingName;
            if (c.unitTypeIndex >= 0) {
                // Editor roster names are English (e.g. "Farm"); the language
                // sheet keys them lower-case ("farm"). Fall back to the editor
                // name when the sheet has no entry (e.g. "Lumber Mill").
                std::string raw = unitTypeCycleName(c.unitTypeIndex);
                std::string key = raw;
                std::transform(key.begin(), key.end(), key.begin(),
                               [](unsigned char ch) { return (ch >= 'A' && ch <= 'Z') ? ch + 32 : ch; });
                buildingName = LM->getText(key);
                if (buildingName == key) {
                    buildingName = raw;
                }
            } else {
                // "Any" building type - use the generic "building" word.
                buildingName = LM->getText("building");
            }
            base = LM->getText("building count format");
            const std::string values[2] = {buildingName, StringUtils::format("%d", c.amount)};
            for (int i = 0; i < 2; i++) {
                std::string ph = StringUtils::format("{%d}", i);
                size_t slot = base.find(ph);
                if (slot != std::string::npos) {
                    base.replace(slot, ph.size(), values[i]);
                } else {
                    base += " " + values[i];
                }
            }
            break;
        }
        case COND_UNIT_DIES:
            base = StringUtils::format("%s dies", c.targetObjectId >= 0 ? describeTargetObject(c.targetObjectId).c_str() : "(no unit)");
            break;
        default:
            base = "Unknown condition";
            break;
    }
    if (c.isRepeat) {
        base += " [Repeat]";
    }
    return base;
}

std::string MapEditor::describeAction(const TriggerAction& a) const {
    std::string base;
    switch (a.type) {
        case TACT_DISPLAY_MESSAGE:
            base = StringUtils::format("Display message: \"%s\"", a.message.c_str());
            break;
        case TACT_CREATE_UNIT: {
            std::string at = a.targetObjectId >= 0 ? describeTargetObject(a.targetObjectId) : StringUtils::format("(%d,%d)", a.tileX, a.tileY);
            base = StringUtils::format("Create %d %s for %s at %s", a.count, unitTypeCycleName(a.unitTypeIndex).c_str(),
                                        kSideNames[a.unitSide], at.c_str());
            break;
        }
        case TACT_REMOVE_UNIT:
            base = StringUtils::format("Remove %s units for %s", unitTypeCycleName(a.unitTypeIndex).c_str(), kSideNames[a.unitSide]);
            break;
        case TACT_SET_SWITCH:
            base = StringUtils::format("%s switch %d", kSwitchActionNames[a.switchAction], a.switchIndex + 1);
            break;
        case TACT_VICTORY:
            base = "Victory";
            break;
        case TACT_DEFEAT:
            base = "Defeat";
            break;
        case TACT_WAIT:
            base = StringUtils::format("Wait %.1f sec", a.waitSeconds);
            break;
        case TACT_CENTER_CAMERA:
            base = a.targetObjectId >= 0 ? StringUtils::format("Center camera at %s", describeTargetObject(a.targetObjectId).c_str())
                                          : StringUtils::format("Center camera at (%d,%d)", a.tileX, a.tileY);
            break;
        case TACT_TALK: {
            std::string at = a.targetObjectId >= 0 ? describeTargetObject(a.targetObjectId) : StringUtils::format("(%d,%d)", a.tileX, a.tileY);
            base = StringUtils::format("Talk at %s (%.1fs): \"%s\"", at.c_str(), a.talkSeconds, a.message.c_str());
            break;
        }
        case TACT_REVEAL_FOG: {
            std::string at = a.targetObjectId >= 0 ? describeTargetObject(a.targetObjectId) : StringUtils::format("(%d,%d)", a.tileX, a.tileY);
            base = StringUtils::format("Reveal fog %s at %s r=%d", a.visionEnabled ? "ON" : "OFF", at.c_str(), a.count);
            break;
        }
        case TACT_ORDER_ATTACK:
            base = StringUtils::format("Order attack: %s %s -> enemy", kSideNames[a.unitSide], unitTypeCycleName(a.unitTypeIndex).c_str());
            break;
        case TACT_MOVE_UNIT:
        case TACT_TELEPORT_UNIT: {
            std::string at = a.targetObjectId >= 0 ? describeTargetObject(a.targetObjectId) : StringUtils::format("(%d,%d)", a.tileX, a.tileY);
            std::string unit = a.sourceObjectId >= 0 ? describeTargetObject(a.sourceObjectId) : "(no unit)";
            base = StringUtils::format("%s: %s -> %s", a.type == TACT_TELEPORT_UNIT ? "Teleport" : "Move", unit.c_str(), at.c_str());
            break;
        }
        case TACT_LOCK_CONTROL:
            base = a.controlLocked ? "Lock player control" : "Unlock player control";
            break;
        default:
            base = "Unknown action";
            break;
    }
    if (a.runWithPrevious) {
        base += " [With Prev]";
    }
    return base;
}

void MapEditor::refreshTriggerDetail() {
    bool hasSelection = selectedTriggerIndex >= 0 && selectedTriggerIndex < static_cast<int>(triggers.size());

    if (tfTriggerName) {
        tfTriggerName->setString(hasSelection ? triggers[selectedTriggerIndex].name : "");
    }

    bool none[kTriggerSideCount] = {false, false, false, false};
    const bool* sidesToShow = hasSelection ? triggers[selectedTriggerIndex].sides : none;
    setMultiHighlight(triggerSideButtonBgs, sidesToShow, kTriggerSideCount);
    if (triggerPreserveBg) {
        bool preserve = hasSelection && triggers[selectedTriggerIndex].preserve;
        triggerPreserveBg->setColor(preserve ? Color3B(255, 221, 120) : Color3B::WHITE);
    }

    conditionListContainer->removeAllChildren();
    actionListContainer->removeAllChildren();
    if (!hasSelection) {
        return;
    }

    const Trigger& t = triggers[selectedTriggerIndex];
    const float rowH = 80;
    const float rowWidth = 760;

    // Overflow guard, same windowing idea as the target dropdown: only a
    // limited number of row slots fit before each list runs into what sits
    // below it (conditions: the Actions section 280pt down; actions: the
    // panel bottom). Longer lists render a shifting window whose first and
    // last slots become Up/Down scroll buttons.
    Size visibleSize = Director::getInstance()->getVisibleSize();
    const float panelHeight = visibleSize.height - 80; // matches setupTriggerPanel
    const int maxCondSlots = 7; // (520 - 240) / rowH
    const int maxActSlots = std::max(3, static_cast<int>((panelHeight - 560 - 20) / rowH));

    auto makeScrollItem = [this](const char* text, bool enabled, float y, const std::function<void()>& onClick) {
        Color4B col = enabled ? Color4B(180, 180, 255, 255) : Color4B(90, 90, 110, 255);
        MenuItemLabel* item = MenuItemLabel::create(LM->getLocalizedLabel(text, col, 18),
                                                    [onClick](Ref*) { onClick(); });
        item->setEnabled(enabled);
        item->setPosition(Vec2(120, y));
        return item;
    };

    int condTotal = static_cast<int>(t.conditions.size());
    bool condNeedsScroll = condTotal > maxCondSlots;
    int condShown = condNeedsScroll ? maxCondSlots - 2 : condTotal;
    conditionListScroll = std::max(0, std::min(conditionListScroll, condTotal - condShown));

    Vector<MenuItem*> condScrollItems;
    int condSlot = 0;
    if (condNeedsScroll) {
        condScrollItems.pushBack(makeScrollItem("▲Up", conditionListScroll > 0,
                                                -static_cast<float>(condSlot++) * rowH - rowH / 2, [this]() {
            --conditionListScroll;
            // Deferred: rebuilding removes the menu that is mid-dispatch (same
            // reason the target dropdown defers its own rebuild).
            this->scheduleOnce([this](float) { refreshTriggerDetail(); }, 0.0f, "cond_scroll");
        }));
    }
    for (int i = conditionListScroll; i < conditionListScroll + condShown; i++, condSlot++) {
        int idx = i;
        float y = -static_cast<float>(condSlot) * rowH - rowH / 2;

        Label* lbl = LM->getLocalizedLabel(describeCondition(t.conditions[i]).c_str(), Color4B::WHITE, 20);
        lbl->setAnchorPoint(Vec2(0, 0.5f));
        lbl->setPosition(Vec2(0, y));
        conditionListContainer->addChild(lbl);

        MenuItemSprite* itemEdit = createTextButton("Edit", 70, rowH - 2, [this, idx](Ref*) { openConditionEditPanel(idx); });
        itemEdit->setPosition(Vec2(rowWidth - 90, y));
        MenuItemSprite* itemDel = createTextButton("X", 44, rowH - 2, [this, idx](Ref*) { deleteConditionAtIndex(idx); });
        itemDel->setPosition(Vec2(rowWidth - 20, y));
        Menu* menu = Menu::create(itemEdit, itemDel, nullptr);
        menu->setPosition(Vec2::ZERO);
        conditionListContainer->addChild(menu);
    }
    if (condNeedsScroll) {
        condScrollItems.pushBack(makeScrollItem("▼Down", conditionListScroll + condShown < condTotal,
                                                -static_cast<float>(condSlot) * rowH - rowH / 2, [this]() {
            ++conditionListScroll;
            this->scheduleOnce([this](float) { refreshTriggerDetail(); }, 0.0f, "cond_scroll");
        }));
        Menu* condScrollMenu = Menu::createWithArray(condScrollItems);
        condScrollMenu->setPosition(Vec2::ZERO);
        conditionListContainer->addChild(condScrollMenu);
    }

    int actTotal = static_cast<int>(t.actions.size());
    bool actNeedsScroll = actTotal > maxActSlots;
    int actShown = actNeedsScroll ? maxActSlots - 2 : actTotal;
    actionListScroll = std::max(0, std::min(actionListScroll, actTotal - actShown));

    Vector<MenuItem*> actScrollItems;
    int actSlot = 0;
    if (actNeedsScroll) {
        actScrollItems.pushBack(makeScrollItem("▲Up", actionListScroll > 0,
                                               -static_cast<float>(actSlot++) * rowH - rowH / 2, [this]() {
            --actionListScroll;
            this->scheduleOnce([this](float) { refreshTriggerDetail(); }, 0.0f, "act_scroll");
        }));
    }
    for (int i = actionListScroll; i < actionListScroll + actShown; i++, actSlot++) {
        int idx = i;
        float y = -static_cast<float>(actSlot) * rowH - rowH / 2;

        Label* lbl = LM->getLocalizedLabel(describeAction(t.actions[i]).c_str(), Color4B::WHITE, 20);
        lbl->setAnchorPoint(Vec2(0, 0.5f));
        lbl->setPosition(Vec2(0, y));
        // Long descriptions (Talk lines especially) must not run under the
        // Up/Down/Copy/Edit/X buttons on the right - shrink to fit the free space.
        const float maxLblW = rowWidth - 280;
        if (lbl->getContentSize().width > maxLblW) {
            lbl->setScale(maxLblW / lbl->getContentSize().width);
        }
        actionListContainer->addChild(lbl);

        Scale9Sprite* upBg = nullptr;
        MenuItemSprite* itemUp = createTextButton("▲", 32, rowH - 2, [this, idx](Ref*) { moveActionAtIndex(idx, -1); }, &upBg);
        itemUp->setPosition(Vec2(rowWidth - 246, y));
        bool canMoveUp = idx > 0;
        itemUp->setEnabled(canMoveUp);
        upBg->setOpacity(canMoveUp ? 255 : 90);

        Scale9Sprite* downBg = nullptr;
        MenuItemSprite* itemDown = createTextButton("▼", 32, rowH - 2, [this, idx](Ref*) { moveActionAtIndex(idx, 1); }, &downBg);
        itemDown->setPosition(Vec2(rowWidth - 210, y));
        bool canMoveDown = idx < actTotal - 1;
        itemDown->setEnabled(canMoveDown);
        downBg->setOpacity(canMoveDown ? 255 : 90);

        MenuItemSprite* itemCopy = createTextButton("Copy", 64, rowH - 2, [this, idx](Ref*) { duplicateActionAtIndex(idx); });
        itemCopy->setPosition(Vec2(rowWidth - 154, y));
        MenuItemSprite* itemEdit = createTextButton("Edit", 64, rowH - 2, [this, idx](Ref*) { openActionEditPanel(idx); });
        itemEdit->setPosition(Vec2(rowWidth - 82, y));
        MenuItemSprite* itemDel = createTextButton("X", 44, rowH - 2, [this, idx](Ref*) { deleteActionAtIndex(idx); });
        itemDel->setPosition(Vec2(rowWidth - 20, y));
        Menu* menu = Menu::create(itemUp, itemDown, itemCopy, itemEdit, itemDel, nullptr);
        menu->setPosition(Vec2::ZERO);
        actionListContainer->addChild(menu);
    }
    if (actNeedsScroll) {
        actScrollItems.pushBack(makeScrollItem("▼Down", actionListScroll + actShown < actTotal,
                                               -static_cast<float>(actSlot) * rowH - rowH / 2, [this]() {
            ++actionListScroll;
            this->scheduleOnce([this](float) { refreshTriggerDetail(); }, 0.0f, "act_scroll");
        }));
        Menu* actScrollMenu = Menu::createWithArray(actScrollItems);
        actScrollMenu->setPosition(Vec2::ZERO);
        actionListContainer->addChild(actScrollMenu);
    }
}

void MapEditor::deleteConditionAtIndex(int index) {
    if (selectedTriggerIndex < 0) {
        return;
    }
    Trigger& t = triggers[selectedTriggerIndex];
    if (index < 0 || index >= static_cast<int>(t.conditions.size())) {
        return;
    }
    t.conditions.erase(t.conditions.begin() + index);
    isDirty = true;
    refreshTriggerDetail();
}

void MapEditor::deleteActionAtIndex(int index) {
    if (selectedTriggerIndex < 0) {
        return;
    }
    Trigger& t = triggers[selectedTriggerIndex];
    if (index < 0 || index >= static_cast<int>(t.actions.size())) {
        return;
    }
    t.actions.erase(t.actions.begin() + index);
    isDirty = true;
    refreshTriggerDetail();
}

void MapEditor::duplicateActionAtIndex(int index) {
    if (selectedTriggerIndex < 0) {
        return;
    }
    Trigger& t = triggers[selectedTriggerIndex];
    if (index < 0 || index >= static_cast<int>(t.actions.size())) {
        return;
    }
    // Copy first - inserting a reference to the vector's own element would
    // read through a dangling reference if the insert reallocates.
    TriggerAction copy = t.actions[index];
    t.actions.insert(t.actions.begin() + index + 1, copy);
    isDirty = true;
    refreshTriggerDetail();
}

void MapEditor::moveActionAtIndex(int index, int dir) {
    if (selectedTriggerIndex < 0) {
        return;
    }
    Trigger& t = triggers[selectedTriggerIndex];
    int other = index + dir;
    if (index < 0 || index >= static_cast<int>(t.actions.size()) ||
        other < 0 || other >= static_cast<int>(t.actions.size())) {
        return;
    }
    std::swap(t.actions[index], t.actions[other]);
    isDirty = true;
    refreshTriggerDetail();
}

std::vector<std::pair<int, std::string>> MapEditor::buildTargetList() const {
    std::vector<std::pair<int, std::string>> list;
    list.push_back({-1, "Manual (Tile X/Y)"});
    for (const PlacedObject& obj : placedObjects) {
        if (strcmp(kObjectTypes[obj.typeIndex].name, "Tree") == 0) continue;
        list.push_back({obj.id, StringUtils::format("%s #%d", kObjectTypes[obj.typeIndex].name, obj.id)});
    }
    return list;
}

// "Any" (-1, only when includeAny) followed by every roster entry - the same
// set cycleAction*UnitType/cycleConditionUnitType step through with the arrows.
std::vector<std::pair<int, std::string>> MapEditor::buildUnitTypeList(bool includeAny, bool buildingsOnly) const {
    std::vector<std::pair<int, std::string>> list;
    if (includeAny) {
        list.push_back({-1, "Any"});
    }
    for (int i = 0; i < kObjectTypeCount; i++) {
        if (buildingsOnly && !isBuildingObjectType(i)) {
            continue;
        }
        list.push_back({i, kObjectTypes[i].name});
    }
    return list;
}

int MapEditor::cycleUnitTypeIndex(int current, int dir, bool buildingsOnly) const {
    if (!buildingsOnly) {
        return cyclicAdd(current, dir, -1, kObjectTypeCount - 1);
    }
    // Ordered list of valid values: -1 (Any) followed by each building index.
    std::vector<int> valid;
    valid.push_back(-1);
    for (int i = 0; i < kObjectTypeCount; i++) {
        if (isBuildingObjectType(i)) {
            valid.push_back(i);
        }
    }
    int pos = 0;
    for (int i = 0; i < static_cast<int>(valid.size()); i++) {
        if (valid[i] == current) {
            pos = i;
            break;
        }
    }
    int n = static_cast<int>(valid.size());
    pos = ((pos + dir) % n + n) % n;
    return valid[pos];
}

void MapEditor::openTargetDropdown(bool forCondition) {
    openDropdown(forCondition, DROPDOWN_FIELD_TARGET);
}

void MapEditor::openUnitTypeDropdown(bool forCondition) {
    openDropdown(forCondition, DROPDOWN_FIELD_UNIT_TYPE);
}

void MapEditor::openDropdown(bool forCondition, DropdownField field) {
    if (isTargetDropdownOpen) {
        closeTargetDropdown();
        return;
    }
    isTargetDropdownForCondition = forCondition;
    dropdownField = field;
    if (field == DROPDOWN_FIELD_TARGET) {
        targetDropdownList = buildTargetList();
        if (forCondition) {
            // Conditions have no tile-coordinate fallback - entry 0's id is still
            // -1 ("not set"), only the label changes.
            targetDropdownList[0].second = "(none)";
        }
    } else if (field == DROPDOWN_FIELD_MOVE_SOURCE) {
        // Same candidate list as a target row, but entry 0 means "no unit
        // chosen yet" rather than "fall back to manual tile X/Y".
        targetDropdownList = buildTargetList();
        targetDropdownList[0].second = "(Select Unit)";
    } else if (field == DROPDOWN_FIELD_ARRIVE_UNIT) {
        // Entry 0 (-1) means "Any unit matching Side/Unit Type", the
        // pre-existing behavior of the Arrives condition.
        targetDropdownList = buildTargetList();
        targetDropdownList[0].second = "(Any)";
    } else {
        // Create Unit is the only field where "Any" (-1) isn't a legal value -
        // it always creates a concrete unit type.
        bool includeAny = !(!forCondition && actionDraft.type == TACT_CREATE_UNIT);
        // Building Count counts buildings only, so its Unit Type list hides
        // non-building units and heroes.
        bool buildingsOnly = forCondition && conditionDraft.type == COND_BUILDING_COUNT;
        targetDropdownList = buildUnitTypeList(includeAny, buildingsOnly);
    }
    int currentId = currentDropdownValue();
    targetDropdownScroll = 0;
    for (int i = 0; i < (int)targetDropdownList.size(); i++) {
        if (targetDropdownList[i].first == currentId) {
            targetDropdownScroll = std::max(0, i - 4);
            break;
        }
    }
    isTargetDropdownOpen = true;
    rebuildTargetDropdown();
}

void MapEditor::closeTargetDropdown() {
    if (targetDropdown) {
        targetDropdown->removeFromParent();
        targetDropdown = nullptr;
    }
    isTargetDropdownOpen = false;
}

int MapEditor::currentDropdownValue() const {
    if (dropdownField == DROPDOWN_FIELD_UNIT_TYPE) {
        return isTargetDropdownForCondition ? conditionDraft.unitTypeIndex : actionDraft.unitTypeIndex;
    }
    if (dropdownField == DROPDOWN_FIELD_MOVE_SOURCE) {
        return actionDraft.sourceObjectId;
    }
    if (dropdownField == DROPDOWN_FIELD_ARRIVE_UNIT) {
        return conditionDraft.sourceObjectId;
    }
    return isTargetDropdownForCondition ? conditionDraft.targetObjectId : actionDraft.targetObjectId;
}

void MapEditor::selectTargetObject(int id) {
    if (dropdownField == DROPDOWN_FIELD_UNIT_TYPE) {
        if (isTargetDropdownForCondition) {
            conditionDraft.unitTypeIndex = id;
            refreshConditionEditPanel();
        } else {
            actionDraft.unitTypeIndex = id;
            refreshActionEditPanel();
        }
    } else if (dropdownField == DROPDOWN_FIELD_MOVE_SOURCE) {
        actionDraft.sourceObjectId = id;
        refreshActionEditPanel();
    } else if (dropdownField == DROPDOWN_FIELD_ARRIVE_UNIT) {
        conditionDraft.sourceObjectId = id;
        refreshConditionEditPanel();
    } else if (isTargetDropdownForCondition) {
        conditionDraft.targetObjectId = id;
        refreshConditionEditPanel();
    } else {
        actionDraft.targetObjectId = id;
        refreshActionEditPanel();
    }
    closeTargetDropdown();
}

// Panel-relative y offset (from groupY, see the setup* functions) of the row
// the currently-open dropdown should anchor under. Every "Unit Type:" row sits
// one slot below "Side:" in both panels, so that half is a constant; the
// target ("At:"/"To:") row's offset instead varies per action/condition type
// depending on how many rows precede it in that type's group.
float MapEditor::dropdownAnchorOffsetY() const {
    if (dropdownField == DROPDOWN_FIELD_UNIT_TYPE) {
        return -50.0f;
    }
    if (dropdownField == DROPDOWN_FIELD_MOVE_SOURCE) {
        return 0.0f; // Move's "Unit:" row is the first row in its group
    }
    if (dropdownField == DROPDOWN_FIELD_ARRIVE_UNIT) {
        return -100.0f; // Arrives' "Unit:" row, below Side/Unit Type
    }
    if (isTargetDropdownForCondition) {
        // Unit Arrives' "At:" row sits 150 below groupY (Side/Unit Type/Unit
        // above it); Unit Dies' "Unit:" row is the only row in its group.
        return conditionDraft.type == COND_UNIT_ARRIVES ? -150.0f : 0.0f;
    }
    switch (actionDraft.type) {
        case TACT_CREATE_UNIT:
            return -100.0f;
        case TACT_MOVE_UNIT:
        case TACT_TELEPORT_UNIT:
            return -50.0f; // "To:" row, now below Move/Teleport's "Unit:" row
        case TACT_REVEAL_FOG:
            return -50.0f;
        default:
            return 0.0f; // CENTER_CAMERA, TALK
    }
}

void MapEditor::rebuildTargetDropdown() {
    if (targetDropdown) {
        targetDropdown->removeFromParent();
        targetDropdown = nullptr;
    }
    Node* host = isTargetDropdownForCondition ? conditionEditPanel : actionEditPanel;
    if (!isTargetDropdownOpen || !host || !host->isVisible()) return;

    const int kMaxVis = 8;
    const float itemH = 36.0f;
    const float dropW = 400.0f;
    const float padV = 6.0f;
    const float scrollBtnH = 30.0f;
    int total = (int)targetDropdownList.size();
    bool needScroll = total > kMaxVis;
    int visCount = std::min(total - targetDropdownScroll, kMaxVis);
    float dropH = padV * 2 + visCount * itemH + (needScroll ? scrollBtnH * 2.0f : 0.0f);

    // Determine panel-relative y of the active row's center.
    // actionEditPanel height=560, conditionEditPanel height=520; both lay out
    // their per-type rows starting at groupY = panelH - 160 (see the setup
    // functions); dropdownAnchorOffsetY() gives the row's offset from groupY.
    float panelH = isTargetDropdownForCondition ? 520.0f : 560.0f;
    float groupY = panelH - 160.0f;
    float atRowPanelY = groupY + dropdownAnchorOffsetY();

    // Try below the "At:" row; if it clips the panel bottom, show above instead
    float dropBottom = atRowPanelY - 22.0f - dropH;
    if (dropBottom < 5.0f) {
        dropBottom = atRowPanelY + 22.0f;
        if (dropBottom + dropH > panelH - 5.0f) dropBottom = panelH - 5.0f - dropH;
    }
    if (dropBottom < 5.0f) dropBottom = 5.0f;

    targetDropdown = Node::create();
    targetDropdown->setPosition(Vec2(180.0f, dropBottom));
    host->addChild(targetDropdown, 10);

    DrawNode* bg = DrawNode::create();
    bg->drawSolidRect(Vec2::ZERO, Vec2(dropW, dropH), Color4F(0.07f, 0.07f, 0.13f, 0.97f));
    bg->drawRect(Vec2::ZERO, Vec2(dropW, dropH), Color4F(0.45f, 0.45f, 0.65f, 1.0f));
    targetDropdown->addChild(bg);

    Vector<MenuItem*> items;

    if (needScroll) {
        bool canUp = targetDropdownScroll > 0;
        Color4B col = canUp ? Color4B(180, 180, 255, 255) : Color4B(90, 90, 110, 255);
        MenuItemLabel* upBtn = MenuItemLabel::create(
            LM->getLocalizedLabel("▲Up", col, 18),
            [this, kMaxVis](Ref*) {
                // Page up: jump a full window of items, not a single row.
                targetDropdownScroll = std::max(0, targetDropdownScroll - kMaxVis);
                this->scheduleOnce([this](float){ rebuildTargetDropdown(); }, 0.0f, "dd_scroll");
            }
        );
        upBtn->setEnabled(canUp);
        upBtn->setPosition(Vec2(dropW / 2, dropH - padV - scrollBtnH / 2));
        items.pushBack(upBtn);
    }

    int currentId = currentDropdownValue();
    float curY = dropH - padV - (needScroll ? scrollBtnH : 0.0f) - itemH / 2;
    for (int i = 0; i < visCount; i++) {
        int idx = targetDropdownScroll + i;
        int capturedId = targetDropdownList[idx].first;
        std::string text = targetDropdownList[idx].second;
        bool isCurrent = (capturedId == currentId);
        Color4B col = isCurrent ? Color4B(100, 230, 100, 255) : Color4B::WHITE;
        Label* lbl = LM->getLocalizedLabel(text.c_str(), col, 20);
        MenuItemLabel* item = MenuItemLabel::create(lbl, [this, capturedId](Ref*) {
            int id = capturedId;
            this->scheduleOnce([this, id](float){ selectTargetObject(id); }, 0.0f, "dd_sel");
        });
        item->setPosition(Vec2(dropW / 2, curY));
        items.pushBack(item);
        curY -= itemH;
    }

    if (needScroll) {
        bool canDown = targetDropdownScroll + kMaxVis < total;
        Color4B col = canDown ? Color4B(180, 180, 255, 255) : Color4B(90, 90, 110, 255);
        MenuItemLabel* downBtn = MenuItemLabel::create(
            LM->getLocalizedLabel("▼Down", col, 18),
            [this, total, kMaxVis](Ref*) {
                // Page down: jump a full window, clamped so the last window
                // stays flush with the end of the list.
                targetDropdownScroll = std::min(std::max(0, total - kMaxVis),
                                                targetDropdownScroll + kMaxVis);
                this->scheduleOnce([this](float){ rebuildTargetDropdown(); }, 0.0f, "dd_scroll");
            }
        );
        downBtn->setEnabled(canDown);
        downBtn->setPosition(Vec2(dropW / 2, padV + scrollBtnH / 2));
        items.pushBack(downBtn);
    }

    if (!items.empty()) {
        Menu* dropMenu = Menu::createWithArray(items);
        dropMenu->setPosition(Vec2::ZERO);
        targetDropdown->addChild(dropMenu);
    }
}

void MapEditor::setupConditionEditPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const float panelWidth = 760;
    const float panelHeight = 520;

    conditionEditPanel = Node::create();
    conditionEditPanel->setPosition(Vec2(300, visibleSize.height - 700));
    conditionEditPanel->setVisible(false);
    this->addChild(conditionEditPanel, 25);

    DrawNode* panelBg = DrawNode::create();
    panelBg->drawSolidRect(Vec2(0, 0), Vec2(panelWidth, panelHeight), Color4F(0.12f, 0.12f, 0.16f, 0.97f));
    conditionEditPanel->addChild(panelBg);

    Label* title = LM->getLocalizedLabel("Edit Condition", Color4B::WHITE, 28);
    title->setPosition(Vec2(panelWidth / 2, panelHeight - 30));
    conditionEditPanel->addChild(title);

    addCycleRow(conditionEditPanel, "Type:", 30, panelHeight - 90, &lblCondType, [this](int dir) { cycleConditionType(dir); });

    const float groupY = panelHeight - 160;

    rowCondElapsed = Node::create();
    rowCondElapsed->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondElapsed);
    addTextFieldRow(rowCondElapsed, "Elapsed (sec):", 30, 0, 150, "30", &tfCondElapsedSeconds);

    rowCondSwitch = Node::create();
    rowCondSwitch->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondSwitch);
    addCycleRow(rowCondSwitch, "Switch:", 30, 0, &lblCondSwitchIndex, [this](int dir) { cycleConditionSwitchIndex(dir); });
    addCycleRow(rowCondSwitch, "State:", 30, -50, &lblCondSwitchState, [this](int dir) { cycleConditionSwitchState(dir); });

    rowCondUnitCount = Node::create();
    rowCondUnitCount->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondUnitCount);
    addCycleRow(rowCondUnitCount, "Side:", 30, 0, &lblCondSide, [this](int dir) { cycleConditionSide(dir); });
    addDropdownOverlay(addCycleRow(rowCondUnitCount, "Unit Type:", 30, -50, &lblCondUnitType, [this](int dir) { cycleConditionUnitType(dir); }),
                       [this]{ openUnitTypeDropdown(true); });
    addCycleRow(rowCondUnitCount, "Comparison:", 30, -100, &lblCondComparison, [this](int dir) { cycleConditionComparison(dir); });
    addTextFieldRow(rowCondUnitCount, "Amount:", 30, -150, 150, "1", &tfCondUnitAmount);

    rowCondResource = Node::create();
    rowCondResource->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondResource);
    addCycleRow(rowCondResource, "Resource:", 30, 0, &lblCondResource, [this](int dir) { cycleConditionResourceKind(dir); });
    addCycleRow(rowCondResource, "Comparison:", 30, -50, &lblCondResourceComparison, [this](int dir) { cycleConditionResourceComparison(dir); });
    addTextFieldRow(rowCondResource, "Amount:", 30, -100, 150, "0", &tfCondResourceAmount);

    rowCondUnitArrives = Node::create();
    rowCondUnitArrives->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondUnitArrives);
    addCycleRow(rowCondUnitArrives, "Side:", 30, 0, &lblCondArriveSide, [this](int dir) { cycleConditionSide(dir); });
    addDropdownOverlay(addCycleRow(rowCondUnitArrives, "Unit Type:", 30, -50, &lblCondArriveUnitType, [this](int dir) { cycleConditionUnitType(dir); }),
                       [this]{ openUnitTypeDropdown(true); });
    // "(Any)" keeps the Side/Unit Type filter; picking a placed unit makes
    // the condition watch that one unit only.
    addDropdownOverlay(addCycleRow(rowCondUnitArrives, "Unit:", 30, -100, &lblCondArriveUnit, [this](int dir) { cycleConditionArriveUnit(dir); }),
                       [this]{ openDropdown(true, DROPDOWN_FIELD_ARRIVE_UNIT); });
    addDropdownOverlay(addCycleRow(rowCondUnitArrives, "At:", 30, -150, &lblCondArriveTarget, [this](int dir) { cycleConditionTargetObject(dir); }),
                       [this]{ openTargetDropdown(true); });
    addTextFieldRow(rowCondUnitArrives, "Range (tiles):", 30, -200, 150, "2", &tfCondArriveRange);

    rowCondBuildingCount = Node::create();
    rowCondBuildingCount->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondBuildingCount);
    addCycleRow(rowCondBuildingCount, "Side:", 30, 0, &lblCondBuildingSide, [this](int dir) { cycleConditionSide(dir); });
    // "(Any)" counts every building of the side; picking a type counts only
    // buildings of that type (non-building types simply never match).
    addDropdownOverlay(addCycleRow(rowCondBuildingCount, "Unit Type:", 30, -50, &lblCondBuildingUnitType, [this](int dir) { cycleConditionUnitType(dir); }),
                       [this]{ openUnitTypeDropdown(true); });
    addCycleRow(rowCondBuildingCount, "Comparison:", 30, -100, &lblCondBuildingComparison, [this](int dir) { cycleConditionComparison(dir); });
    addTextFieldRow(rowCondBuildingCount, "Amount:", 30, -150, 150, "0", &tfCondBuildingAmount);

    rowCondUnitDies = Node::create();
    rowCondUnitDies->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondUnitDies);
    addDropdownOverlay(addCycleRow(rowCondUnitDies, "Unit:", 30, 0, &lblCondDiesTarget, [this](int dir) { cycleConditionTargetObject(dir); }),
                       [this]{ openTargetDropdown(true); });

    Scale9Sprite* isRepeatBg = nullptr;
    MenuItemSprite* itemIsRepeat = createTextButton("Is Repeat", 160, 50,
                                                    [this](Ref*) { toggleConditionIsRepeat(); }, &isRepeatBg);
    itemIsRepeat->setPosition(Vec2(panelWidth / 2, 110));
    Menu* isRepeatMenu = Menu::create(itemIsRepeat, nullptr);
    isRepeatMenu->setPosition(Vec2::ZERO);
    conditionEditPanel->addChild(isRepeatMenu);
    condIsRepeatBg = isRepeatBg;

    MenuItemSprite* itemConfirm = createTextButton("OK", 140, 56, [this](Ref*) { onConfirmCondition(); });
    MenuItemSprite* itemCancel = createTextButton("Cancel", 140, 56, [this](Ref*) { closeConditionEditPanel(); });
    itemConfirm->setPosition(Vec2(panelWidth / 2 - 90, 40));
    itemCancel->setPosition(Vec2(panelWidth / 2 + 90, 40));
    Menu* confirmMenu = Menu::create(itemConfirm, itemCancel, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    conditionEditPanel->addChild(confirmMenu);
}

void MapEditor::openConditionEditPanel(int conditionIndex) {
    if (selectedTriggerIndex < 0) {
        return;
    }
    // The two sub-panels occupy the same screen area at z=25 — never both open.
    if (isActionEditPanelOpen) {
        closeActionEditPanel();
    }
    editingConditionIndex = conditionIndex;
    const Trigger& t = triggers[selectedTriggerIndex];
    bool isNewCondition = !(conditionIndex >= 0 && conditionIndex < static_cast<int>(t.conditions.size()));
    if (!isNewCondition) {
        conditionDraft = t.conditions[conditionIndex];
    } else {
        conditionDraft = TriggerCondition();
    }

    tfCondElapsedSeconds->setString(StringUtils::format("%d", conditionDraft.elapsedSeconds));
    tfCondUnitAmount->setString(StringUtils::format("%d", conditionDraft.amount));
    tfCondResourceAmount->setString(StringUtils::format("%d", conditionDraft.amount));
    // `amount` is shared across condition types (default 1), but Unit Arrives
    // wants a wider default range of 2 tiles for freshly created conditions.
    tfCondArriveRange->setString(StringUtils::format("%d", isNewCondition ? 2 : std::max(1, conditionDraft.amount)));
    tfCondBuildingAmount->setString(StringUtils::format("%d", conditionDraft.amount));
    refreshConditionEditPanel();

    conditionEditPanel->setVisible(true);
    isConditionEditPanelOpen = true;
    setModalDimmerVisible(true);
    setTriggerSubDimmerVisible(true);
}

void MapEditor::closeConditionEditPanel() {
    if (isTargetDropdownForCondition) {
        closeTargetDropdown();
    }
    conditionEditPanel->setVisible(false);
    isConditionEditPanelOpen = false;
    setModalDimmerVisible(isTriggerPanelOpen);
    setTriggerSubDimmerVisible(isActionEditPanelOpen);
}

void MapEditor::cycleConditionType(int dir) {
    if (isTargetDropdownForCondition) {
        closeTargetDropdown();
    }
    conditionDraft.type = static_cast<TriggerConditionType>(cyclicAdd(conditionDraft.type, dir, 0, COND_TYPE_COUNT - 1));
    // Building Count only accepts building types - if a non-building unit was
    // carried over from the previous type, fall back to "Any" so the row never
    // shows a value its own dropdown can't reach.
    if (conditionDraft.type == COND_BUILDING_COUNT &&
        conditionDraft.unitTypeIndex >= 0 && !isBuildingObjectType(conditionDraft.unitTypeIndex)) {
        conditionDraft.unitTypeIndex = -1;
    }
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionSide(int dir) {
    conditionDraft.unitSide = cyclicAdd(conditionDraft.unitSide, dir, 0, kSideCount - 1);
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionUnitType(int dir) {
    // Shared by the Unit Count / Unit Arrives / Building Count rows (only one is
    // visible at a time); Building Count steps through buildings only.
    bool buildingsOnly = (conditionDraft.type == COND_BUILDING_COUNT);
    conditionDraft.unitTypeIndex = cycleUnitTypeIndex(conditionDraft.unitTypeIndex, dir, buildingsOnly);
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionComparison(int dir) {
    conditionDraft.comparison = static_cast<TriggerComparison>(cyclicAdd(conditionDraft.comparison, dir, 0, CMP_COUNT - 1));
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionResourceKind(int dir) {
    conditionDraft.resourceKind = static_cast<TriggerResourceKind>(cyclicAdd(conditionDraft.resourceKind, dir, 0, RESOURCE_KIND_COUNT - 1));
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionResourceComparison(int dir) {
    conditionDraft.comparison = static_cast<TriggerComparison>(cyclicAdd(conditionDraft.comparison, dir, 0, CMP_COUNT - 1));
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionSwitchIndex(int dir) {
    conditionDraft.switchIndex = cyclicAdd(conditionDraft.switchIndex, dir, 0, kTriggerSwitchCount - 1);
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionSwitchState(int dir) {
    conditionDraft.switchState = static_cast<TriggerSwitchState>(cyclicAdd(conditionDraft.switchState, dir, 0, SWITCH_STATE_COUNT - 1));
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionTargetObject(int dir) {
    conditionDraft.targetObjectId = cycleTargetObjectId(conditionDraft.targetObjectId, dir);
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionArriveUnit(int dir) {
    conditionDraft.sourceObjectId = cycleTargetObjectId(conditionDraft.sourceObjectId, dir);
    refreshConditionEditPanel();
}

void MapEditor::toggleConditionIsRepeat() {
    conditionDraft.isRepeat = !conditionDraft.isRepeat;
    if (condIsRepeatBg) {
        condIsRepeatBg->setColor(conditionDraft.isRepeat ? Color3B(255, 221, 120) : Color3B::WHITE);
    }
}

void MapEditor::refreshConditionEditPanel() {
    lblCondType->setString(kConditionTypeNames[conditionDraft.type]);

    rowCondElapsed->setVisible(conditionDraft.type == COND_ELAPSED_TIME);
    rowCondSwitch->setVisible(conditionDraft.type == COND_SWITCH);
    rowCondUnitCount->setVisible(conditionDraft.type == COND_UNIT_COUNT);
    rowCondResource->setVisible(conditionDraft.type == COND_RESOURCE);
    rowCondUnitArrives->setVisible(conditionDraft.type == COND_UNIT_ARRIVES);
    rowCondBuildingCount->setVisible(conditionDraft.type == COND_BUILDING_COUNT);
    rowCondUnitDies->setVisible(conditionDraft.type == COND_UNIT_DIES);

    lblCondSwitchIndex->setString(StringUtils::format("%d", conditionDraft.switchIndex + 1));
    lblCondSwitchState->setString(kSwitchStateNames[conditionDraft.switchState]);
    lblCondSide->setString(kSideNames[conditionDraft.unitSide]);
    lblCondUnitType->setString(unitTypeCycleName(conditionDraft.unitTypeIndex));
    lblCondComparison->setString(kComparisonNames[conditionDraft.comparison]);
    lblCondResource->setString(kResourceKindNames[conditionDraft.resourceKind]);
    lblCondResourceComparison->setString(kComparisonNames[conditionDraft.comparison]);
    lblCondArriveSide->setString(kSideNames[conditionDraft.unitSide]);
    lblCondArriveUnitType->setString(unitTypeCycleName(conditionDraft.unitTypeIndex));
    lblCondArriveUnit->setString(conditionDraft.sourceObjectId >= 0
                                     ? describeTargetObject(conditionDraft.sourceObjectId)
                                     : "(Any)");
    lblCondArriveTarget->setString(conditionDraft.targetObjectId >= 0
                                       ? describeTargetObject(conditionDraft.targetObjectId)
                                       : "(pick a flag/unit)");
    lblCondBuildingSide->setString(kSideNames[conditionDraft.unitSide]);
    lblCondBuildingUnitType->setString(unitTypeCycleName(conditionDraft.unitTypeIndex));
    lblCondBuildingComparison->setString(kComparisonNames[conditionDraft.comparison]);
    lblCondDiesTarget->setString(conditionDraft.targetObjectId >= 0
                                      ? describeTargetObject(conditionDraft.targetObjectId)
                                      : "(none)");
    if (condIsRepeatBg) {
        condIsRepeatBg->setColor(conditionDraft.isRepeat ? Color3B(255, 221, 120) : Color3B::WHITE);
    }
}

void MapEditor::onConfirmCondition() {
    if (selectedTriggerIndex < 0) {
        closeConditionEditPanel();
        return;
    }
    TriggerCondition c = conditionDraft;
    if (c.type == COND_ELAPSED_TIME) {
        c.elapsedSeconds = std::max(0, atoi(tfCondElapsedSeconds->getString().c_str()));
    } else if (c.type == COND_UNIT_COUNT) {
        c.amount = std::max(0, atoi(tfCondUnitAmount->getString().c_str()));
    } else if (c.type == COND_RESOURCE) {
        c.amount = std::max(0, atoi(tfCondResourceAmount->getString().c_str()));
    } else if (c.type == COND_UNIT_ARRIVES) {
        c.amount = std::max(1, atoi(tfCondArriveRange->getString().c_str()));
    } else if (c.type == COND_BUILDING_COUNT) {
        c.amount = std::max(0, atoi(tfCondBuildingAmount->getString().c_str()));
    }

    Trigger& t = triggers[selectedTriggerIndex];
    if (editingConditionIndex >= 0 && editingConditionIndex < static_cast<int>(t.conditions.size())) {
        t.conditions[editingConditionIndex] = c;
    } else {
        t.conditions.push_back(c);
    }
    isDirty = true;
    closeConditionEditPanel();
    refreshTriggerDetail();
}

void MapEditor::setupActionEditPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const float panelWidth = 760;
    const float panelHeight = 560;

    actionEditPanel = Node::create();
    actionEditPanel->setPosition(Vec2(300, visibleSize.height - 740));
    actionEditPanel->setVisible(false);
    this->addChild(actionEditPanel, 25);

    DrawNode* panelBg = DrawNode::create();
    panelBg->drawSolidRect(Vec2(0, 0), Vec2(panelWidth, panelHeight), Color4F(0.12f, 0.12f, 0.16f, 0.97f));
    actionEditPanel->addChild(panelBg);

    Label* title = LM->getLocalizedLabel("Edit Action", Color4B::WHITE, 28);
    title->setPosition(Vec2(panelWidth / 2, panelHeight - 30));
    actionEditPanel->addChild(title);

    addCycleRow(actionEditPanel, "Type:", 30, panelHeight - 90, &lblActType, [this](int dir) { cycleActionType(dir); });

    const float groupY = panelHeight - 160;

    rowActMessage = Node::create();
    rowActMessage->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActMessage);
    addTextFieldRow(rowActMessage, "Message:", 30, 0, 300, "", &tfActMessage);

    rowActCreateUnit = Node::create();
    rowActCreateUnit->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActCreateUnit);
    addCycleRow(rowActCreateUnit, "Side:", 30, 0, &lblActCreateSide, [this](int dir) { cycleActionCreateSide(dir); });
    addDropdownOverlay(addCycleRow(rowActCreateUnit, "Unit Type:", 30, -50, &lblActCreateUnitType, [this](int dir) { cycleActionCreateUnitType(dir); }),
                       [this]{ openUnitTypeDropdown(); });
    addDropdownOverlay(addCycleRow(rowActCreateUnit, "At:", 30, -100, &lblActCreateTarget, [this](int dir) { cycleActionCreateTarget(dir); }),
                       [this]{ openTargetDropdown(); });
    rowActCreateX = addTextFieldRow(rowActCreateUnit, "Tile X:", 30, -150, 150, "0", &tfActCreateX);
    rowActCreateY = addTextFieldRow(rowActCreateUnit, "Tile Y:", 30, -200, 150, "0", &tfActCreateY);
    addTextFieldRow(rowActCreateUnit, "Count:", 30, -250, 150, "1", &tfActCreateCount);

    rowActRemoveUnit = Node::create();
    rowActRemoveUnit->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActRemoveUnit);
    addCycleRow(rowActRemoveUnit, "Side:", 30, 0, &lblActRemoveSide, [this](int dir) { cycleActionRemoveSide(dir); });
    addDropdownOverlay(addCycleRow(rowActRemoveUnit, "Unit Type:", 30, -50, &lblActRemoveUnitType, [this](int dir) { cycleActionRemoveUnitType(dir); }),
                       [this]{ openUnitTypeDropdown(); });

    rowActSwitch = Node::create();
    rowActSwitch->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActSwitch);
    addCycleRow(rowActSwitch, "Switch:", 30, 0, &lblActSwitchIndex, [this](int dir) { cycleActionSwitchIndex(dir); });
    addCycleRow(rowActSwitch, "Action:", 30, -50, &lblActSwitchAction, [this](int dir) { cycleActionSwitchAction(dir); });

    rowActWait = Node::create();
    rowActWait->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActWait);
    addTextFieldRow(rowActWait, "Wait (sec):", 30, 0, 150, "1", &tfActWaitSeconds);

    rowActCamera = Node::create();
    rowActCamera->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActCamera);
    addDropdownOverlay(addCycleRow(rowActCamera, "At:", 30, 0, &lblActCameraTarget, [this](int dir) { cycleActionCameraTarget(dir); }),
                       [this]{ openTargetDropdown(); });
    rowActCameraX = addTextFieldRow(rowActCamera, "Tile X:", 30, -50, 150, "0", &tfActCameraX);
    rowActCameraY = addTextFieldRow(rowActCamera, "Tile Y:", 30, -100, 150, "0", &tfActCameraY);

    rowActTalk = Node::create();
    rowActTalk->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActTalk);
    addDropdownOverlay(addCycleRow(rowActTalk, "At:", 30, 0, &lblActTalkTarget, [this](int dir) { cycleActionTalkTarget(dir); }),
                       [this]{ openTargetDropdown(); });
    rowActTalkX = addTextFieldRow(rowActTalk, "Tile X:", 30, -50, 150, "0", &tfActTalkX);
    rowActTalkY = addTextFieldRow(rowActTalk, "Tile Y:", 30, -100, 150, "0", &tfActTalkY);
    addTextFieldRow(rowActTalk, "Message:", 30, -150, 300, "", &tfActTalkMessage);
    addTextFieldRow(rowActTalk, "Duration (sec):", 30, -200, 150, "3", &tfActTalkSeconds);

    rowActRevealFog = Node::create();
    rowActRevealFog->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActRevealFog);
    addCycleRow(rowActRevealFog, "Vision:", 30, 0, &lblActRevealFogEnabled, [this](int dir) { cycleActionRevealFogEnabled(dir); });
    addDropdownOverlay(addCycleRow(rowActRevealFog, "At:", 30, -50, &lblActRevealFogTarget, [this](int dir) { cycleActionRevealFogTarget(dir); }),
                       [this]{ openTargetDropdown(); });
    rowActRevealFogX = addTextFieldRow(rowActRevealFog, "Tile X:", 30, -100, 150, "0", &tfActRevealFogX);
    rowActRevealFogY = addTextFieldRow(rowActRevealFog, "Tile Y:", 30, -150, 150, "0", &tfActRevealFogY);
    addTextFieldRow(rowActRevealFog, "Radius (fog):", 30, -200, 150, "5", &tfActRevealFogRadius);

    rowActOrderAttack = Node::create();
    rowActOrderAttack->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActOrderAttack);
    addCycleRow(rowActOrderAttack, "Side:", 30, 0, &lblActOrderAttackSide, [this](int dir) { cycleActionOrderAttackSide(dir); });
    addDropdownOverlay(addCycleRow(rowActOrderAttack, "Unit Type:", 30, -50, &lblActOrderAttackUnitType, [this](int dir) { cycleActionOrderAttackUnitType(dir); }),
                       [this]{ openUnitTypeDropdown(); });

    rowActMoveUnit = Node::create();
    rowActMoveUnit->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActMoveUnit);
    addDropdownOverlay(addCycleRow(rowActMoveUnit, "Unit:", 30, 0, &lblActMoveUnit, [this](int dir) { cycleActionMoveSource(dir); }),
                       [this]{ openDropdown(false, DROPDOWN_FIELD_MOVE_SOURCE); });
    addDropdownOverlay(addCycleRow(rowActMoveUnit, "To:", 30, -50, &lblActMoveTarget, [this](int dir) { cycleActionMoveTarget(dir); }),
                       [this]{ openTargetDropdown(); });
    rowActMoveX = addTextFieldRow(rowActMoveUnit, "Tile X:", 30, -100, 150, "0", &tfActMoveX);
    rowActMoveY = addTextFieldRow(rowActMoveUnit, "Tile Y:", 30, -150, 150, "0", &tfActMoveY);

    rowActLockControl = Node::create();
    rowActLockControl->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActLockControl);
    addCycleRow(rowActLockControl, "State:", 30, 0, &lblActLockControlState, [this](int dir) { cycleActionLockControlState(dir); });

    // Applies to every action type (hence outside the per-type groups, and
    // below their deepest row at groupY-250): whether this action starts
    // together with the previous one instead of after it finishes.
    addCycleRow(actionEditPanel, "Timing:", 30, 100, &lblActRunWithPrev, [this](int dir) { cycleActionRunWithPrevious(dir); });

    MenuItemSprite* itemConfirm = createTextButton("OK", 140, 56, [this](Ref*) { onConfirmAction(); });
    MenuItemSprite* itemCancel = createTextButton("Cancel", 140, 56, [this](Ref*) { closeActionEditPanel(); });
    itemConfirm->setPosition(Vec2(panelWidth / 2 - 90, 40));
    itemCancel->setPosition(Vec2(panelWidth / 2 + 90, 40));
    Menu* confirmMenu = Menu::create(itemConfirm, itemCancel, nullptr);
    confirmMenu->setPosition(Vec2::ZERO);
    actionEditPanel->addChild(confirmMenu);
}

void MapEditor::openActionEditPanel(int actionIndex) {
    if (selectedTriggerIndex < 0) {
        return;
    }
    // The two sub-panels occupy the same screen area at z=25 — never both open.
    if (isConditionEditPanelOpen) {
        closeConditionEditPanel();
    }
    editingActionIndex = actionIndex;
    const Trigger& t = triggers[selectedTriggerIndex];
    if (actionIndex >= 0 && actionIndex < static_cast<int>(t.actions.size())) {
        actionDraft = t.actions[actionIndex];
    } else {
        actionDraft = TriggerAction();
    }

    tfActMessage->setString(actionDraft.message);
    tfActCreateX->setString(StringUtils::format("%d", actionDraft.tileX));
    tfActCreateY->setString(StringUtils::format("%d", actionDraft.tileY));
    tfActCreateCount->setString(StringUtils::format("%d", actionDraft.count));
    tfActWaitSeconds->setString(StringUtils::format("%.1f", actionDraft.waitSeconds));
    tfActCameraX->setString(StringUtils::format("%d", actionDraft.tileX));
    tfActCameraY->setString(StringUtils::format("%d", actionDraft.tileY));
    tfActTalkX->setString(StringUtils::format("%d", actionDraft.tileX));
    tfActTalkY->setString(StringUtils::format("%d", actionDraft.tileY));
    tfActTalkMessage->setString(actionDraft.message);
    tfActTalkSeconds->setString(StringUtils::format("%.1f", actionDraft.talkSeconds));
    int revealRadius = (actionDraft.type == TACT_REVEAL_FOG) ? std::max(1, actionDraft.count) : 5;
    tfActRevealFogX->setString(StringUtils::format("%d", actionDraft.tileX));
    tfActRevealFogY->setString(StringUtils::format("%d", actionDraft.tileY));
    tfActRevealFogRadius->setString(StringUtils::format("%d", revealRadius));
    tfActMoveX->setString(StringUtils::format("%d", actionDraft.tileX));
    tfActMoveY->setString(StringUtils::format("%d", actionDraft.tileY));
    refreshActionEditPanel();

    actionEditPanel->setVisible(true);
    isActionEditPanelOpen = true;
    setModalDimmerVisible(true);
    setTriggerSubDimmerVisible(true);
}

void MapEditor::closeActionEditPanel() {
    closeTargetDropdown();
    actionEditPanel->setVisible(false);
    isActionEditPanelOpen = false;
    setModalDimmerVisible(isTriggerPanelOpen);
    setTriggerSubDimmerVisible(isConditionEditPanelOpen);
}

void MapEditor::cycleActionType(int dir) {
    closeTargetDropdown();
    actionDraft.type = static_cast<TriggerActionType>(cyclicAdd(actionDraft.type, dir, 0, TACT_TYPE_COUNT - 1));
    refreshActionEditPanel();
}

void MapEditor::cycleActionCreateSide(int dir) {
    actionDraft.unitSide = cyclicAdd(actionDraft.unitSide, dir, 0, kSideCount - 1);
    refreshActionEditPanel();
}

void MapEditor::cycleActionCreateUnitType(int dir) {
    actionDraft.unitTypeIndex = cyclicAdd(std::max(actionDraft.unitTypeIndex, 0), dir, 0, kObjectTypeCount - 1);
    refreshActionEditPanel();
}

void MapEditor::cycleActionRemoveSide(int dir) {
    actionDraft.unitSide = cyclicAdd(actionDraft.unitSide, dir, 0, kSideCount - 1);
    refreshActionEditPanel();
}

void MapEditor::cycleActionRemoveUnitType(int dir) {
    actionDraft.unitTypeIndex = cyclicAdd(actionDraft.unitTypeIndex, dir, -1, kObjectTypeCount - 1);
    refreshActionEditPanel();
}

void MapEditor::cycleActionSwitchIndex(int dir) {
    actionDraft.switchIndex = cyclicAdd(actionDraft.switchIndex, dir, 0, kTriggerSwitchCount - 1);
    refreshActionEditPanel();
}

void MapEditor::cycleActionSwitchAction(int dir) {
    actionDraft.switchAction = static_cast<TriggerSwitchAction>(cyclicAdd(actionDraft.switchAction, dir, 0, SWITCH_ACTION_COUNT - 1));
    refreshActionEditPanel();
}

// Candidates are "Manual" (-1) followed by every currently placed object (flags
// and regular units alike), in placement order. Re-read fresh each cycle so
// edits made elsewhere in the map are reflected immediately.
int MapEditor::cycleTargetObjectId(int currentId, int dir) const {
    std::vector<int> ids;
    ids.push_back(-1);
    for (const PlacedObject& obj : placedObjects) {
        if (strcmp(kObjectTypes[obj.typeIndex].name, "Tree") == 0) continue;
        ids.push_back(obj.id);
    }
    int idx = 0;
    for (size_t i = 0; i < ids.size(); i++) {
        if (ids[i] == currentId) {
            idx = static_cast<int>(i);
            break;
        }
    }
    idx = cyclicAdd(idx, dir, 0, static_cast<int>(ids.size()) - 1);
    return ids[idx];
}

std::string MapEditor::describeTargetObject(int id) const {
    if (id >= 0) {
        for (const PlacedObject& obj : placedObjects) {
            if (obj.id == id) {
                return StringUtils::format("%s #%d", kObjectTypes[obj.typeIndex].name, obj.id);
            }
        }
    }
    return "Manual (Tile X/Y)";
}

void MapEditor::cycleActionCreateTarget(int dir) {
    actionDraft.targetObjectId = cycleTargetObjectId(actionDraft.targetObjectId, dir);
    refreshActionEditPanel();
}

void MapEditor::cycleActionCameraTarget(int dir) {
    actionDraft.targetObjectId = cycleTargetObjectId(actionDraft.targetObjectId, dir);
    refreshActionEditPanel();
}

void MapEditor::cycleActionTalkTarget(int dir) {
    actionDraft.targetObjectId = cycleTargetObjectId(actionDraft.targetObjectId, dir);
    refreshActionEditPanel();
}

void MapEditor::cycleActionRevealFogEnabled(int dir) {
    actionDraft.visionEnabled = !actionDraft.visionEnabled;
    refreshActionEditPanel();
}

void MapEditor::cycleActionRevealFogTarget(int dir) {
    actionDraft.targetObjectId = cycleTargetObjectId(actionDraft.targetObjectId, dir);
    refreshActionEditPanel();
}

void MapEditor::cycleActionOrderAttackSide(int dir) {
    actionDraft.unitSide = cyclicAdd(actionDraft.unitSide, dir, 0, kSideCount - 1);
    refreshActionEditPanel();
}

void MapEditor::cycleActionOrderAttackUnitType(int dir) {
    actionDraft.unitTypeIndex = cyclicAdd(actionDraft.unitTypeIndex, dir, -1, kObjectTypeCount - 1);
    refreshActionEditPanel();
}

void MapEditor::cycleActionMoveSource(int dir) {
    actionDraft.sourceObjectId = cycleTargetObjectId(actionDraft.sourceObjectId, dir);
    refreshActionEditPanel();
}

void MapEditor::cycleActionMoveTarget(int dir) {
    actionDraft.targetObjectId = cycleTargetObjectId(actionDraft.targetObjectId, dir);
    refreshActionEditPanel();
}

void MapEditor::cycleActionLockControlState(int dir) {
    actionDraft.controlLocked = !actionDraft.controlLocked;
    refreshActionEditPanel();
}

void MapEditor::cycleActionRunWithPrevious(int dir) {
    actionDraft.runWithPrevious = !actionDraft.runWithPrevious;
    refreshActionEditPanel();
}

void MapEditor::refreshActionEditPanel() {
    lblActType->setString(kTriggerActionTypeNames[actionDraft.type]);

    rowActMessage->setVisible(actionDraft.type == TACT_DISPLAY_MESSAGE);
    rowActCreateUnit->setVisible(actionDraft.type == TACT_CREATE_UNIT);
    rowActRemoveUnit->setVisible(actionDraft.type == TACT_REMOVE_UNIT);
    rowActSwitch->setVisible(actionDraft.type == TACT_SET_SWITCH);
    rowActWait->setVisible(actionDraft.type == TACT_WAIT);
    rowActCamera->setVisible(actionDraft.type == TACT_CENTER_CAMERA);
    rowActTalk->setVisible(actionDraft.type == TACT_TALK);
    rowActRevealFog->setVisible(actionDraft.type == TACT_REVEAL_FOG);
    rowActOrderAttack->setVisible(actionDraft.type == TACT_ORDER_ATTACK);
    // Teleport shares Move's rows - both are "this unit goes to this spot",
    // only the runtime behaviour (walk vs. instant placement) differs.
    rowActMoveUnit->setVisible(actionDraft.type == TACT_MOVE_UNIT || actionDraft.type == TACT_TELEPORT_UNIT);
    rowActLockControl->setVisible(actionDraft.type == TACT_LOCK_CONTROL);

    bool createUsesManual = actionDraft.targetObjectId < 0;
    rowActCreateX->setVisible(createUsesManual);
    rowActCreateY->setVisible(createUsesManual);
    bool cameraUsesManual = actionDraft.targetObjectId < 0;
    rowActCameraX->setVisible(cameraUsesManual);
    rowActCameraY->setVisible(cameraUsesManual);
    bool talkUsesManual = actionDraft.targetObjectId < 0;
    rowActTalkX->setVisible(talkUsesManual);
    rowActTalkY->setVisible(talkUsesManual);
    bool revealUsesManual = actionDraft.targetObjectId < 0;
    rowActRevealFogX->setVisible(revealUsesManual);
    rowActRevealFogY->setVisible(revealUsesManual);
    bool moveUsesManual = actionDraft.targetObjectId < 0;
    rowActMoveX->setVisible(moveUsesManual);
    rowActMoveY->setVisible(moveUsesManual);

    lblActCreateSide->setString(kSideNames[actionDraft.unitSide]);
    int createUnitType = actionDraft.unitTypeIndex < 0 ? 0 : actionDraft.unitTypeIndex;
    lblActCreateUnitType->setString(unitTypeCycleName(createUnitType));
    lblActCreateTarget->setString(describeTargetObject(actionDraft.targetObjectId));
    lblActRemoveSide->setString(kSideNames[actionDraft.unitSide]);
    lblActRemoveUnitType->setString(unitTypeCycleName(actionDraft.unitTypeIndex));
    lblActSwitchIndex->setString(StringUtils::format("%d", actionDraft.switchIndex + 1));
    lblActSwitchAction->setString(kSwitchActionNames[actionDraft.switchAction]);
    lblActCameraTarget->setString(describeTargetObject(actionDraft.targetObjectId));
    lblActTalkTarget->setString(describeTargetObject(actionDraft.targetObjectId));
    lblActRevealFogEnabled->setString(actionDraft.visionEnabled ? "Enabled" : "Disabled");
    lblActRevealFogTarget->setString(describeTargetObject(actionDraft.targetObjectId));
    lblActOrderAttackSide->setString(kSideNames[actionDraft.unitSide]);
    lblActOrderAttackUnitType->setString(unitTypeCycleName(actionDraft.unitTypeIndex));
    lblActMoveUnit->setString(actionDraft.sourceObjectId >= 0 ? describeTargetObject(actionDraft.sourceObjectId) : "(Select Unit)");
    lblActMoveTarget->setString(describeTargetObject(actionDraft.targetObjectId));
    lblActLockControlState->setString(actionDraft.controlLocked ? "Locked" : "Unlocked");
    lblActRunWithPrev->setString(actionDraft.runWithPrevious ? "With Previous" : "After Previous");
}

void MapEditor::onConfirmAction() {
    if (selectedTriggerIndex < 0) {
        closeActionEditPanel();
        return;
    }
    TriggerAction a = actionDraft;
    if (a.type == TACT_DISPLAY_MESSAGE) {
        a.message = tfActMessage->getString();
    } else if (a.type == TACT_CREATE_UNIT) {
        a.unitTypeIndex = std::max(0, a.unitTypeIndex);
        if (a.targetObjectId < 0) {
            a.tileX = atoi(tfActCreateX->getString().c_str());
            a.tileY = atoi(tfActCreateY->getString().c_str());
        }
        a.count = std::max(1, atoi(tfActCreateCount->getString().c_str()));
    } else if (a.type == TACT_WAIT) {
        a.waitSeconds = std::max(0.0f, static_cast<float>(atof(tfActWaitSeconds->getString().c_str())));
    } else if (a.type == TACT_CENTER_CAMERA) {
        if (a.targetObjectId < 0) {
            a.tileX = atoi(tfActCameraX->getString().c_str());
            a.tileY = atoi(tfActCameraY->getString().c_str());
        }
    } else if (a.type == TACT_TALK) {
        if (a.targetObjectId < 0) {
            a.tileX = atoi(tfActTalkX->getString().c_str());
            a.tileY = atoi(tfActTalkY->getString().c_str());
        }
        a.message = tfActTalkMessage->getString();
        a.talkSeconds = std::max(0.0f, static_cast<float>(atof(tfActTalkSeconds->getString().c_str())));
    } else if (a.type == TACT_REVEAL_FOG) {
        if (a.targetObjectId < 0) {
            a.tileX = atoi(tfActRevealFogX->getString().c_str());
            a.tileY = atoi(tfActRevealFogY->getString().c_str());
        }
        a.count = std::max(1, atoi(tfActRevealFogRadius->getString().c_str()));
    } else if (a.type == TACT_MOVE_UNIT || a.type == TACT_TELEPORT_UNIT) {
        if (a.targetObjectId < 0) {
            a.tileX = atoi(tfActMoveX->getString().c_str());
            a.tileY = atoi(tfActMoveY->getString().c_str());
        }
    }

    Trigger& t = triggers[selectedTriggerIndex];
    if (editingActionIndex >= 0 && editingActionIndex < static_cast<int>(t.actions.size())) {
        t.actions[editingActionIndex] = a;
    } else {
        t.actions.push_back(a);
    }
    isDirty = true;
    closeActionEditPanel();
    refreshTriggerDetail();
}

void MapEditor::runMap() {
    if (isDirty) {
        saveMap();
        if (isDirty) {
            setStatus("Save before running");
            return;
        }
    }
    std::string json = serialize();
    std::string runPath = FileUtils::getInstance()->getWritablePath() + "stage_70_templete_0.json";
    bool ok = FileUtils::getInstance()->writeStringToFile(json, runPath);
    if (!ok) {
        setStatus("Run failed: could not write map data");
        return;
    }
    GM->nextScene = STAGE_FIELD;
    Director::getInstance()->replaceScene(GameScene::scene(STAGE_CUSTOM, GAME_MODE_NORMAL));
}

void MapEditor::setupInput() {
    keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event*) {
        if (key == EventKeyboard::KeyCode::KEY_CTRL || key == EventKeyboard::KeyCode::KEY_RIGHT_CTRL) {
            ctrlDown = true;
        } else if (key == EventKeyboard::KeyCode::KEY_SHIFT || key == EventKeyboard::KeyCode::KEY_RIGHT_SHIFT) {
            shiftDown = true;
        }
        if (isAnyModalOpen() || isTextFieldFocused) {
            return;
        }
        if (key == EventKeyboard::KeyCode::KEY_1) {
            selectBrush(TERRAIN_GRASS);
        } else if (key == EventKeyboard::KeyCode::KEY_2) {
            selectBrush(TERRAIN_SOIL);
        } else if (key == EventKeyboard::KeyCode::KEY_3) {
            selectBrush(TERRAIN_WATER);
        } else if (key == EventKeyboard::KeyCode::KEY_Z && ctrlDown) {
            if (shiftDown) {
                redo();
            } else {
                undo();
            }
        } else if (key == EventKeyboard::KeyCode::KEY_Y && ctrlDown) {
            redo();
        } else if (key == EventKeyboard::KeyCode::KEY_C && ctrlDown) {
            // Clipboard only means something on the tabs that have a selection.
            if (currentMainTab == TAB_MOVE || currentMainTab == TAB_TRIGGER) {
                copySelectionToClipboard();
            }
        } else if (key == EventKeyboard::KeyCode::KEY_V && ctrlDown) {
            if (currentMainTab == TAB_MOVE || currentMainTab == TAB_TRIGGER) {
                pasteClipboardAtCursor();
            }
        } else if (key == EventKeyboard::KeyCode::KEY_DELETE) {
            if (currentMainTab == TAB_MOVE || currentMainTab == TAB_TRIGGER) {
                deleteSelection();
            }
        } else if (key == EventKeyboard::KeyCode::KEY_EQUAL ||
                   key == EventKeyboard::KeyCode::KEY_KP_PLUS) {
            Vec2 center = Director::getInstance()->getVisibleSize() / 2;
            applyMapZoom(mapScale + kMapScaleStep, center);
        } else if (key == EventKeyboard::KeyCode::KEY_MINUS ||
                   key == EventKeyboard::KeyCode::KEY_KP_MINUS) {
            Vec2 center = Director::getInstance()->getVisibleSize() / 2;
            applyMapZoom(mapScale - kMapScaleStep, center);
        }
    };
    keyListener->onKeyReleased = [this](EventKeyboard::KeyCode key, Event*) {
        if (key == EventKeyboard::KeyCode::KEY_CTRL || key == EventKeyboard::KeyCode::KEY_RIGHT_CTRL) {
            ctrlDown = false;
        } else if (key == EventKeyboard::KeyCode::KEY_SHIFT || key == EventKeyboard::KeyCode::KEY_RIGHT_SHIFT) {
            shiftDown = false;
        }
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = [this](EventMouse* evt) {
        if (isAnyModalOpen()) {
            return;
        }
        Vec2 pos = win32MouseWorldPos(evt);

        // Block map edits when clicking on UI buttons (z >= 10).
        // Menu callbacks fire on mouseUp; without this check, onMouseDown
        // would process a map cell at the button's screen position first.
        auto hitTestUI = [](cocos2d::Node* node, cocos2d::Vec2 wPos, auto& self) -> bool {
            if (!node->isVisible()) return false;
            if (auto* item = dynamic_cast<cocos2d::MenuItem*>(node)) {
                if (node->getParent()) {
                    Vec2 lp = node->getParent()->convertToNodeSpace(wPos);
                    if (item->getBoundingBox().containsPoint(lp)) return true;
                }
            }
            for (auto* child : node->getChildren()) {
                if (self(child, wPos, self)) return true;
            }
            return false;
        };
        for (auto* child : this->getChildren()) {
            if (child->getLocalZOrder() >= 10 && hitTestUI(child, pos, hitTestUI)) {
                return;
            }
        }

        int button = static_cast<int>(evt->getMouseButton());
        if (button == 0) {
            if (currentMainTab == TAB_MOVE || currentMainTab == TAB_TRIGGER) {
                // RTS-style selection on both tabs: drag on empty ground = band
                // select (Shift adds), click = select, double-click = select all
                // of the same type+side, click a selected unit = open the
                // (batch) property editor. The Move tab additionally drags the
                // grabbed unit - or the whole selection - to a new cell.
                int existingIdx = findObjectAtWorldPos(pos);
                if (existingIdx < 0) {
                    beginBandSelect(pos, shiftDown);
                    return;
                }
                if (handleSelectionClick(existingIdx)) {
                    return; // double-click / shift-toggle consumed the press
                }
                if (currentMainTab == TAB_MOVE) {
                    beginMoveDrag(pos);
                } else if (movePressOpensPanel) {
                    // Trigger tab has no dragging - open the editor right away.
                    if (selectedObjectIds.size() > 1) {
                        openEditUnitPanelForSelection();
                    } else {
                        openEditUnitPanel(existingIdx);
                    }
                }
                return;
            }
            if (currentTool == TOOL_TERRAIN) {
                isPainting = true;
                beginStroke(ACTION_TERRAIN);
                paintAtWorldPos(pos);
            } else {
                int existingIdx = findObjectAtWorldPos(pos);
                if (existingIdx >= 0 && ctrlDown) {
                    // Ctrl+click an existing unit opens its property editor instead
                    // of erasing it — plain click/drag still erases as before.
                    openEditUnitPanel(existingIdx);
                    return;
                }
                isPainting = true;
                dragEraseObjects = existingIdx >= 0;
                beginStroke(ACTION_OBJECT);
                objectToolAtWorldPos(pos);
            }
        } else if (button == 1) {
            isPanning = true;
            panStartViewPos = evt->getLocationInView();
            panStartLayerPos = mapRoot->getPosition();
        }
    };
    mouseListener->onMouseMove = [this](EventMouse* evt) {
        Vec2 pos = win32MouseWorldPos(evt);
        lastMouseWorldPos = pos;
        hasMouseWorldPos = true;
        updateHoverTooltip(pos);
        updateMouseCoordLabel(pos);
        if (isAnyModalOpen()) {
            return;
        }
        if (isBandSelecting) {
            updateBandSelect(pos);
        } else if (movingObjectIndex >= 0) {
            updateMoveDrag(pos);
        } else if (isPainting && currentTool == TOOL_TERRAIN) {
            paintAtWorldPos(pos);
        } else if (isPainting && currentTool == TOOL_OBJECT) {
            objectToolAtWorldPos(pos);
        } else if (isPanning) {
            Vec2 delta = evt->getLocationInView() - panStartViewPos;
            mapRoot->setPosition(panStartLayerPos + Vec2(delta.x, delta.y));
        }
    };
    mouseListener->onMouseUp = [this](EventMouse* evt) {
        int button = static_cast<int>(evt->getMouseButton());
        if (button == 0) {
            if (isBandSelecting) {
                endBandSelect(win32MouseWorldPos(evt));
            }
            if (movingObjectIndex >= 0) {
                endMoveDrag(win32MouseWorldPos(evt));
            }
            if (isPainting) {
                endStroke();
            }
            isPainting = false;
        } else if (button == 1) {
            isPanning = false;
        }
    };
    mouseListener->onMouseScroll = [this](EventMouse* evt) {
        if (isAnyModalOpen()) return;
        float scrollY = evt->getScrollY();
        if (fabsf(scrollY) < 0.01f) return;
        // GLFW on Win32 reports wheel-up as negative scrollY, so negate: wheel up = zoom in
        applyMapZoom(mapScale - scrollY * kMapScaleStep, win32MouseWorldPos(evt));
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);
#else
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [this](Touch* touch, Event*) {
        if (isAnyModalOpen()) {
            return false;
        }
        Vec2 pos = touch->getLocation();
        if (currentMainTab == TAB_MOVE) {
            // Drag moves the unit; endMoveDrag() turns a no-drag release
            // into an inspect/edit tap (same panel as the Trigger tab).
            // Touch has no multi-select - always allow the tap-to-edit path.
            movePressOpensPanel = true;
            beginMoveDrag(pos);
            return true;
        }
        if (currentMainTab == TAB_TRIGGER) {
            // The Trigger tab only lets you inspect/edit placed units (e.g. to
            // read a unit's id for a trigger target) — it never paints or places.
            int existingIdx = findObjectAtWorldPos(pos);
            if (existingIdx >= 0) {
                openEditUnitPanel(existingIdx);
            }
            return true;
        }
        if (currentTool == TOOL_TERRAIN) {
            beginStroke(ACTION_TERRAIN);
            paintAtWorldPos(pos);
        } else {
            dragEraseObjects = findObjectAtWorldPos(pos) >= 0;
            beginStroke(ACTION_OBJECT);
            objectToolAtWorldPos(pos);
        }
        return true;
    };
    touchListener->onTouchMoved = [this](Touch* touch, Event*) {
        if (currentMainTab == TAB_MOVE) {
            updateMoveDrag(touch->getLocation());
            return;
        }
        if (currentMainTab == TAB_TRIGGER) {
            // Trigger tab never paints — without this guard a drag that began
            // as an inspect-tap would fall through and paint terrain/objects.
            return;
        }
        if (currentTool == TOOL_TERRAIN) {
            paintAtWorldPos(touch->getLocation());
        } else {
            objectToolAtWorldPos(touch->getLocation());
        }
    };
    touchListener->onTouchEnded = [this](Touch* touch, Event*) {
        if (currentMainTab == TAB_MOVE) {
            endMoveDrag(touch->getLocation());
            return;
        }
        endStroke();
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
#endif
}
