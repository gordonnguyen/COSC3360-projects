// Write your program here


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

#include <pthread.h>
#include <pthread.h>
#include <unistd.h>

#include "huffmanTree.cpp"

//void getCompressed();
void *decodeMessage();

struct decodeArg {
    std::string huffCode;
    vector<int> pos;
    Node *root;
    vector<char> *msgPTR;
};

// Builds Huffman Tree and decode given input text
int main ()
{
    std::string test;
    int num;
    char letter;
    map<char, int> freq;
    std::string text = "CACACADBD";
    std::ofstream fout;
    std::ifstream fin;
    
    std::string filename = "char.txt";
    //cin >> filename;
    fin.open(filename);

    if (fin.is_open()){
                std::cout << "Open success" << std::endl;
    }
    else {
        std::cout << "Open failed" << std::endl;
        return 1;
    }

    //getline(fin, test);
    //std::cout << test;

    // Read and count frequency of appearance of each character
	// and store it in a map
    int strSize = 0;
    int letterCount = 0; // Count number of letters for threading
    while (fin >> letter >> num) {
        strSize += num;
        freq[letter] = num;
        letterCount++;
        //std::cout << "loop end" << std::endl;
    }

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

	cout << "Huffman Codes are :\n" << '\n';
    /*
	for (auto pair: huffmanCode) {
		cout << pair.first << " " << pair.second << '\n';
	}
    */
    inorderPrint(root, huffmanCode);

    /*
	cout << "\nOriginal string was :\n" << text << '\n';

	// print encoded string
	string str = "";
	for (char ch: text) {
		str += huffmanCode[ch];
	}

	cout << "\nEncoded string is :\n" << str << '\n';
	// traverse the Huffman Tree again and this time
	// decode the encoded string
	int index = -1;
	cout << "\nDecoded string is: \n";
	while (index < (int)str.size() - 2) {
		decode(root, index, str);
	}
    */

    vector<char> decodedMSG(strSize);  // Set vector of decoded letters to get decoded message
    pthread_t *tid = new pthread_t[letterCount];
    decodeArg *arg = new decodeArg[letterCount];

    // Get file name
    std::string filename = "compressed.txt";
    //cin >> filename;

    std::ifstream fin;
    std::string line;
    std::string huffCode;
    int position;
    fin.open(filename);
    std::istringstream iss;

    int index = 0;

    // Read line by line then input each number to decode argument
    while( getline(fin, line) ) {
        std::cout << line << endl;
        iss.str(line);
        iss >> huffCode;
        std::cout << "huffCode: " << huffCode << endl;
        arg[index]->huffCode = huffCode;
        while(iss >> position) {    
            std::cout << "Pos:" << position << " ";
            arg[index]->pos.push_back(position);
        }

        arg[index]->msgPTR = &decodedMSG; // Point to decodedMSG
        arg[index]->root = root;
        std::cout << endl;
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

void *decodeMessage(void *decodeVoidPtr) {
    struct decodeArg *arg = (decodeArg *)decodeVoidPtr;

    // determine char from huffcode
    char letter;
    letter = decode(arg->root, -1, arg->huffCode);

    // store char on vector
    for (auto pos: arg.pos) {
        *arg->msgPTR.at(pos) = letter;
    }

    return nullptr;
}


/*
void getCompressed(decodeArg arg[]) {
    // Get Compressed file input

    std::string filename = "compressed.txt";
    std::map<std::string, vector<int>> HMCodePos;
    //cin >> filename;

    std::ifstream fin;
    std::string line;
    std::string huffCode;
    int position;
    fin.open(filename);
    std::istringstream iss;

    while( getline(fin, line) ) {
        std::cout << line << endl;
        iss.str(line);
        iss >> huffCode;
        std::cout << "huffCode: " << huffCode << endl;
        while(iss >> position) {    
            std::cout << "Pos:" << position << " ";
            HMCodePos[huffCode].push_back(position);
            
        }
        std::cout << endl;
        iss.clear();
    }
}
*/
