import controller.*;
import model.*;
import view.*;

import org.junit.Test;

public class ModelTest {
    @Test
    public void ModelTest(){
        Board B = new Board();

        assert B.board[4][6].toString().equals("red leopard");
        assert B.board[6][0].toString().equals("black tiger");

        /** General move test */
        assert B.movePiece(4, 6, 5, 6) == true;
        assert B.movePiece(4, 6, 4, 6) == false;
        assert B.movePiece(2, 6, 2, 5) == false;
        assert B.movePiece(0, 6, 0, 4) == false;

        /** Mouse to river test */
        B.board[3][4] = B.getPieces()[0];
        B.getPieces()[0].setX(3);
        B.getPieces()[0].setY(4);
        assert B.movePiece(3, 4, 4, 4) == true;

        /** Across river test */
        B.board[3][3] = B.getPieces()[14];
        B.getPieces()[14].setX(3);
        B.getPieces()[14].setY(3);
        assert B.movePiece(3, 3, 6, 3);


    }
}
