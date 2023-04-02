// Please note this is a C program
// It compiles without warnings with gcc
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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


// Argument to decode
struct decodeArg {
    std::string huffCode;
    std::vector<int> pos;
    std::vector<char> *msgPTR;
    int portno;
    char *serverArgv;
};

// Thread function to decode message
void *decodeMessage(void *decodeVoidPtr) {
    struct decodeArg *arg = (struct decodeArg *)decodeVoidPtr;

    // decode letter from huffcode
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = arg->portno;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        std::cerr << "ERROR opening socket";
    server = gethostbyname(arg->serverArgv);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting";
        exit(1);
    }

    char letter;
    int index = -1;

    char message[] = "Carlos Rincon";
    int sMessage = strlen(message);
    n = write(sockfd, &sMessage, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    n = write(sockfd, message, sMessage);
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }
    int size;
    n = read(sockfd, &size, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    n = read(sockfd, buffer, size);
    std::cout << buffer << std::endl;

    //Recieve letter from server
    //decode(arg->root, index, arg->huffCode, letter);

    // assign letter to correct position in a vector
    for (auto pos: arg->pos) {
        arg->msgPTR->at(pos) = letter;
    }

    return nullptr;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3)
    {
        std::cerr << "usage " << argv[0] << "hostname port\n";
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        std::cerr << "ERROR opening socket";
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting";
        exit(1);
    }



    std::map<std::string, std::vector<int>> huffMap;
    // Using string stream to store variables
    std::istringstream iss;
    std::string line;
    std::string huffCode;

    int letterCount = 0;
    int strSize;
    int position;
    int index = 0;

    // Read line by line then input information to map
    while(getline(std::cin, line)) {
        std::vector<int> posVector;
        iss.str(line);
        iss >> huffCode;
        while(iss >> position) {    
            posVector.push_back(position);
            if (position > strSize) {strSize = position;}
        }
        huffMap[huffCode] = posVector;
        letterCount++;
    }
    std::cout << letterCount;
    
    // Initilize threads and argument arrays
    std::vector<char> decodedMSG(strSize);  // Set vector of decoded letters to get decoded message
    pthread_t *tid = new pthread_t[letterCount];
    decodeArg *arg = new decodeArg[letterCount];

    // Move map into argument
    int index = 0;
    for (auto pair : huffMap) {
        arg[index].huffCode = pair.first;
        arg[index].pos = pair.second;
        arg[index].msgPTR = &decodedMSG; // Point to decodedMSG
        index++;
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


    delete [] buffer;
    close(sockfd);
    return 0;
}
