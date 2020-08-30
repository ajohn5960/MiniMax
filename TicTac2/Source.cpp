


#include <fstream> 
#include <iostream> 
#include <string> 
#include <vector>  
#include <limits>

using namespace std;


const int N = 3; // size of board
const int M = 8; // size of counter arr
int currentCoord = 0;
char board[N][N] = { {'_', '_', '_'}, {'_', '_', '_'} , {'_', '_', '_'} };
int aiCounter[M] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int humanCounter[M]{ 0, 0, 0, 0, 0, 0, 0, 0 };

    // both counter arr stores the number of pieces of a particular row, col, or diagonal
    // each idx keeps track of the count -> counter arr = [1st row, 2nd row, 3rd row, 1st col, 2nd col, 3rd col, right diagonal, left diagonal]
    // after each turn must update count

void menu(char board[N][N], int aiCounter[M], int humanCounter[M]);
void runGame(char board[N][N], int aiCounter[], int humanCounter[]);
void humanMarkBoard(char board[N][N]);
void aiMarkBoard(char board[N][N], int aiCounter[], int humanCounter[]);
int aiPickIdx(char board[N][N], bool boardFilled, bool aiTurn);
int aiPickIdxHelper(int depth, char board[N][N], int coord, bool boardFilled, bool aiTurn, int maxEval, int minEval);
bool isGameOver(char board[N][N]);
int max(int x, int y);
int min(int x, int y);
void updateArrCounter(int coord, int aiCounter[], int humanCounter[]);
void updateArrCounter2(int coord, int aiCounter[], int humanCounter[], char board[N][N]);
void undoArrCounter(int coord, int aiCounter[], int humanCounter[], char board[N][N]);
char checkForWin(int aiCounter[], int humanCounter[M]);
void undoLastMove(int coord, char board[N][N]);
bool equalBoards(char one[N][N], char two[N][N]);
void resetArr(int ai[], int human[]);
void resetGame();
bool isBoardFilled();
bool isBoardFilled2(char board[N][N]);
void printDirections();
void printBoard(char board[N][N]);

int main(int argc, char* argv)
{
    menu(board, aiCounter, humanCounter);
    return 0;
};

void menu(char board[N][N], int aiCounter[M], int humanCounter[M]) {
    char switch_on = ' ';
    cout << "Welcome to Tic Tac Toe! " << endl;
    cout << "Press n to start new game. " << endl;
    cout << "Press c to continue game." << endl;
    cout << "Press d for directions. " << endl;
    cout << "Press e to end program. " << endl;
    cout << "Option: ";
    cin >> switch_on;

    switch (switch_on)
    {
    case 'n':
        resetGame();
        runGame(board, aiCounter, humanCounter);
    case 'c':
        runGame(board, aiCounter, humanCounter);
    case 'd':
        printDirections();
        menu(board, aiCounter, humanCounter);
    case 'e': exit(0);

    default: menu(board, aiCounter, humanCounter);
    }
}

void runGame(char board[N][N], int aiCounter[], int humanCounter[]) {
    printDirections();
    cout << "Let's play tic tac toe! " << endl;
    printBoard(board);
    char winner = ' ';
    while (true) {
        humanMarkBoard(board);
        updateArrCounter(currentCoord, aiCounter, humanCounter);
        if (isGameOver(board)) {
            cout << "Game Over!" << endl;
            if (checkForWin(aiCounter, humanCounter) == 'X') {
                cout << "X wins!" << endl;
            }
            if (checkForWin(aiCounter, humanCounter) == 'O') {
                cout << "O wins!" << endl;
            }
            if (isBoardFilled2(board) && checkForWin(aiCounter, humanCounter) == ' ') { ///tie
                cout << "It's a tie." << endl;
            }
            break;
        }


        aiMarkBoard(board, aiCounter, humanCounter);
        printBoard(board);
        updateArrCounter(currentCoord, aiCounter, humanCounter);

        if (isGameOver(board)) {
            cout << "Game Over!" << endl;
            if (checkForWin(aiCounter, humanCounter) == 'X') {
                cout << "X wins!" << endl;
            }
            if (checkForWin(aiCounter, humanCounter) == 'O') {
                cout << "O wins!" << endl;
            }
            if (isBoardFilled2(board) && checkForWin(aiCounter, humanCounter) == ' ') { 
                cout << "It's a tie." << endl;
            }
            break;
        }
    }
    resetGame();
    menu(board, aiCounter, humanCounter);

}

void humanMarkBoard(char board[N][N]) {  
    int input;
    string testinput;
    int coord;
    int r = 0;
    int c = 0;
    bool needInput = true;
    string promptMsg = "Enter 0 to go back to menu \nOr enter a position from 1 to 9 to mark board.\n";
    cout << promptMsg;

    while (needInput) {

        try {
            getline(cin, testinput);
            input = stoi(testinput); // throws an exception if cannot convert to int
            if (input == 0) {
                menu(board, aiCounter, humanCounter);
            }
            else {
                input--; // make input 0-indexed to calc row & col 
                r = input / 3;
                c = input % 3;
                if (board[r][c] == '_' && input >= 0 && input < 9) {
                    board[r][c] = 'X';
                    currentCoord = r * N + c;
                    needInput = false;
                    break;
                }
                if (input < 0 || input > 9) {
                    cout << "Out of bound position. " << endl;
                    cout << promptMsg;
                    continue;
                }
                if ((input >= 0 || input < 9) && board[r][c] != ' ') {
                    cout << "Position occupied. " << endl;
                    cout << promptMsg;
                }
            }
        }
        catch (invalid_argument & e) {
            if (testinput.empty()) {
                continue;
            }
            else {
                cout << promptMsg;
                continue;
            }
        }
    }

}

void aiMarkBoard(char board[N][N], int aiCounter[], int humanCounter[]) {
    int coord = aiPickIdx(board, isBoardFilled(), true);
    currentCoord = coord;
    if (coord >= 0 && coord < 9) {
        board[coord / N][coord % N] = 'O';
    }
    else {
        cout << coord << endl;
    }

}
int aiPickIdx(char board[N][N], bool boardFilled, bool aiTurn) {
    int maxEval = numeric_limits<int>::min();
    int minEval = numeric_limits<int>::max();

    int coord = aiPickIdxHelper(0, board, 0, boardFilled, aiTurn, maxEval, minEval);
    return coord;


}
/*  
________________________________________
MINIMAX IMPLEMENTATION - aiPickIdxHelper
________________________________________
if x wins return -1  (min)
if o wins return 1  (max)
else return 0
*/
int aiPickIdxHelper(int depth, char board[N][N], int coord, bool boardFilled, bool aiTurn, int maxEval, int minEval) { 
    int randCoord = 0;
    //save a copy of original board
    //check if changed board is equal to original board, if it is that means reached root of tree so return position not max val
    int eval;
    int saveMaxEval;
    int saveMinEval;
    if (isGameOver(board)) {
        if (checkForWin(aiCounter, humanCounter) == 'X') {
            return -1;
        }
        if (checkForWin(aiCounter, humanCounter) == 'O') {
            return 1;
        }
        if (isBoardFilled2(board) && checkForWin(aiCounter, humanCounter) == ' ') { ///tie
            return 0;
        }
    }
    if (aiTurn) {

        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (board[r][c] == '_') {
                    board[r][c] = 'O';
                    updateArrCounter2(r * N + c, aiCounter, humanCounter, board);
                    depth++;
                    eval = aiPickIdxHelper(depth, board, coord, isGameOver(board), false, maxEval, minEval);
                    depth--;
                    saveMaxEval = maxEval;
                    maxEval = max(maxEval, eval);
                    if (saveMaxEval != maxEval) {  
                        coord = r * N + c;
                    }
                    undoLastMove(r * N + c, board); // undo last move for next iteration 
                }
            }
        }
        if (depth == 0) {
            return coord;
        }
        return maxEval;
    }
    if (aiTurn == false) {

        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (board[r][c] == '_') {
                    board[r][c] = 'X';
                    updateArrCounter2(r * N + c, aiCounter, humanCounter, board);
                    depth++;         
                    eval = aiPickIdxHelper(depth, board, coord, isGameOver(board), true, maxEval, minEval);
                    depth--;
                    saveMinEval = minEval;
                    minEval = min(minEval, eval);
                    if (saveMinEval != minEval) {
                        coord = r * N + c;
                    }
                    undoLastMove(r * N + c, board);
                }
            }
        }
        if (depth == 0) {
            return coord;
        }
        return minEval;
    }


    return randCoord;
}


bool equalBoards(char one[N][N], char two[N][N]) {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (one[r] == two[c]) {
                if (one[r] != two[r]) {
                    return false;
                }
            }
        }
    }
    return true;
}

void undoLastMove(int coord, char board[N][N]) {
    undoArrCounter(coord, aiCounter, humanCounter, board);
    board[coord / 3][coord % 3] = '_';
}

void undoArrCounter(int coord, int aiCounter[], int humanCounter[], char board[N][N]) {
    int r = coord / 3;
    int c = coord % 3; 
    if (board[r][c] == 'O') {
        if (r == c) { 
            //left diagonal
            aiCounter[M - 1]--; //last index   
        }
        if (coord % 2 == 0 && coord > 0 && coord < 8) {
            //right diag
            aiCounter[M - 2]--; //second last index   
        }
        aiCounter[r]--;
        aiCounter[c + N]--; // Col

    }
    if (board[r][c] == 'X') {
        if (r == c) {
            //left diagonal
            humanCounter[M - 1]--; //last index
        }
        if (coord % 2 == 0 && coord > 0 && coord < 8) {
            ///right diag
            humanCounter[M - 2]--; //second last index   
        }
        humanCounter[r]--;
        humanCounter[c + N]--; // COL
    }
}
void updateArrCounter2(int coord, int aiCounter[], int humanCounter[], char board[N][N]) {
    //AI IS O
    int r = coord / 3;
    int c = coord % 3;
    //printBoard(board);  
    if (board[r][c] == 'O') {
        if (r == c) {
            //left diagonal
            aiCounter[M - 1]++; //last index   
        }
        if (coord % 2 == 0 && coord > 0 && coord < 8) {
            //right diag
            aiCounter[M - 2]++; //second last index   
        }
        aiCounter[r]++;
        aiCounter[c + N]++; // Col

    }
    if (board[r][c] == 'X') {
        if (r == c) {
            //left diagonal
            humanCounter[M - 1]++; //last index
        }
        if (coord % 2 == 0 && coord > 0 && coord < 8) {
            ///right diag
            humanCounter[M - 2]++; //second last index   
        }
        humanCounter[r]++;
        humanCounter[c + N]++; // COL

    }

}

void updateArrCounter(int coord, int aiCounter[], int humanCounter[]) {
    //AI IS O
    int r = coord / 3;
    int c = coord % 3;
    if (board[r][c] == 'O') {
        if (r == c) {
            //left diagonal
            aiCounter[M - 1]++; //last index   
        }
        if (coord % 2 == 0 && coord > 0 && coord < 8) {
            //right diag
            aiCounter[M - 2]++; //second last index   
        }
        aiCounter[r]++;
        aiCounter[c + N]++; // Col

    }
    if (board[r][c] == 'X') {
        if (r == c) {
            //left diagonal
            humanCounter[M - 1]++; //last index
        }
        if (coord % 2 == 0 && coord > 0 && coord < 8) {
            ///right diag
            humanCounter[M - 2]++; //second last index   
        }
        humanCounter[r]++;
        humanCounter[c + N]++; // COL

    }

}
bool isGameOver(char board[N][N]) {
    if (checkForWin(aiCounter, humanCounter) == 'X' || checkForWin(aiCounter, humanCounter) == 'O') {
        return true;
    }
    if (isBoardFilled2(board)) {
        return true;
    }
    return false;
}
char checkForWin(int aiCounter[], int humanCounter[M]) {
    for (int i = 0; i < M; i++) {
        if (aiCounter[i] == 3) {
            return 'O';

        }
        if (humanCounter[i] == 3) {
            return 'X';
        }

    }

    return ' ';

}
int max(int x, int y) {
    if (y > x) {
        return y;
    }

    return x;
}
int min(int x, int y) {
    if (y > x) {
        return x;
    }

    return x;
}
bool isBoardFilled() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == '_') {
                return false;
            }
        }
    }
    return true;
}
bool isBoardFilled2(char board[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == '_') {
                return false;
            }
        }
    }
    return true;
}
void resetArr(int ai[], int human[]) {
    for (int i = 0; i < M; i++) {
        ai[i] = 0;
        human[i] = 0;
    }
}

void resetGame() {
    resetArr(aiCounter, humanCounter);
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            board[r][c] = '_';
        }
    }
    currentCoord = 0;
}

void printDirections() {
    ifstream dirFile;
    string dirOutput = "";
    dirFile.open("directions.txt");

    while (getline(dirFile, dirOutput)) {
        //returns true after each line of .txt
        cout << dirOutput << "\n";

    }

    dirFile.close();

}

void  printBoard(char board[N][N]) {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            cout << board[r][c];
            cout << " ";
        }
        cout << endl;
    }
}








