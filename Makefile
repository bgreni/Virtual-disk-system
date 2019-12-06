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

tests: tests.cpp FileSystem.cpp Inode.cpp SuperBlock.cpp CommandParser.cpp Constants.hpp
	$(COMP) tests tests.cpp FileSystem.cpp Inode.cpp SuperBlock.cpp CommandParser.cpp

FileSystem.o: FileSystem.cpp FileSystem.hpp Constants.hpp
fs.o: fs.cpp
Inode.o: Inode.cpp Inode.hpp Constants.hpp
SuperBlock.o: SuperBlock.cpp SuperBlock.hpp Constants.hpp
CommandParser.o: CommandParser.cpp CommandParser.hpp Constants.hpp


compress:
	zip -r fs-sim.zip CommandParser.cpp CommandParser.hpp Constants.hpp FileSystem.cpp FileSystem.hpp fs.cpp Inode.cpp Inode.hpp SuperBlock.cpp SuperBlock.hpp tests.cpp readme.md Makefile