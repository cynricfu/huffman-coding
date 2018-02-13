#include <iostream>
#include "huffman.h"
using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "Usage:\n\t huffmanDecoding inputfile outputfile" << endl;
		exit(1);
	}
	huffman h(argv[1], argv[2]);
	h.recreate_huffman_tree();
	h.decoding_save();
	cout << endl;
	return 0;
}
