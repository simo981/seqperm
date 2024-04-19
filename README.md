# Multi-Threaded word list generator (no repetition)
![Make](https://github.com/simo981/seqperm/actions/workflows/c-cpp.yml/badge.svg)
![CodeQL](https://github.com/simo981/seqperm/actions/workflows/codeql.yml/badge.svg)
![Valgrind](https://github.com/simo981/seqperm/actions/workflows/valgrind.yml/badge.svg)
## Parameters
```
--start        <mininum words to concatenate>
--end          <maximum words to concatenate>
```
### Modifiers
```
--last         <words/chars to be putted at the end of each permutation (separeted by ,)>
--connectors   <char/s to use as connector/s>
--upper        <full/first> print also the permutation with all/first letter/s in uppercase
--leet         <full/vowel> print also the permutation with leet in all/vowel chars positions
--reverse      <full/words> print also the permutation with reversed chars/words
```
### Specializer
```
--only_transformations
if one or more modifiers are setted, print only the transformations created with all modifiers instead of printing each one
```
```
All the words with the delimiter "," (ex. fish,es) will be considered as shortened permutation, in this case only short string or full string will be in a single permutation.
So, if you have in input fishes and fish and max word = 2, the permutation fishesfish will be created.
Instead, if "," is used, and in input you have fish,es the permutation fishesfish will not be created, but you'll have (for example) fishOTHERWORD fishesOTHERWORD
```
```
words/chars go after parameters separeted by space
```
## Random mode parameters
```
You can also generate a random dataset with these parameters
```
```
--random <ntimes,length> where ntimes are the number of strings printed and length the (obv) length of each string
--charset <ascii/num/alpha/alphalower/alphaupper/alphanum/alphanumupper> the charset used
```
```
In which charset parameters are

ascii: [\x00-\x7F]
num: [0-9]
alpha: [A-Za-z]
alpha lower: [a-z]
alpha upper: [A-Z]
alphanum: [0-9A-Za-z]
alphanum upper: [0-9A-Z]
```
```
You can also use your own charset, passed after the parameter --random x,y
```
## Compile and Run
```
1) Change number of threads directly in file main.h (best probably would be same number of phisical core)
2) $ make
3) $ ./seperm <parameters>
```
## Sample usage normal mode
We want all permutation without repetition of ``` a b c d e f g h i l m ``` of 3/4/5 characters length with connectors ```,``` and ```.``` between chars and numbers ```0``` and ```1``` at the end. 

We dont't want any uppercase first character.
```
./seqperm --start 3 --end 5 --last 0,1 --connectors ,. a b c d e f g h i l m
```
We want all the permutation of length 2/3/4 words of `john titor ibm time travel` with shortened string on `titor` with connector `.` and last chars `!?` and numers `2024 24` with leet only with vowel and upper case in first character, we don't want reverse  
To print all permutations for every possible combination of modifiers
```
./seqperm --start 2 --end 4 --connectors . --last !,?,2024,24 --leet vowel --upper first john tit,or ibm time travel
```
If we want only the permutations with all the modifiers simultaneously
```
./seqperm --start 2 --end 4 --connectors . --last !,?,2024,24 --leet vowel --upper first --only-transformations john tit,or ibm time travel
```
## Complexity as number of permutation generated
$$
(\sum_{k=start}^{end}{\binom{words}{k} \cdot k!}) \times (connectors + 1) \times (upper + 1) \times (last + 1) \times (leet + 1) \times (reverse + 1)
$$
## Sample usage random mode
We want to generate 1M strings of length 12 with our own charset
```
./seqperm --random 1000000,12 abcdefgh1234
```
We want to generate 1M strings of length 12 with default charset ascii
```
./seqperm --random 1000000,12 --charset ascii
```