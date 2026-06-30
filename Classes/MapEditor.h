#ifndef __map_editor_scene__
#define __map_editor_scene__

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIScale9Sprite.h"
#include <vector>
#include <functional>
#include <utility>

// Standalone terrain editor. Deliberately has no dependency on Movable /
// EnemyBase / HelloWorldScene — it only paints terrain and places object
// markers, it does not simulate gameplay.
class MapEditor : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
    virtual bool init() override;
    CREATE_FUNC(MapEditor);

    // Public so the file-scope unit roster table (kObjectTypes) can tag each
    // entry by race without needing a parallel set of free constants.
    enum UnitRace {
        RACE_HUMAN = 0,
        RACE_ORC = 1,
        RACE_UNDEAD = 2,
        RACE_NEUTRAL = 3,
        // Gacha-style RPG heroes (UNIT_HERO_* in Movable.h) - Spine-animated,
        // not race-aligned army units, so they get their own tab rather than
        // being folded into Human/Orc/etc.
        RACE_HERO = 4
    };

private:
    enum TerrainType {
        TERRAIN_GRASS = 0,
        TERRAIN_SOIL = 1,
        TERRAIN_WATER = 2
    };

    enum ToolMode {
        TOOL_TERRAIN = 0,
        TOOL_OBJECT = 1
    };

    // The three top-level toolbar tabs. Each owns a row of controls
    // (terrainToolsRow / unitToolsRow / triggerToolsRow) that's shown only
    // while its tab is active; file ops (New/Save/Load) stay visible always.
    enum MainTab {
        TAB_TERRAIN = 0,
        TAB_UNITS = 1,
        TAB_TRIGGER = 2
    };

    enum ActionType {
        ACTION_TERRAIN = 0,
        ACTION_OBJECT = 1
    };

    // Relationship to the player at map start. The two neutral kinds exist
    // because "neutral" covers two different design intents: a unit/building
    // that joins whoever walks up to it, vs. one that only changes hands
    // through a scripted event/trigger.
    enum UnitSide {
        SIDE_ALLY = 0,
        SIDE_ENEMY = 1,
        SIDE_NEUTRAL_APPROACH = 2,
        SIDE_NEUTRAL_EVENT = 3
    };

    struct PlacedObject {
        int typeIndex;
        int tileX;
        int tileY;
        // Sprite* for regular units, but a spine::SkeletonAnimation* (which
        // extends Node, not Sprite) for hero entries - Node is the common
        // base every existing use of this field (getBoundingBox/removeFromParent/
        // setColor) already only needs.
        cocos2d::Node* sprite;
        int side;
        int hp; // -1 = use the unit's default HP
        int id; // stable unique id, assigned on placement and kept across save/load
        int level; // heroes only - HP/ATT scale with level; ignored by regular units
    };

    // StarCraft-style trigger system: a Trigger fires its Actions once all of its
    // Conditions hold true. Authoring only, here — MapEditor builds and saves the
    // trigger data, but does not evaluate it (see class comment); a future runtime
    // interpreter in HelloWorldScene reads the same JSON shape and executes it.
    enum TriggerConditionType {
        COND_ALWAYS = 0,
        COND_ELAPSED_TIME,
        COND_SWITCH,
        COND_UNIT_COUNT,
        COND_RESOURCE,
        COND_TYPE_COUNT
    };

    enum TriggerActionType {
        TACT_DISPLAY_MESSAGE = 0,
        TACT_CREATE_UNIT,
        TACT_REMOVE_UNIT,
        TACT_SET_SWITCH,
        TACT_VICTORY,
        TACT_DEFEAT,
        TACT_WAIT,
        TACT_CENTER_CAMERA,
        // Shows `message` in a speech bubble at the target's position (same
        // target-on-map selection as CENTER_CAMERA). A cutscene is just an
        // ordered run of CENTER_CAMERA/TALK actions within one trigger - the
        // existing action list is already executed in order, so no separate
        // "scenario" container is needed.
        TACT_TALK,
        // Permanently reveals (or re-hides) a circular fog area centred on the
        // target position, independent of unit presence. `visionEnabled` = true
        // enables the revelation; false cancels it. Radius is stored in `count`
        // (fog tiles, default 5). Position uses targetObjectId / tileX / tileY
        // identical to CENTER_CAMERA.
        TACT_REVEAL_FOG,
        TACT_TYPE_COUNT
    };

    enum TriggerComparison {
        CMP_AT_LEAST = 0,
        CMP_AT_MOST,
        CMP_EXACTLY,
        CMP_COUNT
    };

    enum TriggerSwitchState {
        SWITCH_STATE_SET = 0,
        SWITCH_STATE_CLEARED,
        SWITCH_STATE_COUNT
    };

    enum TriggerSwitchAction {
        SWITCH_ACTION_SET = 0,
        SWITCH_ACTION_CLEAR,
        SWITCH_ACTION_TOGGLE,
        SWITCH_ACTION_COUNT
    };

    enum TriggerResourceKind {
        RESOURCE_GOLD = 0,
        RESOURCE_TREE,
        RESOURCE_KIND_COUNT
    };

    static const int kTriggerSwitchCount = 16;
    static const int kTriggerSideCount = 4; // matches UnitSide / kSideCount

    struct TriggerCondition {
        TriggerConditionType type = COND_ALWAYS;
        int elapsedSeconds = 30;             // ELAPSED_TIME
        int switchIndex = 0;                 // SWITCH
        TriggerSwitchState switchState = SWITCH_STATE_SET; // SWITCH
        int unitSide = SIDE_ALLY;            // UNIT_COUNT
        int unitTypeIndex = -1;              // UNIT_COUNT; -1 = Any
        TriggerComparison comparison = CMP_AT_LEAST; // UNIT_COUNT, RESOURCE
        int amount = 1;                      // UNIT_COUNT, RESOURCE
        TriggerResourceKind resourceKind = RESOURCE_GOLD; // RESOURCE
        bool isRepeat = false; // only satisfied on repeated firings (trigger already fired once)
    };

    struct TriggerAction {
        TriggerActionType type = TACT_DISPLAY_MESSAGE;
        std::string message;                 // DISPLAY_MESSAGE, TALK
        int unitSide = SIDE_ALLY;            // CREATE_UNIT, REMOVE_UNIT
        int unitTypeIndex = -1;              // CREATE_UNIT, REMOVE_UNIT; -1 = Any (REMOVE_UNIT only)
        int tileX = 0;                       // CREATE_UNIT, CENTER_CAMERA, TALK, REVEAL_FOG; used when targetObjectId < 0
        int tileY = 0;                       // CREATE_UNIT, CENTER_CAMERA, TALK, REVEAL_FOG; used when targetObjectId < 0
        int targetObjectId = -1;             // CREATE_UNIT, CENTER_CAMERA, TALK, REVEAL_FOG; -1 = use tileX/tileY,
                                              // else the id of a placed flag/unit whose position to use
        int count = 1;                       // CREATE_UNIT; REVEAL_FOG: radius in fog tiles (default 5)
        int switchIndex = 0;                 // SET_SWITCH
        TriggerSwitchAction switchAction = SWITCH_ACTION_SET; // SET_SWITCH
        float waitSeconds = 1.0f;            // WAIT
        bool visionEnabled = true;           // REVEAL_FOG: true = reveal, false = cancel revelation
    };

    struct Trigger {
        std::string name = "Trigger";
        bool sides[4] = {true, false, false, false}; // which sides this trigger runs for
        bool preserve = true; // keep re-checking after firing, vs. run once then disable
        std::vector<TriggerCondition> conditions;
        std::vector<TriggerAction> actions;
    };

    struct TerrainCellChange {
        int x, y;
        int oldValue, newValue;
    };

    struct ObjectCellChange {
        int x, y;
        bool hadObject;
        int oldType;
        int oldSide;
        int oldHp;
        int oldLevel;
        bool hasObject;
        int newType;
        int newSide;
        int newHp;
        int newLevel;
    };

    struct UndoAction {
        ActionType type;
        std::vector<TerrainCellChange> terrainChanges;
        std::vector<ObjectCellChange> objectChanges;
    };

    std::string currentFilePath; // empty = new, never-saved map; set after a successful save/load

    int mapWidth = 40;
    int mapHeight = 40;
    std::vector<int> terrainGrid;
    std::vector<cocos2d::Sprite*> tileSprites;
    cocos2d::Node* mapRoot = nullptr;
    cocos2d::Node* tileBatch = nullptr;
    cocos2d::Node* objectRoot = nullptr;
    std::vector<PlacedObject> placedObjects;
    int nextObjectId = 1;
    cocos2d::Label* toolLabel = nullptr;
    cocos2d::Label* statusLabel = nullptr;
    int currentBrush = TERRAIN_SOIL;
    ToolMode currentTool = TOOL_TERRAIN;
    int currentObjectType = 0;
    int currentRaceTab = RACE_HUMAN;
    std::vector<int> currentRaceUnitIndices; // palette slot -> index into kObjectTypes
    int currentSide = SIDE_ALLY;
    // RACE_HERO has too many entries (45) to fit one unscrolled palette row -
    // paged instead, kHeroesPerPage at a time. Irrelevant for other races.
    int currentHeroPage = 0;

    cocos2d::Node* objectPaletteRow = nullptr;
    cocos2d::Node* heroPageRow = nullptr;
    cocos2d::Label* lblHeroPage = nullptr;
    std::vector<cocos2d::ui::Scale9Sprite*> terrainButtonBgs;
    std::vector<cocos2d::ui::Scale9Sprite*> raceTabBgs;
    std::vector<cocos2d::ui::Scale9Sprite*> objectButtonBgs;
    std::vector<cocos2d::ui::Scale9Sprite*> sideButtonBgs;
    cocos2d::ui::TextField* tfHp = nullptr;
    // Heroes only - HP/ATT scale with level. Hidden for regular units.
    cocos2d::Node* rowLevel = nullptr;
    cocos2d::ui::TextField* tfLevel = nullptr;

    // Hover tooltip for the Units tab palette icons (name + base HP + an
    // optional clarifying line — see ObjectTypeDef::tooltip). Win32 only;
    // touch platforms have no hover concept.
    cocos2d::Node* paletteTooltip = nullptr;
    cocos2d::DrawNode* paletteTooltipBg = nullptr;
    cocos2d::Label* paletteTooltipLabel = nullptr;

    int currentMainTab = TAB_TERRAIN;
    std::vector<cocos2d::ui::Scale9Sprite*> mainTabBgs;
    cocos2d::Node* terrainToolsRow = nullptr;
    cocos2d::Node* unitToolsRow = nullptr;
    cocos2d::Node* triggerToolsRow = nullptr;

    cocos2d::Node* editUnitPanel = nullptr;
    bool isEditUnitPanelOpen = false;
    int editingObjectIndex = -1;
    int editPanelSide = SIDE_ALLY;
    std::vector<cocos2d::ui::Scale9Sprite*> editSideButtonBgs;
    cocos2d::Node* rowEditHp = nullptr;
    cocos2d::ui::TextField* tfEditHp = nullptr;
    cocos2d::Node* rowEditLevel = nullptr;
    cocos2d::ui::TextField* tfEditLevel = nullptr;
    cocos2d::Label* editUnitTitle = nullptr;

    std::vector<UndoAction> undoStack;
    std::vector<UndoAction> redoStack;
    UndoAction currentStroke;
    bool strokeActive = false;
    bool dragEraseObjects = false;

    std::vector<Trigger> triggers;
    int selectedTriggerIndex = -1;

    cocos2d::Node* triggerPanel = nullptr;
    bool isTriggerPanelOpen = false;
    cocos2d::Node* triggerListContainer = nullptr;
    cocos2d::ui::TextField* tfTriggerName = nullptr;
    std::vector<cocos2d::ui::Scale9Sprite*> triggerSideButtonBgs;
    cocos2d::ui::Scale9Sprite* triggerPreserveBg = nullptr;
    cocos2d::ui::Scale9Sprite* condIsRepeatBg = nullptr;
    cocos2d::Node* conditionListContainer = nullptr;
    cocos2d::Node* actionListContainer = nullptr;

    cocos2d::Node* conditionEditPanel = nullptr;
    bool isConditionEditPanelOpen = false;
    int editingConditionIndex = -1; // -1 = adding a new condition
    TriggerCondition conditionDraft;
    cocos2d::Label* lblCondType = nullptr;
    // Each condition type's fields live in their own self-contained row group;
    // only the group matching conditionDraft.type is shown at a time.
    cocos2d::Node* rowCondElapsed = nullptr;
    cocos2d::ui::TextField* tfCondElapsedSeconds = nullptr;
    cocos2d::Node* rowCondSwitch = nullptr;
    cocos2d::Label* lblCondSwitchIndex = nullptr;
    cocos2d::Label* lblCondSwitchState = nullptr;
    cocos2d::Node* rowCondUnitCount = nullptr;
    cocos2d::Label* lblCondSide = nullptr;
    cocos2d::Label* lblCondUnitType = nullptr;
    cocos2d::Label* lblCondComparison = nullptr;
    cocos2d::ui::TextField* tfCondUnitAmount = nullptr;
    cocos2d::Node* rowCondResource = nullptr;
    cocos2d::Label* lblCondResource = nullptr;
    cocos2d::Label* lblCondResourceComparison = nullptr;
    cocos2d::ui::TextField* tfCondResourceAmount = nullptr;

    cocos2d::Node* actionEditPanel = nullptr;
    bool isActionEditPanelOpen = false;
    int editingActionIndex = -1; // -1 = adding a new action
    TriggerAction actionDraft;
    cocos2d::Label* lblActType = nullptr;
    // Each action type's fields live in their own self-contained row group;
    // only the group matching actionDraft.type is shown at a time.
    cocos2d::Node* rowActMessage = nullptr;
    cocos2d::ui::TextField* tfActMessage = nullptr;
    cocos2d::Node* rowActCreateUnit = nullptr;
    cocos2d::Label* lblActCreateSide = nullptr;
    cocos2d::Label* lblActCreateUnitType = nullptr;
    cocos2d::Label* lblActCreateTarget = nullptr;
    cocos2d::Node* rowActCreateX = nullptr;
    cocos2d::ui::TextField* tfActCreateX = nullptr;
    cocos2d::Node* rowActCreateY = nullptr;
    cocos2d::ui::TextField* tfActCreateY = nullptr;
    cocos2d::ui::TextField* tfActCreateCount = nullptr;
    cocos2d::Node* rowActRemoveUnit = nullptr;
    cocos2d::Label* lblActRemoveSide = nullptr;
    cocos2d::Label* lblActRemoveUnitType = nullptr;
    cocos2d::Node* rowActSwitch = nullptr;
    cocos2d::Label* lblActSwitchIndex = nullptr;
    cocos2d::Label* lblActSwitchAction = nullptr;
    cocos2d::Node* rowActWait = nullptr;
    cocos2d::ui::TextField* tfActWaitSeconds = nullptr;
    cocos2d::Node* rowActCamera = nullptr;
    cocos2d::Label* lblActCameraTarget = nullptr;
    cocos2d::Node* rowActCameraX = nullptr;
    cocos2d::ui::TextField* tfActCameraX = nullptr;
    cocos2d::Node* rowActCameraY = nullptr;
    cocos2d::ui::TextField* tfActCameraY = nullptr;
    cocos2d::Node* rowActTalk = nullptr;
    cocos2d::ui::TextField* tfActTalkMessage = nullptr;
    cocos2d::Label* lblActTalkTarget = nullptr;
    cocos2d::Node* rowActTalkX = nullptr;
    cocos2d::ui::TextField* tfActTalkX = nullptr;
    cocos2d::Node* rowActTalkY = nullptr;
    cocos2d::ui::TextField* tfActTalkY = nullptr;
    cocos2d::Node* rowActRevealFog = nullptr;
    cocos2d::Label* lblActRevealFogEnabled = nullptr;
    cocos2d::Label* lblActRevealFogTarget = nullptr;
    cocos2d::Node* rowActRevealFogX = nullptr;
    cocos2d::ui::TextField* tfActRevealFogX = nullptr;
    cocos2d::Node* rowActRevealFogY = nullptr;
    cocos2d::ui::TextField* tfActRevealFogY = nullptr;
    cocos2d::ui::TextField* tfActRevealFogRadius = nullptr;

    cocos2d::Node* targetDropdown = nullptr;
    bool isTargetDropdownOpen = false;
    int targetDropdownScroll = 0;
    std::vector<std::pair<int, std::string>> targetDropdownList;

    cocos2d::LayerColor* modalDimmer = nullptr;
    // Sits at z=22, between triggerPanel (z=20) and conditionEditPanel/actionEditPanel (z=25).
    // Shown whenever a second-level edit panel is open so triggerPanel buttons are blocked.
    cocos2d::LayerColor* triggerSubDimmer = nullptr;

    cocos2d::Node* newMapPanel = nullptr;
    cocos2d::ui::TextField* tfWidth = nullptr;
    cocos2d::ui::TextField* tfHeight = nullptr;
    bool isNewMapPanelOpen = false;

    cocos2d::EventListenerMouse* mouseListener = nullptr;
    cocos2d::EventListenerTouchOneByOne* touchListener = nullptr;
    cocos2d::EventListenerKeyboard* keyListener = nullptr;

    float mapScale = 1.0f;
    static constexpr float kMapScaleMin = 0.25f;
    static constexpr float kMapScaleMax = 3.0f;
    static constexpr float kMapScaleStep = 0.1f;
    void applyMapZoom(float newScale, cocos2d::Vec2 anchorGl);

    bool isDirty = false;
    bool isPainting = false;
    bool isPanning = false;
    bool ctrlDown = false;
    bool shiftDown = false;
    bool isTextFieldFocused = false;
    cocos2d::Vec2 panStartViewPos;
    cocos2d::Vec2 panStartLayerPos;

    void buildMap(int width, int height);
    int terrainAt(int x, int y) const;
    int cornerTerrain(int cx, int cy) const;
    int pickTileIdForCell(int x, int y) const;
    cocos2d::Rect rectForTileId(int tileId) const;
    void refreshTile(int x, int y);
    void refreshAround(int x, int y);
    void paintCell(int x, int y, int terrain);
    bool worldToCell(const cocos2d::Vec2& worldPos, int& outX, int& outY) const;
    void paintAtWorldPos(const cocos2d::Vec2& worldPos);

    int findObjectAt(int x, int y) const;
    int findObjectAtWorldPos(const cocos2d::Vec2& worldPos) const;
    void removeObjectAtIndex(int index);
    cocos2d::Node* spawnObjectAt(int x, int y, int typeIndex, int side, int hp, int id = -1, int level = 1);
    void placeObjectAt(int x, int y);
    void eraseObjectAt(const cocos2d::Vec2& worldPos);
    void objectToolAtWorldPos(const cocos2d::Vec2& worldPos);
    void clearAllObjects();
    cocos2d::Color3B colorForSide(int side) const;

    void beginStroke(ActionType type);
    void endStroke();
    void applyTerrainChange(const TerrainCellChange& change, bool useNew);
    void applyObjectChange(const ObjectCellChange& change, bool useNew);
    void clearUndoHistory();
    void undo();
    void redo();

    void setupUI();
    void setupObjectPalette();
    void rebuildObjectPaletteRow();
    void selectRace(int race);
    void cycleHeroPage(int dir);
    void setupMainTabs();
    void selectMainTab(int tab);
    void setupTriggerToolsRow();
    void highlightGroup(std::vector<cocos2d::ui::Scale9Sprite*>& group, int activeIndex);
    void setupModalDimmer();
    void setModalDimmerVisible(bool visible);
    void setTriggerSubDimmerVisible(bool visible);
    void setupNewMapPanel();
    void showNewMapPanel();
    void hideNewMapPanel();
    void onConfirmNewMap();

    void setupUnitPropertiesBar();
    void selectSide(int side);
    int parsedHpOverride() const;

    void setupPaletteTooltip();
    void updatePaletteTooltip(const cocos2d::Vec2& worldPos);
    cocos2d::Rect paletteSlotWorldRect(int slot) const;
    std::string paletteTooltipText(int typeIndex) const;

    void setupEditUnitPanel();
    void openEditUnitPanel(int objectIndex);
    void closeEditUnitPanel();
    void selectEditSide(int side);
    void onConfirmEditUnit();
    int defaultHpForType(int typeIndex) const;

    void watchTextFieldFocus(cocos2d::ui::TextField* tf);

    cocos2d::Node* addCycleRow(cocos2d::Node* parent, const std::string& title, float x, float y,
                               cocos2d::Label** outValueLabel, const std::function<void(int)>& onCycle);
    cocos2d::Node* addTextFieldRow(cocos2d::Node* parent, const std::string& title, float x, float y,
                                   float fieldWidth, const std::string& initialValue,
                                   cocos2d::ui::TextField** outField);
    void setMultiHighlight(std::vector<cocos2d::ui::Scale9Sprite*>& group, const bool* states, int count);

    void setupTriggerPanel();
    void openTriggerPanel();
    void closeTriggerPanel();
    void refreshTriggerList();
    void selectTrigger(int index);
    void addTrigger();
    void deleteSelectedTrigger();
    void toggleTriggerSide(int side);
    void toggleTriggerPreserve();
    void refreshTriggerDetail();
    std::string describeCondition(const TriggerCondition& cond) const;
    std::string describeAction(const TriggerAction& action) const;

    void setupConditionEditPanel();
    void openConditionEditPanel(int conditionIndex);
    void closeConditionEditPanel();
    void cycleConditionType(int dir);
    void cycleConditionSide(int dir);
    void cycleConditionUnitType(int dir);
    void cycleConditionComparison(int dir);
    void cycleConditionResourceKind(int dir);
    void cycleConditionResourceComparison(int dir);
    void cycleConditionSwitchIndex(int dir);
    void cycleConditionSwitchState(int dir);
    void refreshConditionEditPanel();
    void toggleConditionIsRepeat();
    void onConfirmCondition();
    void deleteConditionAtIndex(int index);

    void setupActionEditPanel();
    void openActionEditPanel(int actionIndex);
    void closeActionEditPanel();
    void cycleActionType(int dir);
    void cycleActionCreateSide(int dir);
    void cycleActionCreateUnitType(int dir);
    void cycleActionRemoveSide(int dir);
    void cycleActionRemoveUnitType(int dir);
    void cycleActionSwitchIndex(int dir);
    void cycleActionSwitchAction(int dir);
    void cycleActionCreateTarget(int dir);
    void cycleActionCameraTarget(int dir);
    void cycleActionTalkTarget(int dir);
    void cycleActionRevealFogEnabled(int dir);
    void cycleActionRevealFogTarget(int dir);
    int cycleTargetObjectId(int currentId, int dir) const;
    std::string describeTargetObject(int id) const;
    void refreshActionEditPanel();
    void onConfirmAction();
    void deleteActionAtIndex(int index);
    std::vector<std::pair<int, std::string>> buildTargetList() const;
    void openTargetDropdown();
    void closeTargetDropdown();
    void selectTargetObject(int id);
    void rebuildTargetDropdown();

    void setupInput();
    bool isAnyModalOpen() const;
    void selectBrush(int terrain);
    void selectObjectTool(int typeIndex);
    void updateToolLabel();
    void setStatus(const std::string& text);

    std::string serialize() const;
    bool deserialize(const std::string& jsonStr);
    void saveMap();
    void saveMapAs();
    void loadMap();
    void runMap();
};

#endif // __map_editor_scene__
