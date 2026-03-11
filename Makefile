CC = gcc
TARGET = classMaker

$(TARGET): main.o writeToFile.o
	$(CC) -o $@ $^
	

main.o: main.c writeToFile.h
writeToFile.o: writeToFile.c writeToFile.h template.h

%.o:%.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o
	rm -f $(TARGET)