#include <cstdlib>
#include <iostream>

#include "huffman.h"

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage:\n\t huffmanDecoding inputfile outputfile" << std::endl;
		return EXIT_FAILURE;
	}

	huffman h(argv[1], argv[2]);
	if (!h.recreate_huffman_tree())
		return EXIT_FAILURE;
	if (!h.decoding_save())
		return EXIT_FAILURE;

	std::cout << "Decompression finished." << std::endl;
	return EXIT_SUCCESS;
}
