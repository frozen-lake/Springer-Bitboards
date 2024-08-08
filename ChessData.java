import java.util.HashMap;

public class ChessData {

    public HashMap<Integer, HashMap<Byte, Byte>> occupancyTable; // Sliding Piece Occupancy Lookup Table
    public HashMap<Integer, Long> lurd; // Left/Up and Right/Down diagonal === \ ===
    public HashMap<Integer, Long> ruld; // Right/Up and Left/Down Diagonal === / ===
    public HashMap<Integer, Long> horizontal; // === -- ===
    public HashMap<Integer, Long> vertical; // === | ===

    public ChessData(){
        occupancyTable = new HashMap<Integer, HashMap<Byte, Byte>>();
        generateDiagonals();
        generateStraights();
        // generateOccupancyTable();
    }

//    private void generateOccupancyTable(){
//
//    }
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
            //bb = ( ruld.get(i) ^ lurd.get(i) ); // XOR to remove intersection.

        }
    }
    private void generateStraights(){

        horizontal = new HashMap<Integer, Long>();
        vertical = new HashMap<Integer, Long>();

        for(int i=0;i<64;i++){
            long bb = (1L << i); // Initialize bit at position i to 1

            // rook moving up
            int numberOfMoves = 7 - (i / 8); int pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos += 9; bb |= (1L << pos);
            }

            // rook moving down
            numberOfMoves = i / 8; pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos -= 9; bb |= (1L << pos);
            }
            vertical.put(i, bb); bb = (1L << i);
            // rook moving right
            numberOfMoves = 7 - (i % 8); pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos -= 7; bb |= (1L << pos);
            }

            // rook moving left
            numberOfMoves = i % 8; pos = i;
            for(int j=0;j<numberOfMoves;j++){
                pos += 7; bb |= (1L << pos);
            }
            horizontal.put(i, bb);
            //bb = ( ruld.get(i) ^ lurd.get(i) ); // XOR to remove intersection.

        }
    }

}
