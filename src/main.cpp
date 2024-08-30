#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "graphTheory.h"

class cWord
{
public:
    std::string word;       // the word
    bool fwild;             // true if wild word added to initial valid words
    int ed;                 // edit distance to final destination

    static std::vector<cWord> theWords; // the available words( valid and wild ) 

    cWord()
        : fwild(false),
          ed(-1)
    {
    }
    cWord(const std::string &w)
        : cWord()
    {
        word = w;
    }

    /// @brief edit distance between this word and another
    /// @param other 
    /// @return 
    
    int editDistance(const cWord &other) const
    {
        int ed = 0;
        for (int i = 0; i < word.length(); i++)
            if (word[i] != other.word[i])
                ed++;
        //std::cout << "ed " << ed <<" "<< word <<" "<< other.word << "\n";
        return ed;
    }

    /// @brief number of available words

    static int size()
    {
        return theWords.size();
    }

    /// @brief  check for availabe word
    /// @param w word to check for
    /// @return true if word is among available words

    static bool find( const std::string& w ) 
    {
        return ( std::find( theWords.begin(), theWords.end(), w ) != theWords.end() );
    }

    /// true if this and other word are the same
    bool operator==(const cWord& other) const
    {
        return ( word == other.word);
    }

    /// @brief set the initial available words
    /// @param v vector of word strengs
    static void set(const std::vector<std::string> &v)
    {
        theWords.clear();
        for (auto &w : v)
            theWords.emplace_back(w);
    }

    /// @brief edit distance betwen two word strings
    /// @param w1 
    /// @param w2 
    /// @return 

    static int editDistance( const std::string& w1, const std::string w2 )
    {
        cWord cw1( w1 );
        cWord cw2( w2 );
        return cw1.editDistance( cw2 );
    }
};

std::vector<cWord> cWord::theWords;

int wildcardCountMax;
int wildCardCount;
raven::graph::sGraphData gd;


bool isSane()
{
    int l = gd.startName.length();
    if (gd.endName.length() != l)
        return false;
    return true;
}

void wildCard(
    const std::string &name1,
    const std::string &name2)
{
    wildCardCount++;
    if (wildCardCount > wildcardCountMax)
    {
        std::cout << "ran out of wild cards\n";
        exit(2);
    }

    // construct required wildcard
    int firstdiff;
    for (int c = 0; c < name1.size(); c++)
        if (name1[c] != name2[c])
        {
            firstdiff = c;
            break;
        }
    std::string wild = name1;
    wild[firstdiff] = name2[firstdiff];

    if ( cWord::find( wild ) )
    {
        firstdiff = -1;
        for (int c = 0; c < name1.size(); c++)
            if (name1[c] != name2[c])
            {
                if (firstdiff == -1)
                {
                    firstdiff = 0;
                    continue;
                }
                firstdiff = c;
                break;
            }
        wild = name1;
        wild[firstdiff] = name2[firstdiff];
    }
    std::cout << " add wild " << wild
              << " between " << name1 << "," << name2
              << "\n";

    // add wildcard
    cWord::theWords.insert(cWord::theWords.begin(), cWord(wild));
}

void display(
    const raven::graph::path_cost_t &path)
{
    if( ! path.first.size() ) {
        std::cout << "No path found\n";
        return;
    }
    for (int i = 0; i < path.first.size() - 1; i++)
    {
        std::cout << gd.g.userName(path.first[i]) << "->";
    }
    std::cout << gd.endName << "\n";
}

void validGraph()
{
    gd.g.clear();
    gd.edgeWeight.clear();
    for (int i = 0; i < cWord::size(); i++)
        for (int j = i + 1; j < cWord::size(); j++)
        {
            int ed = cWord::theWords[i].editDistance( cWord::theWords[j]);
            if (ed > 1)
                continue; // do not add links between words where edit distance > 1
            gd.g.add(cWord::theWords[i].word,cWord::theWords[j].word);
            gd.edgeWeight.push_back(ed);
            gd.edgeWeight.push_back(ed);
        }
    int dbg = 1;
}

void noPath()
{
    std::vector<int> ved;
    for (auto &w : cWord::theWords)
    {
        if (w.word == gd.endName)
            continue;
        ved.push_back(cWord::editDistance(w.word, gd.endName));
    }
    std::sort(ved.begin(), ved.end());
    wildCard(cWord::theWords[ved[0]].word, gd.endName);
    validGraph();
    auto path = raven::graph::path(gd);
    display(path);
}

bool Dijsktra()
{
    // find shortest path from start to end
    auto path = raven::graph::path(gd);
    if (path.second == -1) 
    {
        std::cout << "no path available\n";
        noPath();
        exit(1);
    }

    // loop over path
    int prev = -1;
    for (int i = 0; i < path.first.size() - 1; i++)
    {
        // check if this step comes no closer to end
        auto srcname = gd.g.userName(path.first[i]);
        int src2end = cWord::editDistance(srcname, gd.endName);
        if (src2end == prev)
        {
            // add wildcard
            wildCard(
                cWord::theWords[path.first[i - 1]].word,
                cWord::theWords[path.first[i]].word);

            // request restart
            return false;
        }
        prev = src2end;
        std::cout << srcname << "->";
    }
    std::cout << gd.endName << "\n";

    return true;
}

bool isPath()
{
    // check problem sanity
    if (!isSane())
        return false;

    // check if we can make a path with the available wild cards
    int ed = cWord::editDistance(gd.startName, gd.endName);
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
    cWord::set({"HEAL", "TEAL", "TELL", "TALL"});
    cWord::theWords.emplace_back(gd.startName);
    cWord::theWords.emplace_back(gd.endName);
}
void gen2()
{
    wildcardCountMax = 4;
    gd.startName = "HEAD";
    gd.endName = "TAIL";
    cWord::set({"HEAL", "TELL", "TALL"});
    cWord::theWords.emplace_back(gd.startName);
    cWord::theWords.emplace_back(gd.endName);
}

main()
{
    // generate sample problem
    gen2();

    // generate graph of valid words connected by edit distance of 1
    validGraph();

    // find ladder
    isPath();

    return 0;
}
