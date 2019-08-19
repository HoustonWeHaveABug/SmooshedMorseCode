SM_ALPHA_C_FLAGS=-O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

sm_alpha: sm_alpha.o
	gcc -o sm_alpha sm_alpha.o

sm_alpha.o: sm_alpha.c sm_alpha.make
	gcc -c ${SM_ALPHA_C_FLAGS} -o sm_alpha.o sm_alpha.c

clean:
	rm -f sm_alpha sm_alpha.o
