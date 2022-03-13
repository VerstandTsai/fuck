all: fuck.c
	gcc -o fuck fuck.c
clean:
	rm -f fuck
install: fuck.c
	gcc -o fuck fuck.c
	mv fuck /usr/bin/fuck
