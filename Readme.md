# A Simple SAT Solver

## Installation

* Require [**boost**](https://www.boost.org/users/download/) library.

```shell
mkdir build
cd build
cmake --build .
```

## Usage

```shell
./src/SAT_Solver benchmark_file
```

## Input 

For now, the solver accepts input file in [DIMACS CNF Format](http://beyondnp.org/static/media/uploads/docs/satformat.pdf). Support of SAT Format is in progress.

## Output

The output is in DIMACS format.