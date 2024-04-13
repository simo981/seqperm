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
--leet         <full/first> print also the permutation with leet in all/vowel chars positions
```
```
--only_transformations <y/n> 
if one or more modifiers are setted, print only the transformations created with all modifiers instead of printing each one
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
## TO DO
Implement ```--delim wo,rd``` for considering a word a possible shortener permutation, in a single permutation will be only the full word or its delim wo. Multiple handling of , is a plus