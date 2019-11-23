COMP = g++ -Wall -std=c++17 -O3 -o
OBJ = g++ -Wall -std=c++17 -O3 -c

default: fs

fs: FileSystem.o fs.o Inode.o SuperBlock.o CommandParser.o
	$(COMP) fs FileSystem.o fs.o Inode.o SuperBlock.o CommandParser.o 

%.o: %.cpp
	$(OBJ) $<

compile: FileSystem.o fs.o

clean:
	-rm *.o $(objects)
	-rm fs


FileSystem.o: FileSystem.cpp FileSystem.hpp
fs.o: fs.cpp
Inode.o: Inode.cpp Inode.hpp
SuperBlock.o: SuperBlock.cpp SuperBlock.hpp
CommandParser.o: CommandParser.cpp CommandParser.hpp