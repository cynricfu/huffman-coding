#include "huffman.h"

void huffman::create_node_array()
{
  for (int i = 0; i < 128; ++i)
  {
    node_array[i] = new huffman_node;
    node_array[i]->id = i;
    node_array[i]->freq = 0;
}
}

void huffman::transverse(node_ptr node, string code)
{
  if (node->left == NULL && node->right == NULL)
  {
    node->code = code;
  }
  else
  {
    transverse(node->left, code + '0');
    transverse(node->right, code + '1');
  }
}

int huffman::binary_to_decimal(string& in)
{
  int result = 0;
  for (int i = 0; i < in.size(); i++)
    result = result * 2 + in[i] - '0';
  return result;
}

string huffman::decimal_to_binary(int in)
{
  string temp = "";
  string result = "";
  while (in)
  {
    temp += ('0' + in % 2);
    in /= 2;
  }
  result.append(8 - temp.size(), '0');
  for (int i = temp.size() - 1; i >= 0; i--)
  {
    result += temp[i];
  }
  return result;
}

inline void huffman::build_tree(string& path, char a_code)
{//build a new branch according to the input code and ignore the already existing branches
  node_ptr current = root;
  for (int i = 0; i < path.size(); i++)
  {
    if (path[i] == '0')
    {
      if (current->left == NULL)
        current->left = new huffman_node;
      current = current->left;
    }
    else if (path[i] == '1')
    {
      if (current->right == NULL)
        current->right = new huffman_node;
      current = current->right;
    }
  }
  current->id = a_code;
}

huffman::huffman(string in, string out)
{
  in_file_name = in;
  out_file_name = out;
  create_node_array();
}
void huffman::create_pq()
{
  in_file.open(in_file_name, ios::in);
  in_file.get(id);
  while (!in_file.eof())
  {
    node_array[id]->freq++;
    in_file.get(id);
  }
  in_file.close();
  for (int i = 0; i < 128; i++)
  {
    if (node_array[i]->freq)
    {
      pq.push(node_array[i]);
    }
  }
}

void huffman::create_huffman_tree()
{
  priority_queue<node_ptr, vector<node_ptr>, compare> temp(pq);
  while (temp.size() > 1)
  {//create the huffman tree with highest frequency character being leaf from bottom to top
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
}

void huffman::calculate_huffman_codes()
{
  transverse(root, "");
}

void huffman::coding_save()
{
  in_file.open(in_file_name, ios::in);
  out_file.open(out_file_name, ios::out | ios::binary);
  string in = "", s = "", aux = "";

  in += (char)pq.size();
  priority_queue<node_ptr, vector<node_ptr>, compare> temp(pq);
  while (!temp.empty())
  {//get all characters and their huffman codes for output
    node_ptr current = temp.top();
    in += current->id;
    s.assign(127 - current->code.size(), '0');
    s += '1';
    s.append(current->code);
    aux = s.substr(0, 8);
    in += (char)binary_to_decimal(aux);
    for (int i = 0; i < 15; i++)
    {//cut into 8-bit binary codes that can convert into saving char needed for binary file
      s = s.substr(8);
      aux = s.substr(0, 8);
      in += (char)binary_to_decimal(aux);
    }
    temp.pop();
  }
  s.clear();

  in_file.get(id);
  while (!in_file.eof())
  {//get the huffman code
    s += node_array[id]->code;
    while (s.size() > 8)
    {//cut into 8-bit binary codes that can convert into saving char needed for binary
      aux = s.substr(0, 8);
      in += (char)binary_to_decimal(aux);
      s.substr(8);
    }
    in_file.get(id);
  }
  int count = 8 - s.size();
  if (s.size() < 8)
  {//append number of 'count' '0' to the last few codes to create the last byte of text
    s.append(count, '0');
  }
  in += (char)binary_to_decimal(s);
  in += (char)count;

  out_file.write(in.c_str(), in.size());
  in_file.close();
  out_file.close();
}

void huffman::recreate_huffman_tree()
{
  in_file.open(in_file_name, ios::in | ios::binary);
  unsigned char size;
  in_file.read(reinterpret_cast<char*>(&size), 1);
  root = new huffman_node;
  for (int i = 0; i < size; i++)
  {
    char a_code;
    unsigned char h_code_c[16];
    in_file.read(&a_code, 1);
    in_file.read(reinterpret_cast<char*>(h_code_c), 16);
    string h_code_s = "";
    for (int i = 0; i < 16; i++)
    {//obtain the original 128-bit binary string
      h_code_s += decimal_to_binary(h_code_c[i]);
    }
    int j = 0;
    while (h_code_s[j] == '0')
    {//delete the added '000¡¡1' to get the real huffman code
      j++;
    }
    h_code_s = h_code_s.substr(j+1);
    build_tree(h_code_s, a_code);
  }
  in_file.close();
}

void huffman::decoding_save()
{
  in_file.open(in_file_name, ios::in | ios::binary);
  out_file.open(out_file_name, ios::out);
  unsigned char size;
  in_file.read(reinterpret_cast<char*>(&size), 1);
  in_file.seekg(-1, ios::end);
  char count0;
  in_file.read(&count0, 1);
  in_file.seekg(1 + 17 * size, ios::beg);

  vector<unsigned char> text;
  unsigned char textseg;
  in_file.read(reinterpret_cast<char*>(&textseg), 1);
  while (!in_file.eof())
  {
    text.push_back(textseg);
    in_file.read(reinterpret_cast<char*>(&textseg), 1);
  }
  node_ptr current = root;
  string path;
  for (int i = 0; i < text.size() - 1; i++)
  {//translate the huffman code
    path = decimal_to_binary(text[i]);
    if (i == text.size() - 2)
      path = path.substr(0, 8 - count0);
    for (int j = 0; j < path.size(); j++)
    {
      if (path[j] == '0')
        current = current->left;
      else
        current = current->right;
      if (current->left == NULL && current->right == NULL)
      {
        out_file.put(current->id);
        current = root;
      }
    }
  }
  in_file.close();
  out_file.close();
}
