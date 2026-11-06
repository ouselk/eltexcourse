calculator:
	gcc -fPIC -shared mysum.c -o plugins/sum.so
	gcc -fPIC -shared mydif.c -o plugins/diff.so
	gcc -fPIC -shared mymult.c -o plugins/mult.so
	gcc -fPIC -shared mydiv.c -o plugins/div.so
	gcc -fPIC -shared mystrangefunc.c -o plugins/strange.so
	gcc main.c parser.c calculator.c stack.c operator.c -ldl -lm

clean:
	rm -f plugins/*.so a.out
