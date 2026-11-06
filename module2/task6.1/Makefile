all:
	gcc -c linkedlist.c phonebook.c
	ar rcs libphonebook.a linkedlist.o phonebook.o
	gcc main.c -L. -lphonebook

clean:
	rm -f *.o *.a a.out