# Approximate Distance Oracles (Thorup-Zwick)

## Synopsis

An implementation of the approximate distance oracle data structure (<a href="https://pdfs.semanticscholar.org/ce82/faffefe89d80218eebeeafc08d4e219fff43.pdf">seminal paper</a>) by Thorup and Zwick. Additionally, it contains implementations of Dijkstra's original algorithm, an optimised variant (similar to A\*, without heuristics though), bidirectional dijkstra and A* (uses Thorup-Zwick as heuristic).

## Installation

Before the build, run

```
sudo sh ./install.sh
```

## Build

```
make all
```

## How to run

### File format of graphs

The graphs are based on the file format by the 9th DIMACS SSP [competition](http://www.diag.uniroma1.it//challenge9/competition.shtml). The file format can be seen [here](http://www.diag.uniroma1.it//challenge9/format.shtml).

### Run the algorithm

Executing the algorithms can be done in two ways. One is executing the executable with the required arguments. e.g 

```
./bin/main tz test-files/densegraph.txt results.csv 10 23 431 1
```

where `tz` indicates the Thorup-Zwick is to be run, `test-files/densegraph.txt` is a valid input graph in the DIMACS ssp file format. Note that the DIMACS ssp file format is a strict requirement to run the algorithm. The file `results.csv` is just the .csv we wish to write the results to (doesn't need to exist). The next argument represents the `k` value for the algorithm. The shortest path query is being done one (23, 431), running the Thorup-Zwick query algorithm once (indicated by 1). <br /> <br />

The other way to execute the program is with the script `run_algs.sh` which runs and measures all implemented algorithms, printing out results to a `.csv` file and benchmarks in the `benchmarks` folder.

### Example

```
./run_algs.sh -i test-files/USANY.txt -o results.csv -k 10 -u 1000 -v 150000 -p 3 -q 10
```

runs Thorup-Zwick preprocessing 3 times, query 10 times, and each of the four SSSP algorithms once with the above settings, taking `USANY.txt` as input and writing all results to the file `results.csv`. 

## Benchmarks

Previously run benchmarks can be inspected in the folder `benchmarks`.

## License

MIT License

## Contributors

Matthias Brix
