#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define INPUT_LEN_MAX 65536
#define BUFFER_SIZE INPUT_LEN_MAX+2
#define INPUT_PROMPT "sm_text> "
#define CHOICES_MAX 4

typedef struct node_s node_t;

typedef struct {
	int symbol;
	node_t *next;
}
letter_t;

typedef struct {
	unsigned long input_idx;
	unsigned long val;
}
score_t;

struct node_s {
	unsigned long letters_n;
	letter_t *letters;
	unsigned long scores_n;
	score_t *scores;
};

typedef struct {
	node_t *node;
	unsigned long level;
}
ngram_t;

int load_corpus(char *);
void print_trie(node_t *, unsigned long);
void sm_text(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
int sm_text_next_node(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
letter_t *new_letter(node_t *, int);
letter_t *get_letter(node_t *, int);
void set_letter(letter_t *, int);
void reset_scores(node_t *);
score_t *new_score(node_t *, unsigned long, unsigned long);
score_t *get_score(node_t *, unsigned long);
void set_score(score_t *, unsigned long, unsigned long);
node_t *new_node(void);
void free_node(node_t *);
void reset_ngrams(void);
void shift_ngrams(void);
void copy_ngrams(ngram_t *, ngram_t *);
int ngram_set_word_end(ngram_t *);
int ngram_set_symbol(ngram_t *, int);
void ngram_get_word_end(ngram_t *);
void ngram_get_symbol(ngram_t *, int);
void set_ngram(ngram_t *, node_t *, unsigned long);

int input[INPUT_LEN_MAX], tree[] = { ' ', 'e', 't', 'i', 'a', 'n', 'm', 's', 'u', 'r', 'w', 'd', 'k', 'g', 'o', 'h', 'v', 'f', '?', 'l', '?', 'p', 'j', 'b', 'x', 'c', 'y', 'z', 'q', '?', '?' }, output[INPUT_LEN_MAX*2-1];
unsigned long ngrams_size, best_score_val;
node_t *trie_root;
ngram_t *ngrams, *stack;

int main(int argc, char *argv[]) {
	char *end, buffer[BUFFER_SIZE];
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <size of ngrams> <path to corpus>\n", argv[0]);
		fflush(stderr);
		return EXIT_FAILURE;
	}
	ngrams_size = strtoul(argv[1], &end, 10);
	if (*end || ngrams_size < 1) {
		fprintf(stderr, "<size of ngrams> must be greater than 0\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	ngrams = malloc(sizeof(ngram_t)*ngrams_size);
	if (!ngrams) {
		fprintf(stderr, "Could not allocate memory for ngrams\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	if (!load_corpus(argv[2])) {
		free(ngrams);
		return EXIT_FAILURE;
	}
	printf(INPUT_PROMPT);
	fflush(stdout);
	while (fgets(buffer, BUFFER_SIZE, stdin)) {
		unsigned long input_len;
		for (input_len = 0; input_len < INPUT_LEN_MAX && (buffer[input_len] == '.' || buffer[input_len] == '-'); input_len++) {
			input[input_len] = (int)buffer[input_len];
		}
		if (buffer[0] == 'q' && buffer[1] == '\n') {
			break;
		}
		if (buffer[input_len] != '\n') {
			fprintf(stderr, "Invalid morse string\n");
			fflush(stderr);
			input_len = 0;
		}
		buffer[input_len] = '\0';
		if (input_len > 0) {
			stack = malloc(sizeof(ngram_t)*ngrams_size*input_len);
			if (!stack) {
				fprintf(stderr, "Could not allocate memory for stack\n");
				fflush(stderr);
				free_node(trie_root);
				free(ngrams);
				return EXIT_FAILURE;
			}
			puts(buffer);
			fflush(stdout);
			reset_scores(trie_root);
			reset_ngrams();
			best_score_val = ULONG_MAX;
			sm_text(0UL, 0UL, input_len, 0UL, 0UL);
			free(stack);
		}
		printf(INPUT_PROMPT);
		fflush(stdout);
	}
	free_node(trie_root);
	free(ngrams);
	return EXIT_SUCCESS;
}

int load_corpus(char *path_to_corpus) {
	int c;
	FILE *fd = fopen(path_to_corpus, "r");
	if (!fd) {
		fprintf(stderr, "Could not open corpus\n");
		fflush(stderr);
		return 0;
	}
	trie_root = new_node();
	if (!trie_root) {
		fclose(fd);
		return 0;
	}
	reset_ngrams();
	c = fgetc(fd);
	while (!feof(fd)) {
		int symbol = tolower(c);
		if (isspace(symbol)) {
			if (ngrams->node != trie_root) {
				unsigned long i;
				for (i = 0; i < ngrams_size; i++) {
					if (!ngram_set_word_end(ngrams+i)) {
						free_node(trie_root);
						fclose(fd);
						return 0;
					}
				}
				shift_ngrams();
			}
		}
		else if (ispunct(symbol)) {
			if (symbol != '\'' && ngrams->node != trie_root) {
				unsigned long i;
				for (i = 0; i < ngrams_size; i++) {
					if (!ngram_set_word_end(ngrams+i)) {
						free_node(trie_root);
						fclose(fd);
						return 0;
					}
				}
				reset_ngrams();
			}
		}
		else if (isalnum(symbol)) {
			unsigned long i;
			for (i = 0; i < ngrams_size; i++) {
				if (!ngram_set_symbol(ngrams+i, symbol)) {
					free_node(trie_root);
					fclose(fd);
					return 0;
				}
			}
		}
		else {
			fprintf(stderr, "Unexpected character %c in corpus\n", c);
			fflush(stderr);
		}
		c = fgetc(fd);
	}
	fclose(fd);
	return 1;
}

void print_trie(node_t *node, unsigned long output_idx) {
	unsigned long i;
	if (node->letters_n == 0) {
		unsigned long j;
		for (j = 0; j < output_idx; j++) {
			putchar(output[j]);
		}
		puts("");
	}
	for (i = 0; i < node->letters_n; i++) {
		if (node->letters[i].symbol == '\n') {
			output[output_idx] = ' ';
			print_trie(node->letters[i].next, output_idx+1);
		}
		else {
			output[output_idx] = node->letters[i].symbol;
			print_trie(node->letters[i].next, output_idx+1);
		}
	}
}

void sm_text(unsigned long ngram_idx, unsigned long input_idx, unsigned long input_len, unsigned long output_idx, unsigned long score_val) {
	int choices[CHOICES_MAX];
	unsigned long choices_max, tree_idx, choices_n, i;
	if (input_idx == input_len) {
		score_val += ngrams_size-ngram_idx;
		if (get_letter(ngrams[ngram_idx].node, '\n') && score_val < best_score_val) {
			unsigned long j;
			printf("Score %lu ", score_val);
			for (j = 0; j < output_idx; j++) {
				putchar(output[j]);
			}
			puts("");
			fflush(stdout);
			best_score_val = score_val;
		}
		return;
	}
	choices_max = input_len-input_idx;
	if (choices_max > CHOICES_MAX) {
		choices_max = CHOICES_MAX;
	}
	tree_idx = 0;
	choices_n = 0;
	for (i = input_idx; i < input_idx+choices_max && tree[tree_idx] != '?'; i++) {
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
			return;
		}
		if (tree[tree_idx] != '?') {
			choices[choices_n++] = tree[tree_idx];
		}
	}
	for (i = 0; i < choices_n; i++) {
		letter_t *letter = get_letter(ngrams[ngram_idx].node, choices[i]);
		if (letter) {
			unsigned long stack_idx = input_idx*ngrams_size, j;
			copy_ngrams(ngrams, stack+stack_idx);
			for (j = 0; j < ngrams_size; j++) {
				ngram_get_symbol(ngrams+j, letter->symbol);
			}
			output[output_idx] = letter->symbol;
			sm_text(ngram_idx, input_idx+i+1, input_len, output_idx+1, score_val);
			copy_ngrams(stack+stack_idx, ngrams);
		}
	}
	if (get_letter(ngrams[ngram_idx].node, '\n')) {
		unsigned long stack_idx = input_idx*ngrams_size, j;
		copy_ngrams(ngrams, stack+stack_idx);
		for (j = 0; j < ngrams_size; j++) {
			ngram_get_word_end(ngrams+j);
		}
		shift_ngrams();
		for (j = ngrams_size-1; j > 0; j--) {
			if (ngrams[j].level == j) {
				unsigned long k;
				if (j <= ngram_idx) {
					if (!sm_text_next_node(j, input_idx, input_len, output_idx, score_val+ngrams_size-ngram_idx)) {
						return;
					}
				}
				else {
					if (!sm_text_next_node(j, input_idx, input_len, output_idx, score_val)) {
						return;
					}
				}
				for (k = j; k < ngrams_size; k++) {
					set_ngram(ngrams+k, ngrams[j-1].node, j-1);
				}
			}
		}
		if (!sm_text_next_node(0UL, input_idx, input_len, output_idx, score_val+ngrams_size-ngram_idx)) {
			return;
		}
		copy_ngrams(stack+stack_idx, ngrams);
	}
}

int sm_text_next_node(unsigned long ngram_idx, unsigned long input_idx, unsigned long input_len, unsigned long output_idx, unsigned long score_val) {
	score_t *score = get_score(ngrams[ngram_idx].node, input_idx);
	if (!score || score_val < score->val) {
		output[output_idx] = ' ';
		sm_text(ngram_idx, input_idx, input_len, output_idx+1, score_val);
	}
	score = get_score(ngrams[ngram_idx].node, input_idx);
	if (!score || score_val < score->val) {
		if (score) {
			score->val = score_val;
		}
		else {
			if (!new_score(ngrams[ngram_idx].node, input_idx, score_val)) {
				return 0;
			}
		}
	}
	return 1;
}

letter_t *new_letter(node_t *node, int symbol) {
	if (node->letters_n > 0) {
		letter_t *letters = realloc(node->letters, sizeof(letter_t)*(node->letters_n+1));
		if (!letters) {
			fprintf(stderr, "Could not reallocate memory for letters\n");
			fflush(stderr);
			free(node->letters);
			node->letters_n = 0;
			return NULL;
		}
		node->letters = letters;
	}
	else {
		node->letters = malloc(sizeof(letter_t));
		if (!node->letters) {
			fprintf(stderr, "Could not allocate memory for letters\n");
			fflush(stderr);
			return NULL;
		}
	}
	set_letter(node->letters+node->letters_n, symbol);
	node->letters_n++;
	return node->letters+node->letters_n-1;
}

letter_t *get_letter(node_t *node, int symbol) {
	unsigned long i;
	for (i = 0; i < node->letters_n; i++) {
		if (node->letters[i].symbol == symbol) {
			return node->letters+i;
		}
	}
	return NULL;
}

void set_letter(letter_t *letter, int symbol) {
	letter->symbol = symbol;
	letter->next = NULL;
}

score_t *new_score(node_t *node, unsigned long input_idx, unsigned long val) {
	if (node->scores_n > 0) {
		score_t *scores = realloc(node->scores, sizeof(score_t)*(node->scores_n+1));
		if (!scores) {
			fprintf(stderr, "Could not reallocate memory for scores\n");
			fflush(stderr);
			free(node->scores);
			node->scores_n = 0;
			return NULL;
		}
		node->scores = scores;
	}
	else {
		node->scores = malloc(sizeof(score_t));
		if (!node->scores) {
			fprintf(stderr, "Could not allocate memory for scores\n");
			fflush(stderr);
			return NULL;
		}
	}
	set_score(node->scores+node->scores_n, input_idx, val);
	node->scores_n++;
	return node->scores+node->scores_n-1;
}

void reset_scores(node_t *node) {
	if (node->letters_n > 0) {
		unsigned long i;
		for (i = 0; i < node->letters_n; i++) {
			if (node->letters[i].next) {
				reset_scores(node->letters[i].next);
			}
		}
	}
	if (node->scores_n > 0) {
		node->scores_n = 0;
		free(node->scores);
		node->scores = NULL;
	}
}

score_t *get_score(node_t *node, unsigned long input_idx) {
	unsigned long i;
	for (i = 0; i < node->scores_n; i++) {
		if (node->scores[i].input_idx == input_idx) {
			return node->scores+i;
		}
	}
	return NULL;
}

void set_score(score_t *score, unsigned long input_idx, unsigned long val) {
	score->input_idx = input_idx;
	score->val = val;
}

node_t *new_node(void) {
	node_t *node = malloc(sizeof(node_t));
	if (!node) {
		fprintf(stderr, "Could not allocate memory for node\n");
		fflush(stderr);
		return NULL;
	}
	node->letters_n = 0;
	node->letters = NULL;
	node->scores_n = 0;
	node->scores = NULL;
	return node;
}

void free_node(node_t *node) {
	if (node->letters_n > 0) {
		unsigned long i;
		for (i = 0; i < node->letters_n; i++) {
			if (node->letters[i].next) {
				free_node(node->letters[i].next);
			}
		}
		free(node->letters);
	}
	if (node->scores_n > 0) {
		free(node->scores);
	}
	free(node);
}

void reset_ngrams(void) {
	unsigned long i;
	for (i = 0; i < ngrams_size; i++) {
		set_ngram(ngrams+i, trie_root, 0UL);
	}
}

void shift_ngrams(void) {
	unsigned long i;
	for (i = ngrams_size-1; i > 0; i--) {
		if (ngrams[i].level == i+1) {
			set_ngram(ngrams+i, ngrams[i-1].node, i);
		}
	}
	set_ngram(ngrams, trie_root, 0UL);
}

void copy_ngrams(ngram_t *from, ngram_t *to) {
	unsigned long i;
	for (i = 0; i < ngrams_size; i++) {
		to[i] = from[i];
	}
}

int ngram_set_word_end(ngram_t *ngram) {
	if (!ngram_set_symbol(ngram, '\n')) {
		return 0;
	}
	ngram->level++;
	return 1;
}

int ngram_set_symbol(ngram_t *ngram, int symbol) {
	letter_t *letter = get_letter(ngram->node, symbol);
	if (!letter) {
		letter = new_letter(ngram->node, symbol);
		if (!letter) {
			return 0;
		}
	}
	ngram->node = letter->next;
	if (!ngram->node) {
		ngram->node = new_node();
		if (!ngram->node) {
			return 0;
		}
		letter->next = ngram->node;
	}
	return 1;
}

void ngram_get_word_end(ngram_t *ngram) {
	ngram_get_symbol(ngram, '\n');
	ngram->level++;
}

void ngram_get_symbol(ngram_t *ngram, int symbol) {
	letter_t *letter = get_letter(ngram->node, symbol);
	ngram->node = letter->next;
}

void set_ngram(ngram_t *ngram, node_t *node, unsigned long level) {
	ngram->node = node;
	ngram->level = level;
}
