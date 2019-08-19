SM_TEXT_C_FLAGS=-O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

sm_text: sm_text.o
	gcc -o sm_text sm_text.o

sm_text.o: sm_text.c sm_text.make
	gcc -c ${SM_TEXT_C_FLAGS} -o sm_text.o sm_text.c

clean:
	rm -f sm_text sm_text.o
