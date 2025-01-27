#include "HoverableCCMenuItemSpriteExtra.hpp"
#include "hooks/CCEGLView.hpp"

bool HoverableCCMenuItemSpriteExtra::init(cocos2d::CCNode* sprite, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback, std::function<void(CCObject* sender, CCPoint, bool, bool)> hoverCallback) {
    m_hoverFunction = hoverCallback;
    Hover::get()->addHoverableItem(this);
    return CCMenuItemSpriteExtra::init(sprite, nullptr, target, callback);
}

ObjectSelectPopup* HoverableCCMenuItemSpriteExtra::getCurrentPopup() {
    return m_selectPopup;
}

void HoverableCCMenuItemSpriteExtra::setPopup(ObjectSelectPopup* selectPopup) {
    m_selectPopup = selectPopup;
}

HoverableCCMenuItemSpriteExtra* HoverableCCMenuItemSpriteExtra::create(cocos2d::CCNode* sprite, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback, std::function<void(CCObject* sender, CCPoint, bool, bool)> hoverCallback) {

    auto ret = new HoverableCCMenuItemSpriteExtra();
    if (ret->init(sprite, target, callback, hoverCallback)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void HoverableCCMenuItemSpriteExtra::hover(CCObject* sender, CCPoint point, bool hovering) {
    if (m_hoverFunction) {
        m_hoverFunction(sender, point, hovering, (hovering && !m_inHover) || (!hovering && m_inHover));
    }
    m_inHover = hovering;
}

HoverableCCMenuItemSpriteExtra::~HoverableCCMenuItemSpriteExtra() {
    Hover::get()->removeHoverableItem(this);
}