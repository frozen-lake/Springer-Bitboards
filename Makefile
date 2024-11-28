board: board.c
	gcc -c board.c
	gcc -c game.c
	gcc -c prompt_move.c
	gcc -c generate_moves.c
	gcc -c tests.c
	gcc -o springer_tests.exe board.o game.o prompt_move.o generate_moves.o tests.o
