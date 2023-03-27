// Write the implementation of the member functions of the huffmanTree class here.

// Huffman Tree source: https://gist.github.com/pwxcoo/72d7d3c5c3698371c21e486722f9b34b
/#include "huffmanTree.h"

#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

// A Tree node
struct HuffmanTree::Node
{
	char ch;
	int freq;
	Node *left, *right;
};

// Function to allocate a new tree node
Node* HuffmanTree::getNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// Comparison object to be used to order the heap
struct HuffmanTree::comp
{
	bool operator()(Node* l, Node* r)
	{
		// highest priority item has lowest frequency
		if (l->freq == r->freq) {
			// Compare by ASCII Code
			if (l->ch == r->ch) {
				return l < r;
			}
			return l->ch > r->ch;
		}
		else {
			// Compare by frequency
			return l->freq > r->freq;
		}
	}
};

// traverse the Huffman Tree and store Huffman Codes
// in a map.
void HuffmanTree::encode(Node* root, string str,
			unordered_map<char, string> &huffmanCode)
{
	if (root == nullptr)
		return;

	// found a leaf node
	if (!root->left && !root->right) {
		huffmanCode[root->ch] = str;
	}

	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}

// traverse the Huffman Tree and decode the encoded string
void HuffmanTree::decode(Node* root, int &index, string str, char &letter)
{
	if (root == nullptr) {
		return;
	}

	// found a leaf node
	if (!root->left && !root->right)
	{
		letter = root->ch;
		return;
	}

	index++;

	if (str[index] =='0')
		decode(root->left, index, str, letter);
	else
		decode(root->right, index, str, letter);
}

void HuffmanTree::inorderPrint(struct Node *root, const unordered_map<char, string> &huffmanCode) {
   if (root != NULL) {
      	inorderPrint(root->left, huffmanCode);
		if (root->ch != '\0') {
     		cout << "Symbol: " << root->ch <<", "
			<< "Frequency: " << root->freq << ", "
			<< "Code: " << huffmanCode.at(root->ch) << endl;
		}
      	inorderPrint(root->right, huffmanCode);
   }
}