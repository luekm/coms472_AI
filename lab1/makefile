all: lab1.exe

lab1.exe: lab1.o
	clang++ -std=c++11 -stdlib=libc++ -o lab1.exe lab1.o

lab1.o: lab1.cpp
	clang++ -std=c++11 -stdlib=libc++ -Wall -g -c lab1.cpp
     
clean:
	echo Removing all generated files
	rm -f *.exe *.o

