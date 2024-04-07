CFLAGS = 
CC = clang
TARGET = seqperm
LINK = -lpthread
OBJ = main.o queue.o
.PHONY = clean re

$(TARGET): CFLAGS += -Ofast
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ $(LINK) -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o $(TARGET) 2>/dev/null

re:
	make clean
	make -j

debug: CFLAGS += -g3
debug: $(TARGET)
