#ifndef __TETRIS_LAYER_H__
#define __TETRIS_LAYER_H__
class Tetris;
enum class TetrisAction;
class TetrisLayer : public cocos2d::Layer
{
public:
    /** ��������˹������Ϸ��
     * @param[in] tex2D ����
     * @param[in] gridSize ƴͼ�зֿ���
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
    //����Ĭ�ϵĳ���
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
