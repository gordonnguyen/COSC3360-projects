#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <pthread.h>
#include <pthread.h>
#include <unistd.h>

#include <queue>
#include <unordered_map>
#include <map>

//// Huffman Tree source: https://gist.github.com/pwxcoo/72d7d3c5c3698371c21e486722f9b34b
//

// A Tree node
struct Node
{
	char ch;
	int freq;
	Node *left, *right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// Comparison object to be used to order the heap
struct comp
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

// Traverse the Huffman Tree and store Huffman Codes
// in a map.
void encode(Node* root, std::string str,
			std::unordered_map<char, std::string> &huffmanCode)
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

// Traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int &index, std::string str, char &letter)
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

// Print all huffman codes from left to right
void inorderPrint(struct Node *root, const std::unordered_map<char, std::string> &huffmanCode) {
   if (root != NULL) {
      	inorderPrint(root->left, huffmanCode);
		if (root->ch != '\0') {
     		std::cout << "Symbol: " << root->ch <<", "
			<< "Frequency: " << root->freq << ", "
			<< "Code: " << huffmanCode.at(root->ch) << std::endl;
		}
      	inorderPrint(root->right, huffmanCode);
   }
}

// Argument to decode
struct decodeArg {
    std::string huffCode;
    std::vector<int> pos;
    Node *root;
    std::vector<char> *msgPTR;
};

// Thread function to decode message
void *decodeMessage(void *decodeVoidPtr) {
    struct decodeArg *arg = (struct decodeArg *)decodeVoidPtr;

    // decode letter from huffcode
    char letter;
    int index = -1;
    decode(arg->root, index, arg->huffCode, letter);

    // assign letter to correct position in a vector
    for (auto pos: arg->pos) {
        arg->msgPTR->at(pos) = letter;
    }

    return nullptr;
}


// Builds Huffman Tree and decode given input text
//
int main ()
{
    std::string test;
    int num;
    char letter;
    char charNum;
    std::map<char, int> freq;
    
    std::ofstream fout;
    std::ifstream fin;
    
    std::string filename;
    //std::cin >> filename;
    filename = "input2.txt";
    fin.open(filename);

    // Read and count frequency of appearance of each character
	// and store it in a map
    std::string line;
    int strSize = 0;    // Count number of characters for message
    int letterCount = 0; // Count number of letters for threads number
    while (getline(fin, line)) {
        letter = line.at(0);
        charNum = line.at(2);
        num = int(charNum) - 48;  // Convert from char to int using ASCII value
        strSize += num;
        freq[letter] = num;
        letterCount++;
    }
    fin.close(); // Close file


    // Create a priority queue to store live nodes of
	// Huffman tree;
	std::priority_queue<Node*, std::vector<Node*>, comp> pq;

	// Create a leaf node for each character and add it
	// to the priority queue.
	for (auto pair: freq) {
		pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
	}

	// do till there is more than one node in the queue
	while (pq.size() != 1)
	{
		// Remove the two nodes of highest priority
		// (lowest frequency) from the queue
		Node *left = pq.top(); pq.pop();
		Node *right = pq.top();	pq.pop();

		// Create a new internal node with these two nodes
		// as children and with frequency equal to the sum
		// of the two nodes' frequencies. Add the new node
		// to the priority queue.
		int sum = left->freq + right->freq;
		pq.push(getNode('\0', sum, left, right));
	}


	// root stores pointer to root of Huffman Tree
	Node* root = pq.top();

	// traverse the Huffman Tree and store Huffman Codes
	// in a map.
	std::unordered_map<char, std::string> huffmanCode;
	encode(root, "", huffmanCode);

    // Print letters and its huffman Codes and frequency in inorder traversal
    inorderPrint(root, huffmanCode);

    // Initilize threads and argument arrays
    std::vector<char> decodedMSG(strSize);  // Set vector of decoded letters to get decoded message
    pthread_t *tid = new pthread_t[letterCount];
    decodeArg *arg = new decodeArg[letterCount];

    // Get and open file name
    //std::cin >> filename;
    filename = "compressed2.txt";
    fin.open(filename);

    // Using string stream to store variables
    std::istringstream iss;
    std::string huffCode;
    int position;
    int index = 0;
    // Read line by line then input information to decode argument struct
    while(getline(fin, line)) {
        iss.str(line);
        iss >> huffCode;
        arg[index].huffCode = huffCode;
        while(iss >> position) {    
            arg[index].pos.push_back(position);
        }

        arg[index].msgPTR = &decodedMSG; // Point to decodedMSG
        arg[index].root = root;     // Point to root node
        index++;

        iss.clear(); // Clear string stream for next line
    }

    // Create threads to decode each letter
    for (int i = 0; i < letterCount; i++)
    {
        if (pthread_create(&tid[i], NULL, decodeMessage, &arg[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    // Join threads
    for (int i = 0; i < letterCount; i++) {
        pthread_join(tid[i], NULL);
    }
    
    // Print decoded message
    std::cout << "Original message: ";
    for (auto letter: decodedMSG) {
       std::cout << letter;
    }
    std::cout << std::endl;

    return 0;
}

