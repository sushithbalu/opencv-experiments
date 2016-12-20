
CC = gcc
CFLAGS = -Wall -g 
LDFLAGS = `pkg-config --cflags --libs opencv`

OUT = detector
CODE =eye_detection.c 

all:	
	$(CC) $(CFLAGS)  $(CODE) $(LDFLAGS) -o $(OUT)

clean:
	rm -rf *.o detector
