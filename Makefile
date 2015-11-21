all:  mycipher

mycipher: mycipher.c
	gcc -o mycipher mycipher.c -g

clean:
	rm -f mycipher *.o *~ core
