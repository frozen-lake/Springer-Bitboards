


public class Chess {
    private final Board gameBoard;
    protected boolean turn;

    public Chess(){
        gameBoard = new Board(); turn = true;
    }
    public void playSelfW(){
        while(true){
            if(turn){

            } else {

            }
        }
    }
    public static void main(String[] args){
        Chess c = new Chess();
        Board b = c.gameBoard;
        ChessData cd = new ChessData();

        c.playSelfW();
    }
}
