CXX = g++
CFLAGS = -g -Wall -std=c++11 -Wpedantic -Werror -I.

SRCDIR = src
OBJDIR = bin

PROJ_NAME=battle_ship

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
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
