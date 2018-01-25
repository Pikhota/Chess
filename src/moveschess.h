#ifndef MOVESCHESS_H
#define MOVESCHESS_H

#include "logic.h"

#define WHITE  true
#define BLACK  false

class MovesChess
{
public:
    MovesChess();
    ~MovesChess();

    bool validMoves(int fromX, int fromY, int toX, int toY , bool chessColor, short type);
    bool pawnBHit(int fromX, int fromY, int toX, int toY, bool chessColor);
    bool pawnWHit(int fromX, int fromY, int toX, int toY, bool chessColor);
private:
    bool pawnMove(int fromX, int fromY, int toX, int toY, bool chessColor);
    bool rookMove (int x_way,int y_way);
    bool knightMove(int x_way,int y_way);
    bool bishopMove(int x_way,int y_way);
    bool queenMove(int x_way,int y_way);
    bool kingMove(int x_way, int y_way);
};

#endif // MOVESCHESS_H
