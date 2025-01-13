[Efficient and Effective Directed Minimum Spanning Tree Queries](https://www.mdpi.com/2227-7390/11/9/2200)

## 1) Arbok

For Arbok's directed minimum spanning tree, we use the cpp code from [chistopher](https://github.com/chistopher/arbok).

## 2) DMST

```bash
g++ DMST.cpp -o DMST
```

```bash
./DMST test.gr test.Res
```

## 3) TODO

1. Storing index to files are not provided.
2. The data structure is kind of confusing.
3. Some of the variables are misused although they still generate correct result.
4. Our algorithm can be accerlated by adopting Arbok's techniques into CLE's framework.
