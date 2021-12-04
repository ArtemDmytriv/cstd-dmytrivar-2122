CXX = g++
CFLAGS = -g -Wall -std=c++11 -Wpedantic -Werror -Iinclude -DCLI_COMPILATION

SRCDIR = src
OBJDIR = bin

PROJ_NAME=battle_ship

SOURCES := src/battle_ship.cpp src/battle_main.cpp src/battle_cli.cpp src/battle_ship_ai.cpp
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all : $(OBJDIR) $(PROJ_NAME) 

$(PROJ_NAME) : $(OBJECTS)
	$(CXX) $(CFLAGS) $(LFLAGS) $^ -o $@

$(OBJECTS) : $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR) : 
	mkdir -p $(OBJDIR)

.PHONY: clean
clean :
	rm -rf $(OBJDIR)/*.o $(PROJ_NAME)
