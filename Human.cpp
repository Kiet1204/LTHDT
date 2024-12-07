#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;
#define SIZE 15

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

class Board {
public:
    vector<vector<int>> cells;
    vector<vector<char>> marks;

    Board() {
        int num = 1;
        cells.resize(SIZE, vector<int>(SIZE));
        marks.resize(SIZE, vector<char>(SIZE, ' '));
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                cells[i][j] = num++;
            }
        }
    }

    void display() const {
        cout << "    ";
        for (int j = 0; j < SIZE; j++) {
            cout << (j + 1 < 10 ? " " : "") << j + 1 << "   ";
        }
        cout << endl;

        for (int i = 0; i < SIZE; i++) {
            cout << "   +";
            for (int j = 0; j < SIZE; j++) {
                cout << "---+";
            }
            cout << endl;

            cout << (i + 1 < 10 ? " " : "") << i + 1 << " |";
            for (int j = 0; j < SIZE; j++) {
                if (marks[i][j] == 'X') {
                    setColor(12);
                    cout << " X ";
                }
                else if (marks[i][j] == 'O') {
                    setColor(9);
                    cout << " O ";
                }
                else {
                    setColor(14);
                    cout << (cells[i][j] < 10 ? " " : "") << (cells[i][j] < 100 ? " " : "") << cells[i][j];
                }
                setColor(7);
                cout << "|";
            }
            cout << endl;
        }

        cout << "   +";
        for (int j = 0; j < SIZE; j++) {
            cout << "---+";
        }
        cout << endl;
    }

    bool isEmpty(int move) const {
        int row = (move - 1) / SIZE;
        int col = (move - 1) % SIZE;
        return marks[row][col] == ' ';
    }

    void makeMove(int move, char symbol) {
        int row = (move - 1) / SIZE;
        int col = (move - 1) % SIZE;
        marks[row][col] = symbol;
    }
    bool checkWin(int move, char symbol) const {
        int row = (move - 1) / SIZE;
        int col = (move - 1) % SIZE;
        return checkDirection(row, col, symbol, 1, 0) ||  // Ngang
            checkDirection(row, col, symbol, 0, 1) ||  // Dọc
            checkDirection(row, col, symbol, 1, 1) ||  // Chéo chính
            checkDirection(row, col, symbol, 1, -1);   // Chéo phụ
    }
private:
    // Kiểm tra hướng thắng
    bool checkDirection(int row, int col, char symbol, int dr, int dc) const {
        int count = 1;

        // Kiểm tra cùng hướng
        for (int i = 1; i < 5; i++) {
            int nr = row + dr * i;
            int nc = col + dc * i;
            if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE && marks[nr][nc] == symbol)
                count++;
            else
                break;
        }

        // Kiểm tra ngược hướng
        for (int i = 1; i < 5; i++) {
            int nr = row - dr * i;
            int nc = col - dc * i;
            if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE && marks[nr][nc] == symbol)
                count++;
            else
                break;
        }

        return count >= 5; // 5 quân liên tiếp để thắng
    }
};

class Player {
protected:
    char symbol;

public:
    Player(char symbol) : symbol(symbol) {}
    virtual int getMove(const Board& board) = 0; // Phương thức ảo thuần túy
    char getSymbol() const { return symbol; }
};

class HumanPlayer : public Player {
public:
    HumanPlayer(char symbol) : Player(symbol) {}

    int getMove(const Board& board) override {
        int move;
        while (true) {
            cout << "Player " << symbol << ", enter your move (1-225): ";
            cin >> move;
            if (move < 1 || move > SIZE * SIZE || !board.isEmpty(move)) {
                cout << "Invalid move! Please try again.\n";
                continue;
            }
            return move;
        }
    }
};
class Game {
private:
    Board board;
    Player* player1;
    Player* player2;
    Player* currentPlayer;

    void switchTurn() {
        currentPlayer = (currentPlayer == player1) ? player2 : player1;
    }

public:
    Game() : player1(nullptr), player2(nullptr), currentPlayer(nullptr) {}

    ~Game() {
        delete player1;
        delete player2;
    }

    void setup() {

        cout << " Player vs Player\n";
        char symbol1 = 'X', symbol2 = 'O';

        cout << "Chon ky hieu (X or O): ";
        cin >> symbol1;
        symbol1 = toupper(symbol1);
        symbol2 = (symbol1 == 'X') ? 'O' : 'X';

        player1 = new HumanPlayer(symbol1);
        player2 = new HumanPlayer(symbol2);

        int turn;
        cout << "Chon luot di (1: Player 1, 2: Player 2): ";
        cin >> turn;
        currentPlayer = (turn == 1) ? player1 : player2;
    }

    void play() {
        while (true) {
            board.display();
            int move = currentPlayer->getMove(board);
            board.makeMove(move, currentPlayer->getSymbol());

            if (board.checkWin(move, currentPlayer->getSymbol())) {
                board.display();
                cout << "Player " << currentPlayer->getSymbol() << " wins!\n";
                break;
            }

            switchTurn();
        }
    }
};

int main() {
    srand(time(0));
    Game game;
    game.setup();
    game.play();
    return 0;
}