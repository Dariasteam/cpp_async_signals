CC=clang++
CXXFLAGS= -std=c++11 -pthread -O3

OBJS = logger.cpp ui.cpp main.cpp

all: ${OBJS}
	$(CC) $(CXXFLAGS) -o async_demo ${OBJS}

clean:
	rm -rf *.o async_demo
