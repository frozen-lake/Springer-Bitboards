public class Chess {
    Board gameBoard;
    public void playSelf(){
        gameBoard = new Board();
    }
    public static void main(String[] args){
        int pb = 0b00010100;
        System.out.println(Integer.toBinaryString(pb));
        System.out.println(Integer.toBinaryString(pb << 1));
        System.out.println(Integer.toBinaryString(pb));
        System.out.println(Integer.toBinaryString(pb >> 1));
        System.out.println(Integer.toBinaryString(pb));

    }
}
