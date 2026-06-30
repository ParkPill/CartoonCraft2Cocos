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
#include <cstring>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <commdlg.h>
#include "platform/desktop/CCGLViewImpl-desktop.h"
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

    const char* const BTN_SKIN_NORMAL = "btnGray.png";
    const char* const BTN_SKIN_SELECTED = "btnGreen.png";
    // btnGray.png/btnGreen.png are 267x236 with a thick hand-drawn border; this
    // inset keeps the border/corner art intact while 9-slice-stretching the middle.
    const Rect BTN_SKIN_CAP_INSETS(50, 50, 167, 136);

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
        {"Flag", "redFlag0.png", MapEditor::RACE_NEUTRAL, -1, "Trigger target marker only - not a unit, no HP"},
        // Heroes — Spine-animated gacha units (Movable.h UNIT_HERO_*). HP/ATT
        // scale with Level instead of a flat baseHp; see ObjectTypeDef comments
        // and GameManager::getSpineFileName/getHeroSpine, which this mirrors.
        // "Hero " prefix disambiguates from same-named regular units (Archer, Wizard).
        {"Hero Orc", "orc", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Goblin", "goblin", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Spearman", "spearMan", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Lizardman", "lizard", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Archer", "archer", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Werewolf", "werewolf", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Monk", "monk", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Fighter", "fighter", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Bear", "bear", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Healer", "healer", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Knight", "knight", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Elf Swordsman", "elfSwordMan", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Assassin", "assassin", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Lion", "lion", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Wizard", "wizard", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Tanker", "tanker", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Skeleton", "skeleton", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Reaper", "necromancer", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Ent", "ent", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Salamander", "salamander", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        {"Hero Undine", "undine", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true},
        // The 3 "crazy" variants reuse a base skeleton with a different skin.
        {"Hero Crazy Werewolf", "werewolf", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "crazy_werewolf"},
        {"Hero Crazy Bear", "bear", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "crazy_bear"},
        {"Hero Crazy Lion", "lion", MapEditor::RACE_HERO, -1, kHeroTooltip, false, true, "crazy_lion"},
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

    const char* const kConditionTypeNames[] = {"Always", "Elapsed Time", "Switch", "Unit Count", "Resource"};
    const char* const kTriggerActionTypeNames[] = {
        "Display Message", "Create Unit", "Remove Unit", "Set Switch",
        "Victory", "Defeat", "Wait", "Center Camera", "Talk", "Reveal Fog"
    };
    const char* const kComparisonNames[] = {"At Least", "At Most", "Exactly"};
    const char* const kSwitchStateNames[] = {"Set", "Cleared"};
    const char* const kSwitchActionNames[] = {"Set", "Clear", "Toggle"};
    const char* const kResourceKindNames[] = {"Gold", "Tree"};

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

        Label* lblNormal = LM->getLocalizedLabel(text.c_str(), Color4B::WHITE, 24);
        lblNormal->setPosition(Vec2(width / 2, height / 2));
        normal->addChild(lblNormal);

        Label* lblSelected = LM->getLocalizedLabel(text.c_str(), Color4B::WHITE, 24);
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

        Sprite* iconNormal = Sprite::createWithSpriteFrameName(spriteFrameName);
        Sprite* iconSelected = Sprite::createWithSpriteFrameName(spriteFrameName);
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
        if (!spineSkin.empty()) {
            spt->setSkin(spineSkin);
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
    setupTriggerToolsRow();
    setupMainTabs();
    setupModalDimmer();
    setupNewMapPanel();
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

    if (tileBatch) {
        tileBatch->removeFromParent();
        tileBatch = nullptr;
    }
    if (objectRoot) {
        objectRoot->removeFromParent();
        objectRoot = nullptr;
    }
    placedObjects.clear();
    nextObjectId = 1;
    triggers.clear();
    selectedTriggerIndex = -1;
    currentFilePath.clear();
    isDirty = false;
    clearUndoHistory();
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

    objectRoot = Node::create();
    mapRoot->addChild(objectRoot, 1);
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

Node* MapEditor::spawnObjectAt(int x, int y, int typeIndex, int side, int hp, int id, int level) {
    Vec2 pos = Vec2(x * MAP_TILE_SIZE + MAP_TILE_SIZE / 2.0f, -(y * MAP_TILE_SIZE) - MAP_TILE_SIZE / 2.0f);
    const ObjectTypeDef& def = kObjectTypes[typeIndex];
    Node* spt;
    if (def.isHero) {
        spine::SkeletonAnimation* hero = createHeroSpine(def.spriteFrame, def.spineSkin);
        hero->setScale(0.5f); // matches real in-game hero scale, Movable.cpp
        spt = hero;
    } else {
        Sprite* regular = Sprite::createWithSpriteFrameName(def.spriteFrame);
        regular->setScale(OBJECT_SPRITE_SCALE);
        spt = regular;
    }
    spt->setPosition(pos);
    spt->setColor(colorForSide(side));
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
    int idx = findObjectAt(change.x, change.y);

    if (shouldHave) {
        if (idx >= 0) {
            removeObjectAtIndex(idx);
        }
        spawnObjectAt(change.x, change.y, type, side, hp, -1, level);
    } else if (idx >= 0) {
        removeObjectAtIndex(idx);
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
    setStatus("Undo");
}

void MapEditor::redo() {
    if (redoStack.empty()) {
        return;
    }
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
    return isNewMapPanelOpen || isEditUnitPanelOpen || isTriggerPanelOpen ||
           isConditionEditPanelOpen || isActionEditPanelOpen;
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
    MenuItemSprite* itemSaveAs = createTextButton("Save As", fileBtnW, fileBtnH, [this](Ref*) { saveMapAs(); });
    MenuItemSprite* itemLoad = createTextButton("Load", fileBtnW, fileBtnH, [this](Ref*) { loadMap(); });
    MenuItemSprite* itemRun = createTextButton("Run", fileBtnW, fileBtnH, [this](Ref*) { runMap(); });
    itemRun->getNormalImage()->setColor(Color3B(120, 220, 120));

    itemNew->setPosition(Vec2(0, 0));
    itemSave->setPosition(Vec2(120, 0));
    itemSaveAs->setPosition(Vec2(240, 0));
    itemLoad->setPosition(Vec2(360, 0));
    itemRun->setPosition(Vec2(480, 0));

    Menu* fileMenu = Menu::create(itemNew, itemSave, itemSaveAs, itemLoad, itemRun, nullptr);
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
    objectPaletteRow->setPosition(Vec2(80, visibleSize.height - 165));
    unitToolsRow->addChild(objectPaletteRow);

    Label* paletteLabel = LM->getLocalizedLabel("Objects:", Color4B::WHITE, 22);
    paletteLabel->setAnchorPoint(Vec2(0, 0.5f));
    paletteLabel->setPosition(Vec2(20, visibleSize.height - 165));
    unitToolsRow->addChild(paletteLabel);

    // Only relevant for the Heroes tab (45 entries don't fit one unscrolled
    // row) - hidden for every other race. Sits to the right of the icon row.
    heroPageRow = addCycleRow(unitToolsRow, "Page:", 1050, visibleSize.height - 165, &lblHeroPage,
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
        if (kObjectTypes[i].race == currentRaceTab) {
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
        if (kObjectTypes[i].race == RACE_HERO) {
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
    sideMenu->setPosition(Vec2(110, rowY));
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

    MenuItemSprite* itemDefaultHp = createTextButton("Default HP", 130, sideBtnH, [this](Ref*) {
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

void MapEditor::updatePaletteTooltip(const Vec2& worldPos) {
    if (!paletteTooltip) {
        return;
    }
    if (currentMainTab != TAB_UNITS || isAnyModalOpen()) {
        paletteTooltip->setVisible(false);
        return;
    }
    for (size_t slot = 0; slot < currentRaceUnitIndices.size(); slot++) {
        Rect rect = paletteSlotWorldRect(static_cast<int>(slot));
        if (!rect.containsPoint(worldPos)) {
            continue;
        }
        paletteTooltipLabel->setString(paletteTooltipText(currentRaceUnitIndices[slot]));

        const float paddingX = 12;
        const float paddingY = 10;
        Size textSize = paletteTooltipLabel->getContentSize();
        Size bgSize(textSize.width + paddingX * 2, textSize.height + paddingY * 2);

        paletteTooltipBg->clear();
        paletteTooltipBg->drawSolidRect(Vec2::ZERO, Vec2(bgSize.width, bgSize.height), Color4F(0, 0, 0, 0.85f));
        paletteTooltipLabel->setPosition(Vec2(paddingX, bgSize.height - paddingY));

        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 pos(rect.origin.x, rect.getMaxY() + 8);
        pos.x = std::max(4.0f, std::min(pos.x, visibleSize.width - bgSize.width - 4.0f));
        pos.y = std::min(pos.y, visibleSize.height - bgSize.height - 4.0f);

        paletteTooltip->setPosition(pos);
        paletteTooltip->setVisible(true);
        return;
    }
    paletteTooltip->setVisible(false);
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

void MapEditor::setupMainTabs() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const char* const kMainTabNames[] = {"Terrain", "Units", "Trigger"};
    const int kMainTabCount = 3;
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
    if (paletteTooltip) {
        paletteTooltip->setVisible(false);
    }

    // Switching into Terrain/Units implies "I want to use this tool" — arm it,
    // matching the existing per-race tab behavior. Trigger has no default tool
    // (Open Trigger Editor isn't a placement tool; Flag must be picked explicitly).
    if (tab == TAB_TERRAIN) {
        selectBrush(currentBrush);
    } else if (tab == TAB_UNITS) {
        selectRace(currentRaceTab);
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
    hideNewMapPanel();
    setStatus(StringUtils::format("New map %dx%d", mapWidth, mapHeight));
}

void MapEditor::setupEditUnitPanel() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    const float panelWidth = 620;
    const float panelHeight = 260;

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
    rowEditHp->setVisible(!isHero);
    rowEditLevel->setVisible(isHero);
    tfEditLevel->setString(StringUtils::format("%d", obj.level));

    editUnitPanel->setVisible(true);
    isEditUnitPanelOpen = true;
    setModalDimmerVisible(true);
}

void MapEditor::closeEditUnitPanel() {
    editUnitPanel->setVisible(false);
    isEditUnitPanelOpen = false;
    editingObjectIndex = -1;
    setModalDimmerVisible(false);
}

void MapEditor::selectEditSide(int side) {
    editPanelSide = side;
    highlightGroup(editSideButtonBgs, editPanelSide);
}

void MapEditor::onConfirmEditUnit() {
    if (editingObjectIndex < 0 || editingObjectIndex >= static_cast<int>(placedObjects.size())) {
        closeEditUnitPanel();
        return;
    }
    PlacedObject& obj = placedObjects[editingObjectIndex];

    std::string hpText = tfEditHp->getString();
    int hp = (hpText.empty() || hpText == "default") ? -1 : atoi(hpText.c_str());
    if (hp <= 0) {
        hp = -1;
    }
    // Untouched, the field just displays this unit's base HP (not an explicit
    // override) — keep storing the -1 sentinel so future balance changes to
    // the unit's base HP still apply to it.
    if (hp == defaultHpForType(obj.typeIndex)) {
        hp = -1;
    }

    obj.side = editPanelSide;
    obj.hp = hp;
    if (kObjectTypes[obj.typeIndex].isHero) {
        obj.level = std::max(1, atoi(tfEditLevel->getString().c_str()));
    }
    obj.sprite->setColor(colorForSide(obj.side));
    isDirty = true;
    setStatus(StringUtils::format("Updated %s at (%d,%d)", kObjectTypes[obj.typeIndex].name, obj.tileX, obj.tileY));
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
            writer.Key("count");
            writer.Int(a.count);
            writer.Key("switchIndex");
            writer.Int(a.switchIndex);
            writer.Key("switchAction");
            writer.Int(a.switchAction);
            writer.Key("waitSeconds");
            writer.Double(a.waitSeconds);
            writer.Key("visionEnabled");
            writer.Bool(a.visionEnabled);
            writer.EndObject();
        }
        writer.EndArray();
        writer.EndObject();
    }
    writer.EndArray();

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
            spawnObjectAt(x, y, typeIndex, side, hp, id, level);
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
                    if (aitem.HasMember("visionEnabled")) {
                        a.visionEnabled = aitem["visionEnabled"].GetBool();
                    }
                    t.actions.push_back(a);
                }
            }
            triggers.push_back(t);
        }
    }
    if (triggerPanel) {
        refreshTriggerList();
        refreshTriggerDetail();
    }

    return true;
}

void MapEditor::saveMap() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
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
    const float valueWidth = 260;
    const float leftX = 180;
    const float valueX = leftX + arrowSize / 2 + 10 + valueWidth / 2;
    const float rightX = valueX + valueWidth / 2 + 10 + arrowSize / 2;

    MenuItemSprite* leftBtn = createTextButton("<", arrowSize, arrowSize, [onCycle](Ref*) { onCycle(-1); });
    leftBtn->setPosition(Vec2(leftX, 0));
    MenuItemSprite* rightBtn = createTextButton(">", arrowSize, arrowSize, [onCycle](Ref*) { onCycle(1); });
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

    Label* title = LM->getLocalizedLabel("Trigger Editor", Color4B::WHITE, 30);
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

    MenuItemSprite* itemAddTrigger = createTextButton("+ Add", 90, 44, [this](Ref*) { addTrigger(); });
    itemAddTrigger->setPosition(Vec2(listX + listWidth - 135, panelHeight - 80));
    MenuItemSprite* itemDeleteTrigger = createTextButton("Delete", 90, 44, [this](Ref*) { deleteSelectedTrigger(); });
    itemDeleteTrigger->setPosition(Vec2(listX + listWidth - 45, panelHeight - 80));
    Menu* listBtnMenu = Menu::create(itemAddTrigger, itemDeleteTrigger, nullptr);
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
    switch (a.type) {
        case TACT_DISPLAY_MESSAGE:
            return StringUtils::format("Display message: \"%s\"", a.message.c_str());
        case TACT_CREATE_UNIT: {
            std::string at = a.targetObjectId >= 0 ? describeTargetObject(a.targetObjectId) : StringUtils::format("(%d,%d)", a.tileX, a.tileY);
            return StringUtils::format("Create %d %s for %s at %s", a.count, unitTypeCycleName(a.unitTypeIndex).c_str(),
                                        kSideNames[a.unitSide], at.c_str());
        }
        case TACT_REMOVE_UNIT:
            return StringUtils::format("Remove %s units for %s", unitTypeCycleName(a.unitTypeIndex).c_str(), kSideNames[a.unitSide]);
        case TACT_SET_SWITCH:
            return StringUtils::format("%s switch %d", kSwitchActionNames[a.switchAction], a.switchIndex + 1);
        case TACT_VICTORY:
            return "Victory";
        case TACT_DEFEAT:
            return "Defeat";
        case TACT_WAIT:
            return StringUtils::format("Wait %.1f sec", a.waitSeconds);
        case TACT_CENTER_CAMERA:
            return a.targetObjectId >= 0 ? StringUtils::format("Center camera at %s", describeTargetObject(a.targetObjectId).c_str())
                                          : StringUtils::format("Center camera at (%d,%d)", a.tileX, a.tileY);
        case TACT_TALK: {
            std::string at = a.targetObjectId >= 0 ? describeTargetObject(a.targetObjectId) : StringUtils::format("(%d,%d)", a.tileX, a.tileY);
            return StringUtils::format("Talk at %s: \"%s\"", at.c_str(), a.message.c_str());
        }
        case TACT_REVEAL_FOG: {
            std::string at = a.targetObjectId >= 0 ? describeTargetObject(a.targetObjectId) : StringUtils::format("(%d,%d)", a.tileX, a.tileY);
            return StringUtils::format("Reveal fog %s at %s r=%d", a.visionEnabled ? "ON" : "OFF", at.c_str(), a.count);
        }
        default:
            return "Unknown action";
    }
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
    const float rowH = 40;
    const float rowWidth = 760;

    for (size_t i = 0; i < t.conditions.size(); i++) {
        int idx = static_cast<int>(i);
        float y = -static_cast<float>(i) * rowH - rowH / 2;

        Label* lbl = LM->getLocalizedLabel(describeCondition(t.conditions[i]).c_str(), Color4B::WHITE, 20);
        lbl->setAnchorPoint(Vec2(0, 0.5f));
        lbl->setPosition(Vec2(0, y));
        conditionListContainer->addChild(lbl);

        MenuItemSprite* itemEdit = createTextButton("Edit", 70, rowH - 6, [this, idx](Ref*) { openConditionEditPanel(idx); });
        itemEdit->setPosition(Vec2(rowWidth - 90, y));
        MenuItemSprite* itemDel = createTextButton("X", 40, rowH - 6, [this, idx](Ref*) { deleteConditionAtIndex(idx); });
        itemDel->setPosition(Vec2(rowWidth - 20, y));
        Menu* menu = Menu::create(itemEdit, itemDel, nullptr);
        menu->setPosition(Vec2::ZERO);
        conditionListContainer->addChild(menu);
    }

    for (size_t i = 0; i < t.actions.size(); i++) {
        int idx = static_cast<int>(i);
        float y = -static_cast<float>(i) * rowH - rowH / 2;

        Label* lbl = LM->getLocalizedLabel(describeAction(t.actions[i]).c_str(), Color4B::WHITE, 20);
        lbl->setAnchorPoint(Vec2(0, 0.5f));
        lbl->setPosition(Vec2(0, y));
        actionListContainer->addChild(lbl);

        MenuItemSprite* itemEdit = createTextButton("Edit", 70, rowH - 6, [this, idx](Ref*) { openActionEditPanel(idx); });
        itemEdit->setPosition(Vec2(rowWidth - 90, y));
        MenuItemSprite* itemDel = createTextButton("X", 40, rowH - 6, [this, idx](Ref*) { deleteActionAtIndex(idx); });
        itemDel->setPosition(Vec2(rowWidth - 20, y));
        Menu* menu = Menu::create(itemEdit, itemDel, nullptr);
        menu->setPosition(Vec2::ZERO);
        actionListContainer->addChild(menu);
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

std::vector<std::pair<int, std::string>> MapEditor::buildTargetList() const {
    std::vector<std::pair<int, std::string>> list;
    list.push_back({-1, "Manual (Tile X/Y)"});
    for (const PlacedObject& obj : placedObjects) {
        if (strcmp(kObjectTypes[obj.typeIndex].name, "Tree") == 0) continue;
        list.push_back({obj.id, StringUtils::format("%s #%d", kObjectTypes[obj.typeIndex].name, obj.id)});
    }
    return list;
}

void MapEditor::openTargetDropdown() {
    if (isTargetDropdownOpen) {
        closeTargetDropdown();
        return;
    }
    targetDropdownList = buildTargetList();
    targetDropdownScroll = 0;
    for (int i = 0; i < (int)targetDropdownList.size(); i++) {
        if (targetDropdownList[i].first == actionDraft.targetObjectId) {
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

void MapEditor::selectTargetObject(int id) {
    actionDraft.targetObjectId = id;
    refreshActionEditPanel();
    closeTargetDropdown();
}

void MapEditor::rebuildTargetDropdown() {
    if (targetDropdown) {
        targetDropdown->removeFromParent();
        targetDropdown = nullptr;
    }
    if (!isTargetDropdownOpen || !actionEditPanel || !actionEditPanel->isVisible()) return;

    const int kMaxVis = 8;
    const float itemH = 36.0f;
    const float dropW = 400.0f;
    const float padV = 6.0f;
    const float scrollBtnH = 30.0f;
    int total = (int)targetDropdownList.size();
    bool needScroll = total > kMaxVis;
    int visCount = std::min(total - targetDropdownScroll, kMaxVis);
    float dropH = padV * 2 + visCount * itemH + (needScroll ? scrollBtnH * 2.0f : 0.0f);

    // Determine panel-relative y of the active "At:" row center
    // actionEditPanel height=560, groupY=400 (panelH-160)
    const float panelH = 560.0f;
    const float groupY = panelH - 160.0f;
    float atRowPanelY = (actionDraft.type == TACT_CREATE_UNIT) ? (groupY - 100.0f) : groupY;

    // Try below the "At:" row; if it clips the panel bottom, show above instead
    float dropBottom = atRowPanelY - 22.0f - dropH;
    if (dropBottom < 5.0f) {
        dropBottom = atRowPanelY + 22.0f;
        if (dropBottom + dropH > panelH - 5.0f) dropBottom = panelH - 5.0f - dropH;
    }
    if (dropBottom < 5.0f) dropBottom = 5.0f;

    targetDropdown = Node::create();
    targetDropdown->setPosition(Vec2(180.0f, dropBottom));
    actionEditPanel->addChild(targetDropdown, 10);

    DrawNode* bg = DrawNode::create();
    bg->drawSolidRect(Vec2::ZERO, Vec2(dropW, dropH), Color4F(0.07f, 0.07f, 0.13f, 0.97f));
    bg->drawRect(Vec2::ZERO, Vec2(dropW, dropH), Color4F(0.45f, 0.45f, 0.65f, 1.0f));
    targetDropdown->addChild(bg);

    Vector<MenuItem*> items;

    if (needScroll) {
        bool canUp = targetDropdownScroll > 0;
        Color4B col = canUp ? Color4B(180, 180, 255, 255) : Color4B(90, 90, 110, 255);
        MenuItemLabel* upBtn = MenuItemLabel::create(
            LM->getLocalizedLabel("^ Up", col, 18),
            [this](Ref*) {
                if (targetDropdownScroll > 0) --targetDropdownScroll;
                this->scheduleOnce([this](float){ rebuildTargetDropdown(); }, 0.0f, "dd_scroll");
            }
        );
        upBtn->setEnabled(canUp);
        upBtn->setPosition(Vec2(dropW / 2, dropH - padV - scrollBtnH / 2));
        items.pushBack(upBtn);
    }

    float curY = dropH - padV - (needScroll ? scrollBtnH : 0.0f) - itemH / 2;
    for (int i = 0; i < visCount; i++) {
        int idx = targetDropdownScroll + i;
        int capturedId = targetDropdownList[idx].first;
        std::string text = targetDropdownList[idx].second;
        bool isCurrent = (capturedId == actionDraft.targetObjectId);
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
            LM->getLocalizedLabel("v Down", col, 18),
            [this, total](Ref*) {
                if (targetDropdownScroll + 8 < total) ++targetDropdownScroll;
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
    addCycleRow(rowCondUnitCount, "Unit Type:", 30, -50, &lblCondUnitType, [this](int dir) { cycleConditionUnitType(dir); });
    addCycleRow(rowCondUnitCount, "Comparison:", 30, -100, &lblCondComparison, [this](int dir) { cycleConditionComparison(dir); });
    addTextFieldRow(rowCondUnitCount, "Amount:", 30, -150, 150, "1", &tfCondUnitAmount);

    rowCondResource = Node::create();
    rowCondResource->setPosition(Vec2(0, groupY));
    conditionEditPanel->addChild(rowCondResource);
    addCycleRow(rowCondResource, "Resource:", 30, 0, &lblCondResource, [this](int dir) { cycleConditionResourceKind(dir); });
    addCycleRow(rowCondResource, "Comparison:", 30, -50, &lblCondResourceComparison, [this](int dir) { cycleConditionResourceComparison(dir); });
    addTextFieldRow(rowCondResource, "Amount:", 30, -100, 150, "0", &tfCondResourceAmount);

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
    editingConditionIndex = conditionIndex;
    const Trigger& t = triggers[selectedTriggerIndex];
    if (conditionIndex >= 0 && conditionIndex < static_cast<int>(t.conditions.size())) {
        conditionDraft = t.conditions[conditionIndex];
    } else {
        conditionDraft = TriggerCondition();
    }

    tfCondElapsedSeconds->setString(StringUtils::format("%d", conditionDraft.elapsedSeconds));
    tfCondUnitAmount->setString(StringUtils::format("%d", conditionDraft.amount));
    tfCondResourceAmount->setString(StringUtils::format("%d", conditionDraft.amount));
    refreshConditionEditPanel();

    conditionEditPanel->setVisible(true);
    isConditionEditPanelOpen = true;
    setModalDimmerVisible(true);
    setTriggerSubDimmerVisible(true);
}

void MapEditor::closeConditionEditPanel() {
    conditionEditPanel->setVisible(false);
    isConditionEditPanelOpen = false;
    setModalDimmerVisible(isTriggerPanelOpen);
    setTriggerSubDimmerVisible(isActionEditPanelOpen);
}

void MapEditor::cycleConditionType(int dir) {
    conditionDraft.type = static_cast<TriggerConditionType>(cyclicAdd(conditionDraft.type, dir, 0, COND_TYPE_COUNT - 1));
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionSide(int dir) {
    conditionDraft.unitSide = cyclicAdd(conditionDraft.unitSide, dir, 0, kSideCount - 1);
    refreshConditionEditPanel();
}

void MapEditor::cycleConditionUnitType(int dir) {
    conditionDraft.unitTypeIndex = cyclicAdd(conditionDraft.unitTypeIndex, dir, -1, kObjectTypeCount - 1);
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

    lblCondSwitchIndex->setString(StringUtils::format("%d", conditionDraft.switchIndex + 1));
    lblCondSwitchState->setString(kSwitchStateNames[conditionDraft.switchState]);
    lblCondSide->setString(kSideNames[conditionDraft.unitSide]);
    lblCondUnitType->setString(unitTypeCycleName(conditionDraft.unitTypeIndex));
    lblCondComparison->setString(kComparisonNames[conditionDraft.comparison]);
    lblCondResource->setString(kResourceKindNames[conditionDraft.resourceKind]);
    lblCondResourceComparison->setString(kComparisonNames[conditionDraft.comparison]);
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
    addCycleRow(rowActCreateUnit, "Unit Type:", 30, -50, &lblActCreateUnitType, [this](int dir) { cycleActionCreateUnitType(dir); });
    // addTargetOverlay: attaches a transparent-but-clickable MenuItemSprite over the
    // cycle-row value label so that clicking the text opens the target dropdown.
    // The normal skin is opacity-0 (invisible, showing the label beneath), the
    // selected skin flashes green as press feedback.
    auto addTargetOverlay = [&](Node* row) {
        const float arrowSz = 34.0f, valW = 260.0f, leftX = 180.0f;
        const float valX = leftX + arrowSz / 2 + 10 + valW / 2;
        Scale9Sprite* n = makeButtonSkin(BTN_SKIN_NORMAL, Size(valW, arrowSz + 4));
        n->setOpacity(0);
        Scale9Sprite* s = makeButtonSkin(BTN_SKIN_SELECTED, Size(valW, arrowSz + 4));
        MenuItemSprite* btn = MenuItemSprite::create(n, s, [this](Ref*) {
            this->scheduleOnce([this](float){ openTargetDropdown(); }, 0.0f, "dd_open");
        });
        btn->setPosition(Vec2(valX, 0));
        Menu* m = Menu::create(btn, nullptr);
        m->setPosition(Vec2::ZERO);
        row->addChild(m);
    };
    addTargetOverlay(addCycleRow(rowActCreateUnit, "At:", 30, -100, &lblActCreateTarget, [this](int dir) { cycleActionCreateTarget(dir); }));
    rowActCreateX = addTextFieldRow(rowActCreateUnit, "Tile X:", 30, -150, 150, "0", &tfActCreateX);
    rowActCreateY = addTextFieldRow(rowActCreateUnit, "Tile Y:", 30, -200, 150, "0", &tfActCreateY);
    addTextFieldRow(rowActCreateUnit, "Count:", 30, -250, 150, "1", &tfActCreateCount);

    rowActRemoveUnit = Node::create();
    rowActRemoveUnit->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActRemoveUnit);
    addCycleRow(rowActRemoveUnit, "Side:", 30, 0, &lblActRemoveSide, [this](int dir) { cycleActionRemoveSide(dir); });
    addCycleRow(rowActRemoveUnit, "Unit Type:", 30, -50, &lblActRemoveUnitType, [this](int dir) { cycleActionRemoveUnitType(dir); });

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
    addTargetOverlay(addCycleRow(rowActCamera, "At:", 30, 0, &lblActCameraTarget, [this](int dir) { cycleActionCameraTarget(dir); }));
    rowActCameraX = addTextFieldRow(rowActCamera, "Tile X:", 30, -50, 150, "0", &tfActCameraX);
    rowActCameraY = addTextFieldRow(rowActCamera, "Tile Y:", 30, -100, 150, "0", &tfActCameraY);

    rowActTalk = Node::create();
    rowActTalk->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActTalk);
    addTargetOverlay(addCycleRow(rowActTalk, "At:", 30, 0, &lblActTalkTarget, [this](int dir) { cycleActionTalkTarget(dir); }));
    rowActTalkX = addTextFieldRow(rowActTalk, "Tile X:", 30, -50, 150, "0", &tfActTalkX);
    rowActTalkY = addTextFieldRow(rowActTalk, "Tile Y:", 30, -100, 150, "0", &tfActTalkY);
    addTextFieldRow(rowActTalk, "Message:", 30, -150, 300, "", &tfActTalkMessage);

    rowActRevealFog = Node::create();
    rowActRevealFog->setPosition(Vec2(0, groupY));
    actionEditPanel->addChild(rowActRevealFog);
    addCycleRow(rowActRevealFog, "Vision:", 30, 0, &lblActRevealFogEnabled, [this](int dir) { cycleActionRevealFogEnabled(dir); });
    addTargetOverlay(addCycleRow(rowActRevealFog, "At:", 30, -50, &lblActRevealFogTarget, [this](int dir) { cycleActionRevealFogTarget(dir); }));
    rowActRevealFogX = addTextFieldRow(rowActRevealFog, "Tile X:", 30, -100, 150, "0", &tfActRevealFogX);
    rowActRevealFogY = addTextFieldRow(rowActRevealFog, "Tile Y:", 30, -150, 150, "0", &tfActRevealFogY);
    addTextFieldRow(rowActRevealFog, "Radius (fog):", 30, -200, 150, "5", &tfActRevealFogRadius);

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
    int revealRadius = (actionDraft.type == TACT_REVEAL_FOG) ? std::max(1, actionDraft.count) : 5;
    tfActRevealFogX->setString(StringUtils::format("%d", actionDraft.tileX));
    tfActRevealFogY->setString(StringUtils::format("%d", actionDraft.tileY));
    tfActRevealFogRadius->setString(StringUtils::format("%d", revealRadius));
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
    } else if (a.type == TACT_REVEAL_FOG) {
        if (a.targetObjectId < 0) {
            a.tileX = atoi(tfActRevealFogX->getString().c_str());
            a.tileY = atoi(tfActRevealFogY->getString().c_str());
        }
        a.count = std::max(1, atoi(tfActRevealFogRadius->getString().c_str()));
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
            if (currentMainTab == TAB_TRIGGER) {
                // The Trigger tab only lets you inspect/edit placed units (e.g. to
                // read a unit's id for a trigger target) — it never paints or places.
                int existingIdx = findObjectAtWorldPos(pos);
                if (existingIdx >= 0) {
                    openEditUnitPanel(existingIdx);
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
        updatePaletteTooltip(pos);
        if (isAnyModalOpen()) {
            return;
        }
        if (isPainting && currentTool == TOOL_TERRAIN) {
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
        // scroll up (scrollY > 0) = zoom in
        applyMapZoom(mapScale + scrollY * kMapScaleStep, win32MouseWorldPos(evt));
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);
#else
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [this](Touch* touch, Event*) {
        if (isAnyModalOpen()) {
            return false;
        }
        Vec2 pos = touch->getLocation();
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
        if (currentTool == TOOL_TERRAIN) {
            paintAtWorldPos(touch->getLocation());
        } else {
            objectToolAtWorldPos(touch->getLocation());
        }
    };
    touchListener->onTouchEnded = [this](Touch* touch, Event*) {
        endStroke();
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
#endif
}
