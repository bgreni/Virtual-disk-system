
# Design

## Class structure

I decided to go with an OO style as I think it leads to cleaner and more traceable code. It likely resulted in me having to write quite a bit more code that was needed, but when it came time to debugging I was certainely happy with my choice

Admittedly, in the frenzy of trying to get everything done, some parts of my code did not turn out as clean as I would like, and in many cases I had to resort to some very expensive quick fixes to certain problems. The best example being my implementation of the free block list. I decided to use a bitset instead of a char array for my bit vector, and realized the 7th bit in my bitset actually represented the first bit in the list due to how the bytes are read from the file. To fix this I just wrote a function to flip every "byte" of the list so I could index it naturally. However, I later realized doing so would corrupt the super block on the disk when I wrote back to it, so I had to undo, and redo the changes to the list everytime I write back to the disk, which is terribly inefficient.

Unfortunately performance/resource management were not very high on my list for this project, as my main concerns were readability and correctness.

## System Calls

I don't believe I directly used any system calls, as I heavily used the c++ standard library as they are more convient to use.

# Testing

To test my program I used the given test cases, as well as some of my own, mostly to try and diagnose bugs that I was having. Comparing the expected and actually outputs to see if they matched up. I had wanted to write a script to compared the state of the disk with the expected state but I ran out of time. I also write a small suite of "unit test" type functions just to check simple cases to check if I ever horrifically broke something.


# Sources
This stackoverflow thread taught me how to sort a vector of objects using a custom comparison object.

https://stackoverflow.com/questions/1380463/sorting-a-vector-of-custom-object


