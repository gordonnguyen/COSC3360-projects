// Write your program here

#include <iostream>
#include <fstream>
#include <string>

const int SIZE = 4;

int main() {

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
/*
    fin >> charList[0];
    fin >> freqList[0];
    std::cout << charList[0] << " ";
    std::cout << freqList[0];
    //getline(fin, );

*/
    for (int i = 0; i < SIZE; ++i) {
        //std::cout << i << std::endl;
        fin >> charList[i];
        fin >> freqList[i];
        std::cout << charList[i] << " ";
        std::cout << freqList[i];
        //std::cout << "loop end" << std::endl;
    }

    return 0;
}


void get_compressed() {
    std::string filename = "compressed.txt";
    std::map<char, int>  
    int huffCode[SIZE];
    //cin >> filename;
    std::ifstream fin;
    fin.open(filename);
    for (int i = 0; i < SIZE; ++i) {
        //std::cout << i << std::endl;
        fin >> huffCode[i];
        getline(fin, );
        std::cout << charList[i] << " ";
        std::cout << freqList[i];
        //std::cout << "loop end" << std::endl;
    }

}
