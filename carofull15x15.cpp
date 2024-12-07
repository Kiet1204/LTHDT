#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;
#define SIZE 15

void setColor(int color)//Thay đổi màu sắc trong console, áp dụng cho nước đi người chơi(X màu đỏ, O màu xanh)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

class Board // Bảng trò chơi
{
public:
    vector<vector<int>> cells;//lưu số thứ tự của từng ô(1 đến 225)
    vector<vector<char>> marks;//Lưu trạng thái mỗi ô ('X', 'O', hoặc ' ').
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
//Hiển thị bảng caro, bao gồm số thứ tự các ô và các ký hiệu X/O
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
//Kiểm tra xem một ô có trống không
    bool isEmpty(int move) const {
        int row = (move - 1) / SIZE;
        int col = (move - 1) % SIZE;
        return marks[row][col] == ' ';
    }
//Đánh dấu nước đi tại vị trí tương ứng
    void makeMove(int move, char symbol) {
        int row = (move - 1) / SIZE;
        int col = (move - 1) % SIZE;
        marks[row][col] = symbol;
    }
    //Kiểm tra xem người chơi có thắng không, bằng cách kiểm tra 4 hướng
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
//Lớp người chơi
class Player {
protected:
    char symbol;

public:
    Player(char symbol) : symbol(symbol) {}
    virtual int getMove(const Board& board) = 0; // Phương thức ảo thuần túy
    char getSymbol() const { return symbol; }
};
//Lớp thực hiện nước đi của người chơi
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
//Lớp thực hiện nước đi của máy tính
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
// Hàm tìm nước đi tốt nhất cho máy tính
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
//Lớp quản lý trò chơi
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
//Cho phép người chơi chọn chế độ, ký hiệu, cấp độ AI, và thứ tự chơi.
    void setup() {
        int mode;
        cout << "Choose game mode:\n";
        cout << "1. Player vs Player\n";
        cout << "2. Player vs Computer\n";
        cout << "Enter your choice: ";
        cin >> mode;

        char symbol1 = 'X', symbol2 = 'O';
        if (mode == 1) {
            cout << "Chon ky hieu (X or O): ";
            cin >> symbol1;
            symbol1 = toupper(symbol1);
            symbol2 = (symbol1 == 'X') ? 'O' : 'X';

            player1 = new HumanPlayer(symbol1);
            player2 = new HumanPlayer(symbol2);
        }
        else {
            cout << "Chon ky hieu (X or O): ";
            cin >> symbol1;
            symbol1 = toupper(symbol1);
            symbol2 = (symbol1 == 'X') ? 'O' : 'X';

            player1 = new HumanPlayer(symbol1);

            string difficulty;
            cout << "Chon cap do choi (easy/hard): ";
            cin >> difficulty;

            player2 = new ComputerPlayer(symbol2, difficulty);
        }

        int turn;
        cout << "Chon luot choi (1: Player 1, 2: Player 2): ";
        cin >> turn;
        currentPlayer = (turn == 1) ? player1 : player2;
    }
//Vòng lặp chính của trò chơi
    void play() {
        while (true) {
            board.display();//hiển thị bảng
            int move = currentPlayer->getMove(board);//lấy nước đi từ người chơi hiện tại
            board.makeMove(move, currentPlayer->getSymbol());//cập nhật lại bảng

            if (board.checkWin(move, currentPlayer->getSymbol()))//Kiểm tra người chiến thắng
            {
                board.display();
                cout << "Player " << currentPlayer->getSymbol() << " wins!\n";
                break;
            }

            switchTurn();//chuyển lượt chơi
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