#new makefile for the project
#-------------------------------------------
#--------Installation paths-----------------
#------------------------------------------
PREFIX  ?= /usr/local
BINDIR := $(PREFIX)/bin
#--------------------------------------------
#---------Environment Configaration----------
#--------------------------------------------
-include .env
#--------------------------------------------
#----Project Directories Configaration-------
#--------------------------------------------
APP     := build
I_DIR   := include
SRC_DIR := src
O_DIR   := object
L_DIR   := lib
TARGET  := Kichwa
#-------------------------------------------
#-----Compiler Configaration----------------
#-------------------------------------------
CC        := gcc
WARNINGS  :=-Wall -Wextra
CFLAGS    :=-I$(I_DIR) $(WARNINGS)
LIBS      :=-lm
#--------------------------------------------
#------Custom Cflags (raylib configuration---
#---------------------------i----------------
RAYLIBS_CFLAGS := $(shell pkg-config --cflags raylib)
RAYLIBS_LIBS    := $(shell pkg-config --libs raylib)

CFLAGS += $(RAYLIBS_CFLAGS)
LIBS   += $(RAYLIBS_LIBS)

#------------------------------------------
#----------Building modes------------------
#------------------------------------------
DEBUG_CFLAGS    := -g -O0 -DDEBUG
RELEASE_CFLAGS  := -O2 -DNDEBUG
SANITIZE_CFLAGS := -g -O1 -fsanitize=address,undefined \
		   -fno-omit-frame-pointer -DSANITIZE

MODE ?= release


ifeq ($(MODE),debug)
	CFLAGS += $(DEBUG_CFLAGS)
	OBJDIR := $(O_DIR)/debug
	OUTDIR := $(APP)/debug

else ifeq ($(MODE),sanitize)
	CFLAGS += $(SANITIZE_CFLAGS)
	LIBS   += -fsanitize=address,undefined
	OBJDIR := $(O_DIR)/sanitize
	OUTDIR := $(APP)/sanitize
else
	CFLAGS += $(RELEASE_CFLAGS)
	OBJDIR := $(O_DIR)/release
	OUTDIR := $(APP)/release
endif
#------------------------------------------
#----------Source/Object Files-------------
#------------------------------------------
SRCS :=$(wildcard $(SRC_DIR)/*.c)
OBJS :=$(patsubst $(SRC_DIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
#------------------------------------------
#----To identify Flags mistype-------------
#------------------------------------------
$(info CFLAGS=$(CFLAGS))
$(info MODE=$(MODE))
#--------------------------------------------
#----compile the files-----------------------
#--------------------------------------------
$(OBJDIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/$(TARGET): $(OBJS)
	@mkdir -p $(OUTDIR)
	$(CC) $^ -o $@ $(LIBS)
#-------------------------------------------
#----------Valgrid helper rules-------------
#-------------------------------------------
.PHONY: valgrind

VALGRIND_FLAGS := \
		  --leak-check=full\
		  --show-leak-kinds=all\
		  --track-origins=yes\
		  --errors-for-leak-kinds=all\
		  --error-exitcode=1

valgrind:
	$(MAKE) MODE=debug
	valgrind $(VALGRIND_FLAGS)
#--------------------------------------------
#---------------Targets----------------------
#--------------------------------------------
.PHONY: all debug release sanitize clean run install uninstall

all: release

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

sanitize:
	$(MAKE) MODE=sanitize

clean:
	rm -rf $(O_DIR) $(APP) *~

run: all
	./$(OUTDIR)/$(TARGET)

install: $(OUTDIR)/$(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR)"
	install -d $(DESTDIR)$(BINDIR)
	install -m 0755 $(OUTDIR)/$(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	@echo "removing $(TARGET) from $(BINDIR)"
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)


#--------------------------------------------
#_DEPS=lib.h struct.h filetype.h termina.h reopt.h fileio.h io.h
#DEPS= $(patsubst %,$(I_DIR)/%,$(_DEPS))

#_OBJ = index.o
#OBJ= $(patsubst %,$(O_DIR)/%,$(_OBJ))

#$(O_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS) 

#$(APP)/$(TARGET): $(OBJ)
#	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

#clean Up the created files
#.PHONY: clean

#clean:
#	rm -f $(O_DIR)/*.o *~ $(APP)/$(TARGET) $(INCDIR)/*~

#run:
#	./build/Kichwa

