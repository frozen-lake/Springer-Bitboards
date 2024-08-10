


public class Chess {
    private final Board gameBoard;
    protected boolean turn;
    protected ChessData data;

    public Chess(){
        gameBoard = new Board(this); turn = true;
        data = new ChessData();
    }
    public void playSelfW(){
        while(true){
            if(turn){

            } else {

            }
        }
    }
    public static String formatBitboard(long bb){
        String s = "";
        for(int i=7;i>=0;i--){
            for(int j=0;j<8;j++){
                if((bb >> (i*8 + j) & 1) != 0){
                    s += " 1 ";
                } else {
                    s += " . ";
                }
            }
            s += "\n";
        }
        return s;
    }
    public static void main(String[] args){
        Chess c = new Chess();
        Board b = c.gameBoard;

        //c.playSelfW();
    }
}
