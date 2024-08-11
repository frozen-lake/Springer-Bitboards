import org.junit.Test;

import static org.junit.Assert.*;

public class testChessData {


    @Test
    public void testRowsAndColumns(){
        ChessData cd = new ChessData();

        assertEquals(0b0000000000000000000000000000000000000000111111110000000000000000L, (long)cd.rows.get(2));
        assertEquals(0b1111111100000000000000000000000000000000000000000000000000000000L, (long)cd.rows.get(7));;
        assertEquals(0b0000000000000000000000000000000000000000000000000000000011111111L, (long)cd.rows.get(0));

        assertEquals(0b0010000000100000001000000010000000100000001000000010000000100000L, (long)cd.columns.get(2));
        assertEquals(0b0000000100000001000000010000000100000001000000010000000100000001L, (long)cd.columns.get(7));
        assertEquals(0b1000000010000000100000001000000010000000100000001000000010000000L, (long)cd.columns.get(0));

    }

    @Test
    public void testDiagonals(){
        ChessData cd = new ChessData();

        assertEquals(0b1000000001000000001000000001000000001000L, (long)cd.ruld.get(12));
        assertEquals(0b1000000100000010000001000000100000010000001000000000000000L, (long)cd.lurd.get(15));
    }


    @Test
    public void testOccupancyTable() {
        ChessData cd = new ChessData();

        assertEquals((byte) 0b00110110, (byte) cd.occupancyTable.get(3).get((byte) 0b01101010));
        assertEquals((byte) 0b11111101, (byte) cd.occupancyTable.get(1).get((byte) 0b00000010));
        assertEquals((byte) 0b11111101, (byte) cd.occupancyTable.get(1).get((byte) 0b00000010));
    }

}
