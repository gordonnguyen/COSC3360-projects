#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

#include <pthread.h>
#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

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

// traverse the Huffman Tree and store Huffman Codes
// in a map.
void encode(Node* root, string str,
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
void decode(Node* root, int &index, string str, char &letter)
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

void inorderPrint(struct Node *root, const unordered_map<char, string> &huffmanCode) {
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

struct decodeArg {
    std::string huffCode;
    vector<int> pos;
    Node *root;
    vector<char> *msgPTR;
};




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
int main ()
{
    std::string test;
    int num;
    char letter;
    char charNum;
    map<char, int> freq;
    //std::string text = "CACACADBD";
    std::ofstream fout;
    std::ifstream fin;
    
    std::string filename; // = "input2.txt";
    std::cin >> filename;
    fin.open(filename);

    /*
    if (fin.is_open()){
                std::cout << "Open success" << std::endl;
    }
    else {
        std::cout << "Open failed" << std::endl;
        return 1;
    }
    */

    //getline(fin, test);
    //std::cout << test;

    // Read and count frequency of appearance of each character
	// and store it in a map
    std::string line;
    int strSize = 0;
    int letterCount = 0; // Count number of letters for threads number
    while (getline(fin, line)) {
        //std::cout << line << std::endl;
        letter = line.at(0);
        charNum = line.at(2);
        num = int(charNum) - 48;  // Convert from char to int using ASCII value
        //std::cout << "Frequency: "  << num << std::endl;
        strSize += num;
        freq[letter] = num;
        letterCount++;
    }
    fin.close(); // Close file


    // Create a priority queue to store live nodes of
	// Huffman tree;
	priority_queue<Node*, vector<Node*>, comp> pq;

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
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

    // Print letters and its huffman Codes and frequency in inorder traversal
    inorderPrint(root, huffmanCode);

    // Initilize threads and argument arrays
    vector<char> decodedMSG(strSize);  // Set vector of decoded letters to get decoded message
    pthread_t *tid = new pthread_t[letterCount];
    decodeArg *arg = new decodeArg[letterCount];

    // Get file name
    //filename = "compressed2.txt";
    cin >> filename;

    std::string huffCode;
    int position;
    fin.open(filename);

    /*
    if (fin.is_open()){
        std::cout << "Open success" << std::endl;
    }
    else {
        std::cout << "Open failed" << std::endl;
        return 1;
    }
    */

    std::istringstream iss;

    int index = 0;

    // Read line by line then input each number to decode argument
    while(getline(fin, line)) {
        //std::cout << line << endl;
        iss.str(line);
        iss >> huffCode;
        //std::cout << "huffCode: " << huffCode << endl;
        arg[index].huffCode = huffCode;
        while(iss >> position) {    
            //std::cout << "Pos:" << position << " ";
            arg[index].pos.push_back(position);
        }

        arg[index].msgPTR = &decodedMSG; // Point to decodedMSG
        arg[index].root = root;
        //std::cout << endl;
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
    std::cout << endl;

    return 0;
}

// Write the implementation of the member functions of the huffmanTree class here.

// Huffman Tree source: https://gist.github.com/pwxcoo/72d7d3c5c3698371c21e486722f9b34b


