OBJ := main.o object.o interpreter.o term.o typing.o parser.o lexer.o error.o
CXX := clang++
CXXFLAGS := -std=c++11 -Wall -Wextra -Weffc++ -pedantic -g -I../include

.PHONY: all clean

all: sik


sik: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o sik


depend:
	$(CXX) $(CXXFLAGS) -E -MM $(OBJ:.o=.cpp) > .depend

clean:
	-rm *.o sik
	-rm -rf *.dSYM

include .depend