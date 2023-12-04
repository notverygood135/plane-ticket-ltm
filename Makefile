CC=g++
CFLAGS=-Iinclude
exec = server
sources = $(wildcard src/*.cpp)
objects = $(sources:.cpp=.o)
flags = -g -Wall -lm -ldl -fPIC -lsqlite3 -rdynamic -I./include
# flags = -I./include

$(exec): $(objects)
	g++ $(objects) $(flags) -o $(exec)

%.o: %.c %.h
	g++ -c $(flags) $< -o $@


clean:
	-rm src/*.o
