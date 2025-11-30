#Usage:
#make          # compile project binary
#make clean    # remove all binaries and objects

PROG = part_a
CC = g++
CPPFLAGS = -pthread -Wall
LDFLAGS = -lpthread

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))  # find all .cpp files in the directory
HEADERS = $(wildcard *.h)                        # include all .h files in the directory

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rvf *.o $(PROG)