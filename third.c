#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "third.h"

#define STRING_STORAGE_SIZE (2500)
#define MAIN_CORE_SIZE      (2500)
#define STACK_SIZE          (500)

struct third_obj {
	FILE *input, *output;
	char s[STRING_STORAGE_SIZE];
	int m[MAIN_CORE_SIZE], L, I, T[STACK_SIZE], *S, t, w, f, x;
};

#define I tobj->I
#define t tobj->t
#define w tobj->w
#define f tobj->f

enum primitives {
	PUSH, COMPILE, RUN, DEFINE, IMMEDIATE, READ, LOAD, STORE, SUBTRACT, ADD,
	MULTIPLY, DIVIDE, LESSZ, EXIT, EMIT, KEY, FROMR, TOR, BRANCH, NBRANCH,
	PRINTNUM, QUOTE, COMMA, NOT, EQUAL, SWAP, DUP, DROP, TAIL, LAST_ENUM
};

static const char *names[] = { 
	"@", "!", "-", "+", "*", "/", "<0", "exit", "emit", "key", "r>",
	">r", "jmp", "jmpz", ".", "'", ",", "not", "=", "swap", "dup",
	"drop", "tail", NULL
};

static void compile_word(third_obj_t * tobj, int code, int flag, const char *str)
{
	char *s;
	int *m;
	assert(tobj);
	assert(code < LAST_ENUM);
	assert(t < STRING_STORAGE_SIZE);
	assert((flag && str) || (!flag && !str));
	m = tobj->m;
	s = tobj->s;

	m[m[0]++] = tobj->L;
	tobj->L = *m - 1;
	m[m[0]++] = t;
	m[m[0]++] = code;
	if (flag)
		strcpy(s + t, str);
	else
		fscanf(tobj->input, "%s", s + t);
	t += strlen(s + t) + 1;
	return;
}

third_obj_t *third_initialize(FILE * input, FILE * output)
{
	int *m, i;
	third_obj_t *tobj;
	assert(input && output);

	tobj = calloc(1, sizeof(struct third_obj));
	if(!tobj)
		return NULL;
	i = 0;
	m = tobj->m;
	tobj->input = input;
	tobj->output = output;

	m[0] = 32;
	tobj->L = 1;
	I = 0;
	tobj->S = tobj->T;
	t = 64;
	w = 0;
	f = 0;
	tobj->x = 0;

	compile_word(tobj, DEFINE, true, ":");
	compile_word(tobj, IMMEDIATE, true, "immediate");
	compile_word(tobj, COMPILE, true, "read");
	w = *m;
	m[m[0]++] = READ;
	m[m[0]++] = RUN;
	I = *m;
	m[m[0]++] = w;
	m[m[0]++] = I - 1;
	w = LOAD;
	
	for(i = 0; names[i]; i++) {
		compile_word(tobj, COMPILE, true, names[i]);
		m[m[0]++] = w++;
	}

	m[1] = *m;
	*m += 512;
	return tobj;
}

void third_free(third_obj_t *tobj)
{
	assert(tobj);
	free(tobj);
}

int third_run(third_obj_t * tobj)
{
	char *s = tobj->s;
	int *m = tobj->m, *x = &tobj->x, *S = tobj->S;
	assert(tobj);
	while (true) {
		*x = m[I++];
 INNER:
		assert(I > 0 && I < MAIN_CORE_SIZE);
		assert(m[0] > 0 && m[0] < MAIN_CORE_SIZE);
		assert(m[1] > 0 && m[1] < STACK_SIZE);

		switch (m[(*x)++]) {
		case PUSH:      *++S = f; f = m[I++];                     break;
		case COMPILE:   m[m[0]++] = *x;                           break;
		case RUN:       m[++m[1]] = I; I = *x;                    break;
		case IMMEDIATE: *m -= 2; m[m[0]++] = RUN;                 break;
		case LOAD:      f = m[f];                                 break;
		case STORE:     m[f] = *S--; f = *S--;                    break;
		case SUBTRACT:  f = *S-- - f;                             break;
		case ADD:       f = *S-- + f;                             break;
		case MULTIPLY:  f *= *S--;                                break;
		case DIVIDE:    f = *S-- / f;                             break;
		case LESSZ:     f = 0 > f;                                break;
		case EXIT:      I = m[m[1]--];                            break;
		case EMIT:      fputc(f, tobj->output); f = *S--;         break;
		case KEY:       *++S = f; f = fgetc(tobj->input);         break;
		case FROMR:     *++S = f; f = m[m[1]--];                  break;
		case TOR:       m[++m[1]] = f; f = *S--;                  break;
		case BRANCH:    I += m[I];                                break;
		case NBRANCH:   I += f ? 1 : m[I]; f = *S--;              break;
		case PRINTNUM:  fprintf(tobj->output, "%d", f); f = *S--; break;
		case QUOTE:     *++S = f; f = m[I++];                     break;
		case COMMA:     m[m[0]++] = f; f = *S--;                  break;
		case NOT:       f = !f;                                   break;
		case EQUAL:     f = *S-- == f;                            break;
		case SWAP:      w = f; f = *S--; *++S = w;                break;
		case DUP:       *++S = f;                                 break;
		case DROP:      f = *S--;                                 break;
		case TAIL:      m[1]--;                                   break;
		case DEFINE:
			m[8] = 1;
			compile_word(tobj, COMPILE, false, NULL);
			m[m[0]++] = RUN;
			break;
		case READ:
			m[1]--;
			if(fscanf(tobj->input, "%s", s) < 1)
				return 0;
			for (w = tobj->L; strcmp(s, &s[m[w + 1]]); w = m[w]) 
				;
			if (w - 1) {
				*x = w + 2;
				if (m[8] == 0 && m[*x] == COMPILE)
					(*x)++;
				goto INNER;
			} else {
				if (m[8] != 0) {
					m[m[0]++] = 2;
					m[m[0]++] = atoi(s);
				} else {
					*++S = f;
					f = atoi(s);
				}
			}
			break;
		default:
			fprintf(stderr, "Unknown instruction\n");
			return -1;
		}
	}
}

