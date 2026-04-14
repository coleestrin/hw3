# Cache Simulator — CS3339 Homework 3

Author: Cole Estrin

## Build
```
make
```

## Run
```
./cache_sim <num_entries> <associativity> <input_file>
```

- `num_entries` — total number of cache entries (must be a power of two)
- `associativity` — number of ways (must evenly divide num_entries)
- `input_file` — text file with space-separated word addresses

## Example
```
echo "1 3 5 1 3 1" > input0
./cache_sim 4 2 input0
cat cache_sim_output
```

Expected output:
```
1 : MISS
3 : MISS
5 : MISS
1 : MISS
3 : MISS
1 : HIT
```

## Output
Results are written to `cache_sim_output` in the current directory, one line per address.

## Clean
```
make clean
```