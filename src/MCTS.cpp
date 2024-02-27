/*
 * MCTS.cpp
 *
 *  Created on: 27 Feb 2024
 *      Author: Charlie
 */

#include "../src/headers/MCTS.h"

MCTS::MCTS(AI_Board* aiBoard) {
    ai_board = aiBoard;
    rootNode = nullptr;
}

MCTS::~MCTS() {
    deleteTree(rootNode);
}

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
    double maxUCB = -1.0;
    Node* selected = nullptr;

    for (Node* child : node->children) {
        double ucb = child->score / child->visits + (EXPLORATION_CONSTANT * (sqrt(2 * log(node->visits) / child->visits)));
        if (ucb > maxUCB) {
            maxUCB = ucb;
            selected = child;
        }
    }

    return selected;
}

MCTS::Node* MCTS::expansion(MCTS::Node* node) {
    std::vector<std::pair<int, int>> legalActions;

    for (int i = 0; i < ai_board->getBoardDifficultySize(); ++i) {
        for (int j = 0; j < ai_board->getBoardDifficultySize(); ++j) {
            if (ai_board->getBoardCell(i, j) == '~') {
                legalActions.push_back({i, j});
            }
        }
    }

    if (!legalActions.empty()) {
        int randomIndex = RandomNumberGeneration_helper::nextInt(legalActions.size());
        auto action = legalActions[randomIndex];

        Node* child = new Node(action, node);
        node->children.push_back(child);
        return child;
    }

    return nullptr;
}

double MCTS::simulation(MCTS::Node* node) {
    int x = node->action.first;
    int y = node->action.second;

    bool playerHit;
    ai_board->fire(x, y, playerHit);
    return playerHit ? 1.0 : 0.0;
}

void MCTS::backpropagation(MCTS::Node* node, double result) {
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

std::pair<int, int> MCTS::selectBestShotWithMCTS() {
    if (rootNode == nullptr) {
        rootNode = new Node({-1, -1}, nullptr);
    }

    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::milliseconds(200);

    while (std::chrono::steady_clock::now() < endTime) {
        Node* node = rootNode;

        while (!node->children.empty()) {
            node = selection(node);
        }

        Node* newChild = expansion(node);
        if (newChild != nullptr) {
            double result = simulation(newChild);
            backpropagation(newChild, result);
        }
    }

    double maxScore = -1.0;
    std::pair<int, int> bestAction = {-1, -1};
    for (Node* child : rootNode->children) {
        double averageScore = child->score / child->visits;
        if (averageScore > maxScore) {
            maxScore = averageScore;
            bestAction = child->action;
        }
    }

    return bestAction;
}


