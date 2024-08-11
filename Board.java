import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Board {

    private final Chess game;
    public static final ChessData data = new ChessData();
    protected BoardState state; // bitboards
    protected long[] ATKFR; // attack-from bitboard (which squares does this piece attack? if no piece, all 0s)
    protected long[] ATKTO; // attack-to bitboard (which pieces attack this square?)
    private boolean turnToMove;
    public Board(Chess chess){
        game = chess; turnToMove = true;
        ATKFR = new long[64]; ATKTO = new long[64];
        for(int i=0;i<64;i++){ATKFR[i] = 0L; ATKTO[i] = 0L;}


        state = new BoardState();
        generateMoves();
    }

    public void makeMove(int move){

    }
    public void undoMove(int move){

    }
    public static long occToAttackRowBB(byte occ, int pos){
        return (((((long)(data.occupancyTable.get(pos % 8).get(occ)))&0b11111111L)) << 8*(pos/8))&data.rows.get(pos/8);
    }
    public static long occToAttackColBB(byte occ, int pos){
        return (((((long)(data.occupancyTable.get(7-(pos/8)).get(occ)))&0b11111111L) * data.ruld.get(0)) >> (7-(pos%8)))&data.columns.get(pos%8);
    }

    public MoveList generateMoves(){
        MoveList list = new MoveList();
        long colorBB = turnToMove ? state.white : state.black;

        // Rook moves

        long pieces = state.rook & colorBB; // the piece we are generating for
        long piece;

        while(pieces != 0L){
            int pos = Long.numberOfTrailingZeros(pieces); // Position of least significant bit (piece)
            pieces ^= (pieces & -pieces); // Pop least significant bit

            // Generate occupancy byte
            byte occ = (byte) (((state.white|state.black) & data.rows.get(pos / 8)) >> ((pos / 8))*8); // occupancy key for attack map indexing

            // Index occupancyTable with occ and add that bitboard to ATKFR[pos]
            ATKFR[pos] |= occToAttackRowBB(occ, pos);


            occ = (byte) (((((state.white|state.black)&data.columns.get(pos%8))>>pos%8) * (data.ruld.get(0))) >> (8*7));
//
//            System.out.println("==="+pos+"\n"+Integer.toBinaryString((occ)&0b11111111));
//            System.out.println("==="+pos+"\n"+Integer.toBinaryString(data.occupancyTable.get(7-(pos/8)).get(occ) & 0b11111111));
            //System.out.println("==="+pos+"f\n"+Chess.formatBitboard((((long)(data.occupancyTable.get(7-(pos/8)).get(occ))))));
            //System.out.println("==="+pos+"f\n"+Chess.formatBitboard((((long)(data.occupancyTable.get(7-(pos/8)).get(occ)))) * data.ruld.get(0)));
            //System.out.println("===abb"+pos+"\n"+Chess.formatBitboard(occToAttackColBB(occ, pos)));
            ATKFR[pos] |= occToAttackColBB(occ, pos);

        }



        return list;
    }

    public void printBoard(){
        System.out.println("   -------------------------------");
        for(int i=7;i>=0;i--){
            System.out.print((i+1) + " | ");
            for(int j=0;j<8;j++){
                String s = "";
                if((state.white >> (i*8 + j) & 0b1L) == 0b1L || (state.black >> (i*8 +j) & 0b1L) == 0b1L){
                    if((state.pawn >> (i*8 + j) & 0b1L) == 0b1L) s= "p";
                    if((state.knight>>(i*8 + j) & 0b1L) == 0b1L) s= "n";
                    if((state.bishop>>(i*8 + j) & 0b1L) == 0b1L) s= "b";
                    if((state.rook>>(i*8 + j) & 0b1L) == 0b1L) s= "r";
                    if((state.queen>>(i*8 + j) & 0b1L) == 0b1L) s= "q";
                    if((state.king>>(i*8 + j) & 0b1L) == 0b1L) s= "k";
                } else { s = " "; }
                if((state.white>>(i*8 + j)&0b1L)==0b1L) s=s.toUpperCase();
                s+=" | ";
                System.out.print(s);
            }
            System.out.println();
        }
        System.out.println("    a   b   c   d   e   f   g   h");
    }



    public static int positionToInt(String pos){
        if(pos.length() > 2) return -1;
        List<Character> cb1 = Arrays.asList('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h');
        List<Character> cb2 = Arrays.asList('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H');
        char c1 = pos.charAt(0); char c2 = pos.charAt(1);

        if(!Character.isDigit(c2)) return -1;
        if(cb1.contains(c1)) return cb1.indexOf(c1) + (8 * (Character.getNumericValue(c2) - 1));
        if(cb2.contains(c1)) return cb2.indexOf(c1) + (8 * (Character.getNumericValue(c2) - 1));
        return -1;
    }
}
