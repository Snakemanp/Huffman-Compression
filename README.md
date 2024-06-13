# Huffman Compression

## Introduction
This project provides a C++ implementation for compressing and decompressing files using the Huffman encoding algorithm. The application processes a file, encodes it into a binary format, and can also decode the compressed binary file back to its original form.

## Algorithm
The program employs the Huffman algorithm to encode information into a `.bin` file. The encoded file contains both the data and the Huffman coding information necessary for decompression.

## File Structure
The `.bin` file produced after compression follows this structure:

1. **Number of Different Characters**: Indicates how many unique characters are present in the file.
2. **Total Number of Characters**: The total count of characters in the data.
3. **Encoding Information**: This includes each character followed by its Huffman code length and the Huffman code itself (stored in bits).
4. **Encoded Data**: The actual data encoded using the Huffman codes.

## Limitations
1. The program works best with `.txt` files.
2. The frequency of bits is stored in `long int`.

## Usage
1. Ensure the file to be compressed or decompressed is in the same directory as the executable.
2. Run the program and follow the prompts to either compress or decompress the file.

## Compilation
To compile the program, use a C++ compiler (e.g., g++):
```sh
g++ -o huffman_compression main.cpp 
```
## Execution
```sh
./huffman_compression
