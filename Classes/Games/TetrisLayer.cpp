#include <Precompiled_headers.h>
#include "TetrisLayer.h"
#include "FilterSprite.h"
#include "Tetris.h"
#include <functional>
Scene* TetrisLayer::createScene(const int brickSize)
{
    auto scene = Scene::create();
    auto layer = TetrisLayer::create(brickSize);
    scene->addChild(layer);
    return scene;
}
bool TetrisLayer::init(const int tetrisCellSize)
{
    if (!Layer::init())
        return false;
    const auto& winSize = Director::getInstance()->getVisibleSize();
    const auto& tex2D = Director::getInstance()->getTextureCache()->addImage("dat/tetris.png");
    const int texSize = (int)tex2D->getContentSize().width / 8;
    {
        Texture2D::TexParams params = {GL_NEAREST,GL_NEAREST,GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
        tex2D->setTexParameters(params);
    }
    int tetrisROW, tetrisCOL, cellSize;
    Size tetrisSIZE;
    Vec2 tetrisOFFSET;
    int infoPanelSize;
    //
    Node* nextBricks;
    {//下个块预览区
        cellSize = 16;
        infoPanelSize = cellSize * 6; //分数和下块信息栏 >16x4
        tetrisROW = tetrisCOL = 4;
        if (winSize.height > winSize.width)
            tetrisOFFSET.set(Vec2(winSize.width - infoPanelSize, winSize.height - infoPanelSize));
        else
            tetrisOFFSET.set(Vec2(winSize.width - cellSize*5, winSize.height - infoPanelSize));
        //
        nextBricks = Node::create();
        nextBricks->setPosition(tetrisOFFSET);
        for (auto i = 0; i < tetrisROW; ++i) {
            for (auto j = 0; j < tetrisCOL; ++j){
                auto s = Sprite::createWithTexture(tex2D, Rect(0, 0, texSize, texSize));
                s->setScale(cellSize / (float)texSize);
                s->setPosition(Vec2(j*cellSize + cellSize / 2, (tetrisROW - i)*cellSize - cellSize / 2));
                nextBricks->addChild(s, 0, i*tetrisCOL+j);
            }
        }
        this->addChild(nextBricks);
    }
    Node* tetrisMatrix;
    {//格子区
        cellSize = tetrisCellSize;
        if (winSize.height > winSize.width) {
            tetrisROW = (int)(winSize.height  - infoPanelSize) / cellSize;
            tetrisCOL = (int)winSize.width/ cellSize;
            tetrisSIZE.setSize(tetrisCOL * cellSize, tetrisROW * cellSize);
            tetrisOFFSET.set((winSize.width - tetrisSIZE.width) / 2, (winSize.height - tetrisSIZE.height - infoPanelSize) / 2);
        }
        else {
            tetrisROW = (int)winSize.height / cellSize;
            tetrisCOL = (int)(winSize.width - infoPanelSize)/ cellSize;
            tetrisSIZE.setSize(tetrisCOL * cellSize, tetrisROW * cellSize);
            tetrisOFFSET.set((winSize.width - tetrisSIZE.width - infoPanelSize) / 2, (winSize.height - tetrisSIZE.height) / 2);
        }
        //
        tetrisMatrix = Node::create();
        tetrisMatrix->setPosition(tetrisOFFSET);
        for (auto i = 0; i < tetrisROW; ++i) {
            for (auto j = 0; j < tetrisCOL; ++j){
                auto s = Sprite::createWithTexture(tex2D, Rect(0, 0, texSize, texSize));
                s->setScale(cellSize / (float)texSize);
                s->setPosition(Vec2(j*cellSize + cellSize / 2, (tetrisROW - i) * cellSize - cellSize / 2));
                tetrisMatrix->addChild(s, 0, i*tetrisCOL + j);
            }
        }
        this->addChild(tetrisMatrix);
    }
    {//外边框
        auto drawNode = DrawNode::create();
        drawNode->drawRect(tetrisOFFSET, tetrisOFFSET + tetrisSIZE, Color4F(0, 1, 0, 1));
        this->addChild(drawNode);
    }
    {//逻辑控制
        _tetris = new Tetris(tetrisROW, tetrisCOL, [this, tetrisMatrix, texSize, tetrisCOL](int index, const Tetris::Cell*cell){
            int type = cell ? *cell : 0;
            Sprite* s = static_cast<Sprite*>(tetrisMatrix->getChildByTag(index));
            s->setTextureRect(Rect(type*texSize, 0, texSize, texSize));
        });
        if (_tetris->newBrick(random<int>(0,100), static_cast<Tetris::Cell>(random<int>(1,7)))) {
            _nextBrickType = random<int>(0,100);
            _nextBrickValue = random<int>(1,7);
            Tetris::drawBrick(_nextBrickType, _nextBrickValue, [this,nextBricks,texSize](int index, const Tetris::Cell*cell){
                int type = cell ? *cell : 0;
                Sprite* s = static_cast<Sprite*>(nextBricks->getChildByTag(index));
                s->setTextureRect(Rect(type*texSize, 0, texSize, texSize));
            });
        }
    }
    Label* scoreLabel;
    {//分数下块的信息栏
        Vec2 origin;

        if (winSize.height > winSize.width)
        {
            scoreLabel = Label::createWithTTF("000000", "fnt/American Typewriter.ttf", 64);
            scoreLabel->setAlignment(TextHAlignment::LEFT);
            scoreLabel->setAnchorPoint(Vec2(0,0));
            scoreLabel->setPosition(tetrisOFFSET.x+16, winSize.height - infoPanelSize);
        }
        else
        {
            scoreLabel = Label::createWithTTF("000000", "fnt/American Typewriter.ttf", 20);
            scoreLabel->setAlignment(TextHAlignment::CENTER,TextVAlignment::TOP);
            scoreLabel->setAnchorPoint(Vec2(0.5f,1));
            scoreLabel->setPosition(winSize.width - infoPanelSize/2, winSize.height - infoPanelSize);
        }
        addChild(scoreLabel);
    }
    {
        // touch event监听
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [](Touch *touch, Event *event){ return true; };
        _action = TetrisAction::KEEP_STATE;
        listener->onTouchMoved = [=](Touch *touch, Event *event){
            const auto& delta = touch->getLocation() - touch->getStartLocation();
            const auto angle = atan2(delta.y, delta.x) * 180 / M_PI;
            TetrisAction action;
            if (angle > 60 && angle < 120)
                action = TetrisAction::ROTATE_CW;
            else if (angle >= -60 && angle <= 60)
                action =  TetrisAction::MOVE_RIGHT;
            else if (angle > -120 && angle < -60)
                action = TetrisAction::MOVE_DOWN;
            else
                action = TetrisAction::MOVE_LEFT;
            if (action == _action)
                return;
            if (!_tetris->doAction(action)){
                unschedule(CC_SCHEDULE_SELECTOR(TetrisLayer::updateAction));
                _action = TetrisAction::KEEP_STATE;
            }
            else {
                if (_action == TetrisAction::KEEP_STATE)
                    schedule(CC_SCHEDULE_SELECTOR(TetrisLayer::updateAction), 0.1f, CC_REPEAT_FOREVER, 0.1f);
                _action = action;
            }
        };
        listener->onTouchEnded = [=](Touch *touch, Event *event){
            unschedule(CC_SCHEDULE_SELECTOR(TetrisLayer::updateAction));
            _action = TetrisAction::KEEP_STATE;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
    {
        auto callFunc = CallFunc::create([this, scoreLabel, nextBricks, texSize]{
            if (!_tetris->moveBrick(1, 0)) {
                _tetris->fixBrick();
                int lines = _tetris->clearLines();
                if (lines > 0) {
                    _score += lines;
                    char str[16];
                    sprintf(str, "%06d", _score);
                    scoreLabel->setString(str);
                }
                if (!_tetris->newBrick(_nextBrickType, _nextBrickValue)) {
                    this->stopAllActions();
                }
                else {
                    _nextBrickType = random<int>(0,100);
                    _nextBrickValue = static_cast<Tetris::Cell>(random<int>(1,7));
                    Tetris::drawBrick(_nextBrickType, _nextBrickValue, [this, nextBricks,texSize](int index, const Tetris::Cell*cell){
                        int type = cell ? *cell : 0;
                        Sprite* s = static_cast<Sprite*>(nextBricks->getChildByTag(index));
                        s->setTextureRect(Rect(type*texSize, 0, texSize, texSize));
                    });
                }
            }
        });
        this->runAction(RepeatForever::create(Sequence::create(DelayTime::create(1), callFunc, NULL)));
    }
    return true;
}
void TetrisLayer::updateAction(float dt)
{
    if (_action != TetrisAction::KEEP_STATE && !_tetris->doAction(_action)){
        _action = TetrisAction::KEEP_STATE;
        unschedule(CC_SCHEDULE_SELECTOR(TetrisLayer::updateAction));
    }
}