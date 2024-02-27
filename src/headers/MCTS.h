#ifndef MCTS_H
#define MCTS_H

#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <cmath>
#include <chrono>
#include "../headers/AI_Board.h"
#include "../headers/RandomNumberGeneration_helper.h"

class MCTS {
private:
    struct Node {
        int visits;
        double score;
        std::pair<int, int> action;
        Node* parent;
        std::vector<Node*> children;

        Node(std::pair<int, int> act, Node* par) : visits(0), score(0.0), action(act), parent(par), children() {}
    };

    AI_Board* ai_board;
    const double EXPLORATION_CONSTANT = 1.0 / (2 * sqrt(2.0));
    Node* rootNode;

    Node* selection(Node* node);
    Node* expansion(Node* node);
    double simulation(Node* node);
    void backpropagation(Node* node, double result);
    void deleteTree(Node* node);

public:
    MCTS(AI_Board* grid);
    ~MCTS();

    std::pair<int, int> selectBestShotWithMCTS();
};

#endif /* MCTS_H */
