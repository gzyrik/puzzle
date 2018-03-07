#ifndef __TORTOISE_H__
#define __TORTOISE_H__
class Tortoise : public cocos2d::Node
{
    cocos2d::Sprite3D* _sprite;
    cocos2d::ParticleFlower* _emitter;
    cocos2d::Animate3D* _hurt;
    cocos2d::RepeatForever* _swim;
    float _elapseTransTime;
    int _state;
    float _speed;
    virtual bool init();
    void reachEndCallBack();
    virtual void update(float dt);
    virtual ~Tortoise() {
        if (_hurt)
            _hurt->release();
        if (_swim)
            _swim->release();
    }
public:
    CREATE_FUNC(Tortoise);
    bool onTouch(const cocos2d::Vec2& pos);
    void setTileSize(const cocos2d::Size& tileSize);
};
#endif
