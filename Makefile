CC = gcc
CFLAGS = -Ilibxmp/include
LIBS = libxmp/build/libxmp.a -lm -lasound
TARGET = tracker
SRC = tracker.c

all: $(TARGET)

$(TARGET): $(SRC) libxmp/build/libxmp.a
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LIBS)

libxmp/build/libxmp.a:
	mkdir -p libxmp/build
	cd libxmp/build && cmake .. -DBUILD_STATIC=ON -DBUILD_SHARED=OFF
	$(MAKE) -C libxmp/build

clean:
	rm -f $(TARGET)
	$(MAKE) -C libxmp/build clean
	rm -rf libxmp/build