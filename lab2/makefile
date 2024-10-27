all: lab2.exe

lab2.exe: lab2.o
	clang++ -std=c++11 -stdlib=libc++ -o lab2.exe lab2.o

lab2.o: lab2.cpp
	clang++ -std=c++11 -stdlib=libc++ -Wall -g -c lab2.cpp
     
clean:
	echo Removing all generated files
	rm -f *.exe *.o

