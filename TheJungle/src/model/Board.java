package model;


import java.awt.image.BufferedImage;
import java.io.*;

import javax.imageio.ImageIO;

public class Board {

    private static final int ROW = 9;
    private static final int COL = 7;
    public boolean player;
    public Piece[][] board;
    private static BufferedImage image;

    private boolean movable = true;
    private boolean win_flag;

    /* list of animals */
    private static final Animal[] ANIMALS = {Animal.Mouse, Animal.Cat, Animal.Wolf, Animal.Dog, Animal.Leopard,
            Animal.Tiger, Animal.Lion, Animal.Elephant};

    /* chess pieces */
    private static Piece[] pieces = new Piece[16];

    /* image file names */
    private static final String[] file_prefix = {"red_mouse", "red_cat",
        "red_wolf", "red_dog", "red_leopard", "red_tiger", "red_lion",
        "red_elephant", "black_mouse", "black_cat", "black_wolf", "black_dog", 
        "black_leopard", "black_tiger", "black_lion", "black_elephant"};    

    /* match index to particular chess piece */
    private static final int RED_MOUSE = 0;
    private static final int RED_CAT = 1;
    private static final int RED_WOLF = 2;
    private static final int RED_DOG = 3;
    private static final int RED_LEOPARD = 4;
    private static final int RED_TIGER = 5;
    private static final int RED_LION = 6;
    private static final int RED_ELEPHANT = 7;
    private static final int BLACK_MOUSE = 8;
    private static final int BLACK_CAT = 9;
    private static final int BLACK_WOLF = 10;
    private static final int BLACK_DOG = 11;
    private static final int BLACK_LEOPARD = 12;
    private static final int BLACK_TIGER = 13;
    private static final int BLACK_LION = 14;
    private static final int BLACK_ELEPHANT = 15;

    /** constructor */
    public Board() {
        board = new Piece[COL][ROW];
        initializePieces();
        this.player = true; // red starts
        reset();
    }

    /** initialize all the chess pieces */
    private void initializePieces() {
        for (int i = 0; i < pieces.length; i++) {
            try {
                image = ImageIO.read(new File("images/pieces/"
                        + file_prefix[i] + ".png"));
            } catch (IOException e) {
                System.out.println("Error: " + e.getMessage());
            }
            pieces[i] = new Piece(ANIMALS[i % 8], i % 2, image, (i < 8));
        }
    }

    /** return chess board model */
    public Piece[][] getModel () {
        return board;
    }

    /** move validator */
    public boolean movePiece (int old_X, int old_Y, int new_X, int new_Y) {
        Piece cur = board[old_X][old_Y];
        // return false if there's no piece at current location
        if (cur == null) {
            System.out.println("Illegal Move!");
            movable = false;
            return false;
        }

        // return false if next position is same as current
        if (old_X == new_X && old_Y == new_Y) {
            System.out.println("Illegal Move!");
            movable = false;
            return false;
        }
        
        Animal a = cur.getAnimal();
        // return false if next step is not accessible by chess piece
        if (!accessible(a, old_X, old_Y, new_X, new_Y)) {
            System.out.println("Illegal Move!");
            movable = false;
            return false;
        }

        // capture target if square is occupied by opponent piece and can be 
        // captured by cur piece. return false if target square is occupied
        // and capture is invalid
        if (board[new_X][new_Y] != null
                && !capture(old_X, old_Y, new_X, new_Y, cur)) {
            System.out.println("Illegal Move!");
            movable = false;
            return false;
        }
        
        // set win to true if move will place chess piece in opponent's den
        if (inOpponentDen(cur, new_X, new_Y)) {
            this.win_flag = true;
        }
        
        // set win to true if no enemy's piece are left alive
        for (int i = 0; i < pieces.length; i++) {
            if (cur.getTeam() != pieces[i].getTeam() && pieces[i].isSurvive()) {
                break;
            }
            if (i == pieces.length - 1) this.win_flag = true;
        }
        
        // move piece and remove from current location
        movable = true;
        board[new_X][new_Y] = board[old_X][old_Y];
        board[old_X][old_Y] = null;

        return true;
    }
    /** return true if move is allowed, false otherwise */
    public boolean getMovable(){return this.movable;}

    /** return win status, true if game has been won, false otherwise */
    public boolean gameWon() {
        return this.win_flag;
    }
    
    /** set win status. */
    public void setWin(boolean win_flag) {
        this.win_flag = win_flag;
    }


    /** return true if square is one step away */
    private boolean oneStepAway(int curX, int curY, int nextX, int nextY) {
        return ((Math.abs(nextX - curX) == 1 && nextY == curY)
                || (Math.abs(nextY - curY) == 1 && nextX == curX));
    }

    /** return true if square is in river. takes in its x,y coordinates */
    private boolean inRiver(int x, int y) {
        return (((x > 0 && x < 3) || (x > 3 && x < 6)) && (y > 2 && y < 6));
    }

    /** return true if chess piece will be in opponent's den */
    private boolean inOpponentDen (Piece cp, int x, int y) {
        return (cp.getTeam() && x == 3 && y == 0) || (!cp.getTeam()
                && x == 3 && y == 8);
    }

    /** return true if chess piece will be in own den */
    private boolean inOwnDen (Piece cp, int x, int y) {
        return (!cp.getTeam() && x == 3 && y == 0) || (cp.getTeam()
                && x == 3 && y == 8);
    }

    /** return true if chess piece is in trap */
    private boolean inTrap (Piece cp, int x, int y) {
        if (cp.getTeam()) {
            return (x == 2 && y == 0) || (x == 3 && y == 1) || (x == 4
                    && y == 0);
        }
        else {
            return (x == 2 && y == 8) || (x == 3 && y == 7) || (x == 4
                    && y == 8);
        }
    }

    /** access validator */
    private boolean capture(int old_X, int old_Y, int new_X, int new_Y, Piece cur){
        Animal a = cur.getAnimal();
        Piece target = board[new_X][new_Y];
        Animal b = target.getAnimal();
        // return false if target chess piece has higher rank
        // unless cur is mouse or unless target is in trap
        if (cur.compareTo(target) < 0 && ! a.toString().equals("Mouse")
                && ! inTrap(target, new_X, new_Y)) {
            return false;
        }
        // mouse can only capture elephant and mouse
        // note a mouse cannot capture the other mouse unless they are either 
        // both in the river, or both on land
        if ((a.toString().equals("Mouse")
                && !b.toString().equals("Elephant")
                && !b.toString().equals("Mouse"))
                || (inRiver(new_X, new_Y) != inRiver(new_X, new_Y))){
            return false;
        }
        // elephant cannot capture mouse
        if (a.toString().equals("Elephant")
                && b.toString().equals("Mouse")) {
            return false;
        }
        // capture target piece
        target.capture();
        return true;
    }

    /** Validator for checking chess piece to access certain place */
    private boolean accessible (Animal a, int old_X, int old_Y, int new_X,
            int new_Y) {
        // return false if intended square is more than one step away
        // make exception for lion and tiger if next to river
        if (!oneStepAway(old_X, old_Y, new_X, new_Y)
                && !Across_the_river(a, old_X, old_Y, new_X, new_Y)) {
            return false;
        }
        // returns false if intended square is in river, and animal is not mouse
        if (inRiver(new_X, new_Y) && ! a.toString().equals("Mouse")) {
            return false;
        }
        //return false if intended location is chess piece's own den
        if (inOwnDen(board[old_X][old_Y], new_X, new_Y)) {
            return false;
        }
            
        return true;
    }
    

    /** Validator for Lion and Tiger jumping across the river */
    private boolean Across_the_river(Animal a, int old_X, int old_Y, int new_X, int new_Y){
        // return false if animal is not lion or tiger
        if (a.toString() != "Lion" && a.toString() != "Tiger") {
            return false;
        }
        // check if lion/tiger is on left or right side of river
        // and is meant to jump to the other side
        if ((old_X == 0 || old_X == 3 || old_X == 6) && (old_Y > 2 && old_Y < 6)) {
            // return true if intended position is across the river
            if (Math.abs(new_X - old_X) == 3 && (new_Y == old_Y)) {
                // check if there's a mouse in the river blocking the jump
                for (int i = 1; i < 3; i++) {
                    int sign = (new_X - old_X) % 2;
                    if (board[old_X + sign * i][old_Y] != null) {
                        return false;
                    }
                }
                return true;
            }
        }
        // check if lion/tiger is above or below river and meant to jump to
        // the other side
        if ((old_Y == 2 || old_Y == 6) && (old_X == 1 || old_X == 2 || old_X == 4
                || old_X == 5)) {
            if (Math.abs(new_Y - old_Y) == 4 && (new_X == old_X)) {
                // check if there's a mouse in the river blocking the jump
                for (int i = 1; i < 4; i++) {
                    int sign = (new_Y - old_Y) % 3;
                    if (board[old_X][old_Y + sign * i] != null) {
                        return false;
                    }
                }
                return true;
            }
        }
        // else return false
        return false;
    }

    /** return current player turn - true if red's, false if black */
    public boolean getTurn() {
        return player;
    }

    /** set next player's turn */
    public void setTurn(boolean turn) {
        this.player = turn;
    }

    /** return chess pieces */
    public Piece[] getPieces () {
        return pieces;
    }

    /** save file */
    public static void save() throws IOException {
        FileWriter fw = new FileWriter("save.txt");
        for(int i = 0; i < pieces.length; i++){
            if(pieces[i].isSurvive()){
                fw.write(pieces[i].getAnimal() + " "
                        + pieces[i].getX() + " "
                        + pieces[i].getY() + " ");
            }
        }
        fw.close();
    }

    /** load file */
    public static void load() throws IOException{
        String ani;
        String x;
        String y;

        FileReader fr = null;
        try{
            fr = new FileReader("save.txt");
        }catch(FileNotFoundException fe){
            System.out.println("File not found");
        }

        fr.close();
    }

    /** reset chess pieces to start position */
    public void reset() {
        // clear board
        for (int i = 0; i < board.length; i++) {
            for (int j = 0; j < board[0].length; j++) {
                board[i][j] = null;
            }
        }
        // place pieces in starting positions
        board[0][0] = pieces[BLACK_LION];
        pieces[BLACK_LION].setX(0);
        pieces[BLACK_LION].setY(0);

        board[6][0] = pieces[BLACK_TIGER];
        pieces[BLACK_TIGER].setX(6);
        pieces[BLACK_TIGER].setY(0);

        board[1][1] = pieces[BLACK_DOG];
        pieces[BLACK_DOG].setX(1);
        pieces[BLACK_DOG].setY(1);

        board[5][1] = pieces[BLACK_CAT];
        pieces[BLACK_CAT].setX(5);
        pieces[BLACK_CAT].setY(1);

        board[0][2] = pieces[BLACK_MOUSE];
        pieces[BLACK_MOUSE].setX(0);
        pieces[BLACK_MOUSE].setY(2);

        board[2][2] = pieces[BLACK_LEOPARD];
        pieces[BLACK_LEOPARD].setX(2);
        pieces[BLACK_LEOPARD].setY(2);

        board[4][2] = pieces[BLACK_WOLF];
        pieces[BLACK_WOLF].setX(4);
        pieces[BLACK_WOLF].setY(2);

        board[6][2] = pieces[BLACK_ELEPHANT];
        pieces[BLACK_ELEPHANT].setX(6);
        pieces[BLACK_ELEPHANT].setY(2);

        board[0][6] = pieces[RED_ELEPHANT];
        pieces[RED_ELEPHANT].setX(0);
        pieces[RED_ELEPHANT].setY(6);

        board[2][6] = pieces[RED_WOLF];
        pieces[RED_WOLF].setX(2);
        pieces[RED_WOLF].setY(6);

        board[4][6] = pieces[RED_LEOPARD];
        pieces[RED_LEOPARD].setX(4);
        pieces[RED_LEOPARD].setY(6);

        board[6][6] = pieces[RED_MOUSE];
        pieces[RED_MOUSE].setX(6);
        pieces[RED_MOUSE].setY(6);

        board[1][7] = pieces[RED_CAT];
        pieces[RED_CAT].setX(1);
        pieces[RED_CAT].setY(7);

        board[5][7] = pieces[RED_DOG];
        pieces[RED_DOG].setX(5);
        pieces[RED_DOG].setY(7);

        board[0][8] = pieces[RED_TIGER];
        pieces[RED_TIGER].setX(0);
        pieces[RED_TIGER].setY(8);

        board[6][8] = pieces[RED_LION];
        pieces[RED_LION].setX(6);
        pieces[RED_LION].setY(8);

        // revive all pieces
        for (int i = 0; i < pieces.length; i++) {
            pieces[i].revive();
        }
    }

}