import java.util.BitSet;

public class BoardState {
    // BoardState is a wrapper class for the 8 bitboards which collectively represent the state of the game.

    public BitSet pawn;
    public BitSet knight;
    public BitSet bishop;
    public BitSet rook;
    public BitSet queen;
    public BitSet king;
    public BitSet color;
    public BitSet occupied;
    public BoardState(){
        initializeBoardState();
    }
    private void initializeBoardState(){
        occupied = new BitSet(64);
        color = new BitSet(64);
        pawn = new BitSet(64);
        knight = new BitSet(64);
        bishop = new BitSet(64);
        rook = new BitSet(64);
        queen = new BitSet(64);
        king = new BitSet(64);

        for(int i=0;i<16;i++){ occupied.set(i); color.set(i);}
        for(int i=48;i<64;i++){ occupied.set(i); } // color set to 0 by default

        for(int i=48;i<56;i++){ pawn.set(i); } for(int i=8;i<16;i++){ pawn.set(i); }

        knight.set(1);knight.set(6);knight.set(57);knight.set(62);
        bishop.set(2);bishop.set(5);bishop.set(58);bishop.set(61);
        rook.set(1);rook.set(7);rook.set(56);rook.set(63);
        queen.set(3);queen.set(59);king.set(4);king.set(60);
    }
}
