// Write your program here

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "huffmanTree.cpp"

const int SIZE = 4;

void getCompressed();

// Builds Huffman Tree and decode given input text
int main ()
{
    vector<char> charList;
    vector<int> freqList;
    std::string test;

    std::ofstream fout;
    std::ifstream fin;
    
    std::string filename = "char.txt";
    //cin >> filename;
    fin.open(filename);
	// count frequency of appearance of each character
	// and store it in a map
    int i = 0;
    char letter;
    int freq;

    while (fin >> letter >> freq) {
        //std::cout << i << std::endl;
        //fin >> charList[i];
        //fin >> freqList[i];
        charList.push_back(letter);
        freqList.push_back(freq);
        //std::cout << charList[i] << " ";
        //std::cout << freqList[i] << std::endl;
        //std::cout << "loop end" << std::endl;
    /*
	for (char ch: text) {
		freq[ch]++;
	}
    
    freq['A'] = 3;
    freq['C'] = 3;
    freq['B'] = 1;
    freq['D'] = 2;
    */

    // Create a priority queue to store live nodes of
	// Huffman tree;
	priority_queue<Node*, vector<Node*>, comp> pq;

	// Create a leaf node for each character and add it
	// to the priority queue.
	for (int i = 0; i < charList.size(); i++) {
		pq.push(getNode(charList.at(i), freqList.at(i), nullptr, nullptr));
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
	// in a map. Also prints them
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	cout << "Huffman Codes are :\n" << '\n';
	for (auto pair: huffmanCode) {
		cout << pair.first << " " << pair.second << '\n';
	}

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
    return 0;
}

// Huffman coding algorithm
void mainHuff()
{
	string text = "CACACADBD";

	buildHuffmanTree(text);
}

/*
int oldMain() {

    char charList[SIZE];
    int freqList[SIZE];
    std::string test;

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

    fin >> charList[0];
    fin >> freqList[0];
    std::cout << charList[0] << " ";
    std::cout << freqList[0];
    //getline(fin, );


    // Read and Output sample
    /*
    int i = 0;
    while (fin >> charList[i] >> freqList[i]) {
        //std::cout << i << std::endl;
        //fin >> charList[i];
        //fin >> freqList[i];
        std::cout << charList[i] << " ";
        std::cout << freqList[i] << std::endl;
        i++;
        //std::cout << "loop end" << std::endl;
    }
    */
    mainHuff();
    //getCompressed();

    Node hufftree;

    return 0;
}


void getCompressed() {
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
