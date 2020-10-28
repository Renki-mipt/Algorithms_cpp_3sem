#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int LongestCommonPrefix(std::string& s, std::vector<int>& positions) {
  int n = s.size();
  std::vector<long long> LCP(n - 1);
  std::vector<int> IndexPos(n);
  for (int i = 0; i < n; i++) IndexPos[positions[i]] = i;
  int MinPref = 0;
  for (int i = 0; i < n; i++) {
    if (IndexPos[i] == n - 1) {
      MinPref = 0;
      continue;
    }
    MinPref = std::max(0, MinPref - 1);
    int j = i + MinPref;
    int k = positions[IndexPos[i] + 1] + MinPref;
    while (j < n && k < n && s[j] == s[k]) {
      ++MinPref;
      ++j;
      ++k;
    }
    LCP[IndexPos[i]] = MinPref;
  }
  long long LCPSum = 0;
  for (int i = 0; i < n - 1; ++i) LCPSum += LCP[i];
  long long NumberDifSubstr = (n - 1) * (n - 2) / 2 + n - 1 - LCPSum;
  return NumberDifSubstr;
}

void NewWeights(std::vector<int>& DopPositions, std::vector<int>& weight,
                std::vector<int>& DopWeight, int CurSize) {
  int n = weight.size();
  int w = 0;
  for (int i = 0; i < n; i++) {
    if (i != 0) {
      int pos = (DopPositions[i] + CurSize) % n;
      int pos2 = (DopPositions[i - 1] + CurSize) % n;
      if (weight[DopPositions[i]] != weight[DopPositions[i - 1]] ||
          weight[pos] != weight[pos2])
        w++;
    }
    DopWeight[DopPositions[i]] = w;
  }
}

void NewPositions(std::vector<int>& CountSort, std::vector<int>& DopPositions,
                  std::vector<int>& weight, std::vector<int>& PrevPositions) {
  int n = weight.size();
  int count = 0;
  for (int i = 0; i < n; i++) {
    count += CountSort[i];
    CountSort[i] = count - CountSort[i];
  }
  for (int i = 0; i < n; i++) {
    DopPositions[CountSort[weight[PrevPositions[i]]]] = PrevPositions[i];
    CountSort[weight[PrevPositions[i]]]++;
  }
}

void Preparation(std::vector<int>& positions, std::vector<int>& weight,
                 std::string& s) {
  int n = s.size();
  std::vector<std::pair<char, int>> help;
  for (int i = 0; i < n; i++) help.push_back(std::make_pair(s[i], i));
  std::sort(help.begin(), help.end());
  for (int i = 0; i < n; i++) {
    positions.push_back(help[i].second);
    if (help[i].first == '@')
      weight[help[i].second] = 0;
    else
      weight[help[i].second] = help[i].first - 'a' + 1;
  }
}

int solution(std::string& s) {
  s += '@';
  long long n = s.size();
  std::vector<int> positions;  // хранит текущий порядок суффиксов
  std::vector<int> weight(n, 0);  // веса суффиксов от 1 до n
  Preparation(positions, weight, s);
  std::vector<int> WPP(n, 0);  // weight of privious part
  std::vector<int> PrevPositions;  // хранит номер предыдущего для positions[i]
  std::vector<int> DopPositions(n, 0);  // новый positions
  std::vector<int> DopWeight(n, 0);
  std::vector<int> CountSort(n, 0);  // хранит веса, котрые надо распределить по суффиксам
  for (int i = 0; i < n; i++) positions.push_back(i);
  int CurSize = 1;
  while (CurSize <= n) {
    PrevPositions.resize(0);
    for (int i = 0; i < n; ++i) {
      WPP[i] = 0;
      CountSort[i] = 0;
      DopPositions[i] = 0;
      DopWeight[i] = 0;
    }
    for (int i = 0; i < n; i++) {
      int pos = (positions[i] - CurSize + n) % n;
      PrevPositions.push_back(pos);
      WPP[i] = weight[pos];
      CountSort[weight[pos]]++;
    }
    NewPositions(CountSort, DopPositions, weight, PrevPositions);
    NewWeights(DopPositions, weight, DopWeight, CurSize);
    CurSize *= 2;
    for (int i = 0; i < n; i++) {
      weight[i] = DopWeight[i];
      positions[i] = DopPositions[i];
    }
  }
  return LongestCommonPrefix(s, positions);
}

int main() {
  std::ifstream fin("input.txt");
  std::string s;
  fin >> s;
  for (int i = 0; i < s.size(); ++i) {
    if (s[i] > 'z' || s[i] < 'a') std::cout << s[s.size()];
  }
  long long ans = solution(s);
  std::cout << ans;
  return 0;
}