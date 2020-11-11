#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int get_symbol_number(char c) { 
    return c - 'a'; 
}

struct Node {
  std::vector<int> next;
  std::vector<int> go;
  std::vector<int> flags;
  int link;
  int up;
  int prev;
  char edge;
  Node(int flag) : next(26, -1), go(26, -1), link(-1), up(-1) {}
};

class Trie {
 public:
  void add_string(const std::string& s, int l, int r, int flag);
  int go(int vertex, char c);
  int get_suf_link(int vertex);
  int get_up(int vertex);
  const std::vector<int>& get_flags(int vertex);
  void add_flag_to_vertex(int vertex, int flag);
  Trie() { trie.push_back(Node(-1)); }
  void add_node(int vertex, char c);
 private:
  std::vector<Node> trie;
};

void Trie::add_node(int vertex, char c) {
  int size = trie.size();
  int symbol_number = get_symbol_number(c);
  if (trie[vertex].next[symbol_number] == -1) {
    trie[vertex].next[symbol_number] = size;
    trie.push_back(Node(-1));
    trie[size].prev = vertex;
    trie[size++].edge = c;
  }
}

void Trie::add_string(const std::string& s, int l, int r, int flag) {
  int vertex = 0;
  int size = trie.size();
  for (int i = l; i < r; ++i) {
    add_node(vertex, s[i]);
    vertex = trie[vertex].next[get_symbol_number(s[i])];
  }
  trie[vertex].flags.push_back(flag);
}

int Trie::get_suf_link(int vertex) {
  if (trie[vertex].link == -1)
    if (vertex != 0 && trie[vertex].prev != 0) {
      trie[vertex].link = go(get_suf_link(trie[vertex].prev), trie[vertex].edge);
    } else {
      trie[vertex].link = 0;
    }
  return trie[vertex].link;
}

int Trie::go(int vertex, char c) {
  int symbol_number = get_symbol_number(c);
  if (trie[vertex].go[symbol_number] == -1)
    if (trie[vertex].next[symbol_number] == -1) {
      trie[vertex].go[symbol_number] = vertex != 0 ? go(get_suf_link(vertex), c) : 0;
    } else {
      trie[vertex].go[symbol_number] = trie[vertex].next[symbol_number];
    }
  return trie[vertex].go[symbol_number];
}

int Trie::get_up(int vertex) {
  if (trie[vertex].up == -1)
    if (trie[get_suf_link(vertex)].flags.size() > 0) {
      trie[vertex].up = get_suf_link(vertex);
    } else {
      if (get_suf_link(vertex) == 0) {
        trie[vertex].up = 0;
      } else {
        trie[vertex].up = get_up(get_suf_link(vertex));
      }
    }
  return trie[vertex].up;
}

const std::vector<int>& Trie::get_flags(int vertex) {
  return trie[vertex].flags;
}

void Trie::add_flag_to_vertex(int vertex, int flag) {
  trie[vertex].flags.push_back(flag);
}

std::vector<std::string> Split(const std::string& string, char delimiter) {
  std::vector<std::string> substrings;
  int l = 0;
  int r = 0;
  while (r != string.size()) {
    if (string[r] == delimiter) {
      if (l < r) {
        substrings.push_back(string.substr(l, r - l));
      }
      l = r + 1;
    }
    ++r;
  }
  if (l < r) {
    substrings.push_back(string.substr(l, r - l));
  }
  return substrings;
}

void build_tree_with_substrs(Trie& bohr, int& count_fragments, 
    const std::string& pattern) {
  std::vector<std::string> substrings = Split(pattern, '?');
  int begin_of_substring = 0;
  for (int i = 0; i < substrings.size(); ++i) {
    int left = begin_of_substring;
    int right = begin_of_substring + substrings[i].size();
    bohr.add_string(substrings[i], left, right, right - 1);
  }
  count_fragments = substrings.size();
}


std::vector<int> occurrences_pattern_in_text(std::string& text, std::string& pattern) {
  Trie bohr;
  int count_fragments = 0;
  std::vector<int> pattern_number(text.size());
  int l = 0;
  int r = 0;
  while (r != pattern.size()) {
    if (pattern[r] == '?') {
      if (l < r) {
        ++count_fragments;
        bohr.add_string(pattern, l, r, r - 1);
      }
      l = r + 1;
    }
    ++r;
  }
  if (l < r) {
    ++count_fragments;
    bohr.add_string(pattern, l, r, r - 1);
  }
  int vertex = 0;
  int vertex2 = 0;
  for (int i = 0; i < text.size(); ++i) {
    char c = text[i];
    vertex = bohr.go(vertex, c);
    vertex2 = vertex;
    while (vertex2 != 0) {
      const std::vector<int>& flags = bohr.get_flags(vertex2);
      for (int flag : flags) {
        if (i - flag >= 0) {
          ++pattern_number[i - flag];
        }
      }
      vertex2 = bohr.get_up(vertex2);
    }
  }
  std::vector<int> answer;
  for (int i = 0; i < int(text.size()) - int(pattern.size()) + 1; ++i) {
    if (pattern_number[i] == count_fragments) {
      answer.push_back(i);
    }
  }
  return answer;
}

int main() {
  std::ifstream in("input.txt");
  std::vector<int> answer;
  std::string pattern;
  std::string text;
  in >> pattern >> text;
  answer = occurrences_pattern_in_text(text, pattern);
  for (int i : answer) {
    std::cout << i << ' ';
  }
  return 0;
}
