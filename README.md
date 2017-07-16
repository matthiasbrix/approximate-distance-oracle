# thorup-zwick

## Synopsis

An implementation of the Thorup-Zwick approximate distance oracle data structure (<a href="https://pdfs.semanticscholar.org/ce82/faffefe89d80218eebeeafc08d4e219fff43.pdf">get here</a>). Also, it contains implementations of Dijkstra's original algorithm, an optimised variant (similar to A\*, without heuristics though), bidirectional dijkstra and A* (uses the Thorup-Zwick as heuristic).

## How to run

Executing the algorithms can be done in two ways. One is executing the executable with the required arguments. e.g <br /> <br />

*./bin/main tz tests/densegraph.txt results.csv 10 23 431 1* <br /> <br />

where **tz** indicates the Thorup-Zwick is to be run, tests/densegraph.txt is a valid input graph in the DIMACS ssp file format (which is a necessity), results.csv is just the .csv we wish to write the results to (doesn't need to exist). The next argument represents the *k* value for the algorithm. The shortest path query is being done one (23, 431), running the Thorup-Zwick query algorithm once (indicated by 1). <br /> <br />

The other way to execute the program is with **run_algs.sh** which runs and measures all implemented algorithms, printing out results to the .csv file.

## Motivation

Providing a correct implementation of the Thorup-Zwick algorithm, which may be applicable in some situations.

## Tests

*./run_algs -i tests/USANY.txt -o results.csv -k 10 -u 1000 -v 150000 -p 3 -q 10* <br /> <br />
runs Thorup-Zwick preprocessing 3 times, query 10 times, and each of the four SSSP algorithms once with the above settings, taking USANY.txt as input and writing all results to the file results.csv. 

## Installation

git clone https://github.com/matthiasbrix/thorup-zwick.git <br />
make all

## Contributors

Matthias Brix
