#include "HelloWorldScene.h"
#include <sstream>

using namespace cocos2d;
using namespace cocospriter;

CCScene* HelloWorld::scene()
{
    CCScene * scene = NULL;
    do
    {
        // 'scene' is an autorelease object
        scene = CCScene::create();
        CC_BREAK_IF(! scene);
        
        // 'layer' is an autorelease object
        HelloWorld *layer = HelloWorld::create();
        CC_BREAK_IF(! layer);
        
        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    bool bRet = false;
    do
    {
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////
        
        CC_BREAK_IF(! CCLayer::init());
        
        //////////////////////////////////////////////////////////////////////////
        // add your codes below...
        //////////////////////////////////////////////////////////////////////////
        
        // 1. Add a menu item with "X" image, which is clicked to quit the program.
        
        // Create a "close" menu item with close icon, it's an auto release object.
        CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                                              "CloseNormal.png",
                                                              "CloseSelected.png",
                                                              this,
                                                              menu_selector(HelloWorld::menuCloseCallback) );
        CC_BREAK_IF(! pCloseItem);
        
        // Place the menu item bottom-right conner.
        pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );
        
        // Create a menu with the "close" menu item, it's an auto release object.
        CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
        pMenu->setPosition( CCPointZero );
        CC_BREAK_IF(! pMenu);
        
        // Add the menu to HelloWorld layer as a child layer.
        this->addChild(pMenu, 1);
        
        // 2. Add a label shows "Hello World".
        
        // Create a label and initialize with string "Hello World".
        pLabel = CCLabelTTF::create("Hello World", "Arial", 24);
		pLabel->setColor(ccRED);
        CC_BREAK_IF(! pLabel);
        
        // Get window size and place the label upper.
        CCSize size = CCDirector::sharedDirector()->getWinSize();
        pLabel->setPosition(ccp(size.width / 2, size.height - 50));
        
        // Add the label to HelloWorld layer as a child layer.
        this->addChild(pLabel, 1);
        
        // 3. Add add a splash screen, show the cocos2d splash image.
        CCSprite* pSprite = CCSprite::create("HelloWorld.png");
        CC_BREAK_IF(! pSprite);
        
        // Place the sprite on the center of the screen
        pSprite->setPosition( ccp(size.width/2, size.height/2) );
        
        // Add the sprite to HelloWorld layer as a child layer.
        this->addChild(pSprite, 0);
        
		CCMenuItemImage *item1 = CCMenuItemImage::create("b1.png", "b2.png", this, menu_selector(HelloWorld::backCallback) );
		CCMenuItemImage *item2 = CCMenuItemImage::create("r1.png","r2.png", this, menu_selector(HelloWorld::restartCallback) );
		CCMenuItemImage *item3 = CCMenuItemImage::create("f1.png", "f2.png", this, menu_selector(HelloWorld::nextCallback) );
        
		CCMenu *menu = CCMenu::create(item1, item2, item3, NULL);
        
		menu->setPosition( CCPointZero );
		item1->setPosition(ccp(size.width / 2 - item2->getContentSize().width*2, item2->getContentSize().height/2 + 50));
		item2->setPosition(ccp(size.width / 2, item2->getContentSize().height/2 + 50));
		item3->setPosition(ccp(size.width / 2 + item2->getContentSize().width*2, item2->getContentSize().height/2 + 50));
        
		addChild(menu, 1);
        
		testNode = CCNode::create();
		this->addChild(testNode);
        
		setTestStatus(0);
        
        bRet = true;
    } while (0);
    
    return bRet;
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    // "close" menu item clicked
    CCDirector::sharedDirector()->end();
}

void HelloWorld::restartCallback( CCObject* sender )
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
    
	switch(testStatus){
        case 0:
		{
			cspr->stopAllActions();
			CocoSpriterAnimation *cocospriter_animation = CocoSpriterAnimationCache::getInstance()->getAnimation("Example.scml");
			vector<string> animationNames = cocospriter_animation->getAnimationNames("monster");
			if (++actionStatus>=animationNames.size())
				actionStatus = 0;
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create(cocospriter_animation,"monster",animationNames[actionStatus].c_str());
			cspr->runAction(CCRepeatForever::create(animate));
		}
            break;
        case 1:
		{
			cspr->stopAllActions();
			CocoSpriterAnimation *cocospriter_animation = CocoSpriterAnimationCache::getInstance()->getAnimation("Hero.scml");
			vector<string> animationNames = cocospriter_animation->getAnimationNames("first character");
			if (++actionStatus>=animationNames.size())
				actionStatus = 0;
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create(cocospriter_animation,"first character",animationNames[actionStatus].c_str());
			cspr->runAction(CCRepeatForever::create(animate));
		}
            break;
        case 2:
		{
			stringstream ss;
			ss<<"Test Operating Efficiency \n CocoSpriter Counts: "<<testNode->getChildrenCount()+1;
			pLabel->setString(ss.str().c_str());
            
			CocoSpriter *cspr=CocoSpriter::create();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy");
			cspr->runAction(CCRepeatForever::create(animate));
			cspr->setPosition(ccp(rand()%(int)size.width, rand()%(int)size.height));
			testNode->addChild(cspr);
		}
            break;
        case 3:
		{
			cspr->stopAllActions();
            
			cspr->setPosition(ccp(size.width/5, size.height/2));
			cspr->setScaleX(1);
            
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","walk");
			cspr->runAction(CCRepeatForever::create(animate));
            
			CCAction *action2 = CCRepeatForever::create(
                                                        CCSequence::create(CCMoveBy::create(2,ccp(size.width/5 * 3, 0)),
                                                                           CCScaleTo::create(0,-1,1),
                                                                           CCMoveBy::create(2,ccp(-size.width/5 * 3, 0)),
                                                                           CCScaleTo::create(0,1,1),
                                                                           NULL)
                                                        );
			cspr->runAction(action2);
		}
            break;
        case 4:
		{
			cspr->stopAllActions();
            
			cspr->setPosition(ccp(size.width/5, size.height/2));
			cspr->setScaleX(1);
            
			cspr->runAction(
                            CCRepeatForever::create(
                                                    CCSequence::create(
                                                                       CCSpawn::create(CocoSpriterAnimate::create(2,"Hero.scml","first character","walk"),CCMoveBy::create(2,ccp(size.width/5 * 3, 0)),NULL),
                                                                       CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy"),
                                                                       CCScaleTo::create(0,-1,1),
                                                                       CCSpawn::create(CocoSpriterAnimate::create(2,"Hero.scml","first character","walk"),CCMoveBy::create(2,ccp(-size.width/5 * 3, 0)),NULL),
                                                                       CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy"),
                                                                       CCScaleTo::create(0,1,1),
                                                                       NULL)
                                                    ));
		}
            break;
        case 5:
		{
			CocoSpriterAnimation *cocospriter_animation = CocoSpriterAnimationCache::getInstance()->getAnimation("Hero.scml");
			vector<string> animationNames = cocospriter_animation->getAnimationNames("first character");
			if (++actionStatus>=animationNames.size())
				actionStatus = 0;
            
			cspr->stopAllActions();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create(cocospriter_animation,"first character",animationNames[actionStatus].c_str());
			cspr->runAction(CCRepeatForever::create(animate));
            
			cspr2->stopAllActions();
			CocoSpriterAnimate *animate2 = CocoSpriterAnimate::create(cocospriter_animation,"first character",animationNames[actionStatus].c_str());
			animate2->setFPS(24);
			cspr2->runAction(CCRepeatForever::create(animate2));
		}
            break;
        case 6:
		{
			stringstream ss;
			ss<<"Test Operating Efficiency by 24 FPS\n CocoSpriter Counts: "<<testNode->getChildrenCount()+1;
			pLabel->setString(ss.str().c_str());
            
			CocoSpriter *cspr=CocoSpriter::create();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy");
			animate->setFPS(24);
			cspr->runAction(CCRepeatForever::create(animate));
			cspr->setPosition(ccp(rand()%(int)size.width, rand()%(int)size.height));
			testNode->addChild(cspr);
		}
            break;
    }
}

void HelloWorld::nextCallback( CCObject* sender )
{
	if (++testStatus>6)
		testStatus = 0;
    
	setTestStatus(testStatus);
}

void HelloWorld::backCallback( CCObject* sender )
{
	if (--testStatus<0)
		testStatus = 6;
    
	setTestStatus(testStatus);
}

void HelloWorld::setTestStatus( int i )
{
	testStatus = i;
	actionStatus = 0;
    
	CCSize size = CCDirector::sharedDirector()->getWinSize();
    
	testNode->removeAllChildren();
    
	switch(testStatus){
        case 0:
		{
			pLabel->setString("Test Example.scml");
            
			cspr=CocoSpriter::create();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Example.scml","monster","Idle");
			cspr->runAction(CCRepeatForever::create(animate));
			cspr->setPosition(ccp(size.width/2, size.height/4));
			testNode->addChild(cspr);
		}
            break;
        case 1:
		{
			pLabel->setString("Test Hero.scml");
            
			cspr=CocoSpriter::create();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy");
			cspr->runAction(CCRepeatForever::create(animate));
			cspr->setPosition(ccp(size.width/2, size.height/2));
			testNode->addChild(cspr);
		}
            break;
        case 2:
		{
			pLabel->setString("Test Operating Efficiency \n CocoSpriter Counts: 1");
            
			CocoSpriter *cspr=CocoSpriter::create();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy");
			cspr->runAction(CCRepeatForever::create(animate));
			cspr->setPosition(ccp(rand()%(int)size.width, rand()%(int)size.height));
			testNode->addChild(cspr);
		}
            break;
        case 3:
		{
			pLabel->setString("Test CCSequence");
            
			cspr=CocoSpriter::create();
			cspr->setPosition(ccp(size.width/5, size.height/2));
            
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","walk");
			cspr->runAction(CCRepeatForever::create(animate));
			
			CCAction *action2 = CCRepeatForever::create(
                                                        CCSequence::create(CCMoveBy::create(2,ccp(size.width/5 * 3, 0)),
                                                                           CCScaleTo::create(0,-1,1),
                                                                           CCMoveBy::create(2,ccp(-size.width/5 * 3, 0)),
                                                                           CCScaleTo::create(0,1,1),
                                                                           NULL)
                                                        );
			cspr->runAction(action2);
            
			testNode->addChild(cspr);
		}
            break;
        case 4:
		{
			pLabel->setString("Test CCSpawn");
            
			cspr=CocoSpriter::create();
			cspr->setPosition(ccp(size.width/5, size.height/2));
            
			cspr->runAction(
                            CCRepeatForever::create(
                                                    CCSequence::create(
                                                                       CCSpawn::create(CocoSpriterAnimate::create(2,"Hero.scml","first character","walk"),CCMoveBy::create(2,ccp(size.width/5 * 3, 0)),NULL),	
                                                                       CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy"),
                                                                       CCScaleTo::create(0,-1,1), 
                                                                       CCSpawn::create(CocoSpriterAnimate::create(2,"Hero.scml","first character","walk"),CCMoveBy::create(2,ccp(-size.width/5 * 3, 0)),NULL),
                                                                       CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy"),
                                                                       CCScaleTo::create(0,1,1),
                                                                       NULL)
                                                    ));
			testNode->addChild(cspr);
		}
            break;
        case 5:
		{
			pLabel->setString("Test setFPS\nLEFT:60 FPS RIGHT:24 FPS");
            
			cspr=CocoSpriter::create();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy");
			cspr->runAction(CCRepeatForever::create(animate));
			cspr->setPosition(ccp(size.width/3, size.height/2));
			testNode->addChild(cspr);
            
			cspr2=CocoSpriter::create();
			CocoSpriterAnimate *animate2 = CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy");
			animate2->setFPS(24);
			cspr2->runAction(CCRepeatForever::create(animate2));
			cspr2->setPosition(ccp(size.width/3*2, size.height/2));
			testNode->addChild(cspr2);
		}
            break;
        case 6:
		{
			pLabel->setString("Test Operating Efficiency by 24 FPS\n CocoSpriter Counts: 1");
            
			CocoSpriter *cspr=CocoSpriter::create();
			CocoSpriterAnimate *animate = CocoSpriterAnimate::create("Hero.scml","first character","idle_healthy");
			animate->setFPS(24);
			cspr->runAction(CCRepeatForever::create(animate));
			cspr->setPosition(ccp(rand()%(int)size.width, rand()%(int)size.height));				
			testNode->addChild(cspr);
		}
            break;
	}
}





