#include "huffman.h"

#include <iostream>

void huffman::create_node_array()
{
	for (int i = 0; i < 128; i++)
	{
		node_array[i] = new huffman_node;
		node_array[i]->id = static_cast<unsigned char>(i);
		node_array[i]->freq = 0;
	}
}

void huffman::destroy_tree(node_ptr node)
{
	if (!node)
		return;
	destroy_tree(node->left);
	destroy_tree(node->right);
	delete node;
}

huffman::huffman(const std::string& in, const std::string& out)
	: root(nullptr), in_file_name(in), out_file_name(out)
{
	create_node_array();
}

huffman::~huffman()
{
	if (root)
	{
		destroy_tree(root);
		for (int i = 0; i < 128; i++)
		{
			if (!node_array[i]->freq)
				delete node_array[i];
		}
	}
	else
	{
		for (int i = 0; i < 128; i++)
			delete node_array[i];
	}
}

void huffman::traverse(node_ptr node, const std::string& code)
{
	if (!node)
		return;
	if (node->left == nullptr && node->right == nullptr)
		node->code = code;
	else
	{
		traverse(node->left, code + '0');
		traverse(node->right, code + '1');
	}
}

int huffman::binary_to_decimal(const std::string& in)
{
	int result = 0;
	for (size_t i = 0; i < in.size(); i++)
		result = result * 2 + (in[i] - '0');
	return result;
}

std::string huffman::decimal_to_binary(int in)
{
	std::string temp;
	std::string result;
	while (in)
	{
		temp += static_cast<char>('0' + in % 2);
		in /= 2;
	}
	result.append(8 - temp.size(), '0');
	for (int i = static_cast<int>(temp.size()) - 1; i >= 0; i--)
		result += temp[static_cast<size_t>(i)];
	return result;
}

void huffman::build_tree(const std::string& path, unsigned char a_code)
{
	node_ptr current = root;
	for (size_t i = 0; i < path.size(); i++)
	{
		if (path[i] == '0')
		{
			if (current->left == nullptr)
				current->left = new huffman_node;
			current = current->left;
		}
		else if (path[i] == '1')
		{
			if (current->right == nullptr)
				current->right = new huffman_node;
			current = current->right;
		}
	}
	current->id = a_code;
}

bool huffman::create_pq()
{
	in_file.open(in_file_name.c_str(), std::ios::in | std::ios::binary);
	if (!in_file)
	{
		std::cerr << "Error: cannot open input file: " << in_file_name << std::endl;
		return false;
	}

	unsigned char ch = 0;
	while (in_file.read(reinterpret_cast<char*>(&ch), 1))
	{
		if (ch >= 128)
		{
			std::cerr << "Error: input contains non-ASCII byte (>= 128): 0x"
			          << std::hex << static_cast<unsigned>(ch) << std::dec << std::endl;
			in_file.close();
			return false;
		}
		node_array[ch]->freq++;
	}

	in_file.close();

	for (int i = 0; i < 128; i++)
	{
		if (node_array[i]->freq)
			pq.push(node_array[i]);
	}
	return true;
}

bool huffman::create_huffman_tree()
{
	if (pq.empty())
		return false;

	std::priority_queue<node_ptr, std::vector<node_ptr>, compare> temp(pq);
	if (temp.size() == 1)
	{
		root = temp.top();
		return true;
	}

	while (temp.size() > 1)
	{
		root = new huffman_node;
		root->freq = 0;
		root->left = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		root->right = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		temp.push(root);
	}
	return true;
}

void huffman::calculate_huffman_codes()
{
	if (pq.size() == 1)
	{
		root->code = "0";
		return;
	}
	traverse(root, "");
}

bool huffman::coding_save()
{
	in_file.open(in_file_name.c_str(), std::ios::in | std::ios::binary);
	if (!in_file)
	{
		std::cerr << "Error: cannot open input file: " << in_file_name << std::endl;
		return false;
	}

	out_file.open(out_file_name.c_str(), std::ios::out | std::ios::binary);
	if (!out_file)
	{
		std::cerr << "Error: cannot open output file: " << out_file_name << std::endl;
		in_file.close();
		return false;
	}

	std::string in;
	std::string s;
	const unsigned char symbol_count = static_cast<unsigned char>(pq.size());
	in += static_cast<char>(symbol_count);

	std::priority_queue<node_ptr, std::vector<node_ptr>, compare> temp(pq);
	while (!temp.empty())
	{
		node_ptr current = temp.top();
		in += static_cast<char>(current->id);
		s.assign(127 - current->code.size(), '0');
		s += '1';
		s.append(current->code);
		in += static_cast<char>(binary_to_decimal(s.substr(0, 8)));
		for (int i = 0; i < 15; i++)
		{
			s = s.substr(8);
			in += static_cast<char>(binary_to_decimal(s.substr(0, 8)));
		}
		temp.pop();
	}
	s.clear();

	unsigned char ch = 0;
	while (in_file.read(reinterpret_cast<char*>(&ch), 1))
	{
		s += node_array[ch]->code;
		while (s.size() > 8)
		{
			in += static_cast<char>(binary_to_decimal(s.substr(0, 8)));
			s = s.substr(8);
		}
	}

	const int count = static_cast<int>(8 - s.size());
	if (s.size() < 8)
		s.append(static_cast<size_t>(count), '0');
	in += static_cast<char>(binary_to_decimal(s));
	in += static_cast<char>(count);

	out_file.write(in.c_str(), static_cast<std::streamsize>(in.size()));
	in_file.close();
	out_file.close();
	return true;
}

bool huffman::recreate_huffman_tree()
{
	in_file.open(in_file_name.c_str(), std::ios::in | std::ios::binary);
	if (!in_file)
	{
		std::cerr << "Error: cannot open input file: " << in_file_name << std::endl;
		return false;
	}

	unsigned char size = 0;
	if (!in_file.read(reinterpret_cast<char*>(&size), 1))
	{
		std::cerr << "Error: compressed file is empty or truncated." << std::endl;
		in_file.close();
		return false;
	}

	if (root)
	{
		destroy_tree(root);
		root = nullptr;
	}
	root = new huffman_node;

	for (int i = 0; i < size; i++)
	{
		unsigned char a_code = 0;
		unsigned char h_code_c[16];
		if (!in_file.read(reinterpret_cast<char*>(&a_code), 1) ||
			!in_file.read(reinterpret_cast<char*>(h_code_c), 16))
		{
			std::cerr << "Error: truncated code table in compressed file." << std::endl;
			in_file.close();
			return false;
		}

		std::string h_code_s;
		for (int k = 0; k < 16; k++)
			h_code_s += decimal_to_binary(h_code_c[k]);

		size_t j = 0;
		while (j < h_code_s.size() && h_code_s[j] == '0')
			j++;
		if (j < h_code_s.size())
			h_code_s = h_code_s.substr(j + 1);
		else
			h_code_s.clear();

		build_tree(h_code_s, a_code);
	}

	in_file.close();
	return true;
}

bool huffman::decoding_save()
{
	in_file.open(in_file_name.c_str(), std::ios::in | std::ios::binary);
	if (!in_file)
	{
		std::cerr << "Error: cannot open input file: " << in_file_name << std::endl;
		return false;
	}

	out_file.open(out_file_name.c_str(), std::ios::out | std::ios::binary);
	if (!out_file)
	{
		std::cerr << "Error: cannot open output file: " << out_file_name << std::endl;
		in_file.close();
		return false;
	}

	unsigned char size = 0;
	if (!in_file.read(reinterpret_cast<char*>(&size), 1))
	{
		std::cerr << "Error: compressed file is empty or truncated." << std::endl;
		in_file.close();
		out_file.close();
		return false;
	}

	in_file.seekg(-1, std::ios::end);
	unsigned char count0 = 0;
	if (!in_file.read(reinterpret_cast<char*>(&count0), 1))
	{
		std::cerr << "Error: compressed file is truncated." << std::endl;
		in_file.close();
		out_file.close();
		return false;
	}

	in_file.seekg(static_cast<std::streamoff>(1 + 17 * size), std::ios::beg);

	std::vector<unsigned char> text;
	unsigned char textseg = 0;
	while (in_file.read(reinterpret_cast<char*>(&textseg), 1))
		text.push_back(textseg);

	if (text.empty())
	{
		std::cerr << "Error: no encoded data in compressed file." << std::endl;
		in_file.close();
		out_file.close();
		return false;
	}

	node_ptr current = root;
	for (size_t i = 0; i + 1 < text.size(); i++)
	{
		std::string path = decimal_to_binary(text[i]);
		if (i == text.size() - 2)
			path = path.substr(0, 8 - count0);

		for (size_t j = 0; j < path.size(); j++)
		{
			if (path[j] == '0')
			{
				if (!current->left)
				{
					std::cerr << "Error: invalid Huffman code in compressed data." << std::endl;
					in_file.close();
					out_file.close();
					return false;
				}
				current = current->left;
			}
			else
			{
				if (!current->right)
				{
					std::cerr << "Error: invalid Huffman code in compressed data." << std::endl;
					in_file.close();
					out_file.close();
					return false;
				}
				current = current->right;
			}

			if (current->left == nullptr && current->right == nullptr)
			{
				out_file.put(static_cast<char>(current->id));
				current = root;
			}
		}
	}

	in_file.close();
	out_file.close();
	return true;
}
