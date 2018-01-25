import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2



ApplicationWindow {
    id: root
    title: qsTr("Chess")
    visible: true
    width: 800
    height: 600
    
    property int squareSize: 70

    property var images: [
        [
          {'imgPath' : "/images/white_pawn.svg"},
          {'imgPath' : "/images/white_rook.svg"},
          {'imgPath' : "/images/white_knight.svg"},
          {'imgPath' : "/images/white_bishop.svg"},
          {'imgPath' : "/images/white_queen.svg"},
          {'imgPath' : "/images/white_king.svg"}
        ],
        [
          {'imgPath' : "/images/black_pawn.svg"},
          {'imgPath' : "/images/black_rook.svg"},
          {'imgPath' : "/images/black_knight.svg"},
          {'imgPath' : "/images/black_bishop.svg"},
          {'imgPath' : "/images/black_queen.svg"},
          {'imgPath' : "/images/black_king.svg"}
        ]
    ]

    Component{
        id: chessPiecesPlacementComponent
        Item {
          Repeater {
            model: logic
            Image {
              height: squareSize
              width : squareSize

              x: squareSize * positionX
              y: squareSize * positionY

              source: images[(chessColor) == true ? 0 : 1][type].imgPath

              MouseArea {
                anchors.fill: parent
                drag.target: parent

                property int startX: 0
                property int startY: 0

                onPressed: {
                  startX = parent.x;
                  startY = parent.y;
                  console.log("onPressed: startX,startY " + startX + "," + startY);
                }
                onReleased: {
                  var fromX = startX / squareSize;
                  var fromY = startY / squareSize;
                  var toX   = (parent.x + mouseX) / squareSize;
                  var toY   = (parent.y + mouseY) / squareSize;

                  if (!logic.move(fromX, fromY, toX, toY)) {
                    parent.x = startX;
                    parent.y = startY;
                  }
                }
              }
            }
          }
        }
    }

    Component{
        id: gameBoardComponent
        Image {
            source: "/images/chess_board.jpg"
            width : logic.boardSize * squareSize
            height: logic.boardSize * squareSize
        }
    }

    Component{
        id: startButtonComponent
        Button {
          x: logic.boardSize * squareSize
          width: root.width - x;
          height: squareSize
          text: "Start"

          onClicked: {
            console.log("Start a new game");
            logic.clear();
            logic.startNewGame();
            applicationScreen.pop(null);
            applicationScreen.push(newGameScreen);
          }
        }
    }

    Component{
        id: loadButtonComponent
        Button {
          x: logic.boardSize * squareSize
          y: squareSize
          height: squareSize
          width: root.width - x;

          text: "Load"

          onClicked: {
              if(logic.loadGame())
              {
                  console.log("Load the game");
                  applicationScreen.pop(null);
                  logic.clear();
                  logic.startNewGame();
                  applicationScreen.push(historyScreen);
                  logic.loadGame();
              }
          }
        }
    }

    Component{
        id: saveButtonComponent
        Button {
          x: logic.boardSize * squareSize
          height: squareSize
          width: root.width - x;

          text: "Save"

          onClicked: {
            console.log("Save the game");
            logic.saveGame();
          }
        }
    }

    Component{
        id: stopButtonComponent
        Button {
          id: stopButton
          x: logic.boardSize * squareSize
          y: squareSize
          height: squareSize
          width: root.width - x;

          text: "Stop"

          onClicked: {
            console.log("Stop the game");
            logic.clear();
            applicationScreen.pop(null);
            applicationScreen.push(mainScreen);
            console.log("Stop game and return to the main screen");
          }
        }
    }

    // component which contains prev and next buttons.
    Component{
        id: prevNextButtonsComponent
        Item{
            Button {
              id: prevButton

              x: logic.boardSize * squareSize
              y: squareSize * 2
              height: squareSize
              width: (root.width - x) / 2

              text: "Prev"

              onClicked: {
                console.log("Prev");
                logic.prevLogic();
              }
            }
            Button {
              id: nextButton
              y: squareSize * 2
              anchors.left: prevButton.right
              height: squareSize
              width: (root.width - logic.boardSize * squareSize) / 2
              text: "Next"
              onClicked: {
                console.log("Next");
                logic.nextLogic();
              }
            }
        }
    }

    Component{
        id:mainScreen //The main screen - first screen
        Item{
            Loader{sourceComponent: gameBoardComponent}
            Loader{sourceComponent: startButtonComponent}
            Loader{sourceComponent: loadButtonComponent}
        }
    }

    Component{
        id: newGameScreen // second screen which shows up the game is starting
        Item{
            Loader{sourceComponent: gameBoardComponent}
            Loader{sourceComponent: chessPiecesPlacementComponent}
            Loader{sourceComponent: saveButtonComponent}
            Loader{sourceComponent: stopButtonComponent}
        }
    }

    Component{
       id: historyScreen; // third screen
       Item{
            Loader{sourceComponent: gameBoardComponent}
            Loader{sourceComponent: chessPiecesPlacementComponent}
            Loader{sourceComponent: startButtonComponent}
            Loader{sourceComponent: loadButtonComponent}
            Loader{sourceComponent: prevNextButtonsComponent}
       }
    }

    StackView{
        id:applicationScreen // application screen
        anchors.fill: parent
        initialItem: mainScreen
    }
}
