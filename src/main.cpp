#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "graphTheory.h"

int wildcardCountMax;
int wildCardCount;
std::vector<std::string> valid;
raven::graph::sGraphData gd;

int editDistance(
    const std::string &s,
    const std::string &e)
{
    int ed = 0;
    for (int i = 0; i < s.length(); i++)
        if (s[i] != e[i])
            ed++;
    return ed++;
}

bool isSane()
{
    int l = gd.startName.length();
    if (gd.endName.length() != l)
        return false;
    return true;
}

void wildCard(
    const raven::graph::path_cost_t &path,
    int i)
{
    wildCardCount++;
    if (wildCardCount > wildcardCountMax)
    {
        std::cout << "ran out of wild cards\n";
        exit(2);
    }

    // construct required wildcard
    auto name1 = gd.g.userName(path.first[i - 1]);
    auto name2 = gd.g.userName(path.first[i + 1]);
    int firstdiff;
    for (int c = 0; c < name1.size(); c++)
        if (name1[c] != name2[c])
        {
            firstdiff = c;
            break;
        }
    std::string wild = name1;
    wild[firstdiff] = name2[firstdiff];
    std::cout << " add wild " << wild << "\n";

    // add wildcard
    valid.insert(valid.begin(), wild);
}

bool Dijsktra()
{
    // find shortest path from start to end
    auto path = raven::graph::path(gd);
    if (path.second == -1)
    {
        std::cout << "no path available\n";
        exit(1);
    }

    // loop over path
    int prev = -1;
    for (int i = 0; i < path.first.size() - 1; i++)
    {
        // check if this step comes no closer to end
        auto srcname = gd.g.userName(path.first[i]);
        int src2end = editDistance(srcname, gd.endName);
        if (src2end == prev)
        {
            // add wildcard
            wildCard(path, i);

            // request restart
            return false;
        }
        prev = src2end;
        std::cout << srcname << "->";
    }
    std::cout << gd.endName << "\n";

    return true;
}

void validGraph()
{
    gd.g.clear();
    gd.edgeWeight.clear();
    for (int i = 0; i < valid.size(); i++)
        for (int j = i + 1; j < valid.size(); j++)
        {
            int ed = editDistance(valid[i], valid[j]);
            if (ed > 1)
                continue; // do not add links between words where edit distance > 1
            gd.g.add(valid[i], valid[j]);
            gd.edgeWeight.push_back(ed);
            gd.edgeWeight.push_back(ed);
        }
}

bool isPath()
{
    // check problem sanity
    if (!isSane())
        return false;

    // check if we can make a path with the available wild cards
    int ed = editDistance(gd.startName, gd.endName);
    if (ed < wildcardCountMax)
        return true;

    // add wildcards until Dijsktra can find a path from start to end
    wildCardCount = 0;
    while (!Dijsktra())
    {
        validGraph();
    }

    return false;
}

void gen1()
{
    wildcardCountMax = 4;
    gd.startName = "HEAD";
    gd.endName = "TAIL";
    valid = std::vector<std::string>{"HEAL", "TEAL", "TELL", "TALL"};
    valid.push_back(gd.startName);
    valid.push_back(gd.endName);
}

main()
{
    // generate sample problem
    gen1();

    // generate graph of valid words connected by edit distance of 1
    validGraph();

    // find ladder
    isPath();

    return 0;
}
