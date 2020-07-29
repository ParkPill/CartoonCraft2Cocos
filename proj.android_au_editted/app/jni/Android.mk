LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := $(LOCAL_PATH)/hellocpp/main.cpp \
                   $(LOCAL_PATH)/../../../Classes/AppDelegate.cpp \
                   $(LOCAL_PATH)/../../../Classes/Array2D.h \
                   $(LOCAL_PATH)/../../../Classes/AStar.cpp \
                   $(LOCAL_PATH)/../../../Classes/AwesomeDialogBox.cpp \
                   $(LOCAL_PATH)/../../../Classes/BattleHud.cpp \
                   $(LOCAL_PATH)/../../../Classes/BattleLobby.cpp \
                   $(LOCAL_PATH)/../../../Classes/BuggyServerManager.cpp \
                   $(LOCAL_PATH)/../../../Classes/Cell.h \
                   $(LOCAL_PATH)/../../../Classes/EditorHud.cpp \
                   $(LOCAL_PATH)/../../../Classes/EditorWorld.cpp \
                   $(LOCAL_PATH)/../../../Classes/EndingCredit.cpp \
                   $(LOCAL_PATH)/../../../Classes/Enemies.cpp \
                   $(LOCAL_PATH)/../../../Classes/EventInfo.cpp \
                   $(LOCAL_PATH)/../../../Classes/EventPopup.cpp \
                   $(LOCAL_PATH)/../../../Classes/FindMatch.cpp \
                   $(LOCAL_PATH)/../../../Classes/Fog.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameManager.cpp \
                   $(LOCAL_PATH)/../../../Classes/GameSharing.cpp \
                   $(LOCAL_PATH)/../../../Classes/HelloWorldScene.cpp \
                   $(LOCAL_PATH)/../../../Classes/HeroPage.cpp \
                   $(LOCAL_PATH)/../../../Classes/HudLayer.cpp \
                   $(LOCAL_PATH)/../../../Classes/Intro.cpp \
                   $(LOCAL_PATH)/../../../Classes/Inventory.cpp \
                   $(LOCAL_PATH)/../../../Classes/LanguageManager.cpp \
                   $(LOCAL_PATH)/../../../Classes/Laser.cpp \
                   $(LOCAL_PATH)/../../../Classes/LegendDaryButton.cpp \
                   $(LOCAL_PATH)/../../../Classes/MonthlyEventPopup.cpp \
                   $(LOCAL_PATH)/../../../Classes/Movable.cpp \
                   $(LOCAL_PATH)/../../../Classes/MultiplayLobby.cpp \
                   $(LOCAL_PATH)/../../../Classes/PPLabel.cpp \
                   $(LOCAL_PATH)/../../../Classes/PageBase.cpp \
                   $(LOCAL_PATH)/../../../Classes/RoulettePopup.cpp \
                   $(LOCAL_PATH)/../../../Classes/ServerManager.cpp \
                   $(LOCAL_PATH)/../../../Classes/ShopLayer.cpp \
                   $(LOCAL_PATH)/../../../Classes/SneakyJoystick.cpp \
                   $(LOCAL_PATH)/../../../Classes/SneakyJoystickSkinnedBase.cpp \
                   $(LOCAL_PATH)/../../../Classes/TopBar.cpp \
                   $(LOCAL_PATH)/../../../Classes/Title.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
                    $(LOCAL_PATH)/../../../Source

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/../../../cocos2d)
$(call import-module, cocos)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
