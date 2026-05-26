#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <fstream>
#include <queue>
#include <string>
#include <vector>

struct huffman_node
{
	unsigned char id;
	int freq;
	std::string code;
	huffman_node* left;
	huffman_node* right;
	huffman_node()
	{
		left = right = nullptr;
	}
};
typedef huffman_node* node_ptr;

class huffman
{
protected:
	node_ptr node_array[128];
	std::fstream in_file, out_file;
	node_ptr root;
	std::string in_file_name, out_file_name;

	class compare
	{
	public:
		bool operator()(const node_ptr& c1, const node_ptr& c2) const
		{
			return c1->freq > c2->freq;
		}
	};
	std::priority_queue<node_ptr, std::vector<node_ptr>, compare> pq;

	void create_node_array();
	void destroy_tree(node_ptr node);
	void traverse(node_ptr node, const std::string& code);
	int binary_to_decimal(const std::string& in);
	std::string decimal_to_binary(int in);
	void build_tree(const std::string& path, unsigned char a_code);

public:
	huffman(const std::string& in, const std::string& out);
	~huffman();
	bool create_pq();
	bool create_huffman_tree();
	void calculate_huffman_codes();
	bool coding_save();
	bool recreate_huffman_tree();
	bool decoding_save();
};

#endif
