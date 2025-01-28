#include "ScrollLayerPro.hpp"
#include "HoverableCCMenuItemSpriteExtra.hpp"

void ScrollLayerPro::addButtons(std::vector<CCMenuItem*> buttons) {
    m_buttons = buttons;
}

void ScrollLayerPro::clearButtons() {
    for (CCMenuItem* btn : m_buttons) {
        static_cast<HoverableCCMenuItemSpriteExtra*>(btn)->setPopup(nullptr);
        btn->removeFromParentAndCleanup(false);
    }
    m_buttons.clear();
}

void ScrollLayerPro::clearRows() {
    for (CCNode* row : m_rows) {
        row->removeFromParent();
    }
    m_rows.clear();
}

void ScrollLayerPro::cleanupScroll() {
    clearButtons();
    clearRows();
    m_dragCallback = nullptr;
    unschedule(schedule_selector(ScrollLayerPro::listenForPosChange));
}

void ScrollLayerPro::addRows(std::vector<CCNode*> nodes, float rowHeight, int visibleRowCount) {
    m_rows = nodes;
    m_rowHeight = rowHeight;
    m_rowCount = visibleRowCount;
    int start = 0;
    for (CCNode* row : m_rows) {
        row->setVisible(start < visibleRowCount);
        start++;
    }
}

void ScrollLayerPro::setButtonsEnabled(bool on){
    for (CCMenuItem* btn : m_buttons) {
        btn->setEnabled(on);
        btn->stopAllActions();
        btn->unselected();
    }
}

void ScrollLayerPro::updateRowVisibility() {
    //delay a frame so m_rows is cleared before this is ran again when cleaned up (rare but possible crash)
    queueInMainThread([this] {
        float scrollPos = m_contentLayer->getPositionY();

        float rowsHeight = m_rowCount * m_rowHeight;

        float rowMin = -(scrollPos) - m_rowHeight ;
        float rowMax = -(scrollPos) + rowsHeight + m_rowHeight;

        for (CCNode* row : m_rows) {
            float rowY = row->getPosition().y;
            row->setVisible(rowY > rowMin && rowY < rowMax);
        }
    });
}

void ScrollLayerPro::listenForPosChange(float dt) {
    if (m_positionY != m_contentLayer->getPositionY()) {
        m_positionY = m_contentLayer->getPositionY();
        updateRowVisibility();
    }
}

void ScrollLayerPro::ccTouchMoved(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
	CCScrollLayerExt::ccTouchMoved(p0, p1);
    m_dragCallback(true);

    if (m_buttonsDisabled) return;
    float dY = std::abs(p0->getStartLocation().y - p0->getLocation().y);
    if (dY > 3) {
        m_buttonsDisabled = true;
        setButtonsEnabled(false);
    }
}

void ScrollLayerPro::ccTouchEnded(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
	CCScrollLayerExt::ccTouchEnded(p0, p1);
    setButtonsEnabled(true);
    m_buttonsDisabled = false;
    m_dragCallback(false);
}

ScrollLayerPro::ScrollLayerPro(CCRect const& rect, std::function<void(bool)> dragCallback, bool scrollWheelEnabled, bool vertical) : ScrollLayer(rect, scrollWheelEnabled, vertical) {
    m_dragCallback = dragCallback;
    schedule(schedule_selector(ScrollLayerPro::listenForPosChange), 1/240.f);
}

ScrollLayerPro* ScrollLayerPro::create(CCRect const& rect, std::function<void(bool)> dragCallback, bool scroll, bool vertical) {
    auto ret = new ScrollLayerPro(rect, dragCallback, scroll, vertical);
    ret->autorelease();
    return ret;
}

ScrollLayerPro* ScrollLayerPro::create(CCSize const& size, std::function<void(bool)> dragCallback, bool scroll, bool vertical) {
    return ScrollLayerPro::create({ 0, 0, size.width, size.height }, dragCallback, scroll, vertical);
}