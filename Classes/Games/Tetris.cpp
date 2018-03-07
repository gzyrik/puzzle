#include <Precompiled_headers.h>
#include "Tetris.h"
#include <algorithm>
// ��������
typedef enum {
    BrickType_NULL = -1, // ��
    BrickType_o,   // ������
    BrickType_T,
    BrickType_7,
    BrickType_r7,  // ��7
    BrickType_s,
    BrickType_z,
    BrickType_1,   // ����
    BrickType_Count,
}BrickType;

// ����ģ���ʼ��
static unsigned char m_BrickMode[7] = {0x66, 0xe4, 0xe2, 0xe8, 0x6c, 0xc6, 0xf0};

Tetris::Tetris(int row, int column,
               const std::function<void(int index, const Cell*)>& update) :
    _row(row), _column(column), _update(update)
{
    // �������������������ڴ�
    int size = _row * _column;
    _cellMatrix = new Cell[size];
    memset(_cellMatrix, 0, size*sizeof(Cell));
}

Tetris::~Tetris()
{
    delete[] _cellMatrix;
}

// �����·���
bool Tetris::newBrick(int brickType, const Cell& brickCellValue)
{
    brickType = brickType % BrickType_Count;
    // ��ǰ����λ������
    _posY = 0;
    _posX = (_column-4)/2;
    // ���ԭ�з���
    _curBrickType = 0;
    memset(_curBrick, 0, sizeof(_curBrick));
    // ����ģ����䷽��
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c< 4; c++) {
            if (((m_BrickMode[brickType]<<(r*4+c)) & 0x80) > 0) {
                // �����������λ����Ч���ҷ����������λ��Ҳ��ֵ������Ϸ����
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
    // ����ģ����䷽��
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
//�̻���ǰ��
void Tetris::fixBrick()
{
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (_curBrick[r][c])
                _cellMatrix[(_posY+r)*_column+(_posX+c)] = _curBrick[r][c];
}

//����Ƿ������ƶ�
bool Tetris::checkMove(int posY, int posX)
{
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            // ��ǰ��������λ����ֵ
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
// �����ƶ�����
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
            // �������¸�ֵ���ڴ���λ������
            memmove(_cellMatrix+_column, _cellMatrix, r*_column*sizeof(Cell));
            // ��0�����
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
    // �����β���ת
    if (BrickType_o == _curBrickType) {
        return false;
    }
    // ����ʱ����������תһ��
    Cell tempBrick[4][4];
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c< 4; c++) {
            tempBrick[r][c] = 0;
        }
    }
    // ���η�����ת
    if (BrickType_1 == _curBrickType) {
        // ����
        if (_curBrick[0][1]) {
            for (int i = 0; i < 4; i++) {
                tempBrick[1][i] = _curBrick[i][1];
            }
        }
        // ����
        else {
            for (int i = 0; i < 4; i++) {
                tempBrick[i][1] = _curBrick[1][i];
            }
        }
    }
    // s�η�����ת
    else if (BrickType_s == _curBrickType) {
        // ����
        if (_curBrick[0][0]) {
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[1][2] = _curBrick[1][1];
            tempBrick[2][0] = _curBrick[1][1];
            tempBrick[2][1] = _curBrick[1][1];
        }
        // ����
        else {
            tempBrick[0][0] = _curBrick[1][1];
            tempBrick[1][0] = _curBrick[1][1];
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[2][1] = _curBrick[1][1];
        }
    }
    // z�η�����ת
    else if (BrickType_z == _curBrickType) {
        // ����
        if (_curBrick[0][1]) {
            tempBrick[1][0] = _curBrick[1][1];
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[2][1] = _curBrick[1][1];
            tempBrick[2][2] = _curBrick[1][1];
        }
        // ����
        else {
            tempBrick[0][1] = _curBrick[1][1];
            tempBrick[1][0] = _curBrick[1][1];
            tempBrick[1][1] = _curBrick[1][1];
            tempBrick[2][0] = _curBrick[1][1];
        }
    }
    // ����������ת
    else {
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                tempBrick[r][c] = _curBrick[2 - c][r];
            }
        }
    }
    // �鿴��ת���뷽������Ƿ��ͻ
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            // ��ǰ��������λ����ֵ
            if (tempBrick[r][c]) {
                // ��ߣ������ұߣ������±߳����߽磬��תʧ��
                if (_posX + c < 0 || _posX + c >= _column || _posY + r >= _row) {
                    return false;
                }
                // �����������λ����Ч���ҷ����������λ��Ҳ��ֵ������תʧ��
                int offset = (_posY + r)*_column + (_posX + c);
                if (offset >= 0 && offset < _row*_column &&_cellMatrix[offset]) {
                    return false;
                }
            }
        }
    }
    // ����ͻ����ת
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

