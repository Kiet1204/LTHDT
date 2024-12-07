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

class ComputerPlayer : public Player {
private:
    string difficulty; // Chế độ "easy" hoặc "hard"

public:
    ComputerPlayer(char symbol, string difficulty) : Player(symbol), difficulty(difficulty) {}

    int getMove(const Board& board) override {
        if (difficulty == "easy") {
            return getRandomMove(board);
        }
        return getComputerMove(board);
    }

private:
    // Chọn nước đi ngẫu nhiên cho chế độ dễ
    int getRandomMove(const Board& board) {
        vector<int> availableMoves;
        for (int i = 1; i <= SIZE * SIZE; i++) {
            if (board.isEmpty(i)) {
                availableMoves.push_back(i);
            }
        }
        return availableMoves[rand() % availableMoves.size()];
    }

    // Chế độ khó: Đánh giá nước đi như đã cài đặt
    int evaluateMove(const Board& board, int move, char symbol) {
        int score = 0;
        int row = (move - 1) / SIZE;
        int col = (move - 1) % SIZE;

        const int directions[4][2] = {
            {1, 0}, {0, 1}, {1, 1}, {1, -1}
        };

        for (int i = 0; i < 4; i++) {
            int dr = directions[i][0];
            int dc = directions[i][1];
            int count = 0, block = 0;

            for (int step = 1; step < 5; step++) {
                int nr = row + dr * step;
                int nc = col + dc * step;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                    if (board.marks[nr][nc] == symbol)
                        count++;
                    else if (board.marks[nr][nc] != ' ')
                        block++;
                }
                else {
                    block++;
                }
            }

            for (int step = 1; step < 5; step++) {
                int nr = row - dr * step;
                int nc = col - dc * step;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                    if (board.marks[nr][nc] == symbol)
                        count++;
                    else if (board.marks[nr][nc] != ' ')
                        block++;
                }
                else {
                    block++;
                }
            }

            if (count >= 4) score += 1000;
            else if (count == 3 && block < 2) score += 100;
            else if (count == 2 && block < 2) score += 10;
        }
        return score;
    }

    int getComputerMove(const Board& board) {
        vector<int> availableMoves;
        vector<int> scores(SIZE * SIZE + 1, 0);

        for (int i = 1; i <= SIZE * SIZE; i++) {
            if (board.isEmpty(i)) {
                availableMoves.push_back(i);
            }
        }

        for (int move : availableMoves) {
            scores[move] += evaluateMove(board, move, symbol);
            scores[move] += evaluateMove(board, move, symbol == 'X' ? 'O' : 'X'); // Phòng thủ
        }

        int bestMove = availableMoves[0];
        int maxScore = scores[bestMove];

        for (int move : availableMoves) {
            if (scores[move] > maxScore) {
                maxScore = scores[move];
                bestMove = move;
            }
        }

        return bestMove;
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

        cout << " Player vs Computer\n";
        char symbol1 = 'X', symbol2 = 'O';

        cout << "chon ky hieu (X or O): ";
        cin >> symbol1;
        symbol1 = toupper(symbol1);
        symbol2 = (symbol1 == 'X') ? 'O' : 'X';

        player1 = new HumanPlayer(symbol1);

        string difficulty;
        cout << "chon cap do choi cho may (easy/hard): ";
        cin >> difficulty;

        player2 = new ComputerPlayer(symbol2, difficulty);


        int turn;
        cout << "chon luot di (1: Player 1, 2: Player 2): ";
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
                cout << "Player " << currentPlayer->getSymbol() << " win!\n";
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