#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECTED_IN_LEN 82
#define LETTERS_N 26
#define SYMBOLS_N 2
#define CHOICES_MAX 4
#define OUTPUTS_MAX 2

typedef struct {
	int symbol;
	int code_len;
	int code_mask;
	int locked;
}
letter_t;

void generate_input(int);
int try_letter(int, letter_t *);
int sm_alpha_bonus2(int, int, int, int);
int try_choice(int, int, int, int, letter_t *);

char morse_symbols[] = "-.", best[EXPECTED_IN_LEN+1], input[EXPECTED_IN_LEN+1];
int tree[] = { -1, 16, 2, 21, 15, 8, 1, 24, 20, 18, 14, 11, 7, 4, 0, 25, 23, 22, LETTERS_N, 19, LETTERS_N, 17, 13, 12, 10, 9, 6, 5, 3, LETTERS_N, LETTERS_N }, output[LETTERS_N];
letter_t letters[] = {
	{ 'o', 3, 0, 1 },
	{ 'm', 2, 0, 1 },
	{ 't', 1, 0, 1 },
	{ 'q', 4, 4, 1 },
	{ 'g', 3, 4, 1 },
	{ 'z', 4, 12, 1 },
	{ 'y', 4, 2, 1 },
	{ 'k', 3, 2, 1 },
	{ 'n', 2, 2, 1 },
	{ 'c', 4, 10, 1 },
	{ 'x', 4, 6, 1 },
	{ 'd', 3, 6, 1 },
	{ 'b', 4, 14, 1 },
	{ 'j', 4, 1, 1 },
	{ 'w', 3, 1, 1 },
	{ 'a', 2, 1, 1 },
	{ 'e', 1, 1, 1 },
	{ 'p', 4, 9, 1 },
	{ 'r', 3, 5, 1 },
	{ 'l', 4, 13, 1 },
	{ 'u', 3, 3, 1 },
	{ 'i', 2, 3, 1 },
	{ 'f', 4, 11, 1 },
	{ 'v', 4, 7, 1 },
	{ 's', 3, 7, 1 },
	{ 'h', 4, 15, 1 }
};

int main(void) {
	int i;
	for (i = 0; i < EXPECTED_IN_LEN; i++) {
		best[i] = morse_symbols[SYMBOLS_N-1];
	}
	best[EXPECTED_IN_LEN] = '\0';
	input[EXPECTED_IN_LEN] = '\0';
	generate_input(0);
	return EXIT_SUCCESS;
}

void generate_input(int in_idx) {
	int i;
	if (in_idx == EXPECTED_IN_LEN) {
		strcpy(best, input);
		puts(best);
		sm_alpha_bonus2(in_idx, 0, 0, 1);
		fflush(stdout);
	}
	for (i = 0; i < LETTERS_N && try_letter(in_idx, letters+i); i++);
}

int try_letter(int in_idx, letter_t *letter) {
	int code_mask, i;
	if (!letter->locked) {
		return 1;
	}
	code_mask = letter->code_mask;
	for (i = in_idx; i < in_idx+letter->code_len; i++) {
		input[i] = morse_symbols[code_mask%SYMBOLS_N];
		code_mask /= SYMBOLS_N;
	}
	if (strncmp(input, best, (size_t)(in_idx+letter->code_len)) > 0) {
		return 0;
	}
	letter->locked = 0;
	if (sm_alpha_bonus2(in_idx+letter->code_len, 0, 0, 0) == 1) {
		generate_input(in_idx+letter->code_len);
	}
	letter->locked = 1;
	return 1;
}

int sm_alpha_bonus2(int in_len, int in_idx, int out_idx, int print) {
	int choices_max = in_len-in_idx, tree_idx, choices_n, choices[CHOICES_MAX], r, i;
	if (choices_max == 0) {
		if (print) {
			for (i = 0; i < out_idx; i++) {
				putchar(output[i]);
			}
			puts("");
		}
		return 1;
	}
	if (choices_max > CHOICES_MAX) {
		choices_max = CHOICES_MAX;
	}
	tree_idx = 0;
	choices_n = 0;
	for (i = in_idx; i < in_idx+choices_max && tree[tree_idx] < LETTERS_N; i++) {
		switch (input[i]) {
		case '.':
			tree_idx = tree_idx*2+1;
			break;
		case '-':
			tree_idx = tree_idx*2+2;
			break;
		default:
			fprintf(stderr, "This should never happen\n");
			fflush(stderr);
			return 0;
		}
		if (tree[tree_idx] < LETTERS_N) {
			choices[choices_n++] = tree[tree_idx];
		}
	}
	r = 0;
	for (i = 0; i < choices_n && r < OUTPUTS_MAX; i++) {
		r += try_choice(in_len, in_idx, out_idx, print, letters+choices[i]);
	}
	return r;
}

int try_choice(int in_len, int in_idx, int out_idx, int print, letter_t *choice) {
	int r;
	if (choice->locked) {
		return 0;
	}
	output[out_idx] = choice->symbol;
	choice->locked = 1;
	r = sm_alpha_bonus2(in_len, in_idx+choice->code_len, out_idx+1, print);
	choice->locked = 0;
	return r;
}
