board: board.c
	gcc -c board.c
	gcc -c game.c
	gcc -c springer.c
	gcc -o springer.exe board.o springer.o
