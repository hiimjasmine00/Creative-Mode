#include "ScrollLayerPro.hpp"

void ScrollLayerPro::addButtons(std::vector<Ref<CCMenuItem>> buttons) {
    m_buttons = buttons;
    queueInMainThread([this] {
        if (auto delegate = typeinfo_cast<CCTouchDelegate*>(this)) {
            if (auto handler = CCTouchDispatcher::get()->findHandler(delegate)) {
                CCTouchDispatcher::get()->setPriority(handler->getPriority() - 1, handler->getDelegate());
            }
        }
    });
}

void ScrollLayerPro::setButtonsEnabled(bool on){
    for (CCMenuItem* btn : m_buttons) {
        btn->setEnabled(on);
        btn->stopAllActions();
        btn->unselected();
    }
}

void ScrollLayerPro::ccTouchMoved(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
	CCScrollLayerExt::ccTouchMoved(p0, p1);
    float dY = std::abs(p0->getStartLocation().y - p0->getLocation().y);
    if (dY > 3) {
        setButtonsEnabled(false);
    }
}

void ScrollLayerPro::ccTouchEnded(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
	CCScrollLayerExt::ccTouchEnded(p0, p1);
    setButtonsEnabled(true);
}

ScrollLayerPro::ScrollLayerPro(CCRect const& rect, bool scrollWheelEnabled, bool vertical) : ScrollLayer(rect, scrollWheelEnabled, vertical) {}

ScrollLayerPro* ScrollLayerPro::create(CCRect const& rect, bool scroll, bool vertical) {
    auto ret = new ScrollLayerPro(rect, scroll, vertical);
    ret->autorelease();
    return ret;
}

ScrollLayerPro* ScrollLayerPro::create(CCSize const& size, bool scroll, bool vertical) {
    return ScrollLayerPro::create({ 0, 0, size.width, size.height }, scroll, vertical);
}