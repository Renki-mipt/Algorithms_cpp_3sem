#include <iostream>
#include <algorithm>
#include <vector>

void OccurrencesPinZ(const std::string& s, const std::string& p, const std::vector<int>& z)
{
	int r = 0;
	int l = 0;
	int s_len = s.size();
	int p_len = p.size();
	int substr_len = 0;
	for (int i = 0; i < s_len; ++i) {
		if (r >= i)
		{
			substr_len = std::min(r - i + 1, z[i - l]);
		}
		while (i + substr_len < s_len && substr_len < p_len && p[substr_len] == s[substr_len + i])
		{
			++substr_len;
		}
		if (r < i + substr_len - 1)
		{
			l = i;
			r = i + substr_len - 1;
		}
		if (substr_len == p_len)
		{
			std::cout << i << ' ';
		}
		substr_len = 0;
	}
}


void ZFunctionOfP(std::string& s, std::string& p)
{
	int p_len = p.length();
	int l = 0;
	int r = 0;
	std::vector<int> z(p_len);
	z.push_back(0);
	for (int i = 1; i < p_len; ++i) {
		if (r >= i)
		{
			z[i] = std::min(r - i + 1, z[i - l]);
		}
		while (i + z[i] < p_len && p[z[i]] == p[z[i] + i])
		{
			z[i]++;
		}
		if (r < i + z[i] - 1)
		{
			l = i;
			r = i + z[i] - 1;
		}
	}
	OccurrencesPinZ(s, p, z);
}

int main()
{
	std::string s;
	std::string p;
	std::cin >> p >> s;
	ZFunctionOfP(s, p);
	return 0;
}


