yash: main.o parser.o
	gcc -o yash main.o parser.o -lreadline

main.o: main.c
	gcc -c main.c

parser.o: parser.c
	gcc -c parser.c

clean:
	rm -f yash *.o