package controller;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFrame;
import javax.swing.JLabel;

import view.*;
import model.*;

public class Controller {

    /** class fields */
    /** Associated Chess Board */
    private static Board board;
    /** Associated Board Panel */
    private static BoardPanel panel;
    /** 2D array of invisible buttons */
    private static InvisibleButton[][] squares;
    /** chess board model */
    private static Piece[][] model;
    /** current select mode */
    private static Mode mode;
    /** frame to display when game is won */
    private static JFrame winFrame;
    /** frame to display if illegal move is made */
    private static JFrame illegalMove;
    /** JLabel for win frame */
    private static JLabel winMessage;
    /** JLabel for illegal frame */
    private static JLabel illegalMessage;
    /** score label */
    private static JLabel scoreLabel;
    /** win status */
    private static boolean win = false;
    /** black's current score */
    private static int black = 0;
    /** red's current score */
    private static int red = 0;

    /** selection modes */
    enum Mode {
        SELECT, STEP
    }

    /** store selected chess piece */
    private static Piece selected;

    /** constructor */
    public Controller (BoardPanel panel, JFrame winFrame, JLabel winMessage,
                       JFrame illegalMove, JLabel illegalMessage, JLabel scoreLabel) {
        Controller.panel = panel;
        Controller.board = panel.getChessBoard();
        Controller.squares = Controller.panel.getSquares();
        Controller.model = board.getModel();
        Controller.mode = Mode.SELECT;
        Controller.winFrame = winFrame;
        Controller.winMessage = winMessage;
        Controller.illegalMove = illegalMove;
        Controller.illegalMessage = illegalMessage;
        Controller.scoreLabel = scoreLabel;

        addEventListeners();
    }

    /** add the mouse click listeners to each button */
    private static void addEventListeners () {
        for (int y = 0; y < squares[0].length; y++){
            for (int x = 0; x < squares.length; x++) {
                squares[x][y].addActionListener(new ActionListener() {

                    @Override
                    public void actionPerformed(ActionEvent e) {
                        // only process if win is set to false
                        // board grid cannot be clicked if one of the players
                        // has won and board has not been reset
                        if (!win) {
                            processSelection(e);
                        }
                    }                  
                });
            }
        }
    }

    /** deal with the action listener received by a button */
    private static void processSelection(ActionEvent e) {
        int x = -1;
        int y = -1;
        // find out which square was selected 
        Object source = e.getSource();
        for (int j = 0; j < squares[0].length; j++){
            for (int i = 0; i < squares.length; i++) {
                if (squares[i][j] == source) {
                    x = i;
                    y = j;
                }
            }
        }
        // process selection
        if (x != -1 && y != -1) {
            switch (mode) {
            case SELECT:
                selected = model[x][y];
                // returns if there is no chess piece at location
                if (selected == null) {
                    return;
                }
                // returns if piece does not belong to current player
                if (selected.getTeam() != board.getTurn()) {
                    return;
                }
                // else go to STEP mode
                mode = Mode.STEP;
                updateSelection(x, y);
                break;
            case STEP:
                // change selected piece if on same team
                if ((model[x][y] != null) && 
                        (model[x][y].getTeam() == board.getTurn())) {
                    // de-select all other buttons and highlight this square
                    updateSelection(x, y);
                    // update selected
                    selected = model[x][y];
                    break;
                }
                int origX = selected.getX();
                int origY = selected.getY();
                // get model to move piece if legal
                if (board.movePiece (origX, origY, x, y)) {
                    // remove piece from original location if it
                    // was moved successfully
                    squares[origX][origY].removePiece();
                    // update it's x, y coordinates
                    selected.setX(x);
                    selected.setY(y);
                    // return to select mode
                    mode = Mode.SELECT;
                    // select the square the piece moved to 
                    squares[x][y].setSelected(true);
                    // set selected piece to null
                    selected = null;
                    // de-select initially chosen square
                    squares[origX][origY].setSelected(false);
                    squares[origX][origY].setBorderPainted(false);
                    //if game has been won
                    if (board.gameWon()) {
                        win = true;
                        break;
                    }

                    // update turn to next player's turn
                    board.setTurn(!board.getTurn());
                }
                break;
            }
        }
        panel.repaint();
        // display game won frame and update score if game won
        if (win) {
            showWinMessage();
            updateScore();
        }
    }

    /** de-activate all squares and updates the square if given */
    private static void updateSelection(int x, int y) {
        // de-select all squares
        for (int j = 0; j < squares[0].length; j++){
            for (int i = 0; i < squares.length; i++) {
                squares[i][j].setSelected(false);
                squares[i][j].setBorderPainted(false);
            }
        }
        // highlight this square if needed
        if (x != -1 && y != -1) {
            squares[x][y].setSelected(true);
        }
    }

    /** reset board panel and model */
    public void reset() {
        // reset mode to select and de-select all selected squares
        mode = Mode.SELECT;
        updateSelection(-1, -1);
        // red team starts first
        board.setTurn(true);
        // set win to false
        board.setWin(false);
        win = false;
        // reset model board
        board.reset();
        // clear all squares
        for (int j = 0; j < squares[0].length; j++){
            for (int i = 0; i < squares.length; i++) {
                squares[i][j].removePiece();
            }
        }
        // remove win display if it's visible
        winFrame.setVisible(false);
        // repaint panel
        panel.repaint();
    }

    private static void showIllegalMessage(){
        if(board.getMovable() == false){
            illegalMessage.setText("Illegal Move!");

        }
        illegalMove.setVisible(true);
    }

    public void save(){

    }

    public void load(){

    }

    /** display game won frame */
    private static void showWinMessage() {
        if (board.getTurn()) {
            red++; // increment red's score
            winMessage.setText("Red wins! Click rematch or quit");
        }
        else {
            black++; // increment black's score
            winMessage.setText("Black wins! Click rematch or quit");
        }
        winFrame.setVisible(true);
    }

    /** update score label */
    private static void updateScore() {
        String score = "Red: " + red + " Black: " + black;
        scoreLabel.setText(score);
    }
}
