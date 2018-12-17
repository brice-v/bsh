# the compiler: gcc for C program, define as g++ for C++
# can also use clang as C compiler
CC = gcc

# Compiler Flags:
#  -g         adds debugging information to the executable file
#  -Wall      turns on most, but not all, compiler warnings
#  -Wextra    enables some extra warning flags that are not enabled by -Wall
#  -pedantic  issue all the warnings demanded by strict ISO C
CFLAGS  = -g -Wall -Wextra 

# More Compiler Flags
#  -lm        math library for linking #include <math.h>
#  -ledit     editline library for linking #include <editline/*.h>
LIBS =

# the build target executable:
TARGET = bsh

# CSRCS = $(TARGET).c
# To grab all c files use
CSRCS = $(wildcard *.c)
all: $(TARGET)

$(TARGET): $(TARGET)
	$(CC) $(CFLAGS) $(CSRCS) -o $(TARGET) $(LIBS)

clean:
	$(RM) $(TARGET)