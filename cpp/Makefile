
##############################################################
#
#   make file template for me
#
##############################################################
CC=gcc
XX=g++

CFLAGS=-Wall\
    -O0 \
	-fPIC \
	-Wno-unused \
	-Werror \
	-Wreorder \
	-Wswitch \
	-Wno-multichar \
	-Wconversion \
	-Wpointer-arith \
    -g

# include path
INCLUDE=-I. \
		-I./thread/ \
		-I./util/

# lib 
LIB=-l 

# lib path 
LIBPATH=-L 

# .o store path
OBJPATH=./output/bin/
TARGET=mutex_test singleton_test

default: all

all: $(TARGET)

mutex_test:mutex_test.o
	$(XX) $(CFLAGS) $^ -o $@ $(INCLUDE)

singleton_test:singleton_test.o
	$(XX) $(CFLAGS) $^ -o $@ $(INCLUDE)

%.o:test/%.cpp
	$(XX) $(CFLAGS) -c $^ -o $@ $(INCLUDE)

.PHONY: clean
clean:
	rm -f *.o $(TARGET)
