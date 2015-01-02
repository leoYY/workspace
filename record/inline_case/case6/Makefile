
##############################################################
#
#   make file template for me
#
##############################################################
CC=gcc
XX=g++

CFLAGS=

# include path
INCLUDE=-I. \

# lib 
LIB=-l 

# lib path 
LIBPATH=-L 

default: inline_case.s1 inline_case.s2 inline_case.s3

inline_case.s1: inline_case.cpp
	$(XX) $(CFLAGS) -S inline_case.cpp -o inline_case.s1

inline_case.s2: inline_case.cpp
	$(XX) $(CFLAGS) -S -O1 inline_case.cpp -o inline_case.s2

inline_case.s3: inline_case.cpp
	$(XX) $(CFLAGS) -S -O2 inline_case.cpp -o inline_case.s3


.PHONY: clean
clean:
	@rm -rf inline_case.s*
	@echo "clean up"
