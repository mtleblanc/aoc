#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ranges>

uint64_t part1(std::vector<std::string> &v) {
    uint64_t splits {0};
    std::string oldRow {v[0]};
    for(auto &cur : v | std::views::drop(1)) {
        std::string newRow {cur};
        for(size_t x = 0; x < v[0].size(); x ++) {
            if(oldRow[x] != 'S') continue;
            if(cur[x] == '.') newRow[x] = 'S';
            else {
                splits ++;
                if(x > 0) newRow[x-1] = 'S';
                if(x < cur.size() - 1) newRow[x+1] = 'S';
            }
        }
        oldRow = std::move(newRow);
    }
    return splits;
}

uint64_t part2(std::vector<std::string> &v) {
    std::vector<uint64_t> oldRow (v[0].size());
    size_t start = v[0].find('S');
    oldRow[start] = 1;
    for(auto &cur : v | std::views::drop(1)) {
        std::vector<uint64_t> newRow(oldRow.size());
        for(size_t x = 0; x < v[0].size(); x ++) {
            if(cur[x] == '.') newRow[x] += oldRow[x];
            else {
                if(x > 0) newRow[x-1]+= oldRow[x];
                if(x < cur.size() - 1) newRow[x+1] += oldRow[x];
            }
        }
        oldRow = std::move(newRow);
    }
    uint64_t accum {0};
    for(auto v : oldRow) accum += v;
    return accum;
}



int main()
{
    std::ifstream fs{"input07.txt"};
    std::vector<std::string> v;
    for (std::string s; getline(fs, s);)
    {
        if (s.length() == 0)
            break;
        v.emplace_back(s);
    }

    std::cout << part1(v) << std::endl;
    std::cout << part2(v) << std::endl;
}