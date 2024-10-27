#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

#define UP 'U'
#define DOWN 'D'
#define LEFT 'L'
#define RIGHT 'R'
#define BFS 1
#define IDS 2
#define H1 3
#define H2 4
#define H3 5

char moves[4] = {UP, DOWN, LEFT, RIGHT};

class gameState_t;
using boardArray = std::array<std::array<char, 3>, 3>;

class gameState_t {
   public:
    gameState_t() {
        // default constructor
        isValid = true;
    }
    gameState_t(boardArray initBoard) {  // constructor for goal board
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                board[i][j] = initBoard[i][j];
            }
        }
        dashX = 2;
        dashY = 2;
        isValid = true;
    }
    gameState_t(bool valid) {  // constructor for bad states
        isValid = valid;
    }

    gameState_t(boardArray b, gameState_t *p, char lM, int x, int y, bool v,
                std::vector<char> pa, int d, int f) {
        board = b;
        parent = p;
        lastMove = lM;
        dashX = x;
        dashY = y;
        isValid = v;
        solutionPath = pa;
        depth = d;
        fn = f;
    }

    bool operator==(const gameState_t &other) const {
        // Implement comparison logic here
        // For example, compare each member variable
        return this->board == other.board &&
               this->parent == other.parent /* Add other comparisons */;
    }

    boardArray board;
    gameState_t *parent;
    char lastMove;
    int dashX;
    int dashY;
    int depth;
    bool isValid;
    std::vector<char> solutionPath;
    int fn;
};

class binaryHeap {
   private:
    std::vector<gameState_t> heap;

    void heapifyUp(int index) {
        if (index <= 0) return;
        int parent = (index - 1) / 2;
        if (heap[parent].fn > heap[index].fn) {
            std::swap(heap[parent], heap[index]);
            heapifyUp(parent);
        }
    }

    void heapifyDown(int index) {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;
        if (leftChild < heap.size() && heap[leftChild].fn < heap[smallest].fn) {
            smallest = leftChild;
        }
        if (rightChild < heap.size() &&
            heap[rightChild].fn < heap[smallest].fn) {
            smallest = rightChild;
        }
        if (smallest != index) {
            std::swap(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }

   public:
    binaryHeap() {}

    void push(const gameState_t &value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    void pop() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
    }

    const gameState_t &top() const { return heap.front(); }

    bool empty() const { return heap.empty(); }

    int size() { return heap.size(); }

    gameState_t at(int index) { return heap.at(index); }

    void replace(int index, const gameState_t &value) {
        if (index < 0 || index >= heap.size()) {
            throw std::out_of_range("Index out of range");
        }
    }

    std::pair<bool, size_t> contains(const gameState_t &element) const {
        auto it = std::find(heap.begin(), heap.end(), element);
        if (it != heap.end()) {
            size_t index = std::distance(heap.begin(), it);
            return {true, index};
        } else {
            return {false, -1};
        }
    }
};

std::queue<gameState_t> gameQueue;
std::stack<gameState_t> gameStack;
binaryHeap prioQueue;
std::map<std::string, boardArray> generatedStates;

std::vector<gameState_t> solutions;

std::string fileLoc;
boardArray goalBoard = {{{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '_'}}};
std::string goalString;
clock_t tStart;
int chosenAlg;
bool isSolvable;
int numNodes = 0;

gameState_t goalState(goalBoard);

gameState_t copyState(gameState_t state) {
    gameState_t copiedState;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            copiedState.board[i][j] = state.board[i][j];
        }
    }
    copiedState.parent = state.parent;
    copiedState.lastMove = state.lastMove;
    copiedState.dashX = state.dashX;
    copiedState.dashY = state.dashY;
    copiedState.isValid = state.isValid;

    return copiedState;
}

int manhattan(int locX, int locY, int goalX, int goalY) {
    int x = abs(locX - goalX);
    int y = abs(locY - goalY);
    return x + y;
}

int calcHn(gameState_t s) {
    int hn = 0;
    int i, j, k;
    int expectedRow, expectedCol;
    char tile, otherTile;

    if (chosenAlg == H1) {
        for (i = 0; i < 3; i++) {  // MISPLACED TILE
            for (j = 0; j < 3; j++) {
                if (s.board[i][j] != goalBoard[i][j]) {
                    hn++;
                }
            }
        }
    } else if (chosenAlg == H2) {  // MANHATTAN DISTANCE
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                if (s.board[i][j] != goalBoard[i][j] && s.board[i][j] != '_') {
                    switch (s.board[i][j]) {
                        case '1':
                            hn += manhattan(j, i, 0, 0);
                            break;
                        case '2':
                            hn += manhattan(j, i, 1, 0);
                            break;
                        case '3':
                            hn += manhattan(j, i, 2, 0);
                            break;
                        case '4':
                            hn += manhattan(j, i, 0, 1);
                            break;
                        case '5':
                            hn += manhattan(j, i, 1, 1);
                            break;
                        case '6':
                            hn += manhattan(j, i, 2, 1);
                            break;
                        case '7':
                            hn += manhattan(j, i, 0, 2);
                            break;
                        case '8':
                            hn += manhattan(j, i, 1, 2);
                            break;
                    }
                }
            }
        }
    } else if (chosenAlg == H3) {  // LINEAR CONFLICT MANHATTAN HEURISTIC
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                tile = s.board[i][j];
                if (tile == '_') {
                    continue;
                }

                expectedRow = ((tile - '0') - 1) / 3;
                expectedCol = ((tile - '0') - 1) % 3;

                hn += manhattan(i, j, expectedRow, expectedCol);

                if (i == expectedRow) {
                    for (k = i; k < 3; ++k) {
                        otherTile = s.board[i][k];
                        if (otherTile != '_' &&
                            ((otherTile - '0') - 1) / 3 == expectedRow &&
                            otherTile < tile) {
                            hn += 2;
                        }
                    }
                }
                if (j == expectedCol) {
                    for (k = i; k < 3; ++k) {
                        otherTile = s.board[k][j];
                        if (otherTile != '_' &&
                            ((otherTile - '0') - 1) % 3 == expectedCol &&
                            otherTile < tile) {
                            hn += 2;
                        }
                    }
                }
            }
        }
    }

    return hn;
}

bool isValidMove(gameState_t state, char dir) {
    if (state.dashX == 1 && state.dashY == 1) {
        return true;
    }

    switch (dir) {
        case UP:
            if (state.dashY == 2) {
                return false;
            }
            break;
        case DOWN:
            if (state.dashY == 0) {
                return false;
            }
            break;
        case LEFT:
            if (state.dashX == 2) {
                return false;
            }
            break;
        case RIGHT:
            if (state.dashX == 0) {
                return false;
            }
            break;
    }
    return true;
}

void copyBoard(boardArray &newBoard, boardArray oldBoard) {
    int i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            newBoard[i][j] = oldBoard[i][j];
        }
    }
}

void copySolPath(std::vector<char> &newPath, const std::vector<char> &old) {
    newPath = old;
}

gameState_t move(gameState_t state, char dir) {
    gameState_t newState;
    gameState_t badState = gameState_t(false);
    char temp;
    int newDashY, newDashX, depth, newFn;
    boardArray newBoard{{{'0', '0', '0'}, {'0', '0', '0'}, {'0', '0', '_'}}};
    std::vector<char> newPath;

    copySolPath(newPath, state.solutionPath);

    if (isValidMove(state, dir)) {
        copyBoard(newBoard, state.board);
        switch (dir) {
            case UP:
                temp = state.board[state.dashY + 1][state.dashX];
                newBoard[state.dashY][state.dashX] = temp;
                newDashY = state.dashY + 1;
                newDashX = state.dashX;
                newBoard[newDashY][state.dashX] = '_';
                newPath.push_back(dir);
                depth = state.depth + 1;
                break;
            case DOWN:
                temp = state.board[state.dashY - 1][state.dashX];
                newBoard[state.dashY][state.dashX] = temp;
                newDashY = state.dashY - 1;
                newDashX = state.dashX;
                newBoard[newDashY][state.dashX] = '_';
                newPath.push_back(dir);
                depth = state.depth + 1;
                break;
            case LEFT:
                temp = state.board[state.dashY][state.dashX + 1];
                newBoard[state.dashY][state.dashX] = temp;
                newDashY = state.dashY;
                newDashX = state.dashX + 1;
                newBoard[state.dashY][newDashX] = '_';
                newPath.push_back(dir);
                depth = state.depth + 1;
                break;
            case RIGHT:
                temp = state.board[state.dashY][state.dashX - 1];
                newBoard[state.dashY][state.dashX] = temp;
                newDashY = state.dashY;
                newDashX = state.dashX - 1;
                newBoard[state.dashY][newDashX] = '_';
                newPath.push_back(dir);
                depth = state.depth + 1;
                break;
        }
        newFn = state.depth + calcHn(state);
        newState = gameState_t(newBoard, &state, dir, newDashX, newDashY, true,
                               newPath, depth, newFn);
        return newState;
    }
    return badState;
}

std::string boardToString(const boardArray &board) {
    std::string result;
    for (size_t i = 0; i < board.size(); ++i) {
        for (size_t j = 0; j < board[i].size(); ++j) {
            result += board[i][j];
        }
    }
    return result;
}

bool hasBeenGenerated(boardArray &board) {
    std::string key = boardToString(board);
    bool hasBeen = generatedStates.find(key) != generatedStates.end();
    return hasBeen;
}

bool isSolution(boardArray board) {
    std::string sol;
    sol = boardToString(board);

    if (sol == "12345678_") {
        return true;
    }

    return false;
}

bool isCycle(boardArray board) {
    std::string key = boardToString(board);
    bool isCycle = generatedStates.find(key) != generatedStates.end();
    return isCycle;
}

void astar() {
    gameState_t curState;
    gameState_t nextState;
    std::string key;
    int i;
    while (!prioQueue.empty()) {
        if ((((double)(clock() - tStart)) / CLOCKS_PER_SEC) >= 900) {
            return;
        }

        curState = prioQueue.top();
        numNodes++;
        prioQueue.pop();
        if (isSolution(curState.board)) {
            solutions.push_back(curState);
        }

        key = boardToString(curState.board);
        generatedStates.insert(std::make_pair(key, curState.board));

        for (i = 0; i < 4; i++) {
            nextState = move(curState, moves[i]);
            key = boardToString(nextState.board);
            if (nextState.isValid == false) {
                generatedStates.insert(std::make_pair(key, nextState.board));
                continue;
            }

            // first - bool if in queue, second - index if in queue
            auto inQueue = prioQueue.contains(nextState);

            if (!hasBeenGenerated(nextState.board)) {
                prioQueue.push(nextState);
                generatedStates.insert(std::make_pair(key, nextState.board));
            } else if (inQueue.first) {
                if (nextState.fn > prioQueue.at(inQueue.second).fn) {
                    prioQueue.replace(inQueue.second, nextState);
                }
            }
        }
    }
}

gameState_t cutoffNode;

gameState_t depthLimSearch(int depth) {  // used in ids
    gameState_t result = gameState_t(false);
    gameState_t node;
    // gameState_t cutoffNode;
    gameState_t child;
    int i;
    std::string key;
    while (!gameStack.empty()) {
        node = gameStack.top();
        gameStack.pop();
        if (isSolution(node.board)) {
            return node;
        }
        if (node.depth > depth) {
            result = node;
        } else if (!isCycle(node.board)) {
            for (i = 0; i < 4; i++) {
                child = move(node, moves[i]);
                key = boardToString(child.board);
                gameStack.push(child);
                std::cout << gameStack.size() << std::endl;
                generatedStates.insert(std::make_pair(key, child.board));
            }
        }
    }
    return result;
}

void ids() {  // iterative depth search
    int depth = 0;
    gameState_t result;

    while (true && depth < 9) {
        result = depthLimSearch(depth);
        if (isSolution(result.board)) {
            solutions.push_back(result);
        } else if (!result.isValid) {
            gameStack.push(result);
        }
        depth++;
    }
}

void bfs() {
    int i;
    gameState_t curState;
    gameState_t nextState;
    std::string key;
    while (!gameQueue.empty()) {
        if (((double)(clock() - tStart) / CLOCKS_PER_SEC) >= 900) {
            return;
        }

        curState = gameQueue.front();
        gameQueue.pop();
        numNodes++;
        key = boardToString(curState.board);
        generatedStates.insert(std::make_pair(key, curState.board));

        for (i = 0; i < 4; i++) {  // for each (UP, DOWN, LEFT, RIGHT)
            nextState = move(curState, moves[i]);
            key = boardToString(nextState.board);

            if (nextState.isValid == false) {
                generatedStates.insert(std::make_pair(key, nextState.board));
            } else if (isSolution(nextState.board)) {
                solutions.push_back(nextState);
            } else if (hasBeenGenerated(nextState.board)) {
            } else {
                gameQueue.push(nextState);
            }
        }
    }
}

bool findSolvable(gameState_t s) {
    bool solvable;
    int numInversions = 0;
    int i, j;
    std::string board;
    board = boardToString(s.board);

    for (i = 0; i < board.length(); i++) {
        for (j = 0; j < board.length() - 1; j++) {
            if (board.at(i) || board.at(j)) {
                continue;
            }
            if (board.at(i) > board.at(j)) {
                numInversions++;
            }
        }
    }
    if (numInversions % 2 == 0) {
        solvable = true;
    } else {
        solvable = false;
    }

    return solvable;
}

void parseFile(std::string filePath) {
    gameState_t init;
    init.lastMove = ' ';
    init.parent = NULL;
    init.isValid = true;
    int i = 0, j = 0;
    int fn;
    std::ifstream file(filePath);
    std::string line;
    char mychar;
    if (file.is_open()) {
        while (file && i < 3) {
            while (file && j < 3) {
                mychar = file.get();
                if (mychar == '1' || mychar == '2' || mychar == '3' ||
                    mychar == '4' || mychar == '5' || mychar == '6' ||
                    mychar == '7' || mychar == '8' || mychar == '_') {
                    init.board[i][j] = mychar;
                    j++;
                }
                if (mychar == '_') {
                    init.dashX = j - 1;
                    init.dashY = i;
                }
            }
            j = 0;
            i++;
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                std::cout << init.board[i][j] << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Unable to open file." << std::endl;
        return;
    }

    isSolvable = findSolvable(init);

    fn = calcHn(init);
    init.fn = fn;

    gameQueue.push(init);
    gameStack.push(init);
    prioQueue.push(init);
    numNodes++;
}

int main(int argc, char *argv[]) {
    int i;
    int long_arg;
    int solutionsIndex;
    isSolvable = true;
    chosenAlg = 0;  // 1 == BFS, 2 == IDS, 3 == h1, 4 == h2, 5 == h3
    goalString = boardToString(goalBoard);
    tStart = clock();

    if (argc > 1) {
        for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
            if (argv[i][0] == '-') {
                if (argv[i][1] == '-') {
                    argv[i]++;
                    long_arg = 1;
                }
                if (strcmp(argv[i], "-fPath") == 0) {
                    if (strcmp(argv[i + 1], "--alg") == 0) {
                        std::cout << "Error: No file location provided."
                                  << std::endl;
                        return 1;
                    }
                    if (i + 1 < argc) {
                        fileLoc = argv[i + 1];
                        i++;
                    } else {
                        std::cout << "Error: No file location provided."
                                  << std::endl;
                        return 1;
                    }
                } else if (strcmp(argv[i], "-alg") == 0) {
                    if (i + 1 < argc) {
                        if ((strcmp(argv[i + 1], "BFS") == 0) ||
                            ((strcmp(argv[i + 1], "bfs") == 0))) {
                            chosenAlg = 1;
                        } else if ((strcmp(argv[i + 1], "IDS") == 0) ||
                                   ((strcmp(argv[i + 1], "ids") == 0))) {
                            chosenAlg = 2;
                        } else if (strcmp(argv[i + 1], "h1") == 0) {
                            chosenAlg = 3;
                        } else if (strcmp(argv[i + 1], "h2") == 0) {
                            chosenAlg = 4;
                        } else if (strcmp(argv[i + 1], "h3") == 0) {
                            chosenAlg = 5;
                        } else {
                            std::cout << "Invalid input (Valid: "
                                         "BFS,bfs,IDS,ids,h1,h2,h3)"
                                      << std::endl;
                            return 1;
                        }
                    }
                } else {
                    printf("Unknown option: %s\n", argv[i]);
                    return 1;
                }
            }
        }
    }

    printf("File location: %s\n", fileLoc.c_str());

    parseFile(fileLoc);

    if (!isSolvable) {
        std::cout << "Puzzle is not solvable" << std::endl;
        return 1;
    }

    switch (chosenAlg) {
        case BFS:
            bfs();
            break;
        case IDS:
            ids();
            break;
        case H1:
        case H2:
        case H3:
            astar();
            break;
    }
    std::cout << "Total nodes generated: " << numNodes << "\n";

    if (((double)(clock() - tStart) / CLOCKS_PER_SEC) >= 900) {
        std::cout << "Total time taken: >15 min" << std::endl;
        std::cout << "Path length: Timed out." << std::endl
                  << "Path: Timed out" << std::endl;

        return 0;
    }
    printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);

    if (solutions.size() == 0) {
        std::cout << "No solution found" << std::endl;
        return 1;
    }
    /* TODO
    ids if u arent dogshit
    */

    solutionsIndex = 0;
    for (i = 0; i < solutions.size(); i++) {
        if (solutions[solutionsIndex].solutionPath.size() >
            solutions[i].solutionPath.size()) {
            solutionsIndex = i;
        }
    }

    std::cout << "Path length: "
              << solutions.at(solutionsIndex).solutionPath.size() << std::endl;
    std::cout << "Path: ";
    for (i = 0; i < solutions.at(solutionsIndex).solutionPath.size(); i++) {
        std::cout << solutions.at(solutionsIndex).solutionPath.at(i);
    }
    std::cout << std::endl;

    return 0;
}