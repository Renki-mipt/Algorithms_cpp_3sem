#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

long long find_palindrom(std::vector<char>& str) {
  int l = 0, r = 0;
  long long count = 0;
  std::vector<int> z(str.size(), 0);
  for (int i = 1; i < str.size(); ++i) {
    if (r > i) z[i] = std::min(r - i, z[r + l - i]);
    while (i + z[i] < str.size() - 1 && i - z[i] > 0 &&
           str[i - z[i] - 1] == str[i + z[i] + 1])
      ++z[i];
    if (i + z[i] > r) {
      r = i + z[i];
      l = i - z[i];
    }
    count += z[i] / 2;
  }
  return count;
}

int main() {
  std::string s;
  std::cin >> s;
  std::vector<char> str(1, '*');
  for (int i = 0; i < s.size(); ++i) {
    str.push_back(s[i]);
    str.push_back('*');
  }
  std::cout << find_palindrom(str);
  return 0;
}
