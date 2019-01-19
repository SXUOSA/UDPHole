PROJECT = udphole
SOURCEDIR = ./src
CC = gcc
CFLAGS = -Wall -Wextra -I $(SOURCEDIR) -flto -Ofast
LINKS = -lpthread 

SOURCES = $(shell find "$(SOURCEDIR)" -name "*.cpp" -o -name "*.c" -o -name "*.S")
OBJECTS = $(patsubst %.S, %.o, $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCES))))

all: $(PROJECT)

$(PROJECT) : $(OBJECTS)
	$(CC) -o $(PROJECT) $(OBJECTS) $(CFLAGS) $(LINKS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find src -name "*.o"  | xargs rm -f
	rm -f $(PROJECT)
	
#Makefile By Aodzip