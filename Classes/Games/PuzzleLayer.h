#ifndef __PUZZLE_LAYER_H__
#define __PUZZLE_LAYER_H__
class PuzzleLayer : public cocos2d::Layer
{
public:
    /** 创建拼图游戏层
     * @param[in] tex2D 拼图
     * @param[in] gridSize 拼图切分块数
     */
    static PuzzleLayer* create(cocos2d::Texture2D* tex2D, const cocos2d::Size& gridSize)
    {
        auto layer = new(std::nothrow) PuzzleLayer();
        if (layer && layer->init(tex2D, gridSize))
        {
            layer->autorelease();
            return layer;
        }
        CC_SAFE_DELETE(layer);
        return nullptr;
    }
    //创建默认的场景
    static cocos2d::Scene* createScene(cocos2d::Texture2D* tex2D);
private:
    //拼图块
    class Tile : public cocos2d::Sprite
    {
        typedef std::function<int(cocos2d::Vec2&, int&)> Detector;
        int       _index;
        int       _angle;//当前角度
        int       _delta;//与目标块距离
        cocos2d::Vec2 _texScale;//纹理伸缩
        Detector  _detector;//检测器

    public:
        /**创建拼图块
         * @param[in] tex2D     纹理
         * @param[in] texScale  纹理伸缩
         * @param[in] texRect   纹理区域
         * @param[in] detector  坐标检测器
         * @return 返回拼图块
         */
        static Tile* create(cocos2d::Texture2D* tex2D,
                            const cocos2d::Rect& texRect,
                            const Detector& detector);
        void reset(const cocos2d::Vec2& texScale, const cocos2d::Vec2& dstPos, const cocos2d::Vec2& curPos);
        //准备拖动
        void onTouchBegan();
        /** 结束拖动
         * @param startPos      拖动前指针的GL坐标
         * @param[in] endPos    拖动后指针的GL坐标
         * @return 离目标距离
         */
        int onTouchEnded(const cocos2d::Vec2& startPos, const cocos2d::Vec2& endPos);
        //是否已匹配
        bool matched() const { return !_delta && !_angle; }
    };
private:
    virtual bool init(cocos2d::Texture2D* tex2D, const cocos2d::Size& gridSize);
    std::list<Tile*> _tiles;
    std::vector<bool> _masks;
};

#endif // __PUZZLE_LAYER_H__
