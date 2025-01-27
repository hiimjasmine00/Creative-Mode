#pragma once

#include <Geode/Geode.hpp>
#include "popups/ObjectSelectPopup.hpp"

using namespace geode::prelude;

class HoverableCCMenuItemSpriteExtra : public CCMenuItemSpriteExtra {

protected:
    std::function<void(CCObject* sender, CCPoint, bool, bool)> m_hoverFunction;
    bool m_inHover;
    ObjectSelectPopup* m_selectPopup;
public:
    bool init(cocos2d::CCNode* sprite, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback, std::function<void(CCObject* sender, CCPoint, bool, bool)> hoverCallback);
    void hover(CCObject* sender, CCPoint point, bool hovering);
    ObjectSelectPopup* getCurrentPopup();
    void setPopup(ObjectSelectPopup*);
    static HoverableCCMenuItemSpriteExtra* create(cocos2d::CCNode* sprite, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback, std::function<void(CCObject* sender, CCPoint, bool, bool)> hoverCallback);

    ~HoverableCCMenuItemSpriteExtra();
};