#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include "../HoverableCCMenuItemSpriteExtra.hpp"
#include "../LimitedCCMenu.hpp"

using namespace geode::prelude;

class Hover {
public:
    std::vector<HoverableCCMenuItemSpriteExtra*> m_hoverableItems;

    void addHoverableItem(HoverableCCMenuItemSpriteExtra* item) {
        m_hoverableItems.push_back(item);
    }

    bool isNodeVisible(CCNode* node) {
        if (!node) {
            return false;
        }

        CCNode* currentNode = node;
        while (currentNode) {
            if (!currentNode->isVisible()) {
                return false;
            }
            if (!currentNode->getParent()) {
                return currentNode != node;
            }
            currentNode = currentNode->getParent();
        }

        return false; 
    }

    void removeHoverableItem(HoverableCCMenuItemSpriteExtra* item) {
        auto it = std::find(m_hoverableItems.begin(), m_hoverableItems.end(), item);
        if (it != m_hoverableItems.end()) m_hoverableItems.erase(it);
    }

    void updateHover(CCPoint point) {
        CCPoint local;
        CCMenu* currentParent  = nullptr;
        LimitedCCMenu* currentLimitedCCMenu = nullptr;
        bool currentParentVisible  = false;
        for (HoverableCCMenuItemSpriteExtra* item : m_hoverableItems) {
            if (ObjectSelectPopup* popup = item->getCurrentPopup()) {
                if (popup->isDraggingScroll()) {
                    return;
                }
            }
            CCMenu* parent = static_cast<CCMenu*>(item->getParent());
            if (parent != currentParent) {
                currentParent = parent;
                currentParentVisible = parent && parent->isVisible();
                if (currentParentVisible) {
                    local = parent->convertToNodeSpace(point);
                }
                currentLimitedCCMenu = typeinfo_cast<LimitedCCMenu*>(currentParent);
            }
            if (currentParentVisible && isNodeVisible(item)) {
                if (currentLimitedCCMenu) {
                    item->hover(item, point, item->boundingBox().containsPoint(local) && currentLimitedCCMenu->testLocation(point));
                }
                else {
                    item->hover(item, point, item->boundingBox().containsPoint(local));
                }
            }
        }
    }

    static Hover* get() {
        static Hover* instance = nullptr;
        if (!instance) {
            instance = new Hover();
        }
        return instance;
    }
};

#ifdef GEODE_IS_WINDOWS

class $modify(CCEGLView) {
    void onGLFWMouseMoveCallBack(GLFWwindow* w, double x, double y) {
        CCEGLView::onGLFWMouseMoveCallBack(w, x, y);
        Hover::get()->updateHover(getMousePos());
    }
};

#endif