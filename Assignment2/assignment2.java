import java.util.Random;
public class assignment2
{
    public static void main(String[] args)
    {
        Globals glob = new Globals();
        Winners[] wins = new Winners[2];
        int[][] letter_count = new int[Globals.THREAD_MAX][Globals.LETTER_COUNT];
        char[][] matrix = new char[Globals.MATRIX_SIZE][Globals.MATRIX_SIZE];
        fill_arrays(matrix, letter_count, wins);

        Thread[] threads = new Thread[Globals.THREAD_MAX];
        for(int i = 0; i < Globals.THREAD_MAX; i++)
        {
            threads[i] = new Thread(new MyThread(matrix, letter_count, glob));
            threads[i].start();
            try
            {
                threads[i].join();
            } 
            catch(Exception ex)
            {
                System.out.println("Exception has been caught: " + ex); 
            }
        }

        int winners_idx = 0;
        for(int i = 0; i < Globals.THREAD_MAX; i++)
        {
            winners_idx = i % 2; //0: Row, 1: Col
            for(int j = 0; j < Globals.LETTER_COUNT; j++)
            {
                if(letter_count[i][j] > wins[winners_idx].getCount()) //TODO: Figure out how to save all the ties
                {
                    wins[winners_idx].setIndex((int)Math.floor(i/2) + 1);
                    wins[winners_idx].setCount(letter_count[i][j]);
                    wins[winners_idx].setLetter((char)(j + 65));
                }
            }
        }
        for(int i = 0; i < Globals.MATRIX_SIZE; i++)
        {
            for(int j = 0; j < Globals.MATRIX_SIZE; j++)
            {
                System.out.printf("%c ", matrix[i][j]);
            }
            System.out.print("\n");
        }

        System.out.print("Indices start at 1\n");
        System.out.printf("Row %d: %d occurrences of letter %c\n", wins[0].getIndex(), wins[0].getCount(), wins[0].getLetter());
        System.out.printf("Column %d: %d occurrences of letter %c\n", wins[1].getIndex(), wins[1].getCount(), wins[1].getLetter());
    }

    private static void fill_arrays(char[][] matrix, int[][] letter_count, Winners[] wins)
    {
        char letter;
        Random ran = new Random();
        for(int i = 0; i < Globals.MATRIX_SIZE; i++)
        {
            for(int j = 0; j < Globals.MATRIX_SIZE; j++)
            {
                letter = (char)(ran.nextInt(Globals.LETTER_COUNT) + 65); //Capital letters in ASCII start at 65
                matrix[i][j] = letter;
            }
        }

        for(int i = 0; i < Globals.MATRIX_SIZE; i++)
        {
            for(int j = 0; j < Globals.LETTER_COUNT; j++)
            {
                letter_count[i][j] = 0;
            }
        }

        for(int i = 0; i < wins.length; i++)
        {
            wins[i] = new Winners();
        }
    }
}

class MyThread implements Runnable
{
    private char[][] matrix;
    private int[][] letter_count;
    private Globals glob;
    public MyThread(char[][] matrix, int[][] letter_count, Globals glob)
    {
        this.matrix = matrix;
        this.letter_count = letter_count;
        this.glob = glob;
    }

    public void run()
    {
        char letter;
        if(glob.getThreadNum() % 2 == 0) //Row
        {
            for(int i = 0; i < Globals.MATRIX_SIZE; i++)
            {
                letter = matrix[glob.getRowNum()][i];
                int letter_idx = (int)letter - 65;
                letter_count[glob.getThreadNum()][letter_idx]++;
            }
            glob.setRowNum(glob.getRowNum() + 1);
        }
        else //Col
        {
            for(int i = 0; i < Globals.MATRIX_SIZE; i++)
            {
                letter = matrix[i][glob.getColNum()];
                int letter_idx = (int)letter - 65;
                letter_count[glob.getThreadNum()][letter_idx]++;
            }
            glob.setColNum(glob.getColNum() + 1);
        }
        glob.setThreadNum(glob.getThreadNum() + 1);
    }
}

class Winners {
    private int index;
    private int count;
    private char letter;

    // constructor
    public Winners() {
        this.index = 0;
        this.count = 0;
    }

    public int getIndex() {
        return this.index;
    }

    public int getCount() {
        return this.count;
    }

    public char getLetter() {
        return this.letter;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public void setCount(int count) {
        this.count = count;
    }

    public void setLetter(char letter) {
        this.letter = letter;
    }

}

class Globals {
    public static final int MATRIX_SIZE = 7;
    public static final int THREAD_MAX = MATRIX_SIZE * 2;
    public static final int LETTER_COUNT = 26;
    private int thread_num, row_num, col_num;

    public Globals() {
        this.thread_num = 0;
        this.row_num = 0;
        this.col_num = 0;
    }

    public int getThreadNum() {
        return this.thread_num;
    }

    public int getRowNum() {
        return this.row_num;
    }

    public int getColNum() {
        return this.col_num;
    }

    public void setThreadNum(int thread_num) {
        this.thread_num = thread_num;
    }

    public void setRowNum(int row_num) {
        this.row_num = row_num;
    }

    public void setColNum(int col_num) {
        this.col_num = col_num;
    }
}

