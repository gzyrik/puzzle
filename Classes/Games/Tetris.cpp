#include <Precompiled_headers.h>
#include "Tetris.h"
#include <algorithm>
// 方块类型
typedef enum {
    BrickType_NULL = -1, // 空
    BrickType_o,   // 正方形
    BrickType_T,
    BrickType_7,
    BrickType_r7,  // 反7
    BrickType_s,
    BrickType_z,
    BrickType_1,   // 条形
    BrickType_Count,
}BrickType;

// 方块模板初始化
static unsigned char m_BrickMode[7] = {0x66, 0xe4, 0xe2, 0xe8, 0x6c, 0xc6, 0xf0};

Tetris::Tetris(int row, int column,
               const std::function<void(int index, const Cell*)>& update) :
    _row(row), _column(column), _update(update)
{
    // 根据行数和列数申请内存
    int size = _row * _column;
    _cellMatrix = new Cell[size];
    memset(_cellMatrix, 0, size*sizeof(Cell));
}

Tetris::~Tetris()
{
    delete[] _cellMatrix;
}

// 设置新方块
bool Tetris::newBrick(int brickType, const Cell& brickCellValue)
{
    brickType = brickType % BrickType_Count;
    // 当前方块位置重置
    _posY = 0;
    _posX = (_column-4)/2;
    // 清空原有方块
    _curBrickType = 0;
    memset(_curBrick, 0, sizeof(_curBrick));
    // 根据模板填充方块
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c< 4; c++) {
            if (((m_BrickMode[brickType]<<(r*4+c)) & 0x80) > 0) {
                // 方块矩阵的这个位置有效，且方块矩阵的这个位置也有值，则游戏结束
                if (_cellMatrix[r*_column+(_posX+c)]){
                    memset(_curBrick, 0, sizeof(_curBrick));
                    return false;
                }
                _curBrick[1+r][c] = brickCellValue;
            }
        }
    }
    _curBrickType = brickType;
    return true;
}
void Tetris::drawBrick(int brickType, const Cell& cellValue,
                       const std::function<void(int index, const Cell*)>& update)
{
    brickType = brickType % BrickType_Count;
    // 根据模板填充方块
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c< 4; c++) {
            int index = (1+r)*4 + c;
            if (((m_BrickMode[brickType]<<(r*4+c)) & 0x80) > 0)
                update(index, &cellValue);
            else
                update(index, nullptr);
        }
    }
}
//固化当前块
void Tetris::fixBrick()
{
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (_curBrick[r][c])
                _cellMatrix[(_posY+r)*_column+(_posX+c)] = _curBrick[r][c];
}

//检测是否允许移动
bool Tetris::checkMove(int posY, int posX)
{
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            // 当前方块的这个位置有值
            if (_curBrick[r][c]) {
                if (posY + r < 0 || posY + r >= _row)
                    return false;
                if (posX + c < 0 || posX + c >= _column)
                    return false;
                if (_cellMatrix[(posY + r)*_column + (posX + c)])
                    return false;
            }
        }
    }
    return true;
}
// 方块移动操作
bool Tetris::moveBrick(int dY, int dX)
{
    if (dY == 0 && dX == 0)
        return true;
    if (!checkMove(_posY + dY, _posX + dX))
        return false;

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (_curBrick[r][c])
                _update((_posY+r)*_column + _posX+c, nullptr);
    _posY += dY;
    _posX += dX;
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (_curBrick[r][c])
                _update((_posY+r)*_column + _posX+c, &_curBrick[r][c]);
    return true;
}
int Tetris::clearLines()
{
    int startLine, lines = 0;
    const int rStart = std::min(_row-1, _posY+4);
    const int rEnd = std::max(0, _posY);
    for (int r = rStart; r >= rEnd; --r) {
        bool canClearLine = true;
        for (int c = 0; c < _column; ++c) {
            if (!_cellMatrix[r*_column+c]) {
                canClearLine = false;
                break;
            }
        }
        if (canClearLine) {
            // 从上往下赋值，内存移位处理即可
            memmove(_cellMatrix+_column, _cellMatrix, r*_column*sizeof(Cell));
            // 第0行清空
            memset(_cellMatrix, 0, _column * sizeof(void*));
            if (0 == lines++)
                startLine = r;
        }
    }
    if (lines > 0) {
        for (int r = startLine; r >= 0; --r)
            for (int c = 0; c < _column; ++c)
                _update(r*_column + c, &_cellMatrix[r*_column+c]);
    }
    return lines;
}
bool Tetris::rotateBrick()
{
    // 正方形不旋转
    if (BrickType_o == _curBrickType) {
        return false;
    }
    // 用临时方块试着旋转一下
    Cell tempBrick[4][4];
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c< 4; c++) {
            tempBrick[r][c] = 0;
        }
    }
    // 条形方块旋转
    if (BrickType_1 == _curBrickType) {
        // 竖形
        if (_curBrick[0][1]) {
            for (int i = 0; i < 4; i++) {
                tempBrick[1][i] = _curBrick[i][1];
            }
        }
        // 横形
        else {
            for (int i = 0; i < 4; i++) {
                tempBrick[i][1] = _curBrick[1][i];
            }
        }
    }
    // s形方块旋转
    else if (BrickType_s == _curBrickType) {
        // 竖形
        if (_curBrick[0][0]) {
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[1][2] = _curBrick[1][1];
            tempBrick[2][0] = _curBrick[1][1];
            tempBrick[2][1] = _curBrick[1][1];
        }
        // 横形
        else {
            tempBrick[0][0] = _curBrick[1][1];
            tempBrick[1][0] = _curBrick[1][1];
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[2][1] = _curBrick[1][1];
        }
    }
    // z形方块旋转
    else if (BrickType_z == _curBrickType) {
        // 竖形
        if (_curBrick[0][1]) {
            tempBrick[1][0] = _curBrick[1][1];
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[2][1] = _curBrick[1][1];
            tempBrick[2][2] = _curBrick[1][1];
        }
        // 横形
        else {
            tempBrick[0][1] = _curBrick[1][1];
            tempBrick[1][0] = _curBrick[1][1];
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[2][0] = _curBrick[1][1];
        }
    }
    // 其他方块旋转
    else {
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                tempBrick[r][c] = _curBrick[2 - c][r];
            }
        }
    }
    // 查看旋转后与方块矩阵是否冲突
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            // 当前方块的这个位置有值
            if (tempBrick[r][c]) {
                // 左边，或者右边，或者下边超出边界，旋转失败
                if (_posX + c < 0 || _posX + c >= _column || _posY + r >= _row) {
                    return false;
                }
                // 方块矩阵的这个位置有效，且方块矩阵的这个位置也有值，则旋转失败
                int offset = (_posY + r)*_column + (_posX + c);
                if (offset >= 0 && offset < _row*_column &&_cellMatrix[offset]) {
                    return false;
                }
            }
        }
    }
    // 不冲突则旋转
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (_curBrick[r][c])
                _update((_posY+r)*_column + _posX+c, nullptr);

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            _curBrick[r][c] = tempBrick[r][c];
            if (_curBrick[r][c])
                _update((_posY+r)*_column + _posX+c, &_curBrick[r][c]);
        }
    }
    return true;
}

