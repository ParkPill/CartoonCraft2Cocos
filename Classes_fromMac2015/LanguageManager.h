//
//  LanguageManager.h
//  LegendDaryKakao
//
//  Created by SUNG PILL PACK on 14. 8. 13..
//
//

#ifndef __LegendDaryKakao__LanguageManager__
#define __LegendDaryKakao__LanguageManager__


#include <iostream>
#include "cocos2d.h"
#include "../cocos2d/cocos/ui/UIText.h"


#define STR_PET_NAME_0_BOOGI 23
#define STR_PET_NAME_1_BURI 24
#define STR_PET_NAME_2_CHUS 25
#define STR_PET_NAME_3_DORA_0 26
#define STR_PET_NAME_4_DORIS_0 27
#define STR_PET_NAME_5_FRITH_0 28
#define STR_PET_NAME_6_KIWI 29
#define STR_PET_NAME_7_KUMURI 30
#define STR_PET_NAME_8_LEXY 31
#define STR_PET_NAME_9_LINDIS_0 32
#define STR_PET_NAME_10_NEPTUN 33
#define STR_PET_NAME_11_OXTO 34
#define STR_PET_NAME_12_PLUTO 35
#define STR_PET_NAME_13_RIUS 36
#define STR_PET_NAME_14_ROLO_0 37
#define STR_PET_NAME_15_RURIK_0 38
#define STR_PET_NAME_16_TERRY 39
#define STR_PET_NAME_17_TORI 40
#define STR_PET_NAME_18_TORORI 41
#define STR_PET_NAME_19_UNO 42
#define STR_PET_NAME_20_VETH 43
#define STR_PET_NAME_21_WALOONG_0 44

#define STR_PET_DESCRIPTION_0_BOOGI 45
#define STR_PET_DESCRIPTION_1_BURI 46
#define STR_PET_DESCRIPTION_2_CHUS 47
#define STR_PET_DESCRIPTION_3_DORA_0 48
#define STR_PET_DESCRIPTION_4_DORIS_0 49
#define STR_PET_DESCRIPTION_5_FRITH_0 50
#define STR_PET_DESCRIPTION_6_KIWI 51
#define STR_PET_DESCRIPTION_7_KUMURI 52
#define STR_PET_DESCRIPTION_8_LEXY 53
#define STR_PET_DESCRIPTION_9_LINDIS_0 54
#define STR_PET_DESCRIPTION_10_NEPTUN 55
#define STR_PET_DESCRIPTION_11_OXTO 56
#define STR_PET_DESCRIPTION_12_PLUTO 57
#define STR_PET_DESCRIPTION_13_RIUS 58
#define STR_PET_DESCRIPTION_14_ROLO_0 59
#define STR_PET_DESCRIPTION_15_RURIK_0 60
#define STR_PET_DESCRIPTION_16_TERRY 61
#define STR_PET_DESCRIPTION_17_TORI 62
#define STR_PET_DESCRIPTION_18_TORORI 63
#define STR_PET_DESCRIPTION_19_UNO 64
#define STR_PET_DESCRIPTION_20_VETH 65
#define STR_PET_DESCRIPTION_21_WALOONG_0 66

using namespace cocos2d;
using namespace cocos2d::ui;
class LanguageManager : public Ref
{
 
private:
public:
    cocos2d::LanguageType getLanguageType();
    ValueMap languageTable;
    void loadLanguageSheet();
    
    LanguageManager();
    static LanguageManager* m_mySingleton;
    static LanguageManager* getInstance();
    
    std::string getText(std::string textId);
    cocos2d::Label* getLocalizedLabel(const char* text, Color4B color, int size = 40);
    cocos2d::Label* getLocalizedLabel();
    const char* getLocalizedFont();
    
    
    void setLocalizedString(Label* lbl, std::string str);
    void setLocalizedString(Text* lbl, std::string str);
    void setLocalizedStringNotKey(Text* lbl, std::string text);
};



#endif /* defined(__LegendDaryKakao__LanguageManager__) */



