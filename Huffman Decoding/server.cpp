// A simple server in the internet domain using TCP
// The port nu1mber is passed as an argument

// Please note this is a C program
// It compiles without warnings with gcc

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#include <fstream>
#include <string>
#include <sstream>

#include <queue>
#include <unordered_map>
#include <map>

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

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


int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman); 
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding";
        exit(1);
    }

    // INPUT FILE
    std::string test;
    int num;
    char letter;
    char charNum;
    std::map<char, int> freq;

    std::string line;
    int strSize = 0;    // Count number of characters for message
    int letterCount = 0; // Count number of letters for threads number
    while (getline(std::cin, line)) {
        letter = line.at(0);
        charNum = line.at(2);
        num = int(charNum) - 48;  // Convert from char to int using ASCII value
        strSize += num;
        freq[letter] = num;
        letterCount++;
    }


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


    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            if (newsockfd < 0)
            {
                std::cerr << "ERROR on accept";
                exit(1);
            }
            int size;
            n = read(newsockfd, &size, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            char *buffer = new char[size+1];
            bzero(buffer, size+1);
            n = read(newsockfd, buffer, size);
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            std::cout << "Size: " << size << std:: endl;
            std::cout << "Name of the student that knows how to use sockets: " << buffer << std::endl;
            char message[] = "You got a 100!";
            int sMessage = strlen(message);
            n = write(newsockfd, &sMessage, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, message, sMessage);
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            close(newsockfd);
            delete[] buffer;
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}
