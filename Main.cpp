#include <cmath>
#include <unordered_map>

#include "Graph.h"
#include "API.h"

const int MAZE_SIZE = 16; // Configure maze dimesnions here (ONLY CNAGE THIS VALUE)
const int GOAL_L = MAZE_SIZE / 2 - 1;
const int GOAL_R = MAZE_SIZE / 2;
const int START_X = 0, START_Y = 0;

enum Direction { NORTH, EAST, SOUTH, WEST };

Direction myRotation = NORTH;

int oldX = 0;
int oldY = -1;

Graph graph;

struct cell {
    int x, y;
    double f, g, h;

    cell(int x = 0, int y = 0, double f = 0, double g = 0, double h = 0) 
        : x(x), y(y), f(f), g(g), h(h) {}
};

cell current;

void log(const std::string& text) {
    std::cerr << text << std::endl;
}

void printCell(cell C){
    std::string str = " x: " + std::to_string(C.x) + " y: " + std::to_string(C.y)
    + " f: " + std::to_string(C.f) + " g: " + std::to_string(C.g) + " h: " + std::to_string(C.h) 
     + " - "+ std::to_string(myRotation);
    log(str);
}

struct CompareF {
    bool operator()(const cell& a, const cell& b) {
        if (a.f == b.f) {
            return a.h > b.h; // Compare h values if f values are tied
        }
        return a.f > b.f; // Otherwise, compare f values
    }
};


int calculateHeuristic(int x, int y) {
    // The four center cells of the 16x16 maze
    std::vector<std::pair<int, int>> goals = {{GOAL_L, GOAL_L}, {GOAL_L, GOAL_R}, {GOAL_R, GOAL_L}, {GOAL_R, GOAL_R}};
    int minDist = std::abs(x - goals[0].first) + std::abs(y - goals[0].second);
    for (int i = 1; i < goals.size(); i++) {
        int dist = std::abs(x - goals[i].first) + std::abs(y - goals[i].second);
        if (dist < minDist) minDist = dist;
    }
    return minDist;
}

int convertCords(int x, int y){
    return x * MAZE_SIZE + y;
}

bool checkSurroundings(std::pair<int, int> dir){

    switch(myRotation) {
            case NORTH:
                if ((dir.first == 1 && !API::wallFront()) ||
                    (dir.first == -1) ||
                    (dir.second == -1 && !API::wallLeft()) ||
                    (dir.second == 1 && !API::wallRight())) return true;
                else return false;
                break;
            case SOUTH:
                if ((dir.first == 1) ||
                    (dir.first == -1 && !API::wallFront()) ||
                    (dir.second == -1 && !API::wallRight()) ||
                    (dir.second == 1 && !API::wallLeft())) return true;
                else return false;
                break;
            case EAST:
                if ((dir.first == 1 && !API::wallLeft()) ||
                    (dir.first == -1 && !API::wallRight()) ||
                    (dir.second == -1) ||
                    (dir.second == 1 && !API::wallFront())) return true;
                else return false;
                break;
            case WEST:
                if ((dir.first == 1 && !API::wallRight()) ||
                    (dir.first == -1 && !API::wallLeft()) ||
                    (dir.second == -1 && !API::wallFront()) ||
                    (dir.second == 1)) return true;
                else return false;
                break;
        }
    return false;
}

bool isGoal(int x, int y) {
    return (x == GOAL_L || x == GOAL_R) && (y == GOAL_L || y == GOAL_R);
}

bool isValid(int x, int y) {
    return (x >= 0 && x < MAZE_SIZE && y >= 0 && y < MAZE_SIZE);
}

bool trueNeighbour(int x, int y, int endX, int endY){
    std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, -1}, {0, 1}};
    for (auto dir : directions) {
        int newX = x + dir.first;
        int newY = y + dir.second;
        if(newX == endX && newY == endY && checkSurroundings(dir)) return true;
    }
    return false;  
}

void moveOne(int newX, int newY, int currentX, int currentY){
    int dx = newX - currentX;
    int dy = newY - currentY;

    if (dx == 1) { //N
        switch(myRotation){
            case NORTH:
                API::moveForward();
                break;
            case SOUTH:
                API::turnLeft();
                API::turnLeft();
                API::moveForward();
                break;
            case EAST:
                API::turnLeft();
                API::moveForward();
                break;
            case WEST:
                API::turnRight();
                API::moveForward();
                break;
        }
        myRotation = NORTH;
    } 
    else if (dx == -1) { //S
        switch(myRotation){
            case NORTH:
                API::turnLeft();
                API::turnLeft();
                API::moveForward();
                break;
            case SOUTH:
                API::moveForward();
                break;
            case EAST:
                API::turnRight();
                API::moveForward();
                break;
            case WEST:
                API::turnLeft();
                API::moveForward();
                break;
        }
        myRotation = SOUTH;
    } 
    else if (dy == 1) { // E
        switch(myRotation){
            case NORTH:
                API::turnRight();
                API::moveForward();
                break;
            case SOUTH:
                API::turnLeft();
                API::moveForward();
                break;
            case EAST:
                API::moveForward();
                break;
            case WEST:
                API::turnLeft();
                API::turnLeft();
                API::moveForward();
                break;
        }
        myRotation = EAST;
    } 
    else if (dy == -1) { // W
        switch(myRotation){
            case NORTH:
                API::turnLeft();
                API::moveForward();
                break;
            case SOUTH:
                API::turnRight();
                API::moveForward();
                break;
            case EAST:
                API::turnLeft();
                API::turnLeft();
                API::moveForward();
                break;
            case WEST:
                API::moveForward();
                break;
        }
        myRotation = WEST;
    }
}

void moveTo(int newX, int newY, int currentX, int currentY) {

    if(newX == 0 && newY == 0) return;

    int dist = std::abs(newX - currentX) + std::abs(newY- currentY);

    //log(std::to_string(dist));

    if(!trueNeighbour(currentX, currentY, newX, newY)){
        
        

        //Find a shortest path
        std::vector<int> path = graph.bfs_path(convertCords(currentX, currentY), convertCords(newX, newY));

        //Printing backtrack path
        // for(auto x : path){
        //     std::cerr << "-> " << x << " ";
        // }

        while(path.size() > 1){
            int from = path.back();
            path.pop_back();
            int to = path.back();
            moveOne(to/MAZE_SIZE, to%MAZE_SIZE, from/MAZE_SIZE, from%MAZE_SIZE);
        }
    }
    else{
        moveOne(newX, newY, currentX, currentY);
    }
}

void aStarMazeSolver() {

    
    std::priority_queue<cell, std::vector<cell>, CompareF> openList;
    std::unordered_map<int, bool> closedList;
    
    

    // Start position
    int startX = START_X, startY = START_Y;
    cell startCell(startX, startY, 0.0, 0.0, calculateHeuristic(startX, startY));

    //printCell(startCell);

    openList.push(startCell);
    
    int x, y;

    while (!openList.empty()) {

        
        current = openList.top();
        openList.pop();

        

        printCell(current);

        x = current.x;
        y = current.y;

        if (isGoal(x, y)) {
            moveTo(x, y, oldX, oldY);
            std::string str = "Goal reached at (" + std::to_string(x) + ", " + std::to_string(y) + ")\n";
            log(str);
            return;
        }

        // Mark cell as visited
        closedList[x * MAZE_SIZE + y] = true;

        // Move robot to current cell
        moveTo(x, y, oldX, oldY);
        
        // Explore neighbors (forward, backward, left, right)
        std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, -1}, {0, 1}};

        for (auto dir : directions) {
            int newX = x + dir.first;
            int newY = y + dir.second;

            //printSet(newX, newY);
            if (isValid(newX, newY) && !closedList[newX * MAZE_SIZE + newY]) {

                if (checkSurroundings(dir)) {

                    double gNew = current.g + 1.0;  // Assume each move costs 1
                    double hNew = calculateHeuristic(newX, newY);
                    double fNew = gNew + hNew;

                    cell neighbor(newX, newY, fNew, gNew, hNew);
                    graph.add_edge(convertCords(newX, newY), convertCords(x, y));
                    //graph.print();
                    // log("Open list:");
                    // printCell(neighbor);
                    openList.push(neighbor);
                }
            }
        }
        log("____________________________________________________________________");
        oldX = x;
        oldY = y;
    }
    std::cout << "No path found to the goal.\n";
}

int main() {
    aStarMazeSolver();
    return 0;
}