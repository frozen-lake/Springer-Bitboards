

public class BoardState {
    // BoardState is a wrapper class for the 8 bitboards which collectively represent the state of the game.

    protected long pawn;
    protected long knight;
    protected long bishop;
    protected long rook;
    protected long queen;
    protected long king;
    protected long white;
    protected long black;
    public BoardState(){
        initializeBoardState();
    }
    private void initializeBoardState(){
        white = 0b0000000000000000000000000000000000000000000000001111111111111111L;
        black = 0b1111111111111111000000000000000000000000000000000000000000000000L;
        pawn = 0b0000000011111111000000000000000000000000000000001111111100000000L;

        knight = 0b0100001000000000000000000000000000000000000000000000000001000010L;
        bishop = 0b0010010000000000000000000000000000000000000000000000000000100100L;
        rook = 0b1000000100000000000000000000000000000000000000000000000010000001L;

        queen = 0b0000100000000000000000000000000000000000000000000000000000001000L;
        king = 0b0001000000000000000000000000000000000000000000000000000000010000L;

    }
//    public static int LS1B(BitSet bb){
//        return 0;
//    }
//    public static int MS1B(BitSet bb){
//        return 0;
//    }
}
