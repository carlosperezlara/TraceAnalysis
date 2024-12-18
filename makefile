all: generic.o test timingdaq

generic.o: src/generic.cxx
	g++ -fPIC -c -o generic.o src/generic.cxx -Iinc

test: test.cc generic.o
	g++ -o test test.cc generic.o -Iinc

timingdaq: timingdaq.cc generic.o
	g++ -o timingdaq timingdaq.cc generic.o -Iinc `root-config --cflags --libs` 
