

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
$ iquery -aq "cross_between_(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), 1, 2, 2, 3, 4, 2, 5, 3)" 
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

Other test outputs using the same left array, but different choices of regions: 

 - two sets of regions
```
$ iquery -aq "cross_between_(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), 1, 2, 2, 3, 
                                                                            1, 2, 5, 5)"
{i,j} val
{1,2} 2
{1,3} 3
{2,2} 4
{2,3} 6
{3,2} 6
{3,3} 9
{4,2} 8
{4,3} 12
{5,2} 10
{5,3} 15
```

 -  can handle `null`-s
```
$ iquery -aq "cross_between_(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), null, 2, 1, 3, 3, 2, null, null)"
{i,j} val
{0,2} 0
{0,3} 0
{1,2} 2
{1,3} 3
{3,2} 6
{3,3} 9
{4,2} 8
{4,3} 12
{5,2} 10
{5,3} 15
{6,2} 12
{6,3} 18
```
 - some erroneous selection of regions (2nd region is not fully specified):
 ```
$ iquery -aq "cross_between_(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), 1, 2, 2, 3, 1, 2, 5)"
UserQueryException in file: src/query/parser/Translator.cpp function: passAFLOperator line: 756
Error id: scidb::SCIDB_SE_SYNTAX::SCIDB_LE_WRONG_OPERATOR_ARGUMENTS_COUNT2
Error description: Query syntax error. Operator 'cross_between_' expected more arguments.
cross_between_(build(<val:double>[i=0:6,1,0,j=2:3,1,0], i*j), 1, 2, 2, 3, 1, 2, 5)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 
 ```

