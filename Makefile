CC = gcc
SRCS = proj3.c
TARGET = proj3
default:
	$(CC) -o $(TARGET) $(SRCS) -lpthread
clean:
	rm $(TARGET)

