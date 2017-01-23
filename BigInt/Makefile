CC = g++-6
CFLAGS = -std=c++14 -Wall
TARGET = bigint
DEPS = BigInt.hpp
OBJ = main.o BigInt.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $(TARGET) $^ $(CFLAGS)

clean:
	rm *.o

run: main
	./$(TARGET)
