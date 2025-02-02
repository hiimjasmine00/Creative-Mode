#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "EditButtonBar.hpp"
#include "../popups/ObjectSelectPopup.hpp"
#include "../ObjectNames.hpp"
#include "CCMenuItemSpriteExtra.hpp"

using namespace geode::prelude;
using namespace std::placeholders;

class $modify(MyEditorUI, EditorUI) {

	static void onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");
        (void) self.setHookPriorityAfterPost("EditorUI::onCreateButton", "alphalaneous.editorsounds");
    }

	struct Fields {
		std::map<int, Ref<GameObject>> m_gameObjects{};
		std::unordered_map<int, std::vector<std::pair<int, Ref<GameObject>>>> m_tabObjects{};
		CCMenu* m_creativeMenu;
		ObjectSelectPopup* m_objectSelectPopup = nullptr;
		CCNode* m_tooltip;
		CCNode* m_gradientBG;
		CCLayerGradient* m_newGradientBG;
		CCLayerColor* m_darkenedBG;
		CCNode* m_lineNode;
		CCLayerColor* m_linePart1;
		CCLayerColor* m_linePart2;
		CCScale9Sprite* m_tooltipBG;
		CCLabelBMFont* m_tooltipText;
		CCLabelBMFont* m_tooltipObjID;
    	bool m_queueVisible = false;
	};

    bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) return false;
		auto fields = m_fields.self();
		ObjectNames::get();
    	Mod* mod = Mod::get();

		CCSprite* creativeSpr = CCSprite::create("search-btn.png"_spr);
		creativeSpr->setScale(0.5f);
		CCSprite* circleSpr = CCSprite::create("circle.png"_spr);
		circleSpr->setScale(0.5f);
		circleSpr->setAnchorPoint({0, 0});
		circleSpr->setZOrder(-1);
		circleSpr->setOpacity(10);
		circleSpr->setID("highlight");

		CCMenuItemSpriteExtra* creativeBtn = CCMenuItemSpriteExtra::create(creativeSpr, this, menu_selector(MyEditorUI::onCreativeMenu));
		static_cast<HoverEnabledCCMenuItemSpriteExtra*>(creativeBtn)->enableHover(std::bind(&MyEditorUI::onSearchHover, this, _1, _2, _3, _4));
		creativeBtn->setPosition({15, 15});
		creativeBtn->setID("creative-button"_spr);

		creativeBtn->addChild(circleSpr);

		CCSize winSize = CCDirector::get()->getWinSize();

		if (mod->getSettingValue<bool>("enable-new-tab-ui")) {
			fields->m_gradientBG = geode::cocos::getChildBySpriteName(this, "edit_barBG_001.png");
			fields->m_gradientBG->setVisible(false);
		}

		fields->m_newGradientBG = CCLayerGradient::create({127, 127, 127, 200}, {100, 100, 100, 127});
		fields->m_newGradientBG->setContentSize({winSize.width, m_toolbarHeight});
		fields->m_newGradientBG->setAnchorPoint({0, 0});
		fields->m_newGradientBG->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
		fields->m_newGradientBG->setZOrder(-1);

		fields->m_darkenedBG = CCLayerColor::create({0, 0, 0, 145});
		fields->m_darkenedBG->setContentSize({winSize.width, m_toolbarHeight});
		fields->m_darkenedBG->setAnchorPoint({0, 0});

		fields->m_lineNode = CCNode::create();

		fields->m_linePart1 = CCLayerColor::create({255, 255, 255, 255});
		fields->m_linePart1->setContentSize({winSize.width, m_tabsMenu->getScale()});
		fields->m_linePart1->setAnchorPoint({0, 0});
		fields->m_linePart1->ignoreAnchorPointForPosition(false);

		fields->m_linePart2 = CCLayerColor::create({255, 255, 255, 255});
		fields->m_linePart2->setContentSize({winSize.width, m_tabsMenu->getScale()});
		fields->m_linePart2->setAnchorPoint({1, 0});
		fields->m_linePart2->setPosition({winSize.width, 0});
		fields->m_linePart2->ignoreAnchorPointForPosition(false);

		fields->m_lineNode->addChild(fields->m_linePart1);
		fields->m_lineNode->addChild(fields->m_linePart2);
		fields->m_lineNode->setAnchorPoint({0, 0});
		fields->m_lineNode->setPosition({0, m_toolbarHeight});
		fields->m_lineNode->setZOrder(101);

		std::pair<float, float> edges = getSelectedButtonEdges();

		fields->m_linePart1->setContentWidth(edges.first);
		fields->m_linePart2->setContentWidth(winSize.width - edges.second);

		if (mod->getSettingValue<bool>("enable-new-tab-ui")) {
			addChild(fields->m_newGradientBG);
			addChild(fields->m_darkenedBG);
			addChild(fields->m_lineNode);
		}

		fields->m_creativeMenu = CCMenu::create();
		fields->m_creativeMenu->setContentSize({30, 30});
		fields->m_creativeMenu->setAnchorPoint({1, 1});
		fields->m_creativeMenu->ignoreAnchorPointForPosition(false);
		fields->m_creativeMenu->setZOrder(10);
		fields->m_creativeMenu->setID("creative-menu"_spr);

		fields->m_tooltip = CCNode::create();
		fields->m_tooltip->setID("tooltip");
		fields->m_tooltip->setZOrder(110);
		fields->m_tooltipBG = CCScale9Sprite::create("square02_001.png");
		fields->m_tooltipBG->setOpacity(225);
		fields->m_tooltipBG->setAnchorPoint({0, 0});
		fields->m_tooltipBG->setZOrder(-11);
		fields->m_tooltipBG->setID("background");
		fields->m_tooltip->setVisible(false);
		fields->m_tooltip->setAnchorPoint({0, 0});
		fields->m_tooltipText = CCLabelBMFont::create("", "chatFont.fnt");
		fields->m_tooltipText->setAnchorPoint({0.f, 1.f});
		fields->m_tooltipText->setID("tooltip-label");
		fields->m_tooltipText->setScale(0.75);
		fields->m_tooltipText->setPositionX(5);
		fields->m_tooltipObjID = CCLabelBMFont::create("", "chatFont.fnt");
		fields->m_tooltipObjID->setAnchorPoint({0.f, 0.f});
		fields->m_tooltipObjID->setID("object-id-label");
		fields->m_tooltipObjID->setColor({0, 255, 75});
		fields->m_tooltipObjID->setScale(0.5);
		fields->m_tooltipObjID->setPositionX(5);

		fields->m_tooltip->addChild(fields->m_tooltipText);
		fields->m_tooltip->addChild(fields->m_tooltipBG);
		fields->m_tooltip->addChild(fields->m_tooltipObjID);

		addChild(fields->m_tooltip);

		float x = 85;
		float y = 90;
		
		if (CCNode* toolbarTogglesMenu = getChildByID("toolbar-toggles-menu")) {
			x = toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() - 1;
			y = toolbarTogglesMenu->getPositionY() + toolbarTogglesMenu->getScaledContentHeight();
		}
		
		fields->m_creativeMenu->setPosition({x, y});
		fields->m_creativeMenu->setScale(m_tabsMenu->getScale());

		fields->m_creativeMenu->addChild(creativeBtn);
		addChild(fields->m_creativeMenu);

		m_tabsMenu->setPositionY(m_toolbarHeight);
		m_tabsMenu->setAnchorPoint({0.5f, 0.f});

		if (mod->getSettingValue<bool>("enable-new-tab-ui")) {
			for (CCNode* tab : CCArrayExt<CCNode*>(m_tabsMenu->getChildren())) {
				changeBG(tab, 127, 172, 86);
				HoverEnabledCCMenuItemSpriteExtra* hoverBtn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(tab);
				hoverBtn->setUserObject("orig-y"_spr, CCFloat::create(hoverBtn->getPositionY()));
				hoverBtn->enableHover(std::bind(&MyEditorUI::onTabHover, this, _1, _2, _3, _4));
			}
			changeBG(m_tabsMenu->getChildByTag(m_selectedTab), 200, 255, 145);
		}

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

	std::pair<float, float> getSelectedButtonEdges() {

		if (m_selectedMode == 2) {
			CCNode* tab = m_tabsMenu->getChildByTag(m_selectedTab);

			float xLeft = m_tabsMenu->getPositionX() - m_tabsMenu->getScaledContentWidth()/2;
			float tabLeft = tab->getPositionX() - tab->getScaledContentWidth()/2;

			float start = xLeft + tabLeft * m_tabsMenu->getScale();
			float end = xLeft + (tabLeft + tab->getScaledContentWidth()) * m_tabsMenu->getScale();

			return {start, end};
		}
		return {0, 0};
	}

	CCNode* createCoolBG(int opacity, int outlineOpacity, int overlayOpacity) {

		CCNode* containerNode = CCNode::create();

		CCSprite* outlineHalf1 = CCSprite::create("GJ_square07.png");
		CCSprite* outlineHalf2 = CCSprite::create("GJ_square07.png");

		CCSprite* innerPartHalf1 = CCSprite::create("square02b_001.png");
		CCSprite* innerPartHalf2 = CCSprite::create("square02b_001.png");

		CCSprite* innerPartOverlayHalf1 = CCSprite::create("square02b_001.png");
		CCSprite* innerPartOverlayHalf2 = CCSprite::create("square02b_001.png");

		float scaleFactor = 3.f;

		outlineHalf1->setTextureRect({outlineHalf1->getContentWidth() - outlineHalf1->getContentWidth()/scaleFactor, 0, outlineHalf1->getContentWidth()/scaleFactor, outlineHalf1->getContentHeight()/scaleFactor});
		outlineHalf1->setAnchorPoint({0, 0});
		outlineHalf1->setPositionX(outlineHalf1->getContentWidth());
		outlineHalf1->setOpacity(outlineOpacity);

		outlineHalf2->setTextureRect({0, 0, outlineHalf2->getContentWidth()/scaleFactor, outlineHalf2->getContentHeight()/scaleFactor});
		outlineHalf2->setAnchorPoint({0, 0});
		outlineHalf2->setOpacity(outlineOpacity);

		innerPartHalf1->setTextureRect({innerPartHalf1->getContentWidth() - innerPartHalf1->getContentWidth()/scaleFactor, 0, innerPartHalf1->getContentWidth()/scaleFactor, innerPartHalf1->getContentHeight()/scaleFactor});
		innerPartHalf1->setOpacity(opacity);
		innerPartHalf1->setPositionX(innerPartHalf1->getContentWidth());
		innerPartHalf1->setAnchorPoint({0, 0});
		innerPartHalf1->setColor({175, 175, 175});
		innerPartHalf1->setZOrder(-2);
		innerPartHalf1->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

		innerPartHalf2->setTextureRect({0, 0, innerPartHalf2->getContentWidth()/scaleFactor, innerPartHalf2->getContentHeight()/scaleFactor});
		innerPartHalf2->setOpacity(opacity);
		innerPartHalf2->setAnchorPoint({0, 0});
		innerPartHalf2->setColor({175, 175, 175});
		innerPartHalf2->setZOrder(-2);
		innerPartHalf2->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

		innerPartOverlayHalf1->setTextureRect({innerPartOverlayHalf1->getContentWidth() - innerPartOverlayHalf1->getContentWidth()/scaleFactor, 0, innerPartOverlayHalf1->getContentWidth()/scaleFactor, innerPartOverlayHalf1->getContentHeight()/scaleFactor});
		innerPartOverlayHalf1->setPositionX(innerPartOverlayHalf1->getContentWidth());
		innerPartOverlayHalf1->setAnchorPoint({0, 0});
		innerPartOverlayHalf1->setColor({0, 0, 0});
		innerPartOverlayHalf1->setOpacity(overlayOpacity);
		innerPartOverlayHalf1->setZOrder(-1);

		innerPartOverlayHalf2->setTextureRect({0, 0, innerPartOverlayHalf2->getContentWidth()/scaleFactor, innerPartOverlayHalf2->getContentHeight()/scaleFactor});
		innerPartOverlayHalf2->setAnchorPoint({0, 0});
		innerPartOverlayHalf2->setColor({0, 0, 0});
		innerPartOverlayHalf2->setOpacity(overlayOpacity);
		innerPartOverlayHalf2->setZOrder(-1);

		containerNode->setZOrder(-1);

		containerNode->addChild(outlineHalf1);
		containerNode->addChild(outlineHalf2);

		containerNode->addChild(innerPartHalf1);
		containerNode->addChild(innerPartHalf2);
		
		containerNode->addChild(innerPartOverlayHalf1);
		containerNode->addChild(innerPartOverlayHalf2);

		containerNode->setAnchorPoint({0, 0});
		containerNode->setScale(0.6f);

		return containerNode;
	}

	void setLines() {
		if (Mod::get()->getSettingValue<bool>("enable-new-tab-ui")) {
			auto fields = m_fields.self();
			CCSize winSize = CCDirector::get()->getWinSize();

			std::pair<float, float> edges = getSelectedButtonEdges();

			if (fields->m_linePart1) fields->m_linePart1->setContentWidth(edges.first);
			if (fields->m_linePart2) fields->m_linePart2->setContentWidth(winSize.width - edges.second);
		}
	}

	void changeBG(CCNode* node, int opacity, int outlineOpacity, int overlayOpacity) {
		for (CCMenuItemSpriteExtra* toggles : CCArrayExt<CCMenuItemSpriteExtra*>(node->getChildren())) {
			if (CCSprite* bg = toggles->getChildByType<CCSprite*>(0)) {
				bg->setOpacity(0);
				if (CCNode* icon = bg->getChildByType<CCNode*>(0)) {
					if (icon->getTag() != 1) {
						icon->setTag(1);
						icon->setScale(icon->getScale() * 0.8f);
					}
				}
			}
			toggles->m_scaleMultiplier = 1;
			toggles->removeChildByID("background"_spr);
			CCNode* newBG = createCoolBG(opacity, outlineOpacity, overlayOpacity);
			newBG->setID("background"_spr);
			toggles->addChild(newBG);
		}
	}

	void selectBuildTab(int tab) {
		EditorUI::selectBuildTab(tab);
		if (Mod::get()->getSettingValue<bool>("enable-new-tab-ui")) {
			setLines();
			CCNode* currentTab = m_tabsMenu->getChildByTag(m_selectedTab);

			if (m_selectedMode == 2) {
				for (CCNode* tab : CCArrayExt<CCNode*>(m_tabsMenu->getChildren())) {
					changeBG(tab, 127, 172, 86);
				}
				changeBG(currentTab, 200, 255, 145);
			}
			HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(currentTab);
			if (CCFloat* yFloat = static_cast<CCFloat*>(btn->getUserObject("orig-y"_spr))) {
				float origY = yFloat->getValue();
				if (currentTab->getTag() == m_selectedTab) {
					btn->setPositionY(origY);
				}
			}
		}
	}

	void setTooltipText(std::string text, int id) {
		auto fields = m_fields.self();
		if (fields->m_tooltipText) {
			if (id == 0) {
				fields->m_tooltipText->setString("");
				return;
			}
			fields->m_tooltipText->setString(text.c_str());
			id = std::abs(id);
			fields->m_tooltipText->setString(text.c_str());
			fields->m_tooltipObjID->setString(fmt::format("(#{})", id).c_str());

			float scaleFactor = 4.f;

			fields->m_tooltip->setContentSize({fields->m_tooltipText->getScaledContentSize().width + 10, fields->m_tooltipText->getScaledContentSize().height + fields->m_tooltipObjID->getScaledContentSize().height + 5});
			fields->m_tooltipBG->setContentSize(fields->m_tooltip->getContentSize() * scaleFactor);
			fields->m_tooltipBG->setScale(1 / scaleFactor);

			fields->m_tooltipText->setPositionY(fields->m_tooltip->getContentSize().height - 2);
			fields->m_tooltipObjID->setPositionY(3);
		}
	}

	void setTooltipPosition(CCPoint point) {
		auto fields = m_fields.self();

		CCSize winSize = CCDirector::get()->getWinSize();

		if (fields->m_tooltip->getContentSize().width + point.x > winSize.width) {
			point.x = winSize.width - fields->m_tooltip->getContentSize().width;
		}

		if (fields->m_tooltip) fields->m_tooltip->setPosition(point);
	}

	void setTooltipVisible(bool visible) {
		auto fields = m_fields.self();
    	if (std::string_view(fields->m_tooltipText->getString()).empty()) return;
		if (!fields->m_queueVisible && visible) {
			fields->m_queueVisible = true;
			if (fields->m_tooltip) {
				fields->m_tooltip->retain();
				queueInMainThread([this, fields] {
					fields->m_tooltip->setVisible(true);
					fields->m_tooltip->release();
					fields->m_queueVisible = false;
				});
			}
		}
		
		if (fields->m_tooltip) fields->m_tooltip->setVisible(visible);
	}

    void toggleMode(cocos2d::CCObject* sender) {
		EditorUI::toggleMode(sender);
		if (m_fields->m_creativeMenu) m_fields->m_creativeMenu->setVisible(m_selectedMode == 2);
		setLines();
	}

	void showUI(bool show) {
		EditorUI::showUI(show);
		auto fields = m_fields.self();
		if (m_selectedMode == 2) fields->m_creativeMenu->setVisible(show);
		if (Mod::get()->getSettingValue<bool>("enable-new-tab-ui")) {
			fields->m_gradientBG->setVisible(false);
			fields->m_newGradientBG->setVisible(show);
			fields->m_lineNode->setVisible(show);
			fields->m_darkenedBG->setVisible(show);
			setTooltipVisible(false);
		}
	}

	void onCreativeMenu(CCObject* sender) {
		auto fields = m_fields.self();

		fields->m_objectSelectPopup = ObjectSelectPopup::create(this);
		fields->m_objectSelectPopup->show();
	}

	void updateCreateMenu(bool p0) {
		EditorUI::updateCreateMenu(p0);
		if (Mod::get()->getSettingValue<bool>("enable-new-tab-ui")) {
			for (CreateMenuItem* item : CCArrayExt<CreateMenuItem*>(m_createButtonArray)) {
				if (ButtonSprite* buttonSprite = item->getChildByType<ButtonSprite*>(0)) {
					if (CCNode* overlay = buttonSprite->getChildByID("slot-overlay")) {
						overlay->setVisible(item->m_objectID == m_selectedObjectIndex);
					}
				}
			}
			for (CreateMenuItem* item : CCArrayExt<CreateMenuItem*>(m_customObjectButtonArray)) {
				if (ButtonSprite* buttonSprite = item->getChildByType<ButtonSprite*>(0)) {
					if (CCNode* overlay = buttonSprite->getChildByID("slot-overlay")) {
						overlay->setVisible(item->m_objectID == m_selectedObjectIndex);
					}
				}
			}
		}
	}

	void onObjectButton(CCObject* sender) {
		auto fields = m_fields.self();
		int id = sender->getTag();

		HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(sender);
		CCNode* overlay = btn->getChildByID("slot-overlay");
		onCreateButton(sender);

		m_selectedObjectIndex = id;
		selectBuildTab(static_cast<CCInteger*>(btn->getUserObject("tab"_spr))->getValue());
		updateCreateMenu(true);

		for (CCMenuItem* btn : fields->m_objectSelectPopup->m_buttons) {
			CCNode* overlay2 = btn->getChildByID("slot-overlay");
			overlay2->setVisible(btn->getTag() == id);
		}

		overlay->setVisible(true);
	}

	void onSearchHover(CCObject* sender, CCPoint point, bool hovering, bool isStart) {
		HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(sender);
		if (isStart && hovering) {
			static_cast<CCSprite*>(btn->getChildByID("highlight"))->setOpacity(127);
		}
		if (isStart && !hovering) {
			static_cast<CCSprite*>(btn->getChildByID("highlight"))->setOpacity(10);
		}
	}

	void onTabHover(CCObject* sender, CCPoint point, bool hovering, bool isStart) {
		HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(sender);
		float origY = static_cast<CCFloat*>(btn->getUserObject("orig-y"_spr))->getValue();
		if (sender->getTag() == m_selectedTab) {
			btn->setPositionY(origY);
		}
		else {
			if (isStart && hovering) {
				btn->stopActionByTag(2);
				bool doAction = true;
				if (CCAction* action = btn->getActionByTag(1)) {
					doAction = action->isDone();
				}
				if (doAction) {
					CCMoveTo* moveUp = CCMoveTo::create(0.1f, {btn->getPositionX(), origY + 1});
					moveUp->setTag(1);
					btn->runAction(moveUp);
				}
			}
			if (!hovering) {
				btn->stopActionByTag(1);
				bool doAction = true;
				if (CCAction* action = btn->getActionByTag(2)) {
					doAction = action->isDone();
				}
				if (doAction) {
					CCMoveTo* moveDown = CCMoveTo::create(0.1f, {btn->getPositionX(), origY});
					moveDown->setTag(2);
					btn->runAction(moveDown);
				}
			}
		}
	}

	void onObjectButtonHover(CCObject* sender, CCPoint point, bool hovering, bool isStart) {
		HoverEnabledCCMenuItemSpriteExtra* btn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(sender);
		auto fields = m_fields.self();

		if (isStart && hovering) {
			std::string name;
			if (btn->getTag() < 0) {
				name = "Custom Object";
			}
			else if (btn->getTag() > 0) {
				name = ObjectNames::get()->nameForID(btn->getTag());
			}
			fields->m_objectSelectPopup->setTooltipText(name, btn->getTag());
			fields->m_objectSelectPopup->setTooltipVisible(true);
			btn->getChildByID("highlight")->setVisible(true);
		}
		if (isStart && !hovering) {
			fields->m_objectSelectPopup->setTooltipVisible(false);
			btn->getChildByID("highlight")->setVisible(false);
		}

		if (hovering) {
			fields->m_objectSelectPopup->setTooltipPosition(point);
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

	HoverEnabledCCMenuItemSpriteExtra* createObjectButton(int id, int tab, MyEditorUI::Fields* fields) {
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
		CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(objectContainer, this, menu_selector(MyEditorUI::onObjectButton));
		HoverEnabledCCMenuItemSpriteExtra* hoverBtn = static_cast<HoverEnabledCCMenuItemSpriteExtra*>(btn);
		hoverBtn->enableHover(std::bind(&MyEditorUI::onObjectButtonHover, this, _1, _2, _3, _4));
		objectContainer->setPosition({objectContainer->getPositionX() + 2.75f * objectContainer->getScale(), objectContainer->getPositionY() + 1 * objectContainer->getScale()});
        btn->m_scaleMultiplier = 1;
		btn->addChild(slotSprite);
		btn->addChild(slotOverlay);
		btn->addChild(highlight);
		btn->setContentSize({25.6, 25.6});
		highlight->setContentSize(btn->getContentSize());

		btn->setTag(id);
		btn->setUserObject("tab"_spr, CCInteger::create(tab));
		slotSprite->setPosition(btn->getContentSize()/2);
		slotSprite->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);
		slotOverlay->setPosition(btn->getContentSize()/2);
		slotOverlay->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);

		return hoverBtn;
	}
};