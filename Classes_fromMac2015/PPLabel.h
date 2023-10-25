//
//  PPLabel.hpp
//  CartoonCraft
//
//  Created by PACKSUNG PILL on 20/03/2018.
//

#ifndef PPLabel_hpp
#define PPLabel_hpp

#include <stdio.h>

#include "cocos2d.h"

using namespace cocos2d;
class PPLabel : public Node
{
private:
    
public:
    SpriteBatchNode* spriteBatch;
    Label* lblNormal = nullptr;
    virtual bool init() override;
    Vector<Sprite*> sprites;
    void setWordColor(std::string str, Color3B color);
    CREATE_FUNC(PPLabel);
    static PPLabel* create(std::string txt, int size, Color3B color = Color3B::WHITE, bool isSystemLabel = true);
    static PPLabel* create(std::string txt, int size, Color3B color, bool isSystemLabel, bool isBordered, TextHAlignment align, bool shadow);
    bool isImageLabel = false;
    void setString(std::string txt);
    void setFontSize(int size, bool refresh = true);
    void setFontColor(Color3B color, bool refresh = true);
    void setShadowEnabled(bool enable, float distance = 0, bool refresh = true);
    void setBorderEnabled(bool enable, bool refresh = true);
    void setTextHAlignment(TextHAlignment align, bool refresh = true);
    void setWidth(float width, bool refresh = true);
    float textWidth = -1;
    TextHAlignment hAlign = TextHAlignment::LEFT;
    int fontSize = 50;
    std::string text;
    bool isShadowEnabled = false;
    float shadowDistance = 8;
    bool isBorderEnabled = false;
    Color3B fontColor;
    cocos2d::Size getSize();
    float totalWidth = 0;
    float totalHeight = 0;
    virtual void setAnchorPoint(const Vec2& anchor) override;
//    virtual void setColor(const Color3B& color) override;
//    virtual void setOpacity(GLubyte opacity) override;
//    virtual Action* runAction(Action* action) override;
};

#endif /* PPLabel_hpp */
