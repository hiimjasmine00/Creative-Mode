#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include "CCMenuItemSpriteExtra.hpp"

using namespace geode::prelude;
using namespace std::placeholders;

class $modify(MyEditButtonBar, EditButtonBar) {

	static void onModify(auto& self) {
        (void) self.setHookPriorityBeforePre("EditButtonBar::loadFromItems", "razoom.object_groups");\
    }

	struct Fields {
		std::vector<int> m_objectIDs{};
	};

    void onObjectButtonHover(CCObject* sender, CCPoint point, bool hovering, bool isStart);
	void loadFromItems(CCArray* p0, int p1, int p2, bool p3);
};
