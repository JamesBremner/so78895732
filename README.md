# Problem

I am dealing with a variation of the game Word Ladder where a player must transform a starting string into an ending string of the same length different using only single letter substitutions to create valid strings (a list of valid strings are given).

For example, if the starting word is HEAD and the ending word is TAIL, HEAD -> HEAL -> TEAL -> TELL -> TALL -> TAIL would be the most optimal solution granted that all the strings were valid. I am also allowed to add some number of wilcard strings to the list of valid strings to find a better solution. For example, adding the wildcard string TAAL to the list of valid strings could shorten the solution to HEAD -> HEAL -> TEAL -> TAAL -> TAIL. Other strings can produce the minimum 4 move solution but the exact string doesn't matter.

# Algorithm

- Create a graph of all the valid words connected by a single character change
- Use Dijskra to find path from tart to end
- Loop over path until a hop does NOT reduce the edit distance to the end
- Construct wildcard that helps
- Add wild card to valid words
- Repeat until all hops reduce edit distance to end word

# Example Output
```
HEAD->HEAL->TEAL-> add wild TAAL
HEAD->HEAL->TEAL->TAAL->TAIL
```
