

# Traditional output of `cross_between`

Signature of `cross_between` is typically:
```
iquery -aq "cross_between(LEFT_ARRAY, RIGHT_ARRAY)"
```
where the `RIGHT_ARRAY` is constructed just to pick out sub-blocks of indices. 

Here is a sample `RIGHT_ARRAY` (notice how the sets are disjoint on the index `i_lo, i_hi`):
```
$ iquery -aq "apply(build(<i_lo:int64>[i=0:1,1,0], (i+1)*(i+1)), j_lo, 2, i_hi, i_lo+1, j_hi, 3)"
{i} i_lo,j_lo,i_hi,j_hi
{0} 1,2,2,3
{1} 4,2,5,3
```

Now, use this `RIGHT_ARRAY` to pick out the values from a larger `RIGHT_ARRAY`:
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

# SgFree `cross_between`

For the above example, the SG free `cross_between` would have the following signature:

```
iquery -aq "cross_between_(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), 1, 2, 2, 3, 4, 2, 5, 3)" 
```
