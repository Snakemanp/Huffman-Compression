#include <iostream>
#include"Header.h"
#include<queue>
#include<map>
int main()
{
    std::cout << "Enter 1 for Encoding and 2 for Decoding\n(Enter):";
    char t;
    std::cin >> t;
    std::string file;
    if (t == '1') {
        std::cout << "Enter .txt file name\n(Without .txt):";
        cin >> file;
        Compressor::Compress(file);
    }
    else if (t == '2') {
        std::cout << "Enter .bin file name\n(Without .bin):";
        cin >> file;
        Decompressor::Decompress(file);
    }
    else {
        std::cout << "Invalid input exiting code\n";
    }
    return 0;
}
