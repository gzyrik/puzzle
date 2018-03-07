#ifndef __TETRIS_LAYER_H__
#define __TETRIS_LAYER_H__
class Tetris;
enum class TetrisAction;
class TetrisLayer : public cocos2d::Layer
{
public:
    /** 创建俄罗斯方块游戏层
     * @param[in] tex2D 方块
     * @param[in] gridSize 拼图切分块数
     */
    static TetrisLayer* create(const int brickSize)
    {
        auto layer = new(std::nothrow) TetrisLayer();
        if (layer && layer->init(brickSize))
        {
            layer->autorelease();
            return layer;
        }
        CC_SAFE_DELETE(layer);
        return nullptr;
    }
    //创建默认的场景
    static cocos2d::Scene* createScene(const int brickSize);
private:
    virtual bool init(const int brickSize);
    void updateAction(float dt);
    Tetris* _tetris;
    TetrisAction _action;
    int _score;
    int _nextBrickType;
    int _nextBrickValue;
};
#endif // __TETRIS_LAYER_H__
