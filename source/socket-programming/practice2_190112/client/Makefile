CC = gcc
CFALGS = -W -Wall
TARGET = client
OBJECTS = client_main.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o 
	rm -f $(TARGET)

