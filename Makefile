CC = g++
CFLAGS = -g -Wall -Wpedantic -Werror -I.

SRCDIR = src
OBJDIR = bin


PROJ_NAME=battle_ship.exe

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all : $(OBJDIR) $(PROJ_NAME) 

$(PROJ_NAME) : $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

$(OBJECTS) : $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR) : 
	@if not exist $(OBJDIR) mkdir $(OBJDIR)

.PHONY: clean
clean :
	del $(OBJDIR)\*.o $(PROJ_NAME)
