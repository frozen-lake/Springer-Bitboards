cd src
gcc -c board.c -g
gcc -c generate_moves.c -g
gcc -c game.c -g
gcc -c prompt_move.c -g
gcc -c tests.c -g
gcc -o ../springer_tests.exe board.o generate_moves.c game.c prompt_move.c tests.c 
