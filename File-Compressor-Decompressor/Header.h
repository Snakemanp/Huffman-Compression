#pragma once
#include<map>
#include<queue>
#include<fstream>
#include<iostream>
#include<string>
#include<bitset>
using namespace std;
namespace Compressor {
	class block {
	public:
		char cnt;
		unsigned long int freq;
		const block* l = nullptr;
		const block* r = nullptr;
		block(char c, unsigned long int f) : cnt(c), freq(f) {}

		block(const block& a) : cnt(a.cnt), freq(a.freq) {
			if (a.l) l = a.l;
			if (a.r) r = a.r;
		}
		~block() {
			if (l) delete l;
			if (r) delete r;
		}
	};
	class Compare {
	public:
		bool operator()(const block* a, const block* b) {
			return a->freq > b->freq;
		}
	};

	long long int freq_caliculator(ifstream& file, map<char, unsigned long int> &freq) {
		long long int count=0;
		if (!file.is_open()) return 0;
		file.seekg(0);
		char ch;
		while (file.get(ch)) {
			freq[ch]++;
			count++;
		}
		return count;
	}
	void huffman_tree(map<char, unsigned long int>& freq, priority_queue<block*, vector<block*>, Compare>& minheap) {
		for (auto i = freq.begin(); i != freq.end(); i++) {
			minheap.push(new block(i->first, i->second));
		}
		freq.clear();
		if (minheap.size() == 0) return;
		unsigned long int a = 0;
		do {
			a = 0;
			block* encoding = new block(0, 0);
			encoding->l = minheap.top();
			a += minheap.top()->freq;
			minheap.pop();
			if (minheap.size() != 0) {
				encoding->r = minheap.top();
				a += minheap.top()->freq;
				minheap.pop();
			}
			encoding->freq = a;
			minheap.push(encoding);
		} while (minheap.size() > 1);
		return;
	}
	void huffman_code(const block* root,map<char,string> &codes,string current_code="") {
		if (!root) return;
		if (!root->l && !root->r) {
			codes[root->cnt] = current_code;
			cout << root->cnt << ' ' << current_code << '\n';
		}
		if (root->l) huffman_code(root->l, codes, current_code + '0');
		if (root->r) huffman_code(root->r, codes, current_code + '1');
	}
	void write_char(const string& code, char& c1, int& c1_len, ofstream& file,bool fill) {
		for (int i = 0; i < code.length(); i++) {
			c1 <<= 1;
			c1_len++;
			if (code[i] == '1') {
				c1 |= 1;
			}
			if (c1_len == sizeof(char)*8) {
				file.write(&c1, sizeof(char));
				c1 = 0;
				c1_len = 0;
			}
		}
		if (fill && c1_len>0) {
			for (int i = c1_len; i < sizeof(char)*8; i++) {
				c1 <<= 1;
			}
			file.write(&c1, sizeof(char));
			c1_len = 0;
			c1 = 0;
		}
	}
	void write_codes(map<char, string>& codes, ifstream& infile, long long int count, string file = "Compressed.bin") {
		ofstream outfile(file, ios::binary);
		char c1=0;
		int c1_len=0;
		if (!outfile.is_open()) {
			cout << "Error opening output file!" << endl;
			return;
		}
		
		size_t numChars = codes.size();
		outfile.write(reinterpret_cast<const char*>(&numChars), sizeof(size_t));
		outfile.write(reinterpret_cast<const char*>(&count), sizeof(long long int));

		for (const auto& pair : codes) {
			char character = pair.first;
			const string& huffmanCode = pair.second;

			// Write character followed by its Huffman code length (as a byte)
			outfile.write(&character, sizeof(char));
			size_t codeLength = huffmanCode.length();
			outfile.write(reinterpret_cast<const char*>(&codeLength), sizeof(size_t));

			// Write Huffman code as bits
			write_char(huffmanCode, c1, c1_len, outfile, true);
		}
		
		char ch;
		infile.seekg(0);

		while (infile.get(ch)) {
			const string code = codes[ch];
			write_char(code, c1, c1_len, outfile, false);
		}

		for (int i = c1_len; i < sizeof(char)*8; i++) {
			c1 <<= 1;
		}
		outfile.write(&c1, sizeof(char));
		c1_len = 0;
		c1 = 0;

		outfile.close();
	}
	void Compress(string file) {
		long long int count;
		ifstream infile(file+".txt");
		map<char, unsigned long int> freq;
		count = freq_caliculator(infile, freq);
		priority_queue<block*, vector<block*>, Compare> minheap;
		huffman_tree(freq, minheap);
		map<char, string> codes;
		huffman_code(minheap.top(), codes);
		minheap.pop();
		infile.close();
		infile.open(file + ".txt");
		write_codes(codes, infile, count, file+".bin");
		infile.close();
		codes.clear();
	}
}

namespace Decompressor {
	class block {
	public:
		char cnt;
		block * l, * r;
		block(char c) : cnt(c) {
			l = nullptr;
			r = nullptr;
		}
		~block() {
			if (l) delete l;
			if (r) delete r;
		}
	};
	long long int read_codes(ifstream& infile, map<char, string>& codes,streampos &pos) {
		infile.seekg(0);
		size_t size = 0;
		infile.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		long long int count;
		infile.read(reinterpret_cast<char*>(&count), sizeof(long long int));
		for (int i = 0; i < size; i++) {
			char ch;
			size_t code_len;
			infile.read(&ch, sizeof(char));
			infile.read(reinterpret_cast<char*>(&code_len), sizeof(size_t));
			string code = "";
			while (code_len > 0) {
				char t;
				infile.read(&t, sizeof(char));
				for (int j = 7; j >= 0 && code_len > 0; j--, code_len--) {
					bool bit = (t >> j) & 1;
					if (bit) code += '1';
					else code += '0';
				}
			}
			codes[ch] = code;
			cout << ch << ' ' << code << '\n';
		}
		pos = infile.tellg();
		return count;
	}
	void make_block(block* &root, const string& code, char c) {
		block* ptr;
		if (!root) root = new block(char(0));
		ptr = root;
		for (int i = 0; i < code.length(); i++) {
			if (code[i] == '1') {
				if (!ptr->r) ptr->r = new block(char(0));
				ptr = ptr->r;
			}
			if (code[i] == '0') {
				if (!ptr->l) ptr->l = new block(char(0));
				ptr = ptr->l;
			}
		}
		ptr->cnt = c;
		return;
	}
	void make_tree(map<char, string>& codes, block* &root) {
		if (root) delete(root);
		root = nullptr;
		for (const pair<char,string> &i : codes) {
			make_block(root, i.second, i.first);
		}
		codes.clear();
	}
	void print_tree(block* ptr, string current_code="") {
		if (!ptr->l && !ptr->r) {
			cout << ptr->cnt << ' ' << current_code << '\n';
			return;
		}
		print_tree(ptr->l, current_code + '0');
		print_tree(ptr->r, current_code + '1');
	}
	void print_char(bool bit, block*& root, block*& ptr, ofstream& outfile, long long int& count) {
		if (count == 0) return;
		if (!ptr) ptr = root;
		if (bit) ptr = ptr->r;
		else ptr = ptr->l;
		if (!ptr->l && !ptr->r) {
			outfile.write(&(ptr->cnt), sizeof(char));
			cout << ptr->cnt;
			ptr = root;
			count--;
		}
	}

	void print_content(ifstream& infile, block*& root, long long count, streampos& pos, string file = "Output.txt") {
		char ch;
		block* ptr = root;
		infile.seekg(pos);
		ofstream ofile(file, ios::binary);
		while (infile.get(ch)) {
			for (int i = 7; i >= 0; i--) {
				bool bit = (ch >> i) & 1;
				print_char(bit, root, ptr, ofile, count);
				if (count == 0) break;
			}
			if (count == 0) break;
		}
		ofile.close();
		infile.close();
	}
	void Decompress(string file) {
		long long int count;
		map<char, string> codes;
		ifstream infile;
		infile.open(file + ".bin", ios::binary);
		streampos pos;
		Decompressor::block* root = nullptr;
		count = Decompressor::read_codes(infile, codes, pos);
		Decompressor::make_tree(codes, root);
		Decompressor::print_content(infile, root, count, pos,file+"_.txt");
		codes.clear();
	}
}