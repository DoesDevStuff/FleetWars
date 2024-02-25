#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <cmath>
#include <chrono>
#include "AI_Board.h"
#include "RandomNumberGeneration_helper.h"

using namespace std;
class MCTS {
private:
    AI_Board* ai_board;
    const double EXPLORATION_CONSTANT = 1.0 / (2 * sqrt(2.0));

    struct Node {
        int visits;
        double score;
        pair<int, int> action;
        Node* parent;
        vector<Node*> children;

        Node(pair<int, int> act, Node* par) : visits(0), score(0.0), action(act), parent(par), children() {}
    };

    Node* rootNode;

    // Methods for MCTS
    Node* selection(Node* node);
    Node* expansion(Node* node);
    double simulation(Node* node);
    void backpropagation(Node* node, double result);
    void deleteTree(Node* node);

public:
    MCTS(AI_Board* grid) : ai_board(grid), rootNode(nullptr) {}
    ~MCTS() { deleteTree(rootNode); }

    // Method to select the best shot using MCTS within the time limit
    pair<int, int> selectBestShotWithMCTS();
};

/*
 * Calculate UCT value:
 * https://www.chessprogramming.org/UCT
 *
 * Uct =  Xi + sqrt((C * ln( n )) / ni)
 *		where
 *		Xi is the win ratio of the child
 *		n is the number of times the parent has been visited
 *		ni is the number of times the child has been visited
 *		C is a constant to adjust the amount of exploration and incorporates the sqrt(2) from the UCB1 formula
 */
MCTS::Node* MCTS::selection(MCTS::Node* node) {
    // Select child node with highest UCB score
    double maxUCB = -1.0;
    Node* selected = nullptr;

    for (Node* child : node->children) {
        double ucb = child->score / child->visits + (EXPLORATION_CONSTANT * (sqrt(2 * log(node->visits) / child->visits) ));
        if (ucb > maxUCB) {
            maxUCB = ucb;
            selected = child;
        }
    }

    return selected;
}

MCTS::Node* MCTS::expansion(MCTS::Node* node) {
    // Add a new child node by selecting a legal action
    vector<pair<int, int>> legalActions;

    // Find all legal actions (empty cells) on the grid
    for (int i = 0; i < ai_board->getBoardDifficultySize(); ++i) {
        for (int j = 0; ai_board->getBoardDifficultySize(); ++j) {
            if (ai_board->getBoardCell(i, j) == '~') {
                legalActions.push_back({i, j});
            }
        }
    }

    // Randomly select one of the legal actions
    if (!legalActions.empty()) {
        int randomIndex = RandomNumberGeneration_helper::nextInt(legalActions.size());
        auto action = legalActions[randomIndex];

        // Create a new child node for the selected action
        Node* child = new Node(action, node);
        node->children.push_back(child);
        return child;
    }

    return nullptr;
}

double MCTS::simulation(MCTS::Node* node) {
    // Simulate a game from the current node
    int x = node->action.first;
    int y = node->action.second;

    // Fire at the selected cell and return the result (1 for hit, 0 for miss)
    bool playerHit;
    ai_board->fire(x, y, playerHit);
    return playerHit ? 1.0 : 0.0;
}

void MCTS::backpropagation(MCTS::Node* node, double result) {
    // update information at all nodes
    while (node != nullptr) {
        node->visits++;
        node->score += result;
        node = node->parent;
    }
}

void MCTS::deleteTree(MCTS::Node* node) {
    if (node != nullptr) {
        for (Node* child : node->children) {
            deleteTree(child);
        }
        delete node;
    }
}

pair<int, int> MCTS::selectBestShotWithMCTS() {
    // Create root node if it does not exist
    if (rootNode == nullptr) {
        rootNode = new Node({-1, -1}, nullptr);
    }

    // Initialize time values
    auto startTime = chrono::steady_clock::now();
    auto endTime = startTime + chrono::milliseconds(200); // Max time of 200 milliseconds

    // Perform MCTS iterations until the time limit is reached
    while (chrono::steady_clock::now() < endTime) {
        // Perform MCTS iterations
        Node* node = rootNode;

        // traverse till child leaf is reached
        while (!node->children.empty()) {
            node = selection(node);
        }

        // add new child node
        Node* newChild = expansion(node);
        if (newChild != nullptr) {

            double result = simulation(newChild);
            backpropagation(newChild, result);
        }
    }

    // Select the best action based on MCTS results and return it
    double maxScore = -1.0;
    pair<int, int> bestAction = {-1, -1};
    for (Node* child : rootNode->children) {
        double averageScore = child->score / child->visits;
        if (averageScore > maxScore) {
            maxScore = averageScore;
            bestAction = child->action;
        }
    }

    return bestAction;
}
