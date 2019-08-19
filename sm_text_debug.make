SM_TEXT_DEBUG_C_FLAGS=-g -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

sm_text_debug: sm_text_debug.o
	gcc -g -o sm_text_debug sm_text_debug.o

sm_text_debug.o: sm_text.c sm_text_debug.make
	gcc -c ${SM_TEXT_DEBUG_C_FLAGS} -o sm_text_debug.o sm_text.c

clean:
	rm -f sm_text_debug sm_text_debug.o
