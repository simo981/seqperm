# Multi-Threaded word list generator (no repetition)

## Parameters
```
--start        <mininum words to concatenate>
--end          <maximum words to concatenate>
--last         <words/chars to be putted at the end of each permutation (separeted by ,)>
--connectors   <char/s to use as connector/s>
--upper        <y/n> print also the permutation with first letter in uppercase
```
```
words/chars go after parameters separeted by space
```
## Sample Usage
We want all permutation without repetition of ``` a b c d e f g h i l m ``` of 3/4/5 characters length with connectors ```,``` and ```.``` between chars and numbers ```0``` and ```1``` at the end. 

We dont't want any uppercase first character.
```
./seqperm --upper n --start 3 --end 5 --last 0,1 --connectors ,. a b c d e f g h i l m
```
