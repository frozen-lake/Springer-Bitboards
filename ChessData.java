import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

public class ChessData {

    public ArrayList<HashMap<Byte, Byte>> occupancyTable; // Sliding Piece Occupancy Lookup Table
    // Maps occupancy rows/columns/diagonals to attack maps for sliding pieces such as rooks, bishops and queens (RBQs.)
    public HashMap<Integer, Long> lurd; // Left/Up and Right/Down diagonal === \ ===
    public HashMap<Integer, Long> ruld; // Right/Up and Left/Down Diagonal === / ===
    public ArrayList<Long> rows; // === -- ===
    public ArrayList<Long> columns; // === | ===

    public ChessData(){
        occupancyTable = new ArrayList<HashMap<Byte, Byte>>();
        generateDiagonals();
        generateStraights();
        generateAllBytes();
        generateOccupancyTable();
    }


    // Manually calculates the squares an RBQ attacks from location 'pos'.
    // The attack map for every row and position will be pre-generated and stored in the occupancy table.
    private byte attackFromOccupancy(int pos, byte row){
        return row;
    }
    private HashSet<Byte> generateAllBytes(){
        byte[] b4 = new byte[]{(byte) 0b0000, (byte) 0b0001, (byte) 0b0010, (byte) 0b0100, (byte) 0b1000, (byte) 0b0011, (byte) 0b0110, (byte) 0b1100, (byte) 0b0101, (byte) 0b1001, (byte) 0b1010, (byte) 0b0111, (byte) 0b1110, (byte) 0b1101, (byte) 0b1011, (byte) 0b1111};
        byte[] b3 = new byte[]{(byte) 0b000, (byte) 0b001, (byte) 0b010, (byte) 0b100, (byte) 0b011, (byte) 0b110, (byte) 0b101, (byte) 0b111};
        HashSet<Byte> keys = new HashSet<Byte>();
        for(int i=0;i<b4.length;i++){
            for(int j=0;j<b4.length;j++){
                keys.add((byte) ((b4[i]<<4) | b4[j]));
            }
        }

        return keys;

    }
    private void generateOccupancyTable(){
        for(int i=0;i<8;i++){ // i is the position of the RBQ within the row, column or diagonal
            HashMap<Byte, Byte> map = new HashMap<Byte, Byte>();

            // Generate all keys (occupancy rows)
            HashSet<Byte> keys = generateAllBytes();


            // Calculate attack rows for each key
            for(byte key: keys){
                if(((key>>(i)) & 0b01)==0){ // ignore key if the spot our piece is in is not occupied
                    continue;
                }

                byte attack = 0b0;

                for(int j=i+1;j<8;j++){ // go left from i
                    attack |= 0b01<<j;
                    if((key&(1<<j)) != 0){ // if j is occupied, add it and then stop here
                        break;
                    }
                }
                for(int j=i-1;j>=0;j--) { // go right from i
                    attack |= 0b01<<j;
                    if((key&(1<<j)) != 0){ // if j is occupied, add it and then stop here
                        break;
                    }
                }
                map.put(key, attack);


            }
            occupancyTable.add(map);
        }
    }
    private void generateDiagonals(){
        lurd = new HashMap<Integer, Long>();
        ruld = new HashMap<Integer, Long>();

        for(int i=0;i<64;i++){
            long bb = (1L << i); // Initialize bit at position i to 1

            // bishop moving up and right
            int numberOfMoves = Math.min(7 - (i % 8), 7 - (i / 8)); int pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos += 9; bb |= (1L << pos);
            }

            // bishop moving down and left
            numberOfMoves = Math.min(i % 8, i / 8); pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos -= 9; bb |= (1L << pos);
            }
            ruld.put(i, bb); bb = (1L << i);
            // bishop moving down and right
            numberOfMoves = Math.min(7 - (i % 8), i / 8); pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos -= 7; bb |= (1L << pos);
            }

            // bishop moving up and left
            numberOfMoves = Math.min(i % 8, 7 - (i / 8)); pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos += 7; bb |= (1L << pos);
            }
            lurd.put(i, bb);

        }
    }
    private void generateStraights(){

        rows = new ArrayList<Long>();
        columns = new ArrayList<Long>();

        rows.add(0b0000000000000000000000000000000000000000000000000000000011111111L);
        rows.add(0b0000000000000000000000000000000000000000000000001111111100000000L);
        rows.add(0b0000000000000000000000000000000000000000111111110000000000000000L);
        rows.add(0b0000000000000000000000000000000011111111000000000000000000000000L);
        rows.add(0b0000000000000000000000001111111100000000000000000000000000000000L);
        rows.add(0b0000000000000000111111110000000000000000000000000000000000000000L);
        rows.add(0b0000000011111111000000000000000000000000000000000000000000000000L);
        rows.add(0b1111111100000000000000000000000000000000000000000000000000000000L);
        columns.add(0b0000000100000001000000010000000100000001000000010000000100000001L);
        columns.add(0b0000001000000010000000100000001000000010000000100000001000000010L);
        columns.add(0b0000010000000100000001000000010000000100000001000000010000000100L);
        columns.add(0b0000100000001000000010000000100000001000000010000000100000001000L);
        columns.add(0b0001000000010000000100000001000000010000000100000001000000010000L);
        columns.add(0b0010000000100000001000000010000000100000001000000010000000100000L);
        columns.add(0b0100000001000000010000000100000001000000010000000100000001000000L);
        columns.add(0b1000000010000000100000001000000010000000100000001000000010000000L);
//
//        for(int i=0;i<64;i++){
//            long bb = (1L << i); // Initialize bit at position i to 1
//
//            // rook moving up
//            int numberOfMoves = 7 - (i / 8); int pos = i;
//            for(int j=0;j<numberOfMoves;j++){
//                pos += 8; bb |= (1L << pos);
//            }
//
//            // rook moving down
//            numberOfMoves = i / 8; pos = i;
//            for(int j=0;j<numberOfMoves;j++){
//                pos -= 8; bb |= (1L << pos);
//            }
//            columns.put(i, bb); bb = (1L << i);
//
//            // rook moving right
//            numberOfMoves = 7 - (i % 8); pos = i;
//            for(int j=0;j<numberOfMoves;j++){
//                pos -= 8; bb |= (1L << pos);
//            }
//
//            // rook moving left
//            numberOfMoves = i % 8; pos = i;
//            for(int j=0;j<numberOfMoves;j++){
//                pos += 8; bb |= (1L << pos);
//            }
//            rows.put(i, bb);
//
//        }
    }

}
