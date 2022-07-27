main:main.o avltree.o
	gcc $^ -o $@

main.o:main.c avltree.o avltree.h
	gcc -c $< -o $@

avltree.o:avltree.c avltree.h
	gcc -c $< -o $@

.PHONY:
clean:
	rm -f main.o avltree.o main
