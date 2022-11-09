CFLAGS = -O3 -Werror -Wextra -pedantic
CC = gcc
TARGET = wlgen
LINK = -lpthread
OBJ = main.o queue.o
.PHONY = clean re

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ $(LINK) -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(TARGET)

re:
	make clean
	make -j
