#include "logic.h"
#include "moveschess.h"
#include <QHash>
#include <iostream>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QDateTime>

struct Logic::Impl
{
  QList<Figure> figures;
  int findByPosition(int x, int y);
};

int Logic::Impl::findByPosition(int x, int y) {
  for (int i(0); i<figures.size(); ++i) {
    if (figures[i].x != x || figures[i].y != y )continue;
    return i;
  }
  return -1;
}

Logic::Logic(QObject *parent)
  : QAbstractListModel(parent)
  , impl(new Impl()),movesChess(new MovesChess()){

}

void Logic::startNewGame()
{
    // placement black and white Pawns
    for (int x = 0; x < BOARD_SIZE; ++x) {
        impl->figures << Figure {BLACK,Pawn, x, 1 };
        impl->figures << Figure {WHITE,Pawn, x, 6 };
    }
    //placement white figurs
    impl->figures << Figure { WHITE,Rook, 0, 7 };
    impl->figures << Figure { WHITE,Rook, 7, 7 };
    impl->figures << Figure { WHITE,Knight, 1, 7 };
    impl->figures << Figure { WHITE,Knight, 6, 7 };
    impl->figures << Figure { WHITE,Bishop, 2, 7 };
    impl->figures << Figure { WHITE,Bishop, 5, 7 };
    impl->figures << Figure { WHITE,Queen, 3, 7 };
    impl->figures << Figure { WHITE,King, 4, 7 };

    //placement black figurs
    impl->figures << Figure { BLACK,Rook, 0, 0 };
    impl->figures << Figure { BLACK,Rook, 7, 0 };
    impl->figures << Figure { BLACK,Knight, 1, 0 };
    impl->figures << Figure { BLACK,Knight, 6, 0 };
    impl->figures << Figure { BLACK,Bishop, 2, 0 };
    impl->figures << Figure { BLACK,Bishop, 5, 0 };
    impl->figures << Figure { BLACK,Queen, 3, 0 };
    impl->figures << Figure { BLACK,King, 4, 0 };
    _turn = WHITE;
    step = 0;
    qDebug("Logic: the new game created");
}

Logic::~Logic() {

}
int Logic::boardSize() const {
  return BOARD_SIZE;
}

int Logic::rowCount(const QModelIndex & ) const {
  return impl->figures.size(); 
}



QHash<int, QByteArray> Logic::roleNames() const {
  QHash<int, QByteArray> names;
  names.insert(Roles::ChessColor, "chessColor");
  names.insert(Roles::Type      , "type");
  names.insert(Roles::PositionX , "positionX");
  names.insert(Roles::PositionY , "positionY");
  return names;
}

QVariant Logic::data(const QModelIndex & modelIndex, int role) const { 
  if (!modelIndex.isValid()) {
    return QVariant();
  }
    
  int index = static_cast<int>(modelIndex.row());
  
  if (index >= impl->figures.size() || index < 0) {
    return QVariant();
  }

  Figure & figure = impl->figures[index];

  switch (role) {
    case Roles::ChessColor    : return figure.chessColor;
    case Roles::Type          : return figure.type;
    case Roles::PositionX     : return figure.x;
    case Roles::PositionY     : return figure.y;
  }
  return QVariant(); 
}

void Logic::clear() {
  beginResetModel();
  impl->figures.clear();
  history.clear();
  clearMoveHistory(movesHistory);
  endResetModel();
  _turn = WHITE;
  qDebug("Logic: chess board is empty");
}
bool Logic::move(int fromX, int fromY, int toX, int toY) {
  int index = impl->findByPosition(fromX, fromY);
  int indexTarget = impl->findByPosition(toX,toY);
  bool outSideBoard = toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE;

  if (outSideBoard)
  {
      qDebug("OutSideBoard");
      return false;
  }

  if(index > 0){
      bool _color = impl->figures[index].chessColor == WHITE;
      int _type = impl->figures[index].type;

      if (!movesChess->validMoves(fromX, fromY, toX, toY, _color, _type)) {
          return false;
      }
  }

  if(isMoveBlocked(fromX,fromY,toX,toY,index,indexTarget, _turn, isHit)){
      qDebug("The move is blocked");
      return false;
  }
  QJsonObject json;
  saveMove(fromX, fromY, toX, toY, index,isHit, json);
  movesHistory.append(json);
  executeMove(toX, toY, index);
  _turn = _turn == WHITE ? BLACK : WHITE;
  return true;
}

void Logic::executeMove(int toX, int toY, int index)
{
    beginResetModel();
    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight);
    endResetModel();
}

bool Logic::isMoveBlocked(int fromX, int fromY, int toX, int toY,int index, int indexTarget, bool turn, bool &isHit){
    isHit = false;
    bool isPawn = impl->figures[index].type == Pawn;
    bool isKnight = impl->figures[index].type == Knight;
    bool checkColor = impl->figures[index].chessColor == WHITE;
    int stepX = toX > fromX ? (toX == fromX ? 0 : 1) :(toX == fromX ? 0 : -1);
    int stepY = toY > fromY ? (toY == fromY ? 0 : 1) :(toY == fromY ? 0 : -1);
    bool isColor = impl->figures[index].chessColor;
    if(isColor != turn)
    {
        (isColor ?qDebug("The turn is BLACK"):qDebug("The turn is WHITE"));
        return true;
    }
    bool isColorTarget;
    if(indexTarget >= 0)
        isColorTarget = impl->figures[indexTarget].chessColor;
    bool isFight = isColor != isColorTarget;
    bool isPawnWHit = movesChess->pawnWHit(fromX,fromY,toX,toY, checkColor);
    bool isPawnBHit = movesChess->pawnBHit(fromX,fromY,toX,toY, checkColor);
    fromX = stepX != 0 ? fromX + stepX : fromX;
    fromY = stepY != 0 ? fromY + stepY : fromY;
    if((isPawn && indexTarget >= 0 && isPawnWHit && isFight) ^ (isPawn && indexTarget >= 0 && isPawnBHit && isFight)){
        isHit = true;
        executeMove(-1,-1,indexTarget);
        return false;
    }
    else if((isPawn && indexTarget < 0 && isPawnWHit) ^ (isPawn && indexTarget < 0 && isPawnBHit))
        return true;
    if(isKnight && indexTarget < 0){
        return false;
    }
    else if(isKnight && indexTarget >= 0 && isFight){
        isHit = true;
        executeMove(-1,-1,indexTarget);
        return false;
    }
    while(fromX != toX || fromY != toY){
        short barrier = impl->findByPosition(fromX,fromY);
        if(barrier >= 0)
           return true;
        fromX += stepX;
        fromY += stepY;
    }
    if(indexTarget >= 0 && isFight && !isPawn)
    {
        isHit = true;
        executeMove(-1,-1,indexTarget);
        return false;
    }
    return indexTarget >= 0;
}

void Logic::saveMove(int fromX, int fromY,int toX, int toY, int index, bool isHit, QJsonObject &json){
    json["Index"] = index;
    json["toX"] = toX;
    json["toY"] = toY;
    json["fromX"] = fromX;
    json["fromY"] = fromY;
    json["HIT"] = isHit;
}

void Logic::saveGame(){
    QString path = "savegame.json";

    QFile saveFile(path);
    saveFile.remove();
    if(!saveFile.open(QIODevice::WriteOnly)){
        qDebug("Couln't open save file");
    }

    QJsonDocument saveDoc(movesHistory);
    saveFile.write(saveDoc.toJson());
    if(movesHistory.empty())
        saveFile.remove();
    if(saveFile.isOpen())
        saveFile.close();
}

bool Logic::loadGame(){

    QString path = "savegame.json";
    QFile loadFile(path);

    if(!loadFile.open(QIODevice::ReadOnly)){
        qDebug("Couldn't open save file");
        return false;
    }
    else
    {
        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
        convertJsonToObj(loadDoc.array(),history);
    }
    return true;
}

void Logic::nextLogic(){
    if(step != history.size())
    {
        History hist = history[step];
        int indexTo = impl->findByPosition(hist.targetX, hist.targetY);
        if(indexTo >= 0)
        {
            died.append(Died(indexTo,hist.targetX, hist.targetY));
            executeMove(-1,-1,indexTo);
        }
        executeMove(hist.targetX,hist.targetY,hist.figureIndex);
        step++;
    }
}

void Logic::clearMoveHistory(QJsonArray &movesHistory)
{
    while (!movesHistory.empty()) {
        movesHistory.pop_back();
    }
}

void Logic::prevLogic(){
    step--;
    if(step < 0)
        step = 0;
    History hist = history[step];
    bool hit = hist.isHit;
    if(hit)
    {
        executeMove(died.back().x,died.back().y,died.back().index);
        died.pop_back();
    }
    executeMove(hist.posX,hist.posY,hist.figureIndex);
}

void Logic::convertJsonToObj(const QJsonArray &arr, QList<History> &history){
    for (int i = 0; i < arr.size(); ++i) {
       QJsonObject json = QJsonObject(arr[i].toObject());
       int index = json["Index"].toInt();
       int toX = json["toX"].toInt();
       int toY = json["toY"].toInt();
       int fromX = json["fromX"].toInt();
       int fromY = json["fromY"].toInt();
       bool isHit = json["HIT"].toBool();
       history.append(History(index,toX,toY,fromX,fromY,isHit));
    }
}
