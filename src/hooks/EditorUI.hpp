#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "EditButtonBar.hpp"
#include "../popups/ObjectSelectPopup.hpp"
#include "../HoverableCCMenuItemSpriteExtra.hpp"
#include "../ObjectNames.hpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {

	static void onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");
    }

	struct Fields {
		std::map<int, Ref<GameObject>> m_gameObjects{};
		std::unordered_map<int, std::vector<std::pair<int, Ref<GameObject>>>> m_tabObjects{};
		CCMenu* m_creativeMenu;
		ObjectSelectPopup* m_objectSelectPopup = nullptr;
	};

    bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) return false;
		auto fields = m_fields.self();
		ObjectNames::get();

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
			int tab = buttonBar->m_tabIndex;
			if (tab <= -1 || tab >= 13) continue;
			if (!fields->m_tabObjects.contains(tab)) {
				for (int id : buttonBar->m_fields->m_objectIDs) {
					fields->m_tabObjects[tab].push_back({id, createGameObject(id, fields)});
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
		auto fields = m_fields.self();

		fields->m_objectSelectPopup = ObjectSelectPopup::create(this);
		fields->m_objectSelectPopup->show();
	}

	void onObjectButton(CCObject* sender) {
		auto fields = m_fields.self();
		int id = sender->getTag();

		HoverableCCMenuItemSpriteExtra* btn = static_cast<HoverableCCMenuItemSpriteExtra*>(sender);
		ObjectSelectPopup* popup = btn->getCurrentPopup();
		CCNode* overlay = btn->getChildByID("slot-overlay");

		m_selectedObjectIndex = id;
		updateCreateMenu(false);

		for (CCMenuItem* btn : popup->m_buttons) {
			CCNode* overlay2 = btn->getChildByID("slot-overlay");
			overlay2->setVisible(btn->getTag() == id);
		}

		overlay->setVisible(true);
	}

	void onObjectButtonHover(CCObject* sender, CCPoint point, bool hovering, bool isStart) {
		HoverableCCMenuItemSpriteExtra* btn = static_cast<HoverableCCMenuItemSpriteExtra*>(sender);

		if (isStart && hovering) {
			btn->getCurrentPopup()->setTooltipVisible(true);
			btn->getCurrentPopup()->setTooltipText(fmt::format("{}", ObjectNames::get()->nameForID(btn->getTag())), btn->getTag());
			btn->getChildByID("highlight")->setVisible(true);
		}
		if (isStart && !hovering) {
			btn->getCurrentPopup()->setTooltipVisible(false);
			btn->getChildByID("highlight")->setVisible(false);
		}

		if (hovering) {
			btn->getCurrentPopup()->setTooltipPosition(point);
		}
	}

	GameObject* createGameObject(int id, MyEditorUI::Fields* fields) {
		if (id == 0) return nullptr;
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

	HoverableCCMenuItemSpriteExtra* createObjectButton(int id, MyEditorUI::Fields* fields) {
		CCNode* objectContainer = CCNode::create();
		CCSprite* slotSprite = CCSprite::create("slot.png"_spr);
		CCSprite* slotOverlay = CCSprite::create("slot-overlay.png"_spr);
		slotSprite->setZOrder(-10);
		slotOverlay->setZOrder(10);
		slotOverlay->setVisible(false);
		slotOverlay->setID("slot-overlay");
		slotOverlay->setColor({255, 255, 0});

		CCLayerColor* highlight = CCLayerColor::create({255, 255, 255, 127});
		highlight->setVisible(false);
		highlight->setID("highlight");
		highlight->setZOrder(-11);

		objectContainer->setAnchorPoint({1, 0.5});
		objectContainer->setScale(ObjectSelectPopup::s_scaleMult * 0.7);
		if (GameObject* obj = createGameObject(id, fields)) {
			objectContainer->addChild(obj);
		}

		objectContainer->setContentSize({40, 40});
		using namespace std::placeholders;
		HoverableCCMenuItemSpriteExtra* btn = HoverableCCMenuItemSpriteExtra::create(objectContainer, this, menu_selector(MyEditorUI::onObjectButton), std::bind(&MyEditorUI::onObjectButtonHover, this, _1, _2, _3, _4));
		objectContainer->setPosition({objectContainer->getPositionX() + 2.75f * objectContainer->getScale(), objectContainer->getPositionY() + 1 * objectContainer->getScale()});
		btn->addChild(slotSprite);
		btn->addChild(slotOverlay);
		btn->addChild(highlight);
		btn->setContentSize({25.6, 25.6});
		highlight->setContentSize(btn->getContentSize());

		btn->setTag(id);
		slotSprite->setPosition(btn->getContentSize()/2);
		slotSprite->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);
		slotOverlay->setPosition(btn->getContentSize()/2);
		slotOverlay->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);

		return btn;
	}
};