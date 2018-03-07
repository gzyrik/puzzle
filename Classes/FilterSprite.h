#ifndef __FILTERSPIRTE_H__
#define __FILTERSPIRTE_H__

USING_NS_CC;


class FilterSprite : public Sprite
{
public:
    FilterSprite():_colorMatrix(Mat4::IDENTITY){}
    static FilterSprite* create()
    {
        auto sprite = new (std::nothrow) FilterSprite();
        if (sprite && sprite->init())
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    static FilterSprite* create(const std::string& filename)
    {
        auto sprite = new (std::nothrow) FilterSprite();
        if (sprite && sprite->initWithFile(filename))
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    static FilterSprite* create(const std::string& filename, const Rect& rect)
    {
        auto sprite = new (std::nothrow) FilterSprite();
        if (sprite && sprite->initWithFile(filename, rect))
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    static FilterSprite* createWithTexture(Texture2D *pTexture)
    {
        auto sprite = new (std::nothrow) FilterSprite();
        Rect rect = Rect::ZERO;
        rect.size = pTexture->getContentSize();
        if (sprite && sprite->initWithTexture(pTexture,rect))
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    static FilterSprite* createWithTexture(Texture2D *pTexture, const Rect& rect)
    {
        auto sprite = new (std::nothrow) FilterSprite();
        if (sprite && sprite->initWithTexture(pTexture, rect))
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    static FilterSprite* createWithSpriteFrame(SpriteFrame *pSpriteFrame)
    {
        auto sprite = new (std::nothrow) FilterSprite();
        if (sprite && pSpriteFrame && sprite->initWithSpriteFrame(pSpriteFrame))
        {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }
    static FilterSprite* createWithSpriteFrameName(const std::string& spriteFrameName)
    {
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
#if COCOS2D_DEBUG > 0
        char msg[256] = {0};
        sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
        CCASSERT(frame != nullptr, msg);
#endif
        return createWithSpriteFrame(frame);
    }
    void setColorMatrix(const cocos2d::Mat4& matrix) { _colorMatrix = matrix; }
protected:
    bool initWithTexture(Texture2D* pTexture, const Rect& tRect) override;
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    void onDraw(const Mat4 &transform, uint32_t flags);
    CustomCommand _customCommand;
private:
    cocos2d::Mat4 _colorMatrix;
};

#endif
