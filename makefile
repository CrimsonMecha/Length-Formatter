CC = gcc
SRC = formatter
EXEC = formatter
TEXT = hollow

all: $(EXEC) test-me

$(EXEC): $(SRC).o
	$(CC) -g -o $(EXEC) $(SRC).o

$(SRC).o: $(SRC).c
	$(CC) -g -c $(SRC).c

test-me:
	./$(EXEC) $(TEXT).txt out.txt 50

clean:
	rm -f $(EXEC) $(SRC).o
