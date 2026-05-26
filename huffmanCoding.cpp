#include <cstdlib>
#include <iostream>

#include "huffman.h"

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage:\n\t huffmanCoding inputfile outputfile" << std::endl;
		return EXIT_FAILURE;
	}

	huffman h(argv[1], argv[2]);
	if (!h.create_pq())
		return EXIT_FAILURE;
	if (!h.create_huffman_tree())
	{
		std::cerr << "Error: input file is empty." << std::endl;
		return EXIT_FAILURE;
	}
	h.calculate_huffman_codes();
	if (!h.coding_save())
		return EXIT_FAILURE;

	std::cout << "Compression finished." << std::endl;
	return EXIT_SUCCESS;
}
