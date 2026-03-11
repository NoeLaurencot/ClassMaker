CC = gcc

classMaker: main.o writeToFile.o
	$(CC) -o $@ $^
	rm -f *.o

main.o: main.c writeToFile.h
writeToFile.o: writeToFile.c writeToFile.h template.h

%.o:%.c
	$(CC) -c $<