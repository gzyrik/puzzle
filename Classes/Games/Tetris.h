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
    //单元格类
    typedef uint8_t Cell;
    // 游戏区方块矩阵
    const int _row, _column;
public:
    /**
     * @param[in] row
     * @param[in] column
     * @param[in] update 单元格更新函数
     */
    Tetris(const int row, const int column,
           const std::function<void(int index, const Cell*)>& update);

    ~Tetris();
    /**
     * 移动当前活动块
     * @param[in] dY  上下移动
     * @param[in] dX  左右移动
     * @return 不允许返回false
     */
    bool moveBrick(int dY, int dX);

    //旋转活动块
    bool rotateBrick();

    /**
     * 移动当前活动块
     * @return 不允许返回false
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
     * 固化当前活动块
     */
    void fixBrick();

    /**
     * 生成新活动块
     * @param[in] brickType 块形状类型
     * @param[in] cellValue 块中有效单元格的值
     * @return 不允许生成返回false
     */
    bool newBrick(int brickType, const Cell& cellValue);

    //执行消除并返回消除的行数
    int clearLines();
    
    cocos2d::Vec2 getPosition(int brickSize, const cocos2d::Vec2& offset)
    {
        return cocos2d::Vec2(_posX, _row - _posY) * brickSize + offset;
    }
    static void drawBrick(int brickType, const Cell& cellValue,
                          const std::function<void(int index, const Cell*)>& update);
private:
    Cell *_cellMatrix;//_row x _column 二维数组
    //单元格更新函数
    const std::function<void(int index, const Cell*)> _update;
    // 当前方块
    int  _curBrickType;
    Cell _curBrick[4][4];
    // 当前方块的位置
    int _posY, _posX;
    //检测位置(posY,posX)是否允许放置活动块
    bool checkMove(int posY, int posX);
};

#endif
