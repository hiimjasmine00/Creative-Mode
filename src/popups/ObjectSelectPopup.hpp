#pragma once

#include <Geode/Geode.hpp>
#include "../ScrollLayerPro.hpp"

using namespace geode::prelude;

struct NameData {
    int id;
    std::string name;
    int score;
};

class ObjectSelectPopup : public geode::Popup<> {

protected:
    CCMenu* m_tabsMenu;
    bool setup() override;
    bool init(EditorUI* editorUI);
private:
    EditorUI* m_editorUI;
    ScrollLayerPro* m_scrollLayer;
    CCNode* m_tooltip;
    CCScale9Sprite* m_tooltipBG;
    CCLabelBMFont* m_tooltipText;
    CCLabelBMFont* m_tooltipObjID;
    CCNode* m_searchBar;
    geode::TextInput* m_searchInput;
    int m_tab = -1;
    bool m_isDraggingScroll = false;
    bool m_queueVisible = false;
public:
    static constexpr float s_scaleMult = 0.8f;
    static ObjectSelectPopup* create(EditorUI* editorUI);
    void onClose(cocos2d::CCObject*) override;
    void generateList(int tab, std::string query = "", bool reset = false);
    CCMenuItemSpriteExtra* createObjectButton(int id);
    void onTab(CCObject* obj);
    void onClear(CCObject* obj);
    void setScroll(float scroll);
    void setTooltipText(std::string text, int id);
    void setTooltipPosition(CCPoint point);
    void setTooltipVisible(bool visible);
    void clearSearch(CCObject* sender);
    bool isDraggingScroll();
    float getScroll();
    CCMenuItemSpriteExtra* createTabButton(std::string spriteName, cocos2d::SEL_MenuHandler callback, int tag, float scale = 1);
};