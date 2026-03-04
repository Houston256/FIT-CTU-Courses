# Map coloring with CSP algorithms
This repo implements BT, BJ, and MAC-BT variants to solve map coloring problems. The algorithms are tested on three map coloring problems with increasing difficulty.

## Usage
```bash
uv run python main.py
```
## Results
### Australia dataset
In this primitive instance, the more complex algorithms only add overhead.

The best algorithms for this instance are Backtracking and Backjumping.
The difference in speed is negligible.
However, Backtracking performs ~20-30% fewer constraint checks.

### USA states dataset
In this medium instance, the more complex algorithms start to show their advantages.
Backjumping significantly outperforms Backtracking, reducing constraint checks from ~4k to ~150.
MAC-BT variants perform well but the added complexity is not justified for this size.

The worst algorithms are Backtracking and MAC-BT with Random variable ordering.

Overall, I would pick Backjumping for this instance.

### US counties dataset
In this large instance, the MAC-BT variants dominate. The only tested variants are MRV and MRV+LCV. 
I tried the other variants too, but they did not finish.

Backjumping worked surprisingly well, even on such a large instance. In terms of runtime, it was only ~3x slower than the MAC-BT variants, but it performed ~12x more contraint checks.

The MAC-BT variants are the best choice for this instance, with MRV+LCV being ~5% faster than MRV alone with ~2% less constraint checks.
```

                                                      Australia state borders                                                      
                                                   7 nodes | 9 edges | 3 colors                                                    
┏━━━━━━━━━━━━━━━━━━━┳━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━┓
┃ Algorithm         ┃ Nodes ┃ Backtracks ┃ Constraints ┃ Arc Revs ┃ Select (ms) ┃ Consist (ms) ┃ Propag (ms) ┃ Total (ms) ┃ Valid ┃
┡━━━━━━━━━━━━━━━━━━━╇━━━━━━━╇━━━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━━━╇━━━━━━━┩
│ Backtracking      │     7 │          0 │          25 │        0 │       0.002 │        0.003 │       0.000 │      0.018 │   Yes │
│ Backjumping       │     7 │          0 │          35 │        0 │       0.002 │        0.003 │       0.000 │      0.017 │   Yes │
│ MAC-BT Static     │     7 │          0 │         146 │       41 │       0.002 │        0.002 │       0.020 │      0.047 │   Yes │
│ MAC-BT Random     │    11 │         10 │         227 │       81 │       0.006 │        0.004 │       0.042 │      0.081 │   Yes │
│ MAC-BT Random (2) │     7 │          0 │         147 │       41 │       0.006 │        0.002 │       0.015 │      0.040 │   Yes │
│ MAC-BT MRV        │     7 │          0 │         157 │       45 │       0.005 │        0.001 │       0.018 │      0.041 │   Yes │
│ MAC-BT MRV+LCV    │     7 │          0 │         157 │       45 │       0.003 │        0.001 │       0.017 │      0.045 │   Yes │
│ MAC-BT Degree     │     7 │          0 │         144 │       41 │       0.004 │        0.001 │       0.016 │      0.037 │   Yes │
│ MAC-BT Degree+LCV │     7 │          0 │         144 │       41 │       0.003 │        0.001 │       0.014 │      0.039 │   Yes │
│ MAC-BT Static+LCV │     7 │          0 │         146 │       41 │       0.001 │        0.001 │       0.015 │      0.039 │   Yes │
└───────────────────┴───────┴────────────┴─────────────┴──────────┴─────────────┴──────────────┴─────────────┴────────────┴───────┘
                                                   USA states connected by road                                                    
                                                  49 nodes | 107 edges | 4 colors                                                  
┏━━━━━━━━━━━━━━━━━━━┳━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━┓
┃ Algorithm         ┃ Nodes ┃ Backtracks ┃ Constraints ┃ Arc Revs ┃ Select (ms) ┃ Consist (ms) ┃ Propag (ms) ┃ Total (ms) ┃ Valid ┃
┡━━━━━━━━━━━━━━━━━━━╇━━━━━━━╇━━━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━━━╇━━━━━━━┩
│ Backtracking      │ 4,222 │      4,173 │      51,687 │        0 │       2.471 │        3.320 │       0.000 │     10.347 │   Yes │
│ Backjumping       │   196 │        147 │       2,656 │        0 │       0.129 │        0.186 │       0.000 │      0.584 │   Yes │
│ MAC-BT Static     │    49 │          0 │       2,559 │      642 │       0.026 │        0.020 │       0.525 │      0.780 │   Yes │
│ MAC-BT Random     │ 6,461 │     12,508 │     318,729 │  143,614 │       2.440 │        3.661 │      99.166 │    126.931 │   Yes │
│ MAC-BT Random (2) │    50 │          4 │       2,712 │      737 │       0.025 │        0.020 │       0.493 │      0.755 │   Yes │
│ MAC-BT MRV        │    49 │          0 │       2,862 │      656 │       0.060 │        0.017 │       0.425 │      0.683 │   Yes │
│ MAC-BT MRV+LCV    │    49 │          0 │       2,856 │      652 │       0.056 │        0.016 │       1.567 │      1.881 │   Yes │
│ MAC-BT Degree     │    49 │          0 │       2,489 │      591 │       0.055 │        0.015 │       0.344 │      0.587 │   Yes │
│ MAC-BT Degree+LCV │    49 │          0 │       2,461 │      576 │       0.055 │        0.015 │       0.348 │      0.660 │   Yes │
│ MAC-BT Static+LCV │    49 │          0 │       2,556 │      638 │       0.024 │        0.015 │       0.372 │      0.634 │   Yes │
└───────────────────┴───────┴────────────┴─────────────┴──────────┴─────────────┴──────────────┴─────────────┴────────────┴───────┘
                                                        US county borders                                                         
                                                3225 nodes | 9479 edges | 5 colors                                                
┏━━━━━━━━━━━━━━━━┳━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━━━┳━━━━━━━━━━━━━┳━━━━━━━━━━━━┳━━━━━━━┓
┃ Algorithm      ┃   Nodes ┃ Backtracks ┃ Constraints ┃ Arc Revs ┃ Select (ms) ┃ Consist (ms) ┃ Propag (ms) ┃ Total (ms) ┃ Valid ┃
┡━━━━━━━━━━━━━━━━╇━━━━━━━━━╇━━━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━━━━━╇━━━━━━━━━━━━━╇━━━━━━━━━━━━╇━━━━━━━┩
│ Backjumping    │ 191,202 │    187,977 │   4,117,612 │        0 │   5,900.336 │      333.516 │       0.000 │  6,607.724 │   Yes │
│ MAC-BT MRV     │   3,225 │          0 │     347,764 │   70,152 │     255.723 │        2.250 │   2,022.291 │  2,433.202 │   Yes │
│ MAC-BT MRV+LCV │   3,225 │          0 │     342,774 │   68,648 │     257.149 │        2.046 │   1,912.008 │  2,327.689 │   Yes │
└────────────────┴─────────┴────────────┴─────────────┴──────────┴─────────────┴──────────────┴─────────────┴────────────┴───────┘


```