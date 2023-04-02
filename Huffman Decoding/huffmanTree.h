// Write the definition of the huffmanTree class here.
#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

#include "huffmanTree.cpp"

class HuffmanTree {
public:
    struct Node;
    struct comp;
    Node* getNode(char ch, int freq, Node* left, Node* right);
    void encode(Node* root, string str, unordered_map<char, string> &huffmanCode);
    void decode(Node* root, int &index, string str, char &letter);
    void inorderPrint(struct Node *root, const unordered_map<char, string> &huffmanCode);
private:
};

#endif