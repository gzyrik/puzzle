#ifndef __PUZZLE_LAYER_H__
#define __PUZZLE_LAYER_H__
class PuzzleLayer : public cocos2d::Layer
{
public:
    /** ����ƴͼ��Ϸ��
     * @param[in] tex2D ƴͼ
     * @param[in] gridSize ƴͼ�зֿ���
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
    //����Ĭ�ϵĳ���
    static cocos2d::Scene* createScene(cocos2d::Texture2D* tex2D);
private:
    //ƴͼ��
    class Tile : public cocos2d::Sprite
    {
        typedef std::function<int(cocos2d::Vec2&, int&)> Detector;
        int       _index;
        int       _angle;//��ǰ�Ƕ�
        int       _delta;//��Ŀ������
        cocos2d::Vec2 _texScale;//��������
        Detector  _detector;//�����

    public:
        /**����ƴͼ��
         * @param[in] tex2D     ����
         * @param[in] texScale  ��������
         * @param[in] texRect   ��������
         * @param[in] detector  ��������
         * @return ����ƴͼ��
         */
        static Tile* create(cocos2d::Texture2D* tex2D,
                            const cocos2d::Rect& texRect,
                            const Detector& detector);
        void reset(const cocos2d::Vec2& texScale, const cocos2d::Vec2& dstPos, const cocos2d::Vec2& curPos);
        //׼���϶�
        void onTouchBegan();
        /** �����϶�
         * @param startPos      �϶�ǰָ���GL����
         * @param[in] endPos    �϶���ָ���GL����
         * @return ��Ŀ�����
         */
        int onTouchEnded(const cocos2d::Vec2& startPos, const cocos2d::Vec2& endPos);
        //�Ƿ���ƥ��
        bool matched() const { return !_delta && !_angle; }
    };
private:
    virtual bool init(cocos2d::Texture2D* tex2D, const cocos2d::Size& gridSize);
    std::list<Tile*> _tiles;
    std::vector<bool> _masks;
};

#endif // __PUZZLE_LAYER_H__
