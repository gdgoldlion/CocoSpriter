#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocospriter.h"

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();

    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(HelloWorld);

	void restartCallback(CCObject* sender);
	void nextCallback(CCObject* sender);
	void backCallback(CCObject* sender);

	cocos2d::CCLabelTTF* pLabel;

	cocos2d::CCNode *testNode;
	cocospriter::CocoSpriter *cspr;
	cocospriter::CocoSpriter *cspr2;
	int testStatus;
	int actionStatus;

	void setTestStatus(int i);
};

#endif // __HELLOWORLD_SCENE_H__
