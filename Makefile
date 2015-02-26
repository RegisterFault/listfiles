all: listfiles.c
	gcc -o listfiles listfiles.c
debug: listfiles.c
	gcc -ggdb -o listfiles listfiles.c
clean:
	rm listfiles
