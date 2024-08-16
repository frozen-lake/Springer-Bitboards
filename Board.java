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

    // Attack bitboard generators
    public long ruldATKBB(int pos){
        return occToAttackRULDBB(ruldOccupancy(pos), pos);
    }
    public long lurdATKBB(int pos){
        return occToAttackLURDBB(lurdOccupancy(pos), pos);
    }
    public long diagonalATKBB(int pos){
        return ruldATKBB(pos) | lurdATKBB(pos);
    }
    public long rowATKBB(int pos){
        return occToAttackRowBB(rowOccupancy(pos), pos);
    }
    public long colATKBB(int pos){
        return occToAttackColBB(columnOccupancy(pos), pos);
    }


    // Occupancy byte generators

    private byte rowOccupancy(int pos){
        return (byte) (((state.white|state.black) & data.rows.get(pos / 8)) >> ((pos / 8))*8);
    }
    private byte columnOccupancy(int pos){
        return (byte) (((((state.white|state.black)&data.columns.get(pos%8))>>pos%8) * (data.ruld.get(0))) >> (8*7));
    }
    private byte ruldOccupancy(int pos){
        return (byte) (((data.ruld.get(pos) & (state.white|state.black))*data.columns.get(0)) >> 56);
    }
    private byte lurdOccupancy(int pos){
        return (byte) (((data.lurd.get(pos) & (state.white|state.black))*data.columns.get(0)) >> 56);
    }

    // Occupancy byte to proper attack bitboard functions (static)
    private static long occToAttackRowBB(byte occ, int pos){
        return (((((long)(data.occupancyTable.get(pos % 8).get(occ)))&0b11111111L)) << 8*(pos/8))&data.rows.get(pos/8);
    }
    private static long occToAttackColBB(byte occ, int pos){
        return (((((long)(data.occupancyTable.get(7-(pos/8)).get(occ)))&0b11111111L) * data.ruld.get(0)) >> (7-(pos%8)))&data.columns.get(pos%8);
    }
    private static long occToAttackRULDBB(byte occ, int pos){
        if(pos%8 >= pos/8) { // lower/right half of RULD diagonal
            return  (((((((long)data.occupancyTable.get(pos % 8).get(occ))) >> (pos % 8 - pos / 8)) * data.columns.get(0)) & data.ruld.get(0)) << (pos % 8 - pos / 8));
        } else {
            return ((((((long)data.occupancyTable.get(pos % 8).get(occ)))*data.columns.get(0))) & data.ruld.get(pos));
        }
    }
    private static long occToAttackLURDBB(byte occ, int pos){
        if(pos%8 >= (7-pos/8)){ // upper/right half of LURD diagonal
            return (((long) ((((data.occupancyTable.get(pos % 8).get(occ)))))&0b11111111L) * data.columns.get(0)) & data.lurd.get(pos);
        } else {
            return ((((long) (data.occupancyTable.get(pos % 8).get(occ)& 0b11111111L ) &0b11111111L) & ((long)(Math.pow(2, (pos%8)+1) - 1))) * data.columns.get(0)) & data.lurd.get(pos);
        }
        //(long)((((data.occupancyTable.get(pos%8).get(occ) >> (pos%8 - pos/8))*data.columns.get(0)) & data.ruld.get(0)) << (pos%8 - pos/8));
    }

    public MoveList generateMoves(){
        MoveList list = new MoveList();
        long colorBB = turnToMove ? state.white : state.black;

        // Rook moves

        long pieces = (state.rook | state.queen) & colorBB; // the piece we are generating for
        byte occ;

        // Generate occupancy byte, index attack table, convert attack byte to bitboard, then  attacks to ATKFR[pos].
        while(pieces != 0L){
            int pos = Long.numberOfTrailingZeros(pieces); // Position of least significant bit (piece)
            pieces ^= (pieces & -pieces); // Pop least significant bit

            // Index occupancyTable with occ, convert to bb and add to ATKFR[pos]
            ATKFR[pos] |= rowATKBB(pos);
            ATKFR[pos] |= colATKBB(pos);
        }

        pieces = (state.bishop | state.queen) & colorBB;
        while(pieces != 0L){
            int pos = Long.numberOfTrailingZeros(pieces);
            pieces ^= (pieces & -pieces);

            ATKFR[pos] |= ruldATKBB(pos);
            ATKFR[pos] |= lurdATKBB(pos);

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
