#include <iostream>
#include <set>
#include <vector>

#define PLAYER_COLOR 0
#define AI_COLOR 1

std::vector<std::vector<int>> curBoard(15, std::vector<int>(15, -1));
std::vector<std::pair<int, int>> playedMoves;
std::set<std::vector<std::vector<int>>> possibleStates;
bool isGameRunning = true;
bool needInput = true;
bool gameWon = false;
int curDepth;
int maxDepth = 0;
int turnNum = 0;

struct MoveValue {
    std::pair<int, int> move;
    int value;
};

bool findNumInRow(int num, int color) {
    for (int y = 0; y < 15; y++) {
        for (int x = 0; x < 15; x++) {
            if (curBoard[y][x] == color) {
                int count = 0;
                for (int i = x; i < 15 && curBoard[y][i] == color; i++) {
                    count++;
                    if (count == num) return true;
                }

                count = 0;
                for (int i = y; i < 15 && curBoard[i][x] == color; i++) {
                    count++;
                    if (count == num) return true;
                }

                count = 0;
                for (int i = x, j = y;
                     i < 15 && j < 15 && curBoard[j][i] == color; i++, j++) {
                    count++;
                    if (count == num) return true;
                }

                count = 0;
                for (int i = x, j = y;
                     i < 15 && j >= 0 && curBoard[j][i] == color; i++, j--) {
                    count++;
                    if (count == num) return true;
                }
            }
        }
    }

    return false;
}

bool contains(const std::vector<std::pair<int, int>>& vec,
              const std::pair<int, int>& element) {
    return std::find(vec.begin(), vec.end(), element) != vec.end();
}

std::vector<std::vector<int>> copyBoard(std::vector<std::vector<int>>& board) {
    std::vector<std::vector<int>> newBoard(board);
    return newBoard;
}

void moveGenerator(std::vector<std::vector<int>> board) {
    // iterate thru all legal moves and add those states to possibleStates
    //
    possibleStates.clear();

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (curBoard[i][j] == -1) {
                std::vector<std::vector<int>> newBoard = copyBoard(curBoard);

                newBoard[i][j] = AI_COLOR;

                possibleStates.insert(newBoard);
            }
        }
    }
}

int evaluateLine(int cell1, int cell2, int cell3, int cell4, int cell5) {
    int aiCount = 0;
    int playerCount = 0;

    if (cell1 == AI_COLOR)
        aiCount++;
    else if (cell1 == PLAYER_COLOR)
        playerCount++;

    if (cell2 == AI_COLOR)
        aiCount++;
    else if (cell2 == PLAYER_COLOR)
        playerCount++;

    if (cell3 == AI_COLOR)
        aiCount++;
    else if (cell3 == PLAYER_COLOR)
        playerCount++;

    if (cell4 == AI_COLOR)
        aiCount++;
    else if (cell4 == PLAYER_COLOR)
        playerCount++;

    if (cell5 == AI_COLOR)
        aiCount++;
    else if (cell5 == PLAYER_COLOR)
        playerCount++;

    if (aiCount == 5) return 100000;
    if (playerCount == 5) return -100000;
    if (aiCount == 4 && playerCount == 0) return 10000;
    if (playerCount == 4 && aiCount == 0) return -10000;

    return aiCount * aiCount * aiCount -
           playerCount * playerCount * playerCount;
}

int evaluateBoard(const std::vector<std::vector<int>>& board) {
    int score = 0;

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 11; ++j) {
            score += evaluateLine(board[i][j], board[i][j + 1], board[i][j + 2],
                                  board[i][j + 3], board[i][j + 4]);
        }
    }

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 11; ++j) {
            score += evaluateLine(board[j][i], board[j + 1][i], board[j + 2][i],
                                  board[j + 3][i], board[j + 4][i]);
        }
    }

    for (int i = 0; i < 11; ++i) {
        for (int j = 0; j < 11; ++j) {
            score += evaluateLine(board[i][j], board[i + 1][j + 1],
                                  board[i + 2][j + 2], board[i + 3][j + 3],
                                  board[i + 4][j + 4]);
        }
    }

    for (int i = 0; i < 11; ++i) {
        for (int j = 4; j < 15; ++j) {
            score += evaluateLine(board[i][j], board[i + 1][j - 1],
                                  board[i + 2][j - 2], board[i + 3][j - 3],
                                  board[i + 4][j - 4]);
        }
    }

    return score;
}

MoveValue minimax(std::vector<std::vector<int>> node, int depth) {
    MoveValue bestMoveValue;
    bestMoveValue.value = -std::numeric_limits<int>::max();

    if (depth == 0) {
        bestMoveValue.value = evaluateBoard(node);
        return bestMoveValue;
    }

    for (const auto& state : possibleStates) {
        int x = state[0][0];
        int y = state[0][1];

        if (x >= 0 && x < node.size() && y >= 0 && y < node[0].size()) {
            if (node[x][y] == -1) {
                node[x][y] = AI_COLOR;

                MoveValue currentMoveValue = minimax(node, depth - 1);

                node[x][y] = -1;

                if (currentMoveValue.value > bestMoveValue.value) {
                    bestMoveValue.value = currentMoveValue.value;
                    bestMoveValue.move = {x, y};
                }
            }
        }
    }

    return bestMoveValue;
}

void aiTurn() {
    std::pair<int, int> bestMove = {-1, -1};
    int bestValue = -std::numeric_limits<int>::max();

    for (const auto& state : possibleStates) {
        int value = minimax(state, maxDepth).value;

        if (value > bestValue) {
            bestValue = value;
            for (int i = 0; i < 15; ++i) {
                for (int j = 0; j < 15; ++j) {
                    if (state[i][j] == AI_COLOR && curBoard[i][j] == -1) {
                        bestMove = {i, j};
                        break;
                    }
                }
            }
        }
    }

    curBoard[bestMove.first][bestMove.second] = AI_COLOR;
    playedMoves.push_back(bestMove);

    needInput = true;
}

void checkWin() {}

void printBoard() {
    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            switch (curBoard[i][j]) {
                case -1:
                    std::cout << "+---";

                    break;
                case PLAYER_COLOR:
                    std::cout << "\033[1;91m█\033[0m---";

                    break;
                case AI_COLOR:
                    std::cout << "\033[1;94m█\033[0m---";

                    break;
            }
        }
        std::cout << "+" << i + 1 << std::endl;

        for (int j = 0; j < 14; ++j) {
            std::cout << "|   ";
        }
        std::cout << "|" << std::endl;
    }

    for (int j = 0; j < 14; ++j) {
        switch (curBoard[14][j]) {
            case -1:
                std::cout << "+---";
                break;
            case PLAYER_COLOR:
                std::cout << "\033[1;91m█\033[0m---";
                break;
            case AI_COLOR:
                std::cout << "\033[1;94m█\033[0m---";
                break;
        }
    }
    switch (curBoard[14][14]) {
        case -1:
            std::cout << "+15" << std::endl;
            break;
        case PLAYER_COLOR:
            std::cout << "\033[1;91m█\033[0m" << std::endl;
            break;
        case AI_COLOR:
            std::cout << "\033[1;94m█\033[0m" << std::endl;
            break;
    }
    std::cout << "1   2   3   4   5   6   7   8   9   10  11  12  13  14  15"
              << std::endl;
}

int main() {
    int x, y;
    std::cout << "Enter maximum depth for algorithm: ";
    std::cin >> maxDepth;
    moveGenerator(curBoard);
    while (isGameRunning) {
        while (needInput) {
            if (turnNum == 0) {
                std::cout << "Must play move: 8 8" << std::endl;
            } else if (turnNum == 1) {
                std::cout << "Must play 3 intersections away" << std::endl;
            }

            std::cout << "Enter coordinates for move. (Format: x y): ";
            if (std::cin >> x >> y) {
                if (contains(playedMoves, std::make_pair(x, y))) {
                    std::cout << "\033[1;91m-=- Move has already been played "
                                 "-=-\033[0m"
                              << std::endl;
                } else if (turnNum == 1 &&
                           (std::abs(x - 8) <= 2 && std::abs(y - 8) <= 2)) {
                    std::cout << "\033[1;91m-=- Invalid move. Must be at least "
                                 "3 intersections away from the center square. "
                                 "-=-\033[0m"
                              << std::endl;

                } else {
                    if (x <= 15 && x >= 1 && y <= 15 && y >= 1) {
                        curBoard[y - 1][x - 1] = PLAYER_COLOR;
                        playedMoves.push_back(std::make_pair(x, y));
                        needInput = false;
                        turnNum++;
                    } else {
                        std::cout
                            << "\033[1;91m-=- Invalid coordinates. Must be "
                               "between 1-15 -=-\033[0m"
                            << std::endl;
                    }
                }
            } else {
                std::cout << "\033[1;91m-=- Invalid input. Please enter "
                             "integers. -=-\033[0m"
                          << std::endl;

                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
            }
        }
        if (findNumInRow(5, PLAYER_COLOR)) {
            std::cout << "win";
        }
        aiTurn();
        if (findNumInRow(5, AI_COLOR)) {
            std::cout << "win";
        }
        printBoard();
    }

    // TODO - take care of switching turns, and setting needInput to true after
    // the AI's turn

    return 1;
}