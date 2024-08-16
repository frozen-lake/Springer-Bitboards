


public class Chess {
    protected final Board gameBoard;
    protected boolean turn;

    public Chess(){
        turn = true;
        gameBoard = new Board(this);
    }
    public void playSelfW(){
        gameBoard.printBoard();
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
        // System.out.println("-------");

//        System.out.println("===42\n"+Chess.formatBitboard(b.ATKFR[42]));
//        System.out.println("===36\n"+Chess.formatBitboard(b.ATKFR[36]));
//        System.out.println("===2\n"+Chess.formatBitboard(b.ATKFR[2]));
//        System.out.println("===3\n"+Chess.formatBitboard(b.ATKFR[3]));
//        System.out.println("===5\n"+Chess.formatBitboard(b.ATKFR[5]));
        c.playSelfW();
    }
}
