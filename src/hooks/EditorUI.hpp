#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "EditButtonBar.hpp"
#include "../popups/ObjectSelectPopup.hpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI){

	static void onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");\
    }

	struct Fields {
		std::unordered_map<int, Ref<GameObject>> m_gameObjects;
		std::unordered_map<int, std::vector<Ref<CCMenuItemSpriteExtra>>> m_tabObjects;
		CCMenu* m_creativeMenu;
	};

    bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) return false;
		auto fields = m_fields.self();

		CCSprite* creativeSpr = CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png");
		CCMenuItemSpriteExtra* creativeBtn = CCMenuItemSpriteExtra::create(creativeSpr, this, menu_selector(MyEditorUI::onCreativeMenu));
		creativeBtn->setPosition({15, 15});
		creativeBtn->setID("creative-button"_spr);

		fields->m_creativeMenu = CCMenu::create();
		fields->m_creativeMenu->setContentSize({30, 30});
		fields->m_creativeMenu->setAnchorPoint({0, 1});
		fields->m_creativeMenu->ignoreAnchorPointForPosition(false);
		fields->m_creativeMenu->setZOrder(10);
		fields->m_creativeMenu->setID("creative-menu"_spr);

		float x = 85;
		float y = 90;
		if (Loader::get()->isModLoaded("hjfod.betteredit")) {
			if (CCNode* toolbarCategoriesMenu = getChildByID("toolbar-categories-menu")) {
				x = toolbarCategoriesMenu->getScaledContentWidth() + toolbarCategoriesMenu->getPositionX() + 3;
				y = toolbarCategoriesMenu->getScaledContentHeight() + toolbarCategoriesMenu->getPositionY();
			}
		}
		fields->m_creativeMenu->setPosition({x, y});
		fields->m_creativeMenu->setScale(m_tabsMenu->getScale());

		fields->m_creativeMenu->addChild(creativeBtn);
		addChild(fields->m_creativeMenu);

		for (MyEditButtonBar* buttonBar : CCArrayExt<MyEditButtonBar*>(m_createButtonBars)) {
			int tab = buttonBar->m_unknown;
			if (tab <= -1 || tab >= 13) continue;
			if (!fields->m_tabObjects.contains(tab)) {
				for (int id : buttonBar->m_fields->m_objectIDs) {

					fields->m_tabObjects[tab].push_back(createObjectButton(id, fields));
				}
			}
		}

		return true;
	}

    void toggleMode(cocos2d::CCObject* sender) {
		EditorUI::toggleMode(sender);
		if (m_fields->m_creativeMenu) m_fields->m_creativeMenu->setVisible(m_selectedMode == 2);
	}

	void showUI(bool show) {
		EditorUI::showUI(show);
		if (m_selectedMode == 2) m_fields->m_creativeMenu->setVisible(show);
	}

	void onCreativeMenu(CCObject* sender) {
		ObjectSelectPopup::create(this)->show();
	}

	void onObjectButton(CCObject* sender) {
		auto fields = m_fields.self();
		int id = sender->getTag();

		CCMenuItemSpriteExtra* btn = static_cast<CCMenuItemSpriteExtra*>(sender);
		CCNode* overlay = btn->getChildByID("slot-overlay");

		m_selectedObjectIndex = id;
		updateCreateMenu(false);

		for (auto& [k, v] : fields->m_tabObjects) {
			for (CCMenuItemSpriteExtra* btn : v) {
				CCNode* overlay2 = btn->getChildByID("slot-overlay");
				overlay2->setVisible(false);
			}
		}

		overlay->setVisible(true);
	}

	GameObject* createGameObject(int id, MyEditorUI::Fields* fields) {
		if (fields->m_gameObjects.contains(id)) {
			return fields->m_gameObjects[id];
		}
		else {
			CreateMenuItem* cmi = getCreateBtn(id, 0);
			ButtonSprite* buttonSprite = cmi->getChildByType<ButtonSprite*>(0);
			GameObject* obj = buttonSprite->getChildByType<GameObject*>(0);
			fields->m_gameObjects[id] = obj;
			return obj;
		}
		return nullptr;
	}

	CCMenuItemSpriteExtra* createObjectButton(int id, MyEditorUI::Fields* fields) {
		CCNode* objectContainer = CCNode::create();
		CCSprite* slotSprite = CCSprite::create("slot.png"_spr);
		CCSprite* slotOverlay = CCSprite::create("slot-overlay.png"_spr);
		slotSprite->setZOrder(-10);
		slotOverlay->setZOrder(10);
		slotOverlay->setVisible(false);
		slotOverlay->setID("slot-overlay");
		slotOverlay->setColor({255, 255, 0});
		objectContainer->setAnchorPoint({1, 0.5});
		objectContainer->setScale(ObjectSelectPopup::s_scaleMult * 0.7);
		objectContainer->addChild(createGameObject(id, fields));

		objectContainer->setContentSize({40, 40});

		CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(objectContainer, this, menu_selector(MyEditorUI::onObjectButton));
		objectContainer->setPosition({objectContainer->getPositionX() + 2.75f * objectContainer->getScale(), objectContainer->getPositionY() + 1 * objectContainer->getScale()});
		btn->addChild(slotSprite);
		btn->addChild(slotOverlay);
		btn->setContentSize({25.6, 25.6});
		btn->setTag(id);
		slotSprite->setPosition(btn->getContentSize()/2);
		slotSprite->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);
		slotOverlay->setPosition(btn->getContentSize()/2);
		slotOverlay->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);

		return btn;
	}
};