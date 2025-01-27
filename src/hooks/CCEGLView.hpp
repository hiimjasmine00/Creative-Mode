#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include "../HoverableCCMenuItemSpriteExtra.hpp"

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
        CCNode* parent = nullptr;
        bool isVisible = false;
        for (HoverableCCMenuItemSpriteExtra* item : m_hoverableItems) {
            if (ObjectSelectPopup* popup = item->getCurrentPopup()) {
                if (popup->isDraggingScroll()) break;
            }
            if (parent != item->getParent()) {
                parent = item->getParent();
                if (parent) {
                    isVisible = parent->isVisible();
                    local = parent->convertToNodeSpace(point);
                }
            }
            if (isVisible && isNodeVisible(item)) {
                item->hover(item, point, item->boundingBox().containsPoint(local));
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

class $modify(CCEGLView) {
    void onGLFWMouseMoveCallBack(GLFWwindow* w, double x, double y) {
        CCEGLView::onGLFWMouseMoveCallBack(w, x, y);
        Hover::get()->updateHover(getMousePos());
    }
};