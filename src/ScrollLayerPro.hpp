#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ScrollLayerPro : public geode::ScrollLayer {

protected:
    std::vector<Ref<CCMenuItem>> m_buttons;
    ScrollLayerPro(cocos2d::CCRect const& rect, bool scrollWheelEnabled, bool vertical);

public:
    void addButtons(std::vector<Ref<CCMenuItem>> buttons);
    void setButtonsEnabled(bool on);

    void ccTouchMoved(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) override;
    void ccTouchEnded(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) override;
    static ScrollLayerPro* create(
            cocos2d::CCRect const& rect, bool scrollWheelEnabled = true, bool vertical = true
    );
    static ScrollLayerPro* create(
        cocos2d::CCSize const& size, bool scrollWheelEnabled = true, bool vertical = true
    );
};