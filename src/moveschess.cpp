#include "moveschess.h"
MovesChess::MovesChess(){}

MovesChess::~MovesChess(){}


bool MovesChess::pawnBHit(int fromX, int fromY, int toX, int toY, bool chessColor)
{
     return (!chessColor && toX == fromX - 1 && toY == fromY + 1) ^ (!chessColor && toX == fromX + 1 && toY == fromY + 1);
}

bool MovesChess::pawnWHit(int fromX, int fromY, int toX, int toY, bool chessColor)
{
    return (chessColor && toX == fromX - 1 && toY == fromY - 1) ^ (chessColor && toX == fromX + 1 && toY == fromY - 1);
}

bool MovesChess::pawnMove(int fromX, int fromY, int toX, int toY, bool chessColor)
{
    bool start_position = (fromY == 1 && chessColor == BLACK) ^ (fromY == 6 && chessColor == WHITE);
    int step = chessColor == BLACK ? toY - fromY : fromY - toY;
    bool checkDirection = fromX == toX;
    return (checkDirection && start_position && step > 0 && step < 3) ^ (checkDirection && !start_position && step == 1);
}

bool MovesChess::rookMove(int x_way, int y_way)
{
    return x_way == 0 || y_way == 0;
}

bool MovesChess::knightMove(int x_way, int y_way)
{
    return (x_way == 2 && y_way == 1) ^ (y_way == 2 && x_way == 1);
}

bool MovesChess::bishopMove(int x_way, int y_way)
{
    return x_way != 0 && x_way == y_way;
}

bool MovesChess::queenMove(int x_way, int y_way)
{
    return (x_way == 0 || y_way == 0) ^ (x_way != 0 && x_way == y_way);
}

bool MovesChess::kingMove(int x_way, int y_way)
{
    return x_way == 1 || y_way == 1;
}
bool MovesChess::validMoves(int fromX, int fromY, int toX, int toY, bool chessColor, short type)
{
    int x_way = abs(fromX - toX);
    int y_way = abs(fromY - toY);
    switch (type) {
    case Logic::Pawn:
        return pawnMove(fromX,fromY,toX,toY, chessColor) ||
               pawnWHit(fromX,fromY,toX, toY, chessColor)||
               pawnBHit(fromX,fromY,toX, toY, chessColor);
    case Logic::Rook:
        return rookMove(x_way, y_way);
    case Logic::Knight:
        return knightMove(x_way, y_way);
    case Logic::Bishop:
        return bishopMove(x_way, y_way);
    case Logic::Queen:
        return queenMove(x_way, y_way);
    case Logic::King:
        return kingMove(x_way, y_way);
    default:
        return false;
    }
    return false;
}
