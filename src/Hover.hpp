#pragma once

#include <Geode/Geode.hpp>
#include "LimitedCCMenu.hpp"

using namespace geode::prelude;

class Hover {
public:
    std::vector<CCMenuItemSpriteExtra*> m_hoverEnabledItems;

    void addHoverEnabledItem(CCMenuItemSpriteExtra* item);
    bool isNodeVisible(CCNode* node);
    void removeHoverEnabledItem(CCMenuItemSpriteExtra* item);
    void updateHover(CCPoint point);

    static Hover* get();
};