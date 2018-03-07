#include <Precompiled_headers.h>
#include "PuzzleLayer.h"
#include "Tortoise.h"
#include "PickImage.h"
#include "FilterSprite.h"
USING_NS_CC;
const char *EFFECT_FILE= "snd/ok.wav";
Scene* PuzzleLayer::createScene(cocos2d::Texture2D* tex2D)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(EFFECT_FILE);
    if (!tex2D)
        tex2D = cocos2d::Director::getInstance()->getTextureCache()->addImage("dat/Image.png");
    auto layer = PuzzleLayer::create(tex2D, Size(5, 4));
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene_numRows
    return scene;
}

PuzzleLayer::Tile*
PuzzleLayer::Tile::create(Texture2D* tex2D,
                     const Rect& texRect, //纹理区域原点位于左上角
                     const Detector& detector)
{
    
    Tile *pRet = new(std::nothrow) Tile();
    if (pRet && pRet->initWithTexture(tex2D, texRect))
    {
        pRet->_detector = detector;
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        return nullptr;
    }
}
void  PuzzleLayer::Tile::reset(const cocos2d::Vec2& texScale, const cocos2d::Vec2& dstPos, const cocos2d::Vec2& curPos)
{
    _texScale = texScale;
    _delta = -1;
    _index = -1;
    _angle = random<int>(0,4)*90 % 360;
    setPosition(dstPos);
    setScale(texScale.x, texScale.y);
    runAction(Sequence::create(DelayTime::create(random<float>(0, 1)),
                               Spawn::create(RotateTo::create(1, _angle),
                                             MoveTo::create(1, curPos),
                                             ScaleTo::create(1, texScale.x/2, texScale.y/2),
                                             nullptr),
                               nullptr));
}
void PuzzleLayer::Tile::onTouchBegan()
{
    if (_delta < 0)
        this->runAction(ScaleTo::create(0.1f, _texScale.x, _texScale.y));
}
int PuzzleLayer::Tile::onTouchEnded(const cocos2d::Vec2& startPos, const cocos2d::Vec2& endPos)
{
    auto pos = endPos;
    const bool rotate = (startPos-endPos).lengthSquared() < 4;
    FiniteTimeAction* delay;
    if (rotate){
        _angle = (_angle + 90) % 360;
        delay = RotateTo::create(0.2f, _angle);
    }
    else {
        delay = DelayTime::create(0.1f);
    }
    _delta = _detector(pos, _index);
    if (_delta < 0)
        this->runAction(Sequence::create(delay, ScaleTo::create(0.1f, _texScale.x/2, _texScale.y/2), nullptr));
    else
        this->runAction(Sequence::create(delay, MoveTo::create(0.1f, pos), nullptr));
    return _delta;
}
// on "init" you need to initialize your instance
bool PuzzleLayer::init(cocos2d::Texture2D* tex2D, const cocos2d::Size& gridSize)
{
    if (!tex2D || !Layer::init() )
        return false;
    
    //根据窗口实际形状, 调整网络x,y坐标块数
    cocos2d::Vec2 numTiles(gridSize);
    const auto& visibleSize = Director::getInstance()->getOpenGLView()->getVisibleSize();
    if (visibleSize.width < visibleSize.height)
        std::swap(numTiles.x, numTiles.y);
    _masks.resize(numTiles.x * numTiles.y, false);
    //伸缩全屏背景
    {
        auto background = Sprite::create("ui/desk.jpg");
        const auto& imageSize = background->getContentSize();
        background->setPosition(visibleSize/2);
        background->setScale(visibleSize.width/imageSize.width, visibleSize.height/imageSize.height);
        this->addChild(background);
    }
    //计算布局
    Rect gridRect(Vec2::ZERO, visibleSize);
    Size tileSize;
    {
        if (visibleSize.width >= visibleSize.height)
            gridRect.size.width = visibleSize.width * numTiles.x / (numTiles.x+1);
        else
            gridRect.size.height = visibleSize.height * numTiles.y / (numTiles.y+1);
        //计算拼图尺寸,对齐到整数倍
        tileSize.width = (int)std::min(gridRect.size.width / numTiles.x, gridRect.size.height / numTiles.y);
        tileSize.height = tileSize.width;
        //计算布局
        gridRect.size.setSize(tileSize.width * numTiles.x, tileSize.height * numTiles.y);
        if (visibleSize.width < visibleSize.height)
            gridRect.origin.y = visibleSize.height - gridRect.size.height;
    }
    //计算纹理
    Rect texRect;
    Size texSize;
    {
        const auto& imgSize = tex2D->getContentSize();
        const int wArea = imgSize.width * gridRect.size.height;
        const int hArea = imgSize.height* gridRect.size.width;
        if (wArea > hArea)
            texRect.size = Size(hArea / gridRect.size.height, imgSize.height);
        else
            texRect.size = Size(imgSize.width, wArea / gridRect.size.width);
        //对齐到整数倍
        texSize.width = static_cast<int>(texRect.size.width / numTiles.x);
        texSize.height = static_cast<int>(texRect.size.height / numTiles.y);
        texRect.size = Size(texSize.width * numTiles.x, texSize.height * numTiles.y);
        texRect.origin = (imgSize - texRect.size)/2;
    }
    const Vec2 texScale(tileSize.width/texSize.width, tileSize.height/texSize.height);
    //网络,画原图
    NodeGrid* gridNode;
    Sprite* background, *arrow;
    {
        auto drawNode = DrawNode::create();
        Color4F gridColor(0.0f,1.0f,0.0f,0.7f);
        for(auto y=gridRect.getMinY(); y <= gridRect.getMaxY(); y += tileSize.height)
            drawNode->drawLine(Vec2(gridRect.getMinX(), y), Vec2(gridRect.getMaxX(), y), gridColor);
        for (auto x = gridRect.getMinX(); x <= gridRect.getMaxX(); x += tileSize.width)
            drawNode->drawLine(Vec2(x, gridRect.getMinY()), Vec2(x, gridRect.getMaxY()), gridColor);
        //drawNode->drawSolidRect(gridRect.origin, gridRect.origin+gridRect.size, Color4F(0,0,0,0.5f));
        
        background = Sprite::createWithTexture(tex2D, texRect);
        background->setPosition(gridRect.origin + gridRect.size/2);
        background->setScale(texScale.x, texScale.y);
        
        gridNode = NodeGrid::create(gridRect);
        gridNode->addChild(drawNode);
        gridNode->addChild(background);
        this->addChild(gridNode);
        
        arrow = Sprite::create("ui/arrow.png");
        arrow->setScale(tileSize.width/2/arrow->getContentSize().width);
        if (visibleSize.width >= visibleSize.height) {
            arrow->setAnchorPoint(Vec2(1,0.8f));
            arrow->setPosition(Vec2(-tileSize.width, visibleSize.height));
        }
        else {
            arrow->setAnchorPoint(Vec2(1,0));
            arrow->setPosition(Vec2(-tileSize.width, 0));
        }
        arrow->setVisible(false);
        this->addChild(arrow);
    }
    //创建拼图
    auto callfunc = CallFunc::create([=]{
        gridNode->setGrid(nullptr);
        background->setOpacity(0xFF>>1);
        arrow->setVisible(true);
        arrow->runAction(Sequence::create(DelayTime::create(random<float>(0.8f, 1)),
                                          MoveTo::create(1, Vec2(visibleSize.width, arrow->getPosition().y)),
                                          nullptr));
        
        for(auto y=0; y<numTiles.y; ++y){
            for (auto x=0; x<numTiles.x; ++x){
                auto tile = Tile::create(tex2D,
                                         //图像区域
                                         Rect(texRect.getMinX() + x * texSize.width,
                                              texRect.getMinY() + y * texSize.height,
                                              texSize.width, texSize.height),
                                         //移动后校验函数
                                         [this, gridRect,x,y,tileSize, numTiles](Vec2& pos, int& index){
                                             if (index >= 0)
                                                 _masks[index] = false;
                                             index = -1;
                                             if (!gridRect.containsPoint(pos))
                                                 return -1;
                                             const int cx = (pos.x - gridRect.getMinX())/tileSize.width;
                                             const int cy = (gridRect.getMaxY() - pos.y)/tileSize.height;
                                             const int newindex = cy*numTiles.x + cx;
                                             
                                             if (_masks[newindex])//已被占用
                                                 return -1;
                                             index = newindex;
                                             _masks[index] = true;
                                             pos.x = gridRect.getMinX() + cx * tileSize.width + tileSize.width/2;
                                             pos.y = gridRect.getMaxY() - (cy * tileSize.height + tileSize.height/2);
                                             return  abs(cx - x) + abs(cy-y);
                                         });
                
                Vec2 curPos, dstPos(gridRect.getMinX() + x * tileSize.width + tileSize.width/2,
                                    gridRect.getMaxY() - (y * tileSize.height + tileSize.height/2));
                //随机放置
                if (visibleSize.width >= visibleSize.height)
                    curPos = Vec2(random<float>(gridRect.getMaxX()+tileSize.width/4, visibleSize.width-tileSize.width/4),
                                  random<float>(texSize.height/4, visibleSize.height-texSize.height/4));
                else
                    curPos = Vec2(random<float>(texSize.width/4, visibleSize.width-tileSize.width/4),
                                  random<float>(texSize.height/4, gridRect.getMinY()-texSize.height/4));
                
                tile->reset(texScale, dstPos, curPos);
                tile->setLocalZOrder(1+(int)_tiles.size());
                this->addChild(tile);
                _tiles.push_front(tile);
            }
        }
    });
    //进入动画
    gridNode->runAction(Sequence::create(PageTurn3D::create(1.5, Size(numTiles))->reverse(),
                                         ShakyTiles3D::create(0.5, Size(numTiles), 5, true),
                                         callfunc,
                                         nullptr));
    //拼图块的事件处理
    {
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, arrow](Touch *touch, Event *event){
            const auto& pos = touch->getLocation();
            for(auto iter = _tiles.begin(); iter != _tiles.end(); ++iter){
                auto t = *iter;
                if (t->getBoundingBox().containsPoint(pos)){
                    t->onTouchBegan();
                    t->setLocalZOrder(_tiles.front()->getLocalZOrder()+1);
                    _tiles.erase(iter);
                    _tiles.push_front(t);
                    return true;
                }
            }
            return false;
        };
        listener->onTouchMoved = [this, arrow, tileSize, visibleSize](Touch *touch, Event *event){
            _tiles.front()->setPosition(_tiles.front()->getPosition()+touch->getDelta());
        };
        listener->onTouchEnded = [this, arrow, tileSize, visibleSize](Touch *touch, Event *event){
            auto t = _tiles.front();
            if (t->onTouchEnded(touch->getStartLocation(), touch->getLocation()) >= 0){
                t->setLocalZOrder(_tiles.back()->getLocalZOrder());
                _tiles.pop_front();
                _tiles.push_back(t);
                if (t->matched()){
                    if (std::count_if(_tiles.begin(), _tiles.end(), [](Tile* t){ return t->matched();})
                        == _tiles.size()){
                        arrow->setLocalZOrder(_tiles.front()->getLocalZOrder()+1);
                        _tiles.clear();
                        auto tortoise = Tortoise::create();
                        tortoise->setTileSize(tileSize);
                        this->addChild(tortoise);
                        auto listener = EventListenerTouchOneByOne::create();
                        listener->setSwallowTouches(true);
                        listener->onTouchBegan = [this, tortoise](Touch *touch, Event *event){
                            return tortoise->onTouch(touch->getLocation());
                        };
                        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, tortoise);
                    }
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(EFFECT_FILE);
                }
            }
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
    //退出箭头的事件处理
    {
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, arrow](Touch *touch, Event *event){
            return arrow->getBoundingBox().containsPoint(touch->getLocation());
        };
        listener->onTouchMoved = [this, arrow, tileSize, visibleSize](Touch *touch, Event *event){
            Vec2 pos = arrow->getPosition();
            pos.x = clampf(pos.x+touch->getDelta().x, tileSize.width/2, visibleSize.width);
            arrow->setPosition(pos);
        };
        listener->onTouchEnded = [this, arrow, tileSize, visibleSize](Touch *touch, Event *event){
            const auto& pos = arrow->getPosition();
            if (pos.x >= tileSize.width - 1)
                arrow->runAction(MoveTo::create(0.5f, Vec2(visibleSize.width, pos.y)));
            else{
                arrow->setPosition(Vec2(visibleSize.width, pos.y));
                Director::getInstance()->stopAnimation();
                pickImage([]{}, [](cocos2d::Texture2D* tex2D){
                    Director::getInstance()->startAnimation();
                    if (tex2D)
                        Director::getInstance()->replaceScene(PuzzleLayer::createScene(tex2D));
                });
            }
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
    return true;
}
