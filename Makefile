CXX=g++
CXXFLAGS=-g -Wall -Wpedantic -Werror

SRCDIR=src

SOURCES=$(wildcard $(SRCDIR)/*.c)
PROJ_NAME=battle_ship.exe

all : $(PROJ_NAME)

$(PROJ_NAME) : $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean :
	del *.o $(PROJ_NAME)
