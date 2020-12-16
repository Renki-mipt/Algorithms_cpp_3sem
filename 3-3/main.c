#include <iostream>
#include <vector>

int  SpecialCases(int n, std::vector<int>& first_shot,
    bool is_first_shot, std::vector<int>& queue_len)
{
    if (n == 2)
    {
        if (is_first_shot)
        {
            first_shot.push_back(1);
            first_shot.push_back(2);
        }
        queue_len[2] = 1;
        return 1;
    }
    if (n == 3)
    {
        if (is_first_shot)
            first_shot.push_back(2);
        queue_len[3] = 2;
        return 2;
    }
    return -1;
}

int ShpragaGrandiFunction(int n, std::vector<int>& first_shot, 
    bool is_first_shot, std::vector<int>& queue_len, std::vector<int>& mex_vec)
{
    if (queue_len[n] != -1)
        return queue_len[n];
    int check = SpecialCases(n, first_shot, is_first_shot, queue_len);
    if (check != -1)
        return check;
    int index;
    for (int i = 1; i <= n; ++i)
    {
        if (i <= 2 || i >= n - 1)
            index = ShpragaGrandiFunction(std::max(n - i, i - 1), first_shot,
                0, queue_len, mex_vec);
        else
            index = ShpragaGrandiFunction(i - 1, first_shot,
                0, queue_len, mex_vec) ^ ShpragaGrandiFunction(n - i, first_shot,
                    0, queue_len, mex_vec);
        if (index < n)
        mex_vec[index] = n;
        if (is_first_shot && index == 0)
            first_shot.push_back(i);
    }
    for (int i = 0; i < n; ++i)
        if (mex_vec[i] != n)
        {
            queue_len[n] = i;
            return i;
        }
}


int main()
{
    int n = 0;
    std::cin >> n;
    std::vector<int> queue_len(n + 1, -1);
    std::vector<int> first_shot;
    std::vector<int> mex_vec(n);
    if (ShpragaGrandiFunction(n, first_shot, 1, queue_len, mex_vec) != 0)
    {
        std::cout << "Schtirlitz\n";
        for (int i = 0; i < first_shot.size(); ++i)
            std::cout << first_shot[i] << '\n';
    }
    else
        std::cout << "Mueller";
    return 0;
}

