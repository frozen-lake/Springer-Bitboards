mkdir -p obj
cd obj
gcc -c ../src/board.c -g
gcc -c ../src/generate_moves.c -g
gcc -c ../src/game.c -g
gcc -c ../src/prompt_move.c -g
gcc -c ../src/attack_data.c -g
gcc -c ../src/tests.c -g

cd ..
gcc -o springer_tests.exe obj/board.o obj/generate_moves.o obj/game.o obj/prompt_move.o obj/attack_data.o obj/tests.o
