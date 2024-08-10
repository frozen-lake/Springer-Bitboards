import java.util.ArrayList;

public class MoveList {

    /*
    MoveList is an ordered list of moves encoded by integers.

    - There are 32 bits per move.
    - Bits 0-5: from square (source) (0-2=rank, 3-5=file)
    - Bits 6-11: to square (destination) (6-8=rank, 9-11=file)
    - Bits 12-15:
        0=quiet move, 1=double pawn push, 2=king castle, 3=queen castle, 4=capture, 5=en passant capture
        8-15 are promotion. 8-11 are knight, bishop, rook, queen. 12-15 are promotion+capture
    - Bits 16-18: Previous occupant of from-square (attacker)
        0=empty, 1=pawn, 2=knight, 3=bishop, 4=rook, 5=queen, 6=king
    - Bits 19-21: Previous occupant of to-square (victim)
        0=empty, 1=pawn, 2=knight, 3=bishop, 4=rook, 5=queen, 6=king


     */
    private final ArrayList<Integer> moves;
    public MoveList(){
        moves = new ArrayList<Integer>();
    }
    public int size(){ return moves.size(); }

    public void addMove(int move){
        moves.add(move);
    }

    public void clear(){ moves.clear(); }
}
