#include <Precompiled_headers.h>
#include "AppDelegate.h"
#include "Games/PuzzleLayer.h"
#include "Games/TetrisLayer.h"
#include "PickImage.h"
USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
        glview = GLViewImpl::createWithRect("WBBoy",Rect(0,0,960,640));
#else
        glview = GLViewImpl::createWithFullScreen("WBBoy");
#endif
        director->setOpenGLView(glview);
    }

    //director->getOpenGLView()->setDesignResolutionSize(1136, 640, , ResolutionPolicy::SHOW_ALL);

    // turn on display FPS
    //director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);
    //
    auto puzzle = MenuItemImage::create("ui/puzzle.png", "ui/puzzle.png", [&](Ref *sender) {
        pickImage([]{}, [](cocos2d::Texture2D* tex2D){
            Director::getInstance()->pushScene(PuzzleLayer::createScene(tex2D));
        });
    });
    auto tetris = MenuItemImage::create("ui/tetris.png", "ui/tetris.png", [&](Ref *sender) {
        Director::getInstance()->pushScene(TransitionSlideInT::create(1,TetrisLayer::createScene(32)));
    });
    auto menu = Menu::create(puzzle, tetris, nullptr);
    menu->alignItemsHorizontallyWithPadding(40);
    menu->setScale(0);
    menu->runAction(ScaleTo::create(1,1));
    auto scene = Scene::create();
    scene->addChild(menu);
    Director::getInstance()->runWithScene(scene);
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
