#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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

class Bohr {
 public:
  void add_string(const std::string& s, int l, int r, int flag);
  int go(int vertex, char c);
  int GetSufLink(int vertex);
  int GetUp(int vertex);
  const std::vector<int>& get_flags(int vertex);
  void AddFlagToVertex(int vertex, int flag);
  Bohr() { bohr.push_back(Node(-1)); }

 private:
  std::vector<Node> bohr;
};

void Bohr::add_string(const std::string& s, int l, int r, int flag) {
  int vertex = 0;
  int size = bohr.size();
  for (int i = l; i < r; ++i) {
    int num = s[i] - 'a';
    if (bohr[vertex].next[num] == -1) {
      bohr[vertex].next[num] = size;
      bohr.push_back(Node(-1));
      bohr[size].prev = vertex;
      bohr[size++].edge = s[i];
    }
    vertex = bohr[vertex].next[num];
  }

  bohr[vertex].flags.push_back(flag);
}

int Bohr::GetSufLink(int vertex) {
  if (bohr[vertex].link == -1)
    if (vertex != 0 && bohr[vertex].prev != 0) {
      bohr[vertex].link = go(GetSufLink(bohr[vertex].prev), bohr[vertex].edge);
    } else {
      bohr[vertex].link = 0;
    }
  return bohr[vertex].link;
}

int Bohr::go(int vertex, char c) {
  int num = c - 'a';
  if (bohr[vertex].go[num] == -1)
    if (bohr[vertex].next[num] == -1) {
      bohr[vertex].go[num] = vertex != 0 ? go(GetSufLink(vertex), c) : 0;
    } else {
      bohr[vertex].go[num] = bohr[vertex].next[num];
    }
  return bohr[vertex].go[num];
}

int Bohr::GetUp(int vertex) {
  if (bohr[vertex].up == -1)
    if (bohr[GetSufLink(vertex)].flags.size() > 0) {
      bohr[vertex].up = GetSufLink(vertex);
    } else {
      if (GetSufLink(vertex) == 0) {
        bohr[vertex].up = 0;
      } else {
        bohr[vertex].up = GetUp(GetSufLink(vertex));
      }
    }
  return bohr[vertex].up;
}

const std::vector<int>& Bohr::get_flags(int vertex) {
  return bohr[vertex].flags;
}

void Bohr::AddFlagToVertex(int vertex, int flag) {
  bohr[vertex].flags.push_back(flag);
}

void decision(std::string& text, std::string& pattern,
              std::vector<int>& answer) {
  Bohr B;
  std::vector<int> PatNum(text.size());
  int l = 0;
  int r = 0;
  int CountFragments = 0;
  while (r != pattern.size()) {
    if (pattern[r] == '?') {
      if (l < r) {
        CountFragments++;
        B.add_string(pattern, l, r, r - 1);
      }
      l = r + 1;
    }
    ++r;
  }
  if (l < r) {
    CountFragments++;
    B.add_string(pattern, l, r, r - 1);
  }
  int vertex = 0;
  int vertex2 = 0;
  for (int i = 0; i < text.size(); ++i) {
    char c = text[i];
    vertex = B.go(vertex, c);
    vertex2 = vertex;
    while (vertex2 != 0) {
      const std::vector<int>& flags = B.get_flags(vertex2);
      for (int flag : flags) {
        if (i - flag >= 0) {
          PatNum[i - flag]++;
        }
      }
      vertex2 = B.GetUp(vertex2);
    }
  }
  for (int i = 0; i < int(text.size()) - int(pattern.size()) + 1; ++i) {
    if (PatNum[i] == CountFragments) {
    
      answer.push_back(i);
    }
  }
}

int main() {
  std::ifstream in("input.txt");
  std::vector<int> answer;
  std::string pattern;
  std::string text;
  in >> pattern >> text;
  decision(text, pattern, answer);
  for (int i : answer) {
    std::cout << i << ' ';
  }
  return 0;
}
