#new makefile for the project
#--------------------------------------------
#----adding environment configarations-------
#--------------------------------------------
include .env
#--------------------------------------------
#----configaring the project directories-----
#--------------------------------------------
APP = build
I_DIR = include
SRC_DIR = src
O_DIR = object
L_DIR = lib
TARGET = Kichwa
#-------------------------------------------
#---configaring the compiler----------------
#-------------------------------------------
CC=gcc
CFLAGS=-I$(I_DIR)
#CFLAGS=-std=c99
LIBS=-lm
#--------------------------------------------
#------Custom Cflags-------------------------
#---------------------------i----------------
#CFLANGS += `pkg-config --cflags raylib`
#LIBS += `pkg-config --libs raylib` -lm
#--------------------------------------------
#----compile the files-----------------------
#--------------------------------------------
_DEPS=lib.h struct.h filetype.h termina.h reopt.h fileio.h io.h
DEPS= $(patsubst %,$(I_DIR)/%,$(_DEPS))

_OBJ = index.o
OBJ= $(patsubst %,$(O_DIR)/%,$(_OBJ))

$(O_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(APP)/$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

#clean Up the created files
.PHONY: clean

clean:
	rm -f $(O_DIR)/*.o *~ $(APP)/$(TARGET) $(INCDIR)/*~

run:
	./build/Kichwa

