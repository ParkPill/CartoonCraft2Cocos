#ifndef GameManager_h
#define GameManager_h
#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <zlib.h>
#include "AStar.h"
#include "cocos2d.h"
#include "HudLayer.h"
#include "EditorHud.h"
#include "HelloWorldScene.h"
#include "BattleHud.h"

#include <algorithm>
#include <cctype>
#include <locale>
//#include "LoadBalancing-cpp/inc/Client.h"


//#include "SimpleAudioEngine.h"

#include "../cocos2d/cocos/ui/UILayout.h"
#include "../cocos2d/cocos/ui/UIButton.h"
#include "../cocos2d/cocos/ui/UIText.h"
#include "../cocos2d/cocos/ui/UIImageView.h"
#include "../cocos2d/cocos/ui/UITextField.h"
#include "../cocos2d/cocos/ui/UIHelper.h"
#include "../cocos2d/cocos/ui/UILoadingBar.h"
#include "../cocos2d/cocos/ui/UIScrollView.h"

#define TILE_STATE_EMPTY 'e'
#define TILE_STATE_OCCUPIED 'o'
//#define TILE_STATE_WALL 2

#define ASTAR_STATE_INIT 'i'
#define ASTAR_STATE_OPEN 'o'
#define ASTAR_STATE_CLOSE 'c'
#define ASTAR_STATE_DEAD_END 'd'

using namespace cocos2d;
using namespace cocos2d::ui;
#define DARK_GRAY_3B Color3B(78, 78, 78)
#define BTN_FROM_REF_AND_DISABLE_FOR_A_SEC Button* btn = (Button*)ref;btn->setEnabled(false);btn->runAction(Sequence::create(DelayTime::create(1), CallFuncN::create(CC_CALLBACK_1(GameManager::enableButton, GM)), NULL));
#define BTN_FROM_REF_AND_DISABLE Button* btn = (Button*)ref; btn->setEnabled(false);
#define BTN_FROM_REF Button* btn = (Button*)ref;
#define GM GameManager::getInstance()
#define MM MultiplayManager::getInstance()
#define SM ServerManager::getInstance()
#define BSM BuggyServerManager::getInstance()
#define HUD GameManager::getInstance()->getHudLayer()
#define BHUD GameManager::getInstance()->battleHud
#define RHUD GameManager::getInstance()->raitHud
#define TITLE ((Title*)GameManager::getInstance()->titleLayer)
#define HEROPAGE ((HeroPage*)GameManager::getInstance()->heroPage)
#define EHUD GameManager::getInstance()->getEditorHud()
#define WORLD GameManager::getInstance()->getWorld()
#define LM LanguageManager::getInstance()
#define LMText LanguageManager::getInstance()->getText
#define UD UserDefault::getInstance()
#define UDGetInt UserDefault::getInstance()->getIntegerForKey
#define UDSetInt UserDefault::getInstance()->setIntegerForKey
#define UDGetBool UserDefault::getInstance()->getBoolForKey
#define UDSetBool UserDefault::getInstance()->setBoolForKey
#define UDGetStr UserDefault::getInstance()->getStringForKey
#define UDSetStr UserDefault::getInstance()->setStringForKey
#define UDGetDouble GameManager::getInstance()->getIntForOldDouble
// don't use UserDefault::getInstance()->setDoubleForKey it is not saved in galaxy s10s
// you should replace this into UDSetInt
#define UDSetDouble UserDefault::getInstance()->setIntegerForKey
#define SPT_REMOVE_FUNC CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, spt))
#define SHAKE_ONCE CallFunc::create(CC_CALLBACK_0(HelloWorld::shakeScreenOnce, this))
#define strmake StringUtils::format

#define ITEM_TYPE_WEAPON 0
#define ITEM_TYPE_HELMET 1
#define ITEM_TYPE_SHIELD 2
#define ITEM_TYPE_SHOES 3
#define ITEM_TYPE_ETC 4

#define IAP_FLAG_INIT -1
#define IAP_FLAG_FAILED 2
#define IAP_FLAG_SUCCESS 3
#define IAP_FLAG_RESTORED 4

#define IAP_DETAIL_TICKET_1 "goldenticket1"
#define IAP_DETAIL_PREMIUM_RETRY "premium_retry"
#define IAP_DETAIL_GEM_3 "cc_gem3"
#define IAP_DETAIL_GEM_10 "cc_gem10"
#define IAP_DETAIL_GEM_50 "cc_gem50"
#define IAP_DETAIL_GEM_100 "cc_gem100"
#define IAP_DETAIL_STARTER_1 "cc_starter1"
#define IAP_DETAIL_STARTER_3 "cc_starter3"
#define IAP_DETAIL_STARTER_10 "cc_starter10"
#define IAP_DETAIL_CHAPTER2 "cc_chapter2"
#define IAP_DETAIL_CHAPTER3 "cc_chapter3"
#define IAP_DETAIL_UNIT_PACK_2 "cc_unitpack2"
#define IAP_DETAIL_UNIT_PACK_5 "cc_unitpack5"
#define IAP_DETAIL_UNIT_PACK_8 "cc_unitpack8"
#define IAP_DETAIL_STARTER_KEY "cc_starterkey"
#define IAP_DETAIL_MARTIAL_ART_PACKAGE "cc_martial_art_package"
#define IAP_DETAIL_CLASSIC_WAR_PACKAGE "cc_classic_war_package"
#define IAP_DETAIL_CREW_MANAGER_PACKAGE "cc_crew_manager_package"
#define IAP_DETAIL_MASTER_CREW_PACKAGE "cc_master_crew_package"
#define IAP_DETAIL_EVENT_HERO_PACKAGE "cc_monthly_event_package"
#define IAP_DETAIL_MID_MONTH_PACKAGE "cc_mid_month_event_package"

#define FEELING_TALK 0
#define FEELING_EXCLAIM 1

#define ELEMENT_NONE 0
#define ELEMENT_WATER 1
#define ELEMENT_LIGHTNING 2
#define ELEMENT_GROUND 3
#define ELEMENT_FIRE 4
#define ELEMENT_LIGHT 5
#define ELEMENT_DARK 6

#define WEAPON_NOT_EXIST -1

#define WEAPON_PISTOL 0
#define WEAPON_MAGNUM 1
#define WEAPON_UZI 2
#define WEAPON_RIFLE 3
#define WEAPON_FIRE_BOTTLER 4

#define WEAPON_ANTI_BAT 5
#define WEAPON_SNIPER_RIFLE 6
#define WEAPON_RIFLE_GRENADE 7
#define WEAPON_CROSS_BOW 8
#define WEAPON_MACHINE_GUN 9

#define WEAPON_STRIKER 10
#define WEAPON_NIFE_THROWER 11
#define WEAPON_BALL_SHOOTER 12
#define WEAPON_FLAME_THROWER 13
#define WEAPON_ROCKET_LAUNCHER 14

#define WEAPON_BAZOOKA 15
#define WEAPON_MATT_A4 16
#define WEAPON_LASER_GUN 17
#define WEAPON_ENERGY_GUN 18
#define WEAPON_GUIDED_ROCKET_LAUNCHER 19

#define WEAPON_SLUGGER 20
#define WEAPON_DRAGON_RIFLE 21
#define WEAPON_SLUGGER_NG 22

#define WEAPON_LIGHTNING_GUN 23
#define WEAPON_SHARK_GUN 24
#define WEAPON_LASER_COIN 25

#define WEAPON_NO_MORE 26
#define TOTAL_WEAPON_COUNT 26

#define WEAPON_POWER_MAX_LEVEL 10
#define WEAPON_CRITICAL_MAX_LEVEL 10

//#define CHARACTER_DARY 0
//#define CHARACTER_GABRIEL 1
//#define CHARACTER_JOHN 2
//#define CHARACTER_SUSAN 3
//#define CHARACTER_PAUL 4
//#define CHARACTER_BREE 5
//#define CHARACTER_NINJA 6

#define CHARACTER_NO_MORE 7
#define CHARACTER_NONE 999

#define TOTAL_CHARACTER_COUNT 7


#define MAX_LEVEL 100

//#define ldPistolLevelKey "Key_Pistol_Key"
#define ldPistolUnlockedKey "Key_Pistol_Unlocked"
//#define ldMagnumLevelKey "Key_Magnum_Key"
#define ldMagnumUnlockedKey "Key_Magnum_Unlocked"
//#define ldUziLevelKey "Key_Uzi_Key"
#define ldUziUnlockedKey "Key_Uzi_Unlocked"
//#define ldSniperRifleLevelKey "Key_Sniper_Rifle_Key"
#define ldSniperRifleUnlockedKey "Key_Sniper_Rifle_Unlocked"
//#define ldSluggerLevelKey "Key_Slugger_Key"
#define ldSluggerUnlockedKey "Key_Slugger_Unlocked"
#define ldSluggerNGUnlockedKey "Key_SluggerNG_Unlocked"
//#define ldRocketLauncherLevelKey "Key_Rocket_Launcher_Key"
#define ldRocketLauncherUnlockedKey "Key_Rocket_Launcher_Unlocked"
//#define ldRifleGrenadeLevelKey "Key_Rifle_Grenade_Key"
#define ldRifleGrenadeUnlockedKey "Key_Rifle_Grenade_Unlocked"
//#define ldRifleLevelKey "Key_Rifle_Key"
#define ldRifleUnlockedKey "Key_Rifle_Unlocked"
//#define ldNifeThrowerLevelKey "Key_Nife_Thrower_Key"
#define ldNifeThrowerUnlockedKey "Key_Nife_Thrower_Unlocked"
//#define ldMattA4LevelKey "Key_Matt_A4_Key"
#define ldMattA4UnlockedKey "Key_Matt_A4_Unlocked"
//#define ldMachineGunLevelKey "Key_Machine_Gun_Key"
#define ldMachineGunUnlockedKey "Key_Machine_Gun_Unlocked"
//#define ldLaserGunLevelKey "Key_Laser_Gun_Key"
#define ldLaserGunUnlockedKey "Key_Laser_Gun_Unlocked"
//#define ldGuidedRocketLauncherLevelKey "Key_Guided_Rocket_Launcher_Key"
#define ldGuidedRocketLauncherUnlockedKey "Key_Guided_Rocket_Launcher_Unlocked"
//#define ldFireBottlerLevelKey "Key_Fire_Bottler_Key"
#define ldFireBottlerUnlockedKey "Key_Fire_Bottler_Unlocked"
//#define ldEnergyGunLevelKey "Key_Energy_Gun_Key"
#define ldEnergyGunUnlockedKey "Key_Energy_Gun_Unlocked"
//#define ldDragonRifleLevelKey "Key_Dragon_Rifle_Key"
#define ldDragonRifleUnlockedKey "Key_Dragon_Rifle_Unlocked"
//#define ldCrossBowLevelKey "Key_Cross_Bow_Key"
#define ldCrossBowUnlockedKey "Key_Cross_Bow_Unlocked"
//#define ldBazookaLevelKey "Key_Bazooka_Key"
#define ldBazookaUnlockedKey "Key_Bazooka_Unlocked"
//#define ldBallShooterLevelKey "Key_Ball_Shooter_Key"
#define ldBallShooterUnlockedKey "Key_Ball_Shooter_Unlocked"
//#define ldAntiBatLevelKey "Key_Anti_Bat_Key"
#define ldAntiBatUnlockedKey "Key_Anti_Bat_Unlocked"
//#define ldStrikerLevelKey "Key_Striker_Key"
#define ldStrikerUnlockedKey "Key_Striker_Unlocked"


#define ldCharacterDaryUnlockKey "Dary_Unlock"

#define NETWORK_HANDLE_STATE_NOT_READY 0
#define NETWORK_HANDLE_STATE_SHOULD_REQUEST 1
#define NETWORK_HANDLE_STATE_REQUESTED 2
#define NETWORK_HANDLE_STATE_ARRIVED 3
#define NETWORK_HANDLE_STATE_HANDLED 4
#define NETWORK_HANDLE_STATE_COMPLETE 5
#define NETWORK_HANDLE_STATE_ERROR 6
#define POPUP_ZORDER 200
#define KEY_HERO_LEVEL_FORMAT "KEY_HERO_LEVEL_%d"
#define KEY_HERO_EXP_FORMAT "KEY_HERO_EXP_%d"
#define KEY_HERO_POS_X_FORMAT "KEY_HERO_POS_X_FORMAT_%d"
#define KEY_HERO_POS_Y_FORMAT "KEY_HERO_POS_Y_FORMAT_%d"
#define KEY_HERO_INVENTORY_UNIT_TYPE_FORMAT "KEY_HERO_INVENTORY_UNIT_TYPE_FORMAT_%d"
#define KEY_HERO_INVENTORY_RANK_LEVEL_FORMAT "KEY_HERO_INVENTORY_RANK_LEVEL_FORMAT_%d"
#define KEY_HERO_INVENTORY_INT_SAVED "KEY_HERO_INVENTORY_INT_SAVED"
#define KEY_HERO_DECK_UNIT_TYPE_FORMAT "KEY_HERO_DECK_UNIT_TYPE_FORMAT_%d"
#define KEY_HERO_DECK_RANK_LEVEL_FORMAT "KEY_HERO_DECK_RANK_LEVEL_FORMAT_%d"
#define KEY_HERO_DECK_X_FORMAT "KEY_HERO_DECK_X_FORMAT_%d"
#define KEY_HERO_DECK_Y_FORMAT "KEY_HERO_DECK_Y_FORMAT_%d"
#define KEY_HERO_DECK_INT_SAVED "KEY_HERO_DECK_INT_SAVED"

#define KEY_BATTLE_UNIT_INVENTORY_UNIT_TYPE_FORMAT "KEY_BATTLE_UNIT_INVENTORY_UNIT_TYPE_FORMAT_%d"
#define KEY_BATTLE_UNIT_INVENTORY_RANK_LEVEL_FORMAT "KEY_BATTLE_UNIT_INVENTORY_RANK_LEVEL_FORMAT_%d"
#define KEY_BATTLE_UNIT_INVENTORY_INT_SAVED "KEY_BATTLE_UNIT_INVENTORY_INT_SAVED"
#define KEY_BATTLE_UNIT_DECK_UNIT_TYPE_FORMAT "KEY_BATTLE_UNIT_DECK_UNIT_TYPE_FORMAT_%d"
#define KEY_BATTLE_UNIT_DECK_RANK_LEVEL_FORMAT "KEY_BATTLE_UNIT_DECK_RANK_LEVEL_FORMAT_%d"
#define KEY_BATTLE_UNIT_DECK_X_FORMAT "KEY_BATTLE_UNIT_DECK_X_FORMAT_%d"
#define KEY_BATTLE_UNIT_DECK_Y_FORMAT "KEY_BATTLE_UNIT_DECK_Y_FORMAT_%d"
#define KEY_BATTLE_UNIT_DECK_INT_SAVED "KEY_BATTLE_UNIT_DECK_INT_SAVED"

#define KEY_HERO_WEAPON_FORMAT "KEY_HERO_WEAPON_%d"
#define KEY_HERO_SHIELD_FORMAT "KEY_HERO_SHIELD_%d"
#define KEY_HERO_HELMET_FORMAT "KEY_HERO_HELMET_%d"
#define KEY_HERO_SHOES_FORMAT "KEY_HERO_SHOES_%d"
#define KEY_HERO_NAME_FORMAT "KEY_HERO_NAME_%d"
#define KEY_INVENTORY_ITEM_FORMAT "KEY_INVENTORY_ITEM_FORMAT_%d"
#define KEY_INVENTORY_ITEM_COUNT_FORMAT "KEY_INVENTORY_ITEM_COUNT_FORMAT_%d"
#define KEY_LAST_HIRE_REFRESH_DAY "KEY_LAST_HIRE_REFRESH_DAY"
#define KEY_LAST_SAVE_POINT "KEY_LAST_SAVE_POINT"
#define KEY_HELL_MODE_CLEAR_STAGE "KEY_HARD_MODE_CLEAR_STAGE"
#define KEY_HAD_MODE_CLEAR_STAGE "KEY_HAD_MODE_CLEAR_STAGE"
#define KEY_EASY_MODE_CLEAR_STAGE "KEY_EASY_MODE_CLEAR_STAGE"
#define KEY_LAST_CHECK_POINT "KEY_LAST_CHECK_POINT"
#define KEY_LAST_SAVE_HEALTH "KEY_LAST_SAVE_HEALTH"
#define KEY_NEXT_SPAWN_DIRECTION "KEY_NEXT_SPAWN_DIRECTION"
#define KEY_ALCHY_COMBINE_START_TIME "KEY_ALCHY_COMBINE_START_TIME_STR"
#define KEY_ALCHY_COMBINING_ITEM_NAME "KEY_ALCHY_COMBINING_ITEM_NAME"
#define KEY_INSURANCE_END_TIME "KEY_INSURANCE_END_TIME_STR"
#define KEY_POT_SOUL_COUNT "KEY_POT_SOUL_COUNT"
#define KEY_POT_BROKEN_FORMAT "KEY_POT_BROKEN_FORMAT_%d_%d_%d"
//#define KEY_LAST_CLEAR_STAGE "KEY_LAST_CLEAR_STAGE"
#define KEY_MAP_SAVE_NAME_FORMAT "KEY_MAP_SAVE_NAME_FORMAT%d"
#define KEY_MAP_SAVE_DATA_FORMAT "KEY_MAP_SAVE_DATA_FORMAT%d"
#define KEY_DOUBLE_EXP_GET "DoubleExpGet"
#define KEY_REMOVE_ADS_GET "RemoveAdsGet"
#define KEY_INFINITY_LIFE_GET "InfiniteLivesGet"
#define KEY_LIFE "LifeKey"
#define KEY_RECEIVED_REWARD_INDEX "KEY_RECEIVED_REWARD_INDEX"
#define KEY_HERO_PAGE_TUTORIAL_DONE "KEY_HERO_PAGE_TUTORIAL_DONE"
#define KEY_DEFENCE_RECORD_TUTORIAL_DONE "KEY_DEFENCE_RECORD_TUTORIAL_DONE"
#define KEY_HERO_ARRANGE_TYPE "KEY_HERO_ARRANGE_TYPE"
#define KEY_TODAY_INT "KEY_TODAY_INT"
#define KEY_DAILY_REWARD_RECEIVED_DAY_INT "KEY_DAILY_REWARD_RECEIVED_DAY_INT"
#define KEY_ID_EXIST_CHECK_DONE "KEY_ID_EXIST_CHECK_DONE"
#define KEY_VIDEO_STORE_RANDOM_INDEX "KEY_VIDEO_STORE_RANDOM_INDEX"
#define KEY_VIDEO_STORE_UNIT_INDEX "KEY_VIDEO_STORE_UNIT_INDEX"
#define KEY_VIDEO_STORE_UNIT_DAY "KEY_VIDEO_STORE_UNIT_DAY"
#define KEY_VIDEO_STORE_GEM_INDEX "KEY_VIDEO_STORE_GEM_INDEX"
#define KEY_VIDEO_STORE_RANDOM_WATCHED_TIME "KEY_VIDEO_STORE_RANDOM_WATCHED_TIME_STR"
#define KEY_VIDEO_STORE_UNIT_WATCHED_TIME "KEY_VIDEO_STORE_UNIT_WATCHED_TIME_STR"
#define KEY_VIDEO_STORE_GEM_WATCHED_TIME "KEY_VIDEO_STORE_GEM_WATCHED_TIME_STR"
#define KEY_VIDEO_GACHA_WATCHED_TIME "KEY_VIDEO_GACHA_WATCHED_TIME_STR"
#define KEY_GACHA_NEXT_FREE_TIME "KEY_GACHA_NEXT_FREE_TIME_STR"
#define KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME "KEY_WOOD_CHEST_GACHA_NEXT_FREE_TIME2"
#define KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME "KEY_GOLD_CHEST_GACHA_NEXT_FREE_TIME2"
#define KEY_UPGRADE_ENDTIME_FORMAT "KEY_UPGRADE_ENDTIME_FORMAT_STR%d"
#define KEY_UPGRADE_UNIT_FORMAT "KEY_UPGRADE_UNIT_FORMAT%d"
#define KEY_BATTLE_TUTORIAL_DONE "KEY_BATTLE_TUTORIAL_DONE"
#define KEY_CHAPTER_2_PURCHASED "KEY_CHAPTER_2_PURCHASED"
#define KEY_CHAPTER_3_PURCHASED "KEY_CHAPTER_3_PURCHASED"
#define KEY_WORKER_BOUGHT "KEY_WORKER_BOUGHT"
#define KEY_SINGLE_PLAY_CLEAR_FORMAT "KEY_SINGLE_PLAY_CLEAR_FORMAT_%d"
#define KEY_SINGLE_PLAY_STAR_FORMAT "KEY_SINGLE_PLAY_STAR_FORMAT_%d"
#define KEY_SINGLEPLAY_DATA_FORMAT "KEY_SINGLEPLAY_DATA_%d"
#define KEY_BUILDINGS "KEY_BUILDINGS"
#define KEY_UNITS_INVENTORY "KEY_UNITS_INVENTORY"
#define KEY_UNITS_INVENTORY_COUNT "KEY_UNITS_INVENTORY_COUNT"
#define KEY_UNITS_INVENTORY_FORMAT "KEY_UNITS_INVENTORY_%d"
#define KEY_UNITS_DECK "KEY_UNITS_DECK"
#define KEY_UNITS_DECK_COUNT "KEY_UNITS_DECK_COUNT"
#define KEY_UNITS_DECK_FORMAT "KEY_UNITS_DECK_%d"
#define KEY_UNITS_HERO_INVENTORY "KEY_UNITS_HERO_INVENTORY"
#define KEY_UNITS_HERO_INVENTORY_COUNT "KEY_UNITS_HERO_INVENTORY_COUNT"
#define KEY_UNITS_HERO_INVENTORY_FORMAT "KEY_UNITS_HERO_INVENTORY_FORMAT_%d"
#define KEY_UNITS_HERO_DECK "KEY_UNITS_HERO_DECK"
#define KEY_UNITS_HERO_DECK_COUNT "KEY_UNITS_HERO_DECK_COUNT"
#define KEY_UNITS_HERO_DECK_FORMAT "KEY_UNITS_HERO_DECK_FORMAT_%d"
#define KEY_DEFENCE_RECORD "KEY_DEFENCE_RECORD"
#define KEY_COLLECTION_UNLOCKED_FORMAT "KEY_COLLECTION_UNLOCKED_FORMAT%d"
#define KEY_SAVED_ID "KEY_SAVED_ID"
#define KEY_PREVIOUS_ID "KEY_PREVIOUS_ID"
#define KEY_HERO_INVALIDATE "KEY_HERO_INVALIDATE"
#define KEY_BATTLE_DATA_INVALIDATE "KEY_BATTLE_DATA_INVALIDATE"
#define KEY_MESSAGE_BOX "KEY_MESSAGE_BOX"
#define KEY_GOLDEN_TICKET "KEY_GOLDEN_TICKET"
#define KEY_GOLDEN_TICKET_FREE "KEY_GOLDEN_TICKET_FREE"
#define KEY_GOLDEN_TICKET_FREE_USED_DAY "KEY_GOLDEN_TICKET_FREE_USED_DAY"
#define KEY_PLAYER_ID "KEY_PLAYER_ID"
#define KEY_PLAYER_NAME "KEY_PLAYER_NAME"
#define KEY_LAST_TICKET_USED_TIME "KEY_LAST_TICKET_USED_TIME_STR"
#define KEY_LIFE_SPENT_TIME "LifeSpentTime_STR"
#define KEY_NEXT_MAP_CREATING_INDEX "KEY_NEXT_MAP_CREATING_INDEX"
#define KEY_MAP_DATA_SAVED_FORMAT "KEY_MAP_DATA_SAVED_FORMAT_%d"
#define KEY_BULLET_COUNT_AT_SLOT_FORMAT "BulletCountAtSlot%d"
#define KEY_LOADED_BULLET_COUNT_AT_SLOT_FORMAT "LoadedBulletCountAtSlot%d"
#define KEY_ISLAND_UNLOCKED "Island_Unlock"
#define KEY_DUNGEON_UNLOCKED "Dungeon_Unlock"
#define KEY_FIELD_UNLOCKED "Field_Unlock"
#define KEY_STARTER_PACK_PURCHASED "STARTER_PAC"
#define KEY_RATE_POPUP_NEVER_SHOW "KEY_RATE_POPUP_NEVER_SHOW"
#define KEY_HERO_ALERT_NEVER_SHOW "KEY_HERO_ALERT_NEVER_SHOW"

#define KEY_SEARCH_STATE "KEY_SEARCH_STATE"
#define KEY_SEARCH_UNITS "KEY_SEARCH_UNITS"

#define KEY_IAP_USER "IAP_DONE"
#define KEY_IS_ALL_CLEAR "ISALLCLEAR"
#define KEY_IS_TUTORIAL_CLEAR "ISTUTORIALCLEAR"
#define KEY_ADS_COUNTER "AdsCounter"
#define KEY_IS_KNIGHT_CLEAR "IsKnightClear"
#define KEY_WEAPON_COLLECTED_FORMAT "WeaponCollected%d"

#define KEY_CLEAR_COUNT "DungeonClearCount"
#define KEY_JEWEL_COLLECTED_FORMAT "JewelCollectedFormat%d"
#define KEY_DOES_IT_TALKED_FORMAT "DoesItTalked%s"

#define KEY_AUTO_TARGET_SELECTED "AUTO_TARGET_SELECTED"
#define KEY_FIRST_LAUNCH "KEY_FIRST_LAUNCH"
#define KEY_FIRST_LAUNCH_AFTER_NEW_SERVER "KEY_FIRST_LAUNCH_AFTER_NEW_SERVER"
#define KEY_IS_FREE_USER "KEY_IS_FREE_USER"

#define KEY_TUTORIAL_GHOST_IDNEX "KEY_TUTORIAL_GHOST_IDNEX"
#define DATA_SECURE_OFFSET 1021
#define TUTORIAL_STAGE_START -100
#define TUTORIAL_STAGE_END -94


#define BUTTON_LEFT 0
#define BUTTON_RIGHT 1
#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_LEFT_RIGHT 4
#define BUTTON_A 5
#define BUTTON_B 6


#define STATE_STANDING 0
#define STATE_WALKING 1
//#define STATE_JUMPING 2
#define STATE_FALLING 3

#define ENEMY_MOVE_STAND 0
#define ENEMY_MOVE_WANDER_ON_A_PLATFORM 1
#define ENEMY_MOVE_WANDER_CROSS_PLATFORM 2
#define ENEMY_MOVE_WANDER_CROSS_PLATFORM_CRASH_BY_WALL 3
#define ENEMY_MOVE_WANDER_ON_CEILING 4
#define ENEMY_MOVE_WANDER_ON_WALL 5
#define ENEMY_MOVE_HANG 6
#define ENEMY_MOVE_DROP 7
#define ENEMY_MOVE_DROP_CRASH 8
//#define ENEMY_MOVE_FLY_TO_HERO 9
#define ENEMY_MOVE_FLY_TO_HERO_HORIZONTALLY 10
#define ENEMY_MOVE_FLY_TO_HERO_FOLLOW 11
#define ENEMY_MOVE_FLY_TO_RIGHT 12
#define ENEMY_MOVE_FLY_TO_DOWN 13
#define ENEMY_MOVE_FLY_TO_LEFT 14
#define ENEMY_MOVE_FLY_TO_UP 15
#define ENEMY_MOVE_CUSTOM 16
#define ENEMY_MOVE_FLY_WANDER_STOP 17
#define ENEMY_MOVE_SWIM_TO_HERO_FOLLOW 18
#define ENEMY_MOVE_JUMP_STAND 19
#define ENEMY_MOVE_JUMP_WANDER 20
#define ENEMY_MOVE_JUMP_TO_TARGET 21
#define ENEMY_MOVE_TO_CLIFF 22
#define ENEMY_MOVE_CROSS_TO_TARGET 23


#define ENEMY_ACTION_NONE 0
#define ENEMY_ACTION_FIRE 1
#define ENEMY_ACTION_DEFENCE 2
#define ENEMY_ACTION_MOVE 3
#define ENEMY_ACTION_MOVE_TO_LEFT 4
#define ENEMY_ACTION_MOVE_TO_RIGHT 5


#define ENEMY_AFTER_FIND_HERO_STAND 0
#define ENEMY_AFTER_FIND_HERO_WANDER 1
#define ENEMY_AFTER_FIND_HERO_HANG 2
#define ENEMY_AFTER_FIND_HERO_FLY 3



#define TRAP_TYPE_RAIN 0
#define TRAP_TYPE_LASER 1

#define SOUND_BGM_DUAL 0
#define SOUND_BGM_MAYDAY 1
#define SOUND_DDALKACK 2
#define SOUND_WOOD_HIT 3
#define SOUND_HIT 4
#define SOUND_PAPER_FLIP 5
#define SOUND_ARROW 6
#define SOUND_PENCIL_SHORT 7
#define SOUND_HELICOPTER 8
#define SOUND_EXPLOSION_MIDDLE 9
#define SOUND_NAGATIVE 10


#define EFFECT_TWINKLE 0
#define EFFECT_EXPLODE_SMALL 1
#define EFFECT_EXPLODE_MIDDLE 2
#define EFFECT_EXPLODE_BIG 3
#define EFFECT_EXPLODE_HUGE 4
#define EFFECT_SMOKE 5
#define EFFECT_GREEN_SMOKE 6
#define EFFECT_RED_SMOKE 7
#define EFFECT_BULLET_WALL 8
#define EFFECT_BULLET_WALL_SMALL 9
#define EFFECT_BOMB_WALL 10
#define EFFECT_WING 11
#define EFFECT_BOTTOM_RECT_GLOW 12
#define EFFECT_RISING_PARTICLE 13
#define EFFECT_PARTICLE_TORNADO 14
#define EFFECT_HIT 15
#define EFFECT_FIREBALL_EXPLOSION 16
#define EFFECT_LIGHTNING 17
#define EFFECT_FIREBOMB 18
#define EFFECT_ICE_RAISE 19
#define EFFECT_BLUE_TEETH 20
#define EFFECT_BLUE_SLASH 21
#define EFFECT_HIT_WITH_CIRCLE 22
#define EFFECT_FIRE_SLASH 23
#define EFFECT_GREEN_HIT 24
#define EFFECT_RED_TEETH 25
#define EFFECT_HEAL 26
#define EFFECT_HIT_WITH_CIRCLE_ON_GROUND 27
#define EFFECT_PURPLE_SLASH 28
#define EFFECT_WATER_SPLASH 29
#define EFFECT_LASER 30

#define VEHICLE_NONE 0
#define VEHICLE_GOLIATH 1
#define VEHICLE_BROOM 2
#define VEHICLE_CART 3
#define VEHICLE_TOP 4
#define VEHICLE_BIRD 5
#define VEHICLE_BALLOON 6


#define ACTION_TAG_ANIMATION 0
#define ACTION_TAG_ROTATION 1

#define GUN_ORDINARY 0

#define BOSS_BAZOOKA 0
#define BOSS_MACHINE_GUN 1
#define BOSS_GIANT 2
#define BOSS_GIANT_HOOD 3

#define MARKET_PAID 1
#define MARKET_FREE 2
#define MARKET_SMARTPASS 3
#define MARKET_ONESTORE_PAID 4
#define MARKET_ONESTORE_FREE 5
#define MARKET_SAMSUNG_APPS 6
#define MARKET_MAC 7
#define MARKET_FUNBOX 8
#define MARKET_OUYA 9
#define MARKET_WINDOWS 10
#define MARKET_CHINA_NO_IAP 11

#define FONT_DEFAULT 0
#define FONT_BITDUST_ONE 1
#define FONT_BITDUST_TWO 2
#define FONT_ARIAL 3
#define FONT_VISITOR 4

#define GOLD 0
#define GEM 1

#define TAG_CHECK_MARK 1
#define TAG_FORE_LAYER 1000
#define TAG_UNIT_LAYER 1001
#define TAG_STAGE_LAYER 1002
#define TAG_DECO_LAYER 1003
#define TAG_BACK_LAYER 1004
#define TAG_HIGH_STAGE_LAYER 1005
#define TAG_THEME_LOCK 2001
#define TAG_THEME_LOCK_COVER 2002
#define TAG_THEME_STAR_BOARD 2003
#define TAG_THEME_STAR_COLLECTED_LABEL 2004
#define TAG_THEME_STAR_SLASH_LABEL 2005
#define TAG_THEME_STAR_TOTAL_LABEL 2006
#define TAG_NINJA_VAIL 2007

#define TAG_DISPOSABLE_MESSAGE 9900

#define MISSILE_EFFECT_NONE 0
#define MISSILE_EFFECT_SMOKE 1
#define MISSILE_EFFECT_RED_BALL 2
#define MISSILE_EFFECT_GREEN_SMOKE 3
#define MISSILE_EFFECT_RED_SMOKE 4
#define MISSILE_EFFECT_SHARK_GUN 5



#define FOOD_APPLE 0
#define FOOD_GREEN_APPLE 1
#define FOOD_PINE_APPLE 2
#define FOOD_PEAR 3
#define FOOD_BANANA 4
#define FOOD_STRAWBERRY 5
#define FOOD_CHERRY 6
#define FOOD_CARROT 7
#define FOOD_MUSHROOM 8
#define FOOD_CORN 9
#define FOOD_WATERMELON 10
#define FOOD_FISH 11
#define FOOD_BISCKET 12
#define FOOD_DONUT 13
#define FOOD_BREAD 14
#define FOOD_TOAST 15
#define FOOD_CHICKEN 16
#define FOOD_ICECREAM 17
#define FOOD_HAMBURGER 18
#define FOOD_POTATO_CHIPS 19
#define FOOD_CANDY 20
#define FOOD_HOTDOG 21
#define FOOD_ONIGIRI 22
#define FOOD_LOLLYPOP 23

#define COIN_OFFSET 7973
#define GEM_OFFSET 9997
#define GAME_EXP_UNIT 37

#define KEY_ENEMY_KILL_COUNT_FORMAT "KEY_ENEMY_KILL_COUNT_FORMAT_%d"

#define KEY_FOOD_FORMAT "FOOD_%d"
#define KEY_PREVIOUS_INVENTORY_COUNT "PreviousInventoryCount"
#define KEY_ACHIEVEMENT_GOAL "Achievement_Goal_%d"
#define KEY_ACHIEVEMENT_CURRENT "Achievement_Current_%d"

#define KEY_SOUND_VOLUMN "SoundVolumn"
#define KEY_MUSIC_VOLUMN "MusicVolumn"
#define KEY_BUTTON_SIZE "ButtonSize"
#define KEY_NOTIFICATION_ON "NotificationOn"
#define KEY_ACHIEVEMENT_REWARD_COLLECTED_FORMAT "Achievement_Reward_Collected_%d"
#define KEY_COLLECTION_REWARD_COLLECTED_FORMAT "Collection_Reward_Collected_%d"

#define KEY_STAGE_MASTER_FORMAT "StageMaster_%d"
#define KEY_STAR_COUNT_FORMAT "StarCount_%d_%d"
#define KEY_LAST_CLEAR_STAGE "LastClearStage"
#define KEY_LAST_REWARDED_CODE "KEY_LAST_REWARDED_CODE"
#define KEY_PREMIUM_START "KEY_PREMIUM_START"
#define KEY_SPECIAL_UNITS "KEY_SPECIAL_UNITS"
#define KEY_SPECIAL_OFFER_OPEN_TIME "KEY_SPECIAL_OFFER_OPEN_TIME_STR"
#define KEY_SPECIAL_OFFER_BOUGHT_FORMAT "KEY_SPECIAL_OFFER_BOUGHT_FORMAT_%d"
#define KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED "KEY_SPECIAL_OFFER_LAST_CHANCE_ASKED"
#define KEY_ARENA_CLEAR_COUNT "KEY_ARENA_CLEAR_COUNT"
#define KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX "KEY_CURRENT_COLOSSEUM_LEAGUE_INDEX"
#define KEY_CURRENT_COLOSSEUM_LEAGUE "KEY_CURRENT_COLOSSEUM_LEAGUE"
#define KEY_COLOSSEUM_SCORE "KEY_COLOSSEUM_SCORE"
#define KEY_SERVER "KEY_SERVER"
#define COLOSSEUM_RESULT_FAILED 0
#define COLOSSEUM_RESULT_CLEAR 1
#define KEY_JUST_CLEARED_PREVIOUS_STAR_COUNT "JustClearedPreviousStarCount"
#define KEY_IS_JUST_CLEARED "IsJustCleared"
#define KEY_JUST_CLEARED_STAGE "JustClearedStage"
#define KEY_STAGE_CLEAR_FORMAT "StageCleared_%d"
#define KEY_SELECTED_WEAPON "LastWeapon"
#define KEY_WEAPON_PURCHASED_FORMAT "WeaponPurchased_%d"
#define KEY_SELECTED_CHARACTER "LastCharacter"
#define KEY_CHARACTER_PURCHASED_FORMAT "CharacterPurchased_%d"
#define KEY_WEAPON_POWER_LEVEL "Weapon_Pwr_%d"
#define KEY_WEAPON_CRITICAL_LEVEL "Weapon_Cri_%d"
#define KEY_BONUS_WEAPON_FORMAT "KEY_BONUS_WEAPON_FORMAT_%d"

#define KEY_AD_NOT_TODAY "AdNotToday"

#define KEY_SNAPSHOT_VER "snapShotVersion"
#define KEY_COIN_FARM "coinFarm"
#define KEY_CASTLE_LEVEL "KEY_CASTLE_LEVEL"
#define KEY_DAILY_MISSION_ARENA "KEY_DAILY_MISSION_ARENA"
#define KEY_DAILY_MISSION_CAMPAIGN "KEY_DAILY_MISSION_CAMPAIGN"
#define KEY_DAILY_MISSION_NETWORK_BATTLE "KEY_DAILY_MISSION_NETWORK_BATTLE"
#define KEY_DAILY_MISSION_LOCAL_BATTLE "KEY_DAILY_MISSION_LOCAL_BATTLE"
#define KEY_DAILY_MISSION_OPEN "KEY_DAILY_MISSION_OPEN"
#define KEY_DAILY_MISSION_RECEIVED_FORMAT "KEY_DAILY_MISSION_RECEIVED_%d"
#define KEY_COIN_COUNT "coinCount"
#define KEY_WOOD_KEY_COUNT "KEY_WOOD_KEY_COUNT"
#define KEY_GOLD_KEY_COUNT "KEY_GOLD_KEY_COUNT"
#define KEY_GEM_COUNT "KEY_GEM_COUNT"
#define KEY_TREE_COUNT "KEY_TREE_COUNT"



#define KEY_UPDATE_REWARD_PROVIDED "UpdateIssue"
#define KEY_SELECTED_LANGUAGE "SelectedLanguage"
#define KEY_NAME "KEY_Name"
#define KEY_SEASON_KEY "KEY_SEASON_KEY"
#define KEY_MULTIPLAY_TROPHY "KEY_MULTIPLAY_TROPHY"
#define KEY_INVENTORY_SAVE_RETURN_TO_INDIVIDUAL_SYSTEM "KEY_INVENTORY_SAVE_RETURN_TO_INDIVIDUAL_SYSTEM"
#define KEY_DECK_SAVE_RETURN_TO_INDIVIDUAL_SYSTEM "KEY_DECK_SAVE_RETURN_TO_INDIVIDUAL_SYSTEM"
#define KEY_EVENT_ITEM_BOUGHT_COUNT "KEY_EVENT_ITEM_BOUGHT_COUNT"
#define KEY_MID_MONTH_ITEM_BOUGHT_COUNT "eventSetItemBuyCount"
#define KEY_CAMPAIGN_CHEST_GET_TIME_FORMAT "KEY_CAMPAIGN_CHEST_GET_TIME_%d"
#define KEY_CAMPAIGN_CHEST_STAGE_INDEX_FORMAT "KEY_CAMPAIGN_CHEST_STAGE_INDEX_%d"
#define KEY_MONTHLY_EVENT_INFO "KEY_MONTHLY_EVENT_INFO"
#define KEY_MONTHLY_EVENT_INFO_INVALIDATE "KEY_MONTHLY_EVENT_INFO_INVALIDATE"
#define KEY_MONTHLY_EVENT_ATTEND_DAY "KEY_MONTHLY_EVENT_ATTEND_DAY"
#define KEY_MONTHLY_EVENT_INFO_MONTH "KEY_MONTHLY_EVENT_INFO_MONTH"
#define KEY_MONTHLY_EVENT_INFO_RECEIVE_COUNT "KEY_MONTHLY_EVENT_INFO_RECEIVE_COUNT"
#define KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT "KEY_MONTHLY_EVENT_INFO_PROGRESS_FORMAT_%d"
#define KEY_ARENA_RID "KEY_ARENA_RID"
#define KEY_RID "KEY_RID"
#define KEY_DIFFICULTY_MODE "KEY_DIFFICULTY_MODE"
#define KEY_ROULETTE_VIDEO_BUTTON_ENABLED "KEY_ROULETTE_VIDEO_BUTTON_ENABLED"
#define KEY_MID_MONTH_ROULETTE_ROLL_DAY "KEY_MID_MONTH_ROULETTE_ROLL_DAY"
#define KEY_PVP6_TROPHY "KEY_PVP6_TROPHY"
#define KEY_PVP12_TROPHY "KEY_PVP12_TROPHY"
#define KEY_PVP6_LAST_TIME_LEFT "KEY_PVP6_LAST_TIME_LEFT"
#define KEY_PVP12_LAST_TIME_LEFT "KEY_PVP12_LAST_TIME_LEFT"
#define KEY_TROPHY "KEY_TROPHY"
#define KEY_IAP_TOTAL "KEY_IAP_TOTAL"
#define KEY_IAP_LIST "KEY_IAP_LIST"
#define KEY_KEYS "KEY_KEYS"
#define KEY_KEY_GET_STATE "KEY_KEY_GET_STATE"
#define KEY_POST_MESSAGE "KEY_POST_MESSAGE"
#define KEY_LAST_LAUNCH_DATE "KEY_LAST_LAUNCH_DATE"
#define KEY_SHIELD_END_TIME "KEY_SHIELD_END_TIME_STR"
#define KEY_FIRST_LAUNCH_DATE "KEY_FIRST_LAUNCH_DATE"
#define KEY_ATTEND_RCV_INFO "KEY_ATTEND_RCV_INFO_"
#define KEY_GIFT_DAY "KEY_GIFT_DAY"
#define KEY_DAY_COUNT "DayCount"
#define KEY_TODAYS_SCORE "TodaysScore"
#define KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME "KEY_LAST_VIDEO_FOR_TICKET_WATCHED_TIME_STR"
#define KEY_LAST_VIDEO_FOR_PVP6_TICKET_WATCHED_TIME "KEY_LAST_VIDEO_FOR_PVP6_TICKET_WATCHED_TIME"
#define KEY_LAST_VIDEO_FOR_PVP12_TICKET_WATCHED_TIME "KEY_LAST_VIDEO_FOR_PVP12_TICKET_WATCHED_TIME"
#define KEY_MID_MONTH_ROULETTE_VIDEO_WATCHED_TIME "KEY_MID_MONTH_ROULETTE_VIDEO_WATCHED_TIME"

#define ACHIEVEMENT_COMPLETE_TUTORIAL 0
#define ACHIEVEMENT_BEAT_BOSS 1
#define ACHIEVEMENT_OPEN_GOLD_SUITCASE 2
#define ACHIEVEMENT_PERFECT_SCORE 3
#define ACHIEVEMENT_WHO_IS_THE_HOOD 4
#define ACHIEVEMENT_WEAPON_MASTER 5
#define ACHIEVEMENT_DUNGEON_MASTER 6
#define ACHIEVEMENT_WHERE_IS_THE_PRINCESS 7
#define ACHIEVEMENT_OPEN_JEWELRY_BOX 8
#define ACHIEVEMENT_COLLECT_RELICS 9


#define PAGE_TITLE 0
#define PAGE_THEME_SELECT 1
#define PAGE_STAGE_SELECT 2
#define PAGE_EGG 3
#define PAGE_INVENTORY 4
#define PAGE_COLLECTION 5
#define PAGE_AVHIEVE 6
#define PAGE_SHOP 7
#define PAGE_SETTING 8
#define PAGE_STAGE 9
#define PAGE_START_POPUP 10
#define PAGE_WEAPON_POPUP 11

#define CLOSE_TO_NEXT 0
#define CLOSE_TO_RETRY 1
#define CLOSE_TO_STAGES 2
#define CLOSE_TO_MAIN 3

#define DEFAULT_CRITICAL 5

#define VIDEO_GAME_OVER 1
#define VIDEO_WEAPON_RENT 2
#define VIDEO_GAME_CONTINUE 3
#define VIDEO_SUPPORT_0 4
#define VIDEO_SUPPORT_1 5
#define VIDEO_SUPPORT_2 6
#define VIDEO_TICKET 7
#define VIDEO_SHIELD 8
#define VIDEO_STORE_RANDOM 9
#define VIDEO_STORE_UNIT 10
#define VIDEO_STORE_GEM 11
#define VIDEO_GACHA 12
#define VIDEO_PVP6_TICKET 13
#define VIDEO_PVP12_TICKET 14
#define VIDEO_MIDMONTH_ROULETTE 15

#define EVENT_MISSION_ATTEND 0
#define EVENT_MISSION_COLLECT_EVENT_HERO_PART 1
#define EVENT_MISSION_KILL_GOBLIN 2
#define EVENT_MISSION_KILL_SWORDSMAN 3
#define EVENT_MISSION_KILL_TROLL 4
#define EVENT_MISSION_KILL_ORC_SPEAR 5
#define EVENT_MISSION_KILL_ORC_AXE 6
#define EVENT_MISSION_KILL_ZOMBIE 7
#define EVENT_MISSION_ARENA_CLEAR 8
#define EVENT_MISSION_PVP_6_WIN 9
#define EVENT_MISSION_PVP_12_WIN 10
#define EVENT_MISSION_BATTLE_NETWORK_WIN 11
#define EVENT_MISSION_TOTAL_COUNT 12

//static const Color3B greenColor = {5,126,45};
//static const Color3B yesColor = {241,74,1};
//static const Color3B noColor = {46,124,179};

#define DIRECTION_UP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3

using namespace cocos2d;

class GameManager : public Node
{
private:
    int bulletType;
    
    //Constructor
    GameManager();
    
    //Instance of the singleton
    static GameManager* m_mySingleton;
    HelloWorld* stageLayer;
    HudLayer* hudLayer;
    EditorHud* editorHud;
    Layer* pauseLayer;
    Layer* optionLayer;
    Layer* gameOverLayer;
    Layer* achievementLayer;
    
    Scene* gameOverScene;
    HelloWorld* currentStageLayer = nullptr;
    int sustainSoundTag=-1;
    
    
    char c0;
    char c1;
    char c2;
    char c3;
    char c4;
    char c5;
    char c6;
    
    char g0;
    char g1;
    char g2;
    char g3;
    char g4;
    char g5;
    char g6;
    
    char s0;
    char s1;
    char s2;
    char s3;
    char s4;
    
    int currentBGM;
    std::string userDefaultData;
    
public:
    BattleHud* battleHud = nullptr;
    int collectedJewelIndex = -1;
//    bool isShieldPurcahsed();
//    void setShield(long time);
//    long getShieldPurchasedTime();
    void shakeIt(Node* node);
    bool mapUpdatedNeeded = false;
    bool isWeaponSet=false;
    int lifeUpdateNeeded;
    int life;
    Layer* settingLayer;
    const char* getFont(int font);
    void initGameManager();
    Layer* titleLayer = nullptr;
    Layer* heroPage = nullptr;
    Layer* worldMap;
    bool isGuestPlay;
    bool isVideoReady;
    bool isUsingController;
    const char* convertMyUserDefaultToCharArray();
    void convertCharArrayToMyUserDefault(const char* data);

    int totalThemeCount;
    int totalStage;
    bool gameStarted;
    bool initComplete;
    int recommendedWeapon;
    Sprite* myPhoto;
    void setOutLine(Sprite* spt);
    void saveGameData();
    void setLoadedData(const char* key, const char* value);
    
    cocos2d::Vec2 getGemCountPosition();
    bool isPaidGame;
    bool isInMiddleOfGame;
    bool isLoadingGame = false;
    int loadingSlot = -1;
    bool firstPlayed;
    const char* version;
    
    int versionCode = 0;
    int receivedVersionCode = -1;
    int serverMaintenance = 0;
    int rewardCode = 0;
    int rewardedCode = 0;
    int rewardGemCount = 0;
    std::string rewardMsg = "";
    cocos2d::Size originalSize;
    cocos2d::Size size;
    bool isStageSetOnce;
    int currentStageIndex = -1;
    Color3B getRankColor(int rank);
    bool leftPressed;
    bool rightPressed;
    bool downPressed;
    bool upPressed;
    bool jumpPressed;
    bool cPressed;
    bool firePressed;
    int market;
    int dayCount=-1;
    int ranking=-1;
    int page;
    bool showVPad;
    bool developer;
    bool canJumpForever;
    bool paidUser;
    bool appAlreadyLaunched;
    int theme;
    const char* currentUserID;
    //Get instance of singleton
    static GameManager* getInstance();
    void showVideoDone();
    void showVideoFailed();
    void showVideo(int which);
    int videoIndex;
    bool isVideoDone = false;
    void clickAdsDone();
    void clickAdsFailed();
    void exitGame();
    bool isVideoRewardEnergy;
    bool isVideoRewardAttack;
    bool isVideoRewardCoins;
    void setFontName(Label* lbl, const char* name, float fontSize);
    int getGemForCoin(int coinCount);
    //A function that returns zero "0"
    int ReturnZero(){return 0;}
    // another test function
    void runScene() { CCLOG("test");};
    void buttonDown(int buttonType);
    void buttonUp(int buttonType);
    void setCoin(int howMuch);
    void addCoin(int howMuch);
    int getCoin();
    int getSoul();
    void addSoul(int howMuch);
    void saveCoin();
    void addGem(int howMuch);
    void setGem(int howMuch);
    int getGem();
    void addTree(int howMuch);
    void setTree(int howMuch);
    int getTree();
    void setFontSize(Label* lbl, float size);
    int getGameLevel();
    int getCurrentExp();
    int getCurrentExpMax();
    void showDisposableMessage(std::string msg, Node* parent);
    bool checkDataSecure();
    void addGameExp(int exp);
    void addGameLevel();
    int getRandomFood();
    int getFoodCount(int food);
    void addFoodCount(int food, int count);
    const char* getFoodSpriteName(int food);
    int getTotalStarCount();
    int getWeaponPrice(int index);
    
    void preLoadAllSoundEffect();
    void playSoundEffect(int sound, float gain = 1.0f, float pan = 0);
    void stopSoundEffect(int sound);
    void makeLabelEllipsis(Label* lbl, float width);
    void setMusicVolumn(float vol);
    void setSoundVolumn(float vol);
    void setButtonSize(float siz);
    void setNotificationOn(bool onOff);
    int getBombCount();
    long getTodaysRandom();
    bool isFacebookReady;
    float getDistance(Node* target, Node* source);
    float getDistance(Vec2 target, Vec2 source);
    float getSoundVolumnByDistance(Node* target, Node* source);
    void setFacebookReady(bool ready);
    void setBombCount(int bomb);
    int getPotionCount();
    void setPotionCount(int bomb);
    float getMusicVolumn();
    float getSoundVolumn();
    float getButtonSize();
    bool getNotificationOn();
    double getAngle(cocos2d::Vec2 pos1, cocos2d::Vec2 pos2);
    void SendAchievement(const char* text, int count, int goal);
    const char* getWeaponImageFileName(int weapon);
    void makeItSiluk(Node* node);
    void makeItSiluk(Node* node, float delay, float dur);
    void makeItScaleUpAndDown(Node* node);
    void addMastery(int stageIndex);
    int getMastery(int stageIndex);
    int getWeaponPowerLevel(int weapon);
    void addWeaponPowerLevel(int weapon);
    int getWeaponCriticalLevel(int weapon);
    void addWeaponCriticalLevel(int weapon);

    
    void scrollTheLayer(ui::ScrollView* scrollLayer, bool isLeft, bool isHorizontal, int howMuch);
//    HelloWorld* getStageLayer();
    HudLayer* getHudLayer();
    EditorHud* getEditorHud();
    Scene* getTitleScene();
    Layer* getGameStartLayer();
    Scene* getGameStartScene();

    Layer* getTitleLayer();
    Layer* getShopLayer();
    Layer* getPauseLayer();
//    Layer* getGameOverLayer();
//    Scene* getGameOverScene();
    const char* getBulletName(int weaponType, int playerMissileDemage);
    Layer* getOptionLayer();
    Layer* getAchievementLayer();
    void setCurrentStageLayer(HelloWorld* layer);
    const char* getShortenedKoreanString(std::string str, int length);
    HelloWorld* getWorld();
    
//    void setStageLayer(Layer* layer);
    void setHudLayer(HudLayer* layer);
    void setEditorHud(EditorHud* layer);
    void setStageScene(Scene* scene);
//    RepeatForever* getScaleUpDownAction(float scale);
    
    int getBulletType();
    bool DoesWeaponExist(int weaponType);
    int getNextToThisWeapon(int weaponType);
    int getNextWeaponToUnlock(int weaponType);
    
    int getWeaponPower(int weaponType);
    
    bool getWeaponUnlocked(int weapon);
    bool getCharacterUnlocked(int costume);
    const char* getCharacterUnlockedKey(int costume);
    const char* getCharacterName(int costume);
    const char* getCharacterDesc(int costume);
    
    void pushLayer(Layer* parent, Layer* layer);
    void pushLayerWithoutDisable(Layer* parent, Layer* layer);
    void popLayer(Layer* layer);
    void popLayerWithoutAnimation(Layer* layer);
    void animateFadeIn(Node* layer, Node* parent);
    void animateFadeOut(Node* layer);
    void animationFadeInDone(Node* layer);
    void animationFadeOutDone(Node* layer);
    
//    void disableLayer(Layer* layer);
//    void enableLayer(Layer* layer);
    
    void showParticleExplosion(Node* prt, const char* sptName, cocos2d::Vec2 pos, float scale);
    void showSpriteExplosion(Node* parent, const char* sptName, Vec2 pos, int sptCount, float radius, float dur);
    void scheduleLocalNotification(const char* title, const char* msg, int time);
    void scheduleLocalNotification();
    void unscheduleAllNotifications();
    long strToLong(const char* str);
    Sprite* getGrayScaleImage(std::string src);
//    RenderTexture* createAdditiveBorder( Sprite* label, int size, Color3B color, GLubyte opacity );
//    RenderTexture* createAdditive( Sprite* label, Color3B color, GLubyte opacity, int additiveCount);
    Sprite* getSpriteShapeSolidColorSprite(const char* src, Color3B color);
    bool isBoss(int theme, int stage);
    
    void reset();
    void backup();
    
    
    void arrangeInvetory();
    int getInvetoryCount();
    void initAchievement();
    void setAchievementGoal(int achievementIndex, int goalCount);
    bool setAchievement(int achievementIndex, int countToAdd);
    const char* getAchievementId(int achievementIndex);
    void googleSignInOrOut(bool sign);
    bool isGoogleSigned;
    int getAchievementGoalCount(int achievementIndex);
    int getAchievementCurrentCount(int achievementIndex);
    
    void setCharacterPurchased(int character);
    bool isCharacterPurchased(int character);
    void setWeaponPurchased(int weapon);
    bool isWeaponPurchased(int weapon);
    
    void showInterstitialAds();
    int getInventoryCount();
    
    void nativeControllerButtonEvent(int controller, int button, bool isPressed, float value, bool isAnalog);
    
    void nativeControllerLAxisXEvent(int controller, float value);
    void nativeControllerLAxisYEvent(int controller, float value);
    
    Layout* waitingLayer;
    void showWaiting(Node* parent, cocos2d::Size size);
    void closeWaiting();
    
    ValueVector split(const std::string &str, const std::string &delim);
    
    bool isPlayerSet = false;
    int collectedKeyCount=0;
    void setWeaponCollected(int index);
    
    
    // menu selecting by keyboard
    Node* findUp(Node* currentNode);
    Node* findDown(Node* currentNode);
    Node* findLeft(Node* currentNode);
    Node* findRight(Node* currentNode);
    Node* findWithDirect(cocos2d::Vec2 pos, int direction, Node* currentPanel);
    Node* getNodeAtThisPoint(cocos2d::Vec2 pos, Node* currentNode);
    
    bool isOnlineEnabled = true;
    
    int totalKill=0;
    int totalDestroy=0;
    int totalCoin=0;
    int totalRoom=0;
    int totalBoss=0;
    float totalTime=60*60;
    int totalScore=0;
    
    bool jewelryCollected = false;
    bool isAutoTargetSelected = false;
    Label* lastDisposableLabel = nullptr;
    Sprite* lastDisposableLabelBack = nullptr;
    
    void loadCSV(ValueMap& map, std::string fileName);
    void runAnimation(Sprite* spt, const char* name, bool repeat, bool deleteAfterPlay=false);
    
    
    
    bool isTransported = false;
    int nextScene = 0;
    void setPathState(int x, int y, int state);
    cocos2d::Size mapSize;
    PointArray* getPath(cocos2d::Vec2 start, cocos2d::Vec2 end);
    PointArray* getPathOld(cocos2d::Vec2 start, cocos2d::Vec2 end);
    void getNextAvailableWayAlongWall(int &firstX, int &firstY, int &secondX, int &secondY, int direction, int x, int y, int &firstDirection, int &secondDirection);
    bool getNextAvailableWay(int &firstX, int &firstY, int direction, int x, int y, int &firstDirection);
    void getNextXYForDirection(int direction, int centerX, int centerY, int &x, int &y, int &newDirection);
    int testStepLimit = 1;
    cocos2d::Vec2 testStartPos;
    cocos2d::Vec2 testEndPos;
    void inspectSurrounded(int x, int y, int distanceFromStart);
    void inspectSurroundedNew(int x, int y, int distanceFromStart);
    int inspectPathStackCount = 0;
    int inspectXArray[100];
    int inspectYArray[100];
    int inspectDistanceArray[100];
    void pushInspectInfo(int x, int y, int distanceFromStart);
    void popFirstInspectInfo(int &x, int &y, int &distanceFromStart);
    void inspectSurroundedStack(int x, int y, int distanceFromStart);
    void inspectSurroundedStack();
    
    cocos2d::Size getBuildingOccupySize(int unit);
    int occupyState[100][100];
    char tileState[100][100];
    char blockState[100][100];
    int closeStateTracker[10000];
    int closeStateTrackIndex = 0;
    char blockStateLastPosition[100][100];
    char blockStateFromStart[100][100];
    char blockStateToEnd[100][100];
    bool isPathFound = false;
    int foundPathDistance = 9999999;
    cocos2d::Vec2 pathStart;
    cocos2d::Vec2 pathEnd;
    cocos2d::Vec2 finalEnd;
    void alignToCenter(Node* node0, Node* node1, float gap, float centerX, float offsetX);
    void drawPath();
    int pathFindingCounter = 0;
    void shakeIt(Node* node, float shakeWidth, int shakeCount, float shakeTime = 0.07f);
    cocos2d::Vec2 getRandomPosInCicle(cocos2d::Vec2 center, float radius);
    
    std::string uuid;
    void setTimeLeft(Label* lbl, int time);
    void setTimeLeft(Text* lbl, int time);
    void setTimeLeft(TextBMFont* lbl, int time);
    std::string getTimeLeftInString(int time);
    std::string getTimeLeftInStringHMS(int time);
    
    void makeLabelShrink(Text* lbl);
    void makeLabelShrink(Label* lbl);
    void makeAdditive(Sprite* spt, Color3B color);
    
    bool isReviving = false;
    int getStageObjective(int stage);

    void onIapSuccess();
    void onIapFailed();
    std::string iapDetail = "";
    int iapFlag = 0;
    
    
    std::string compress_string(const std::string& str, int compressionlevel = Z_BEST_SPEED);
    std::string decompress_string(const std::string& str);
    std::string compressString(std::string data);
    std::string decompressString(std::string data);
    
    std::string loadMapData = "";
    std::string playedMapID = "";
    bool isColosseum = false;
    bool isCrossShown = false;
    bool isThisBuilding(int unitType);
    Node* crossPromotionLayer = nullptr;
    bool isVideoButtonAvailable = true;
    void enableVideoButton();
    

    bool isAdsUser();
 
    AStar* astar;
    
    int makeNumberCloseTo(int source, int target);
    
    void enableButton(Ref* ref);
    
    
    std::string getUnitName(int index);
    int getGemForTimeLeft(int timeLeft);
    int singlePlayStageIndex = 0;
    std::string raidEnemyID = "enemyID";
    std::string raidEnemyRID = "";
    std::string raidEnemyName = "enemyName";
    int raidEnemyTrophy = 0;
    int raidEnemyGold = 0;
    int raidEnemyTree = 0;
    std::string raidEnemyBuildings = "enemyBuildings";
    std::string raidEnemyDeck = "enemyDeck";
    std::string raidEnemyHeroDeck = "enemyDeck";
    std::string raidEnemyHeroPos = "";
    int raidMatchState = 0;
    UnitInfo* getUnitInfoFromString(std::string str);
    void loadBattleData();
    ValueMap buildingCountForCastleLevelTable;
    ValueMap buildingPriceForCastleLevelTable;
    ValueMap buildingUpgradePriceForLevelTable;
    ValueMap buildingAbilityForCastleLevelTable;
    ValueMap castleStorageForCastleLevelTable;
    ValueMap unitCompleteTimeTable;
    ValueMap unitHPTable;
    ValueMap unitATTTable;
    Movable* getUnitFromData(UnitInfo* str);
    
    int getUnitHP(int index, int level);
    int getUnitATT(int index, int level);
    void showNode(Node* node, float delay);
    int getGemForHour();
    
    int getDayDiff(int fromY, int fromM, int fromD, int toY, int toM, int toD);
    
    
    int getFoodUseForUnit(int index);
    
    bool isVisiting = false;
    void addConsumedItem(std::string strSkuName);
    void onRestored(std::string strSkuID);
    std::vector<std::string> consumeSkuNameList;
    void rollOpenScroll(ImageView* imgBack);
    
    int raidRewardGold = 0;
    int raidRewardTree = 0;
    int raidRewardTrophy = 0;
    
    double restingForBattleEndTime = 0;
    
    void replaceImageWithSprite(ImageView* img, Sprite* spt);
    
    std::string playerGPSID = "";
    std::string playerIDWithGPSID = "";
    
    int getIntForOldDouble(std::string key, int defaultValue = 0);
    void addLightStormEffect(Node* node);
    
//    int directionStatic = 0;
    std::string getSpineFileName(int unitType);
    
    void addWoodKey(int amount);
    void addGoldKey(int amount);
    void setWoodKey(int amount);
    void setGoldKey(int amount);
    int getWoodKey();
    int getGoldKey();
    
    
    spine::SkeletonAnimation* getHeroSpine(int unitType);
    
//    bool isHeroLoaded = false;
    
    int getMaxGold(int level);
    int getMaxGold();
    int getMaxTree();
    int getMaxTree(int level);
    
    int getElement(int unit);
    
    int playedStageIndexForTitle = -1;
    
    void addYellowRisingBallEffect(Node* parent);
    void addGlowBack(Node* node);
    bool isBattleDataLoadComplete = false;
    int matchFindCount = 0;
    
    void setTextToNumberByTag(Ref* ref);
    int getHeroSkillRate(int unitType, int rank);
    
    void addMonthlyEventProgress(int index, int count);
    void resetMonthlyEventProgress(int receivedCount);
    void checkIsNewMonthlyEvent();
    int getMonthlyEventProgressMax(int index);
//    std::string getMonthlyEventInfoString();
    
    int getTimeLeftForCampaignChest(int index);
    std::string strSmartPassError = "";
    
    void saveTime(const char* key, time_t time);
    time_t getSavedTime(const char* key);
    
    void saveUnitInfo(const char* key, UnitInfo* info);
    UnitInfo* getSavedUnitInfo(const char* key);
    std::vector<UnitInfo*> getHeroInventory();
    void saveHeroInventory(std::vector<UnitInfo*> list);
    std::vector<UnitInfo*> getHeroDeck();
    void saveHeroDeck(std::vector<UnitInfo*> list);
    std::vector<UnitInfo*> getBattleUnitDeck();
    void saveBattleUnitDeck(std::vector<UnitInfo*> list);
    std::vector<UnitInfo*> getBattleUnitInventory();
    void saveBattleUnitInventory(std::vector<UnitInfo*> list);
    int getDailyMissionCampaignStageIndex();
    int lastGem = -1;
    int isThisCampaignFromDailyMission = false;
    
    int getPathCall = 0;
    int getMidMonthHeroType();
    int getMidMonthHeroTimeLeft();
    int getMonthlyHeroType();
    
    void resetAsset();
    
    bool isPvpFromBHUD = false;
    int pvpOpenDate = 0;
    
    int getUnitAP(int unit);
    int getUnitMaxHP(int unit);


    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }
};

#endif
