#include <iostream>
#include <vector>
int main() 
{
    std::vector<unsigned char> str1;
    std::vector<unsigned char> str2(10);
    std::vector<unsigned char> str3(10)
    //str1.at(3) = 'f';
    //str1.at(4) = '1';
    str2.at(8) = 'f';

    for (char i: str2) {
        std::cout << i;
    } 
    std::cout << "Hello world!";
    return 0;
}