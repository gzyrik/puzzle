#include <Precompiled_headers.h>
#include "Tortoise.h"
USING_NS_CC;
enum {
    STATE_SWIMMING,
    STATE_SWIMMING_TO_HURT,
    STATE_HURT,
    STATE_HURT_TO_SWIMMING,
    ACTION_SEQ_MOVE_TAG = 100,
    ACTION_SEQ_HURT_TAG,
};
void Tortoise::reachEndCallBack()
{
    const auto& s = Director::getInstance()->getOpenGLView()->getVisibleSize();
    this->stopActionByTag(ACTION_SEQ_MOVE_TAG);
    const Vec2& curPos = this->getPosition();
    const Vec2 dstPos(s.width - curPos.x, s.height * rand_0_1());
    float time = (dstPos - curPos).length() / _speed;
    auto seq = Sequence::create(DelayTime::create(1.f),
                                MoveTo::create(time, dstPos),
                                CallFunc::create([this]{reachEndCallBack();}),
                                nullptr);
    _sprite->runAction(RotateBy::create(1, Vec3(0, 180, 0)));
    const int angle = (int)_sprite->getRotation3D().y  % 360;
    _emitter->runAction(MoveTo::create(1, _sprite->getPosition()- Vec2(angle == 180 ? -100:100, 0)));
    seq->setTag(ACTION_SEQ_MOVE_TAG);
    this->runAction(seq);
}
void Tortoise::update(float dt)
{
    if (_state == STATE_HURT_TO_SWIMMING) {
        _elapseTransTime += dt;
        
        if (_elapseTransTime >= Animate3D::getTransitionTime()) {
            _sprite->stopAction(_hurt);
            _state = STATE_SWIMMING;
        }
    }
    else if (_state == STATE_SWIMMING_TO_HURT) {
        _elapseTransTime += dt;
        if (_elapseTransTime >= Animate3D::getTransitionTime()) {
            _sprite->stopAction(_swim);
            _state = STATE_HURT;
        }
    }
}
void Tortoise::setTileSize(const cocos2d::Size& tileSize)
{
    _sprite->setScale(tileSize.width/_sprite->getContentSize().width);
}
bool Tortoise::onTouch(const cocos2d::Vec2& pos)
{
    if (_state == STATE_SWIMMING && _sprite->getBoundingBox().containsPoint(pos)){
        _elapseTransTime = 0.0f;
        _state = STATE_SWIMMING_TO_HURT;
        _sprite->stopAction(_hurt);
        _sprite->runAction(_hurt);
        auto seq = Sequence::create(DelayTime::create(_hurt->getDuration() - Animate3D::getTransitionTime()),
                                    CallFunc::create([this]{
            _sprite->runAction(_swim);
            _state = STATE_HURT_TO_SWIMMING;
            _elapseTransTime = 0.0f;
        }),
                                    nullptr);
        _sprite->runAction(seq);
        return true;
    }
    return false;
}
bool Tortoise::init()
{
    _speed = 200;
    _state = STATE_SWIMMING;
    const auto& s = Director::getInstance()->getOpenGLView()->getVisibleSize();
    this->setPosition(Vec2(s.width *rand_0_1(), s.height *rand_0_1()));
    
    const std::string fileName("3d/tortoise.c3b");
    _sprite = Sprite3D::create(fileName);
    if (!_sprite)
        return false;
    auto animation = Animation3D::create(fileName);
    if (!animation)
        return false;
    
    _sprite->setGlobalZOrder(10000);
    this->addChild(_sprite);
    
    _emitter = ParticleFlower::create();
    _emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("fx/stars.png"));
    _emitter->setGlobalZOrder(10000);
    _emitter->setPositionX(_sprite->getPositionX()-100);
    this->addChild(_emitter);

    auto animate = Animate3D::create(animation, 0.f, 1.933f);
    _swim = RepeatForever::create(animate);
    _swim->retain();
    _sprite->runAction(_swim);
    
    _hurt = Animate3D::create(animation, 1.933f, 2.8f);
    _hurt->retain();
    
    this->setPosition(Vec2(0, s.height * rand_0_1()));
    reachEndCallBack();
    scheduleUpdate();
    return true;
}
