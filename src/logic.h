#pragma once
#include <memory>
#include <QAbstractListModel>
#include <QJsonArray>

class MovesChess;

struct Figure
{
  bool chessColor;
  int type;
  int x;
  int y;
};

struct History
{
    int figureIndex;
    int posX;
    int posY;
    int targetX;
    int targetY;
    bool isHit;
    History(int index, int toX,int toY, int fromX, int fromY, bool isHit){
        this->figureIndex = index;
        this->targetX = toX;
        this->targetY = toY;
        this->posX = fromX;
        this->posY = fromY;
        this->isHit = isHit;
    }
};

struct Died
{
    int index;
    int x;
    int y;
    Died(int index, int x, int y)
    {
        this->index = index;
        this->x = x;
        this->y = y;
    }
};

class Logic: public QAbstractListModel
{
    Q_OBJECT
public:

    enum GlobalConstants {
        BOARD_SIZE = 8
    };

    enum Roles {
        ChessColor = Qt::UserRole,
        Type,
        PositionX,
        PositionY,
    };
    
    enum Types{
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
        King,
    };

public:
    explicit Logic(QObject *parent = 0);
    ~Logic();

    Q_PROPERTY(int boardSize READ boardSize CONSTANT)
    int boardSize() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE bool move(int fromX, int fromY, int toX, int toY);
    Q_INVOKABLE void startNewGame();
    Q_INVOKABLE void saveGame();
    Q_INVOKABLE bool loadGame();
    Q_INVOKABLE void nextLogic();
    Q_INVOKABLE void prevLogic();

protected:
    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void convertJsonToObj(const QJsonArray &arr, QList<History> &history);
    void clearMoveHistory(QJsonArray &movesHistory);
private:
    QList<Died> died;
    QList<History> history;
    struct Impl;
    int step;
    bool isHit;
    std::unique_ptr<Impl> impl;
    std::unique_ptr<MovesChess> movesChess;
    QJsonArray movesHistory;
    bool isMoveBlocked(int fromX, int fromY, int toX, int toY,int index,int indexTarget,bool turn, bool &isHit);
    void executeMove(int toX, int toY, int index);
    void saveMove(int fromX, int fromY, int toX, int toY, int index,bool isHit, QJsonObject &json);
    bool _turn;
};
