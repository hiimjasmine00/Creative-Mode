#include "EditButtonBar.hpp"
#include "EditorUI.hpp"

void MyEditButtonBar::onObjectButtonHover(CCObject* sender, CCPoint point, bool hovering, bool isStart) {

    MyEditorUI* editorUI = static_cast<MyEditorUI*>(EditorUI::get());
    Mod* mod = Mod::get();

    if (isStart && hovering) {
        
        if (mod->getSettingValue<bool>("enable-new-tab-ui")) {
            if (ButtonSprite* buttonSprite = static_cast<CCNode*>(sender)->getChildByType<ButtonSprite*>(0)) {
                buttonSprite->getChildByID("highlight")->setVisible(true);
            }
        }

        std::string name;
        if (sender->getTag() < 0) {
            name = "Custom Object";
        }
        else if (sender->getTag() > 0) {
            name = ObjectNames::get()->nameForID(sender->getTag());
        }
        
        if (mod->getSettingValue<bool>("enable-tooltips")) {
            editorUI->setTooltipText(name, sender->getTag());
            editorUI->setTooltipVisible(true);
        }
    }
    if (isStart && !hovering) {
        if (mod->getSettingValue<bool>("enable-new-tab-ui")) {
            if (ButtonSprite* buttonSprite = static_cast<CCNode*>(sender)->getChildByType<ButtonSprite*>(0)) {
                buttonSprite->getChildByID("highlight")->setVisible(false);
            }
        }
        if (mod->getSettingValue<bool>("enable-tooltips")) {
            editorUI->setTooltipVisible(false);
        }
    }
    if (hovering) {
        if (mod->getSettingValue<bool>("enable-tooltips")) {
            editorUI->setTooltipPosition(point);
        }
    }
}

void MyEditButtonBar::loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
    auto fields = m_fields.self();
    Mod* mod = Mod::get();
    if (mod->getSettingValue<bool>("enable-new-tab-ui")) {
        for (CCNode* node : CCArrayExt<CCNode*>(p0)) {
            if (CreateMenuItem* cmi = typeinfo_cast<CreateMenuItem*>(node)) {
                cmi->m_scaleMultiplier = 1;
                if (ButtonSprite* buttonSprite = cmi->getChildByType<ButtonSprite*>(0)) {
                    if (buttonSprite->m_BGSprite) {
                        buttonSprite->m_BGSprite->setVisible(false);
                    }
                    else {
                        for (CCNode* child : CCArrayExt<CCNode*>(buttonSprite->getChildren())) {
                            if (typeinfo_cast<CCSprite*>(child)) {
                                if (typeinfo_cast<GameObject*>(child)) continue;
                                if (!child->getID().empty()) continue;
                                if (child->getContentSize() == CCSize{40, 40}) {
                                    child->setVisible(false);
                                    break;
                                }
                            }
                        }
                    }

                    if (buttonSprite->getChildByID("slot-bg")) continue;
                    
                    CCSprite* slotSprite = CCSprite::create("slot.png"_spr);
                    CCSprite* slotOverlay = CCSprite::create("slot-overlay.png"_spr);
                    slotSprite->setZOrder(-10);
                    slotSprite->setID("slot-bg");
                    slotOverlay->setZOrder(10);
                    slotOverlay->setVisible(false);
                    slotOverlay->setID("slot-overlay");
                    slotOverlay->setColor({255, 255, 0});

                    CCLayerColor* highlight = CCLayerColor::create({255, 255, 255, 127});
                    highlight->setVisible(false);
                    highlight->setID("highlight");
                    highlight->setZOrder(-11);
                    buttonSprite->addChild(slotSprite);
                    buttonSprite->addChild(slotOverlay);
                    buttonSprite->addChild(highlight);
                    highlight->setContentSize(buttonSprite->getContentSize());
                    highlight->setPositionY(highlight->getPositionY() + 2);
                    slotSprite->setPosition(buttonSprite->getContentSize()/2);
                    slotSprite->setPositionY(slotSprite->getPositionY() + 2);
                    slotSprite->setScale(buttonSprite->getContentSize().width / slotSprite->getContentSize().width);
                    slotOverlay->setPosition(buttonSprite->getContentSize()/2);
                    slotOverlay->setPositionY(slotOverlay->getPositionY() + 2);
                    slotOverlay->setScale(buttonSprite->getContentSize().width / slotSprite->getContentSize().width);
                }
            }
        }
    }
    if (fields->m_objectIDs.size() == 0) {
        for (CCNode* node : CCArrayExt<CCNode*>(p0)) {
            if (CreateMenuItem* cmi = typeinfo_cast<CreateMenuItem*>(node)) {
                fields->m_objectIDs.push_back(cmi->m_objectID);
                cmi->setTag(cmi->m_objectID);
                static_cast<HoverEnabledCCMenuItemSpriteExtra*>(static_cast<CCMenuItemSpriteExtra*>(cmi))->enableHover(std::bind(&MyEditButtonBar::onObjectButtonHover, this, _1, _2, _3, _4));
            }
        }
    }
    EditButtonBar::loadFromItems(p0, p1, p2, p3);
}