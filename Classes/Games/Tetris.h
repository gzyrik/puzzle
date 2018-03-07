#ifndef __TETRIS_H__
#define __TETRIS_H__

#include <stdio.h>
enum class TetrisAction
{
    KEEP_STATE = -1,
    ROTATE_CW,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DOWN
};
class Tetris
{
public:
    //��Ԫ����
    typedef uint8_t Cell;
    // ��Ϸ���������
    const int _row, _column;
public:
    /**
     * @param[in] row
     * @param[in] column
     * @param[in] update ��Ԫ����º���
     */
    Tetris(const int row, const int column,
           const std::function<void(int index, const Cell*)>& update);

    ~Tetris();
    /**
     * �ƶ���ǰ���
     * @param[in] dY  �����ƶ�
     * @param[in] dX  �����ƶ�
     * @return ��������false
     */
    bool moveBrick(int dY, int dX);

    //��ת���
    bool rotateBrick();

    /**
     * �ƶ���ǰ���
     * @return ��������false
     */
    bool doAction(const TetrisAction& action)
    {
        switch(action) {
            case TetrisAction::ROTATE_CW:
                return rotateBrick();
            case TetrisAction::MOVE_LEFT:
                return moveBrick(0, -1);
            case TetrisAction::MOVE_RIGHT:
                return moveBrick(0, +1);
            case TetrisAction::MOVE_DOWN:
                return moveBrick(1, 0);
            default:
                return false;
        }
    }

    /**
     * �̻���ǰ���
     */
    void fixBrick();

    /**
     * �����»��
     * @param[in] brickType ����״����
     * @param[in] cellValue ������Ч��Ԫ���ֵ
     * @return ���������ɷ���false
     */
    bool newBrick(int brickType, const Cell& cellValue);

    //ִ����������������������
    int clearLines();
    
    cocos2d::Vec2 getPosition(int brickSize, const cocos2d::Vec2& offset)
    {
        return cocos2d::Vec2(_posX, _row - _posY) * brickSize + offset;
    }
    static void drawBrick(int brickType, const Cell& cellValue,
                          const std::function<void(int index, const Cell*)>& update);
private:
    Cell *_cellMatrix;//_row x _column ��ά����
    //��Ԫ����º���
    const std::function<void(int index, const Cell*)> _update;
    // ��ǰ����
    int  _curBrickType;
    Cell _curBrick[4][4];
    // ��ǰ�����λ��
    int _posY, _posX;
    //���λ��(posY,posX)�Ƿ�������û��
    bool checkMove(int posY, int posX);
};

#endif
