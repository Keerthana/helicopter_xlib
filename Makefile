CXX = g++
CFLAGS = -O3 -Wall
LDFLAGS = -L/usr/X11R6/lib -lX11 -lstdc++

SRC = heli.cpp
OBJ=$(SRC:.cpp=.o)

BIN = heli
all: $(BIN)

$(BIN): $(OBJ)
        $(CXX) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

tester: $(BIN)
        ./$(BIN)

run: grader
grader: $(BIN)
        ./$(BIN) grader

clean:
        rm -f $(BIN) *.o
