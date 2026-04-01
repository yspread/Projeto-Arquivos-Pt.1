all: registros.o funcionalidades.o registers.o main.c
    gcc registros.o funcionalidades.o header.o main.c -o main -std=c99 -Wall -lm
dataRegister.o:
    gcc -c registros.c -o registros.o
functionalities.o:
    gcc -c funcionalidades.c -o funcionalidades.o
headerRegister.o:
    gcc -c header.c -o header.o
run:
    ./main
clean:
    rm .o
    rm.exe
memory:
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./main