CC = gcc
TARGET = classMaker
CFLAGS = -O2

$(TARGET): main.o writeToFile.o
	$(CC) $(CFLAGS) -o $@ $^
	

main.o: main.c writeToFile.h
writeToFile.o: writeToFile.c writeToFile.h template.h

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f $(TARGET)

