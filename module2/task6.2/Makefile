all:
	gcc -fPIC -c linkedlist.c phonebook.c
	gcc -shared -o libphonebook.so linkedlist.o phonebook.o
	gcc main.c -L. -lphonebook

clean:
	rm -f *.o *.so a.out