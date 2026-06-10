#ifndef Win32MouseCursor_h
#define Win32MouseCursor_h

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

class Win32MouseCursor {
public:
    static bool setCursor(const std::string& filename, int hotspotX = 0, int hotspotY = 0);
    static void setDefaultCursor();
    static void setAttackMoveCursor();
    static void restoreSystemCursor();
};

#endif

#endif
