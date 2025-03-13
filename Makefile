CC = gcc
CFLAGS = -Ilibxmp-4.6.2/include
LIBS = libxmp-4.6.2/lib/libxmp.a -lm -lasound
TARGET = tracker
SRC = tracker.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)