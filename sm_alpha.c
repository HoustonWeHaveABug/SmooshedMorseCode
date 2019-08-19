#include <stdio.h>
#include <stdlib.h>

#define EXPECTED_IN_LEN 82
#define LETTERS_N 26
#define CHOICES_MAX 4

int sm_alpha(int, int);
 
char input[EXPECTED_IN_LEN+2];
int letters[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

/* Morse tree shown here - https://fr.wikipedia.org/wiki/Code_Morse_international#/media/Fichier:Morse_tree.svg - flatten in an array */
/* Each value is an index in the letters array */
/* Value = -1 at the root */
/* Value = LETTERS_N when there is no match with any letter */
int codes[] = { -1, 4, 19, 8, 0, 13, 12, 18, 20, 17, 22, 3, 10, 6, 14, 7, 21, 5, LETTERS_N, 11, LETTERS_N, 15, 9, 1, 23, 2, 24, 25, 16, LETTERS_N, LETTERS_N }, used[LETTERS_N];

int full_search, verbose, in_len, output[LETTERS_N];

int main(int argc, char *argv[]) {
	int i;

	/* Check/Read program arguments */
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <full search 0/1> <verbose 0/1>\n", argv[0]);
		fflush(stderr);
		return EXIT_FAILURE;
	}
	full_search = atoi(argv[1]);
	verbose = atoi(argv[2]);

	for (i = 0; i < LETTERS_N; i++) {
		used[i] = 0;
	}
	while (fgets(input, EXPECTED_IN_LEN+2, stdin)) {

		/* Check/Read input string */
		for (in_len = 0; in_len < EXPECTED_IN_LEN && (input[in_len] == '.' || input[in_len] == '-'); in_len++);
		if (in_len < EXPECTED_IN_LEN || input[in_len] != '\n') {
			fprintf(stderr, "Invalid morse string\n");
			fflush(stderr);
			return EXIT_FAILURE;
		}

		/* Call search function */
		printf("%s", input);
		printf("Outputs %d\n", sm_alpha(0, 0));
	}
	fflush(stdout);
	return EXIT_SUCCESS;
}

/* Search function */
/* in_idx: current position in input */
/* out_len: length of output so far */
int sm_alpha(int in_idx, int out_len) {
	int choices_max, code_idx, choices_n, choices[CHOICES_MAX], r, i;

	/* Check if a valid output was found */
	if (in_idx == in_len) {
		if (out_len == LETTERS_N) {
			if (verbose) {
				for (i = 0; i < out_len; i++) {
					putchar(letters[output[i]]);
				}
				puts("");
			}
			return 1;
		}
		return 0;
	}

	/* Set the maximum number of choices */
	choices_max = in_len-in_idx;
	if (choices_max > CHOICES_MAX) {
		choices_max = CHOICES_MAX;
	}

	/* Search the valid choices from the codes array */
	code_idx = 0;
	choices_n = 0;
	for (i = in_idx; i < in_idx+choices_max && codes[code_idx] < LETTERS_N; i++) {

		/* The codes array is the representation of a full binary tree */
		/* so the new code index can be computed from the current each */
		/* time a character is read in input. It may point to a letter */
		/* or not - In the latter case the search is stopped           */
		switch (input[i]) {
		case '.':
			code_idx = code_idx*2+1;
			break;
		case '-':
			code_idx = code_idx*2+2;
			break;
		default:
			fprintf(stderr, "This should never happen\n");
			fflush(stderr);
			return 0;
		}
		if (codes[code_idx] < LETTERS_N) {

			/* Valid choice - Index in the letters array */
			choices[choices_n++] = codes[code_idx];
		}
	}

	/* Try each choice and recurse to the next position in input */
	r = 0;
	for (i = choices_n; i > 0 && (full_search || !r); i--) {
		if (!used[choices[i-1]]) {
			output[out_len] = choices[i-1];
			used[choices[i-1]] = 1;
			r += sm_alpha(in_idx+i, out_len+1);
			used[choices[i-1]] = 0;
		}
	}
	return r;
}
