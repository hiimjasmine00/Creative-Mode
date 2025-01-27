#include "ScrollLayerPro.hpp"

void ScrollLayerPro::addButtons(std::vector<Ref<CCMenuItem>> buttons) {
    m_buttons = buttons;
}

void ScrollLayerPro::addRows(std::vector<Ref<CCNode>> nodes, float rowHeight, int visibleRowCount) {
    m_rows = nodes;
    m_rowHeight = rowHeight;
    m_rowCount = visibleRowCount;
}

void ScrollLayerPro::setButtonsEnabled(bool on){
    for (CCMenuItem* btn : m_buttons) {
        btn->setEnabled(on);
        btn->stopAllActions();
        btn->unselected();
    }
}

void ScrollLayerPro::listenForPosChange(float dt) {
    if (m_positionY != m_contentLayer->getPositionY()) {
        m_positionY = m_contentLayer->getPositionY();
        float scrollPos = m_contentLayer->getPositionY();

        float rowsHeight = m_rowCount * m_rowHeight;

        float rowMin = -(scrollPos) - m_rowHeight ;
        float rowMax = -(scrollPos) + rowsHeight + m_rowHeight;

        if (!m_rows.empty()) {
            for (CCNode* row : m_rows) {
                row->setVisible(row->getPosition().y > rowMin && row->getPosition().y < rowMax);
            }
        }
    }
}

void ScrollLayerPro::ccTouchMoved(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
	CCScrollLayerExt::ccTouchMoved(p0, p1);
    m_dragCallback(true);

    float dY = std::abs(p0->getStartLocation().y - p0->getLocation().y);
    if (dY > 3) {
        setButtonsEnabled(false);
    }
}

void ScrollLayerPro::ccTouchEnded(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
	CCScrollLayerExt::ccTouchEnded(p0, p1);
    setButtonsEnabled(true);
    m_dragCallback(false);
}

ScrollLayerPro::ScrollLayerPro(CCRect const& rect, std::function<void(bool)> dragCallback, bool scrollWheelEnabled, bool vertical) : ScrollLayer(rect, scrollWheelEnabled, vertical) {
    m_dragCallback = dragCallback;
    schedule(schedule_selector(ScrollLayerPro::listenForPosChange));
}

ScrollLayerPro* ScrollLayerPro::create(CCRect const& rect, std::function<void(bool)> dragCallback, bool scroll, bool vertical) {
    auto ret = new ScrollLayerPro(rect, dragCallback, scroll, vertical);
    ret->autorelease();
    return ret;
}

ScrollLayerPro* ScrollLayerPro::create(CCSize const& size, std::function<void(bool)> dragCallback, bool scroll, bool vertical) {
    return ScrollLayerPro::create({ 0, 0, size.width, size.height }, dragCallback, scroll, vertical);
}