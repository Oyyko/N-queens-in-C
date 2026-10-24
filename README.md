# N-queens-in-C
Solving N queens problem (Just find one possible solution, not all situations) with the C programming language
-- with high speed

**⚠️️ Warning: I use some Heuristics Algorithm in this code to get a better performance when N is very large. However these Algorithm may cause a worse performance when N is small.  So DON'T try small number.**

## performance
(On my personal laptop Thinkpad x1 carbon with i7-10710u CPU.)

It can solve X   queens in      Y seconds


|X       | Y     |
|-       |-      |
|10000   | 0.005 |
|50000   | 0.014 |
|100000  | 0.024 |
|200000  | 0.030 |
|300000  | 0.065 |   
|500000  | 0.090 | 
|1000000 | 0.274 |
|2000000 | 0.924 |

## How it works

The board is kept as a permutation (`queen[i]` is the column of the queen in
row `i`), so rows and columns never conflict and only diagonals need to be
tracked. Two counters per diagonal direction give the number of collisions in
O(1) per update.

The solver runs a swap-based local search in the style of Sosič and Gu:

1. Start from a random permutation.
2. Repeatedly pick a queen that is under attack and a random second queen,
   and swap their columns if that lowers the total number of collisions.
3. Keep a list of attacked queens and rebuild it only when the collision count
   drops below half of its value at the last rebuild, instead of after every
   swap.
4. Restart from a new random permutation when the search stops making
   progress.

The heuristics are tuned for large N; for small boards a plain backtracking
search is faster.

## Build and run

```bash
gcc -O2 code.c -o nqueens
./nqueens
```

The program reads N and whether to write the solution to `n-queens-out.txt`
(enter `1` for yes). For N up to 20 the file also contains a drawing of the
board. The compile-time constant `N` in `code.c` bounds the largest board.
