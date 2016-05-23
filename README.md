

# Traditional output of cross_between

## First, a sample output of the left array:
```
$ iquery -aq "apply(build(<i_lo:int64>[i=0:1,1,0], (i+1)*(i+1)), j_lo, 2, i_hi, i_lo+1, j_hi, 3)"
{i} i_lo,j_lo,i_hi,j_hi
{0} 1,2,2,3
{1} 4,2,5,3
```

## Now, use this left array to pick out the values from a larger right array
```
$ iquery -aq "cross_between(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), apply(build(<i_lo:int64>[i=0:1,1,0], (i+1)*(i+1)), j_lo, 2, i_hi, i_lo+1, j_hi, 3))"
{i,j} val
{1,2} 2
{1,3} 3
{2,2} 4
{2,3} 6
{4,2} 8
{4,3} 12
{5,2} 10
{5,3} 15

```

# SgFree Cross Between 

For the above example, the SG free cross_betwen would have the following signature:

```
iquery -aq "cross_between_(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), 1, 2, 2, 3, 4, 2, 5, 3)" 
```
