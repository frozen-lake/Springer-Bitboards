mkdir -p obj
cd obj

gcc -c ../src/board.c -g
gcc -c ../src/game.c -g
gcc -c ../src/move.c -g
gcc -c ../src/attack_data.c -g

gcc -c ../tests/attack_and_move_tests.c -g
gcc -c ../tests/tests.c -g

cd ..
gcc -o springer_tests.exe obj/board.o obj/game.o obj/move.o obj/attack_data.o obj/attack_and_move_tests.o obj/tests.o
