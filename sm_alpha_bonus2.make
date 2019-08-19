SM_ALPHA_BONUS2_C_FLAGS=-O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

sm_alpha_bonus2: sm_alpha_bonus2.o
	gcc -o sm_alpha_bonus2 sm_alpha_bonus2.o

sm_alpha_bonus2.o: sm_alpha_bonus2.c sm_alpha_bonus2.make
	gcc -c ${SM_ALPHA_BONUS2_C_FLAGS} -o sm_alpha_bonus2.o sm_alpha_bonus2.c

clean:
	rm -f sm_alpha_bonus2 sm_alpha_bonus2.o
