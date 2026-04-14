# Cache Simulator — CS3339 Homework 3

Author: Cole Estrin\n
Note that it implemenets all extra credit features.

## Build
```
make
```

## Run
```
./cache_sim <num_entries> <associativity> <input_file> [block_size [L2_entries L2_assoc]]
```

- `num_entries` — total number of L1 cache entries (must be a power of two)
- `associativity` — number of ways (must evenly divide num_entries)
- `input_file` — text file with space-separated word addresses
- `block_size` — (optional) words per block, default 1
- `L2_entries` — (optional) total number of L2 cache entries
- `L2_assoc` — (optional) L2 associativity

## Examples

### Base case
```
echo "1 3 5 1 3 1" > input0
./cache_sim 4 2 input0
```

### Multi-word blocks (block_size = 2)
```
./cache_sim 4 2 input_file 2
```

### With L2 cache (L1: 2 direct-mapped, L2: 4 entries 2-way)
```
./cache_sim 2 1 input_file 1 4 2
```

## Output
Results are written to `cache_sim_output` in the current directory, one line per address.

## Clean
```
make clean
```