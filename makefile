all: registros.o funcionalidades.o header.o fornecidas.o main.c
	gcc registros.o funcionalidades.o header.o fornecidas.o main.c -o main -std=c99 -Wall -lm

registros.o: registros.c
	gcc -c registros.c -o registros.o

funcionalidades.o: funcionalidades.c
	gcc -c funcionalidades.c -o funcionalidades.o

header.o: header.c
	gcc -c header.c -o header.o

fornecidas.o: fornecidas.c
	gcc -c fornecidas.c -o fornecidas.o

run:
	./main

clean:
	rm -f *.o
	rm -f main
	rm -f main.exe *.zip

checkmem:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./main

zip:	
	zip -r trabalho.zip *.c *.h makefile 