game:utils.o card.o game.o
	gcc -o ../target/game utils.o card.o game.o

utils.o:../source/game.h ../source/utils.c
	gcc -c ../source/utils.c

card.o:../source/game.h ../source/card.h ../source/card.c
	gcc -c ../source/card.c

game.o:../source/game.h ../source/card.h ../source/game.c
	gcc -c ../source/game.c

clean:
	rm -rf ../source/game *.o
