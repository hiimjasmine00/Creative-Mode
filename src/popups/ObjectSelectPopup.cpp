#include "ObjectSelectPopup.hpp"
#include "../hooks/EditorUI.hpp"

bool ObjectSelectPopup::init(EditorUI* editorUI){
  
    m_editorUI = editorUI;

    if (!Popup<>::initAnchored(300.f, 240.f, "geode.loader/GE_square02.png")) return false;

    setTitle("Objects");

    m_tabsMenu = CCMenu::create();
    m_tabsMenu->setID("tabs-menu");
    m_tabsMenu->setContentSize({90, 330});
    m_tabsMenu->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setCrossAxisOverflow(false)
            ->setGrowCrossAxis(true)
            ->setGap(5.f)
    );
    m_tabsMenu->setScale(0.5f);
    m_tabsMenu->setAnchorPoint({1, 1});

    m_mainLayer->addChildAtPosition(
        m_tabsMenu,
        Anchor::TopLeft,
        { -2.f, -30.f }
    );

    CCMenuItemSpriteExtra* tab1Btn = createTabButton("square_01_001.png", menu_selector(ObjectSelectPopup::onTab), 0);
    tab1Btn->setOpacity(255);
    m_tabsMenu->addChild(tab1Btn);
    CCMenuItemSpriteExtra* tab2Btn = createTabButton("blockOutline_01_001.png", menu_selector(ObjectSelectPopup::onTab), 1);
    m_tabsMenu->addChild(tab2Btn);
    CCMenuItemSpriteExtra* tab3Btn = createTabButton("triangle_a_02_001.png", menu_selector(ObjectSelectPopup::onTab), 2);
    m_tabsMenu->addChild(tab3Btn);
    CCMenuItemSpriteExtra* tab4Btn = createTabButton("spike_01_001.png", menu_selector(ObjectSelectPopup::onTab), 3);
    m_tabsMenu->addChild(tab4Btn);
    CCMenuItemSpriteExtra* tab5Btn = createTabButton("persp_outline_01_001.png", menu_selector(ObjectSelectPopup::onTab), 4);
    m_tabsMenu->addChild(tab5Btn);
    CCMenuItemSpriteExtra* tab6Btn = createTabButton("ring_01_001.png", menu_selector(ObjectSelectPopup::onTab), 5);
    m_tabsMenu->addChild(tab6Btn);
    CCMenuItemSpriteExtra* tab7Btn = createTabButton("GJBeast01_01_001.png", menu_selector(ObjectSelectPopup::onTab), 6);
    m_tabsMenu->addChild(tab7Btn);
    CCMenuItemSpriteExtra* tab8Btn = createTabButton("pixelb_03_01_001.png", menu_selector(ObjectSelectPopup::onTab), 7);
    m_tabsMenu->addChild(tab8Btn);
    CCMenuItemSpriteExtra* tab9Btn = createTabButton("pixelitem_001_001.png", menu_selector(ObjectSelectPopup::onTab), 8);
    m_tabsMenu->addChild(tab9Btn);
    CCMenuItemSpriteExtra* tab10Btn = createTabButton("particle_01_001.png", menu_selector(ObjectSelectPopup::onTab), 9);
    m_tabsMenu->addChild(tab10Btn);
    CCMenuItemSpriteExtra* tab11Btn = createTabButton("d_spikes_01_001.png", menu_selector(ObjectSelectPopup::onTab), 10);
    m_tabsMenu->addChild(tab11Btn);
    CCMenuItemSpriteExtra* tab12Btn = createTabButton("sawblade_02_001.png", menu_selector(ObjectSelectPopup::onTab), 11);
    m_tabsMenu->addChild(tab12Btn);
    CCMenuItemSpriteExtra* tab13Btn = createTabButton("edit_eTintCol01Btn_001.png", menu_selector(ObjectSelectPopup::onTab), 12);
    m_tabsMenu->addChild(tab13Btn);

    m_tabsMenu->updateLayout();

    generateList(0);

    return true;
}


CCMenuItemSpriteExtra* ObjectSelectPopup::createTabButton(std::string spriteName, cocos2d::SEL_MenuHandler callback, int tag, float scale) {
    CCSprite* spr = CCSprite::createWithSpriteFrameName(spriteName.c_str());
    CCSprite* buttonSpr = CCSprite::create("GJ_button_04.png");

    spr->setPosition(buttonSpr->getContentSize() / 2);

    float sprMax = std::max(spr->getContentSize().width, spr->getContentSize().height);

    spr->setScale(((40 / sprMax) * 0.7f) * scale);

    buttonSpr->setCascadeColorEnabled(true);
    buttonSpr->setCascadeOpacityEnabled(true);

    buttonSpr->addChild(spr);
    
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(buttonSpr, this, callback);
    btn->setTag(tag);
    btn->setCascadeColorEnabled(true);
    btn->setCascadeOpacityEnabled(true);
    btn->setOpacity(127);

    return btn;
}

void ObjectSelectPopup::onTab(CCObject* obj) {
    m_tab = obj->getTag();
    generateList(m_tab);
    for (CCMenuItemSpriteExtra* btn : CCArrayExt<CCMenuItemSpriteExtra*>(m_tabsMenu->getChildren())) {
        btn->setOpacity(127);
    }
    static_cast<CCMenuItemSpriteExtra*>(obj)->setOpacity(255);
}

CCMenuItemSpriteExtra* ObjectSelectPopup::createObjectButton(int id) {

    CCNode* objectContainer = CCNode::create();
    CCSprite* slotSprite = CCSprite::create("slot.png"_spr);
    slotSprite->setZOrder(-10);

    objectContainer->setAnchorPoint({1, 0.5});

    CreateMenuItem* cmi = m_editorUI->getCreateBtn(id, 0);
    ButtonSprite* buttonSprite = cmi->getChildByType<ButtonSprite*>(0);
    GameObject* object = buttonSprite->getChildByType<GameObject*>(0);

    objectContainer->setScale(ObjectSelectPopup::s_scaleMult * 0.8);
    objectContainer->addChild(object);

    objectContainer->setContentSize({40, 40});

    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(objectContainer, nullptr, nullptr);
    objectContainer->setPositionY(objectContainer->getPositionY() - 2 * objectContainer->getScale());
    btn->addChild(slotSprite);
    btn->setContentSize({25.6, 25.6});
    btn->setTag(id);
    slotSprite->setPosition(btn->getContentSize()/2);
    slotSprite->setScale(btn->getContentSize().width / slotSprite->getContentSize().width);

    return btn;
}

float ObjectSelectPopup::getScroll() {
    return m_scrollLayer->m_contentLayer->getPositionY();
}

void ObjectSelectPopup::setScroll(float scroll) {
    float difference = -(m_scrollLayer->m_contentLayer->getContentHeight() - m_scrollLayer->getContentHeight());
    if (scroll < difference) {
        scroll = difference;
    }
    m_scrollLayer->m_contentLayer->setPositionY(scroll);
}

void ObjectSelectPopup::generateList(int tab){

    m_mainLayer->removeChildByID("object-list");
    m_mainLayer->removeChildByID("object-list-scrollbar");

    CCSize contentSize = { m_mainLayer->getContentSize().width - 35.f, m_mainLayer->getContentSize().height - 50.f };

    CCNode* container = CCNode::create();
    container->setContentSize(contentSize);
    container->setPosition({0, 0});
    container->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(contentSize.height)
            ->setCrossAxisOverflow(false)
            ->setGrowCrossAxis(true)
            ->setGap(0.0f)
    );

    auto fields = static_cast<MyEditorUI*>(m_editorUI)->m_fields.self();
    std::vector<Ref<CCMenuItemSpriteExtra>> buttons = fields->m_tabObjects[tab];

    int itemsPerRow = 11;

    int i = 0;
    while (true) {
        CCMenu* row = CCMenu::create();
        row->setContentSize({contentSize.width, 30 * ObjectSelectPopup::s_scaleMult});
        row->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisOverflow(false)
                ->setGrowCrossAxis(true)
                ->setGap(0.0f)
        );
        bool shouldBreak = false;
        for (int j = 0; j < itemsPerRow; j++) {
            int pos = i * itemsPerRow + j;
            if (pos >= buttons.size()) {
                shouldBreak = true;
                break;
            }
            row->addChild(buttons[pos]);
        }
        if (row->getChildrenCount() != 0) {
            row->updateLayout();
            container->addChild(row);
        }
        i++;
        if (shouldBreak) break;
    }

    container->ignoreAnchorPointForPosition(true);
    container->updateLayout();

    m_scrollLayer = ScrollLayer::create(contentSize);
    m_scrollLayer->setID("object-list");
    m_scrollLayer->ignoreAnchorPointForPosition(false);
    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(contentSize.height)
            ->setGap(5.0f)
    );
    m_scrollLayer->m_contentLayer->setContentSize(container->getContentSize());
    m_scrollLayer->m_contentLayer->addChild(container);
    m_scrollLayer->scrollToTop();

    class ScrollbarProMax : public Scrollbar {
    public:
        void drawProMax() {
            draw();
        }
    };

    Scrollbar* scrollbar = Scrollbar::create(m_scrollLayer);
    static_cast<ScrollbarProMax*>(scrollbar)->drawProMax();
    scrollbar->setID("object-list-scrollbar");

    m_mainLayer->addChildAtPosition(
        m_scrollLayer,
        Anchor::Center,
        { -5.0f, -10.0f }
    );

    m_mainLayer->addChildAtPosition(
        scrollbar,
        Anchor::Right,
        { -15.f, -10.f }
    );

    for (auto& [k, v] : fields->m_tabObjects) {
        for (CCMenuItemSpriteExtra* btn : v) {
            CCNode* slotOverlay = btn->getChildByID("slot-overlay");
            slotOverlay->setVisible(static_cast<MyEditorUI*>(m_editorUI)->m_selectedObjectIndex == btn->getTag());
        }
    }
}

bool ObjectSelectPopup::setup() {
    return true;
}

void ObjectSelectPopup::onClose(cocos2d::CCObject*){
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->removeFromParentAndCleanup(true);
}

ObjectSelectPopup* ObjectSelectPopup::create(EditorUI* editorUI) {

    auto ret = new ObjectSelectPopup();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}