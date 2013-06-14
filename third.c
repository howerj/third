#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*Current dictionary pointer is at m[0].*/
#define c m[m[0]++]

char s[2500];
int m[2500] = { 32 }, L = 1, I, T[500], *S = T, t = 64, w, f, x;

enum primitives{
  PUSH,COMPILE,RUN,DEFINE,IMMEDIATE,READ,LOAD,STORE,SUBTRACT,ADD,
  MULTIPLY,DIVIDE,LESSZ,EXIT,EMIT,KEY,FROMR,TOR,BRANCH,NBRANCH,PRINTNUM,
  QUOTE,COMMA,NOT,EQUAL,SWAP,DUP,DROP,TAIL,LAST_ENUM
};

void compile_word(int code)
{
        c=L;
        L = *m - 1;
        c=t;
        c=code;
        scanf("%s", s + t);
        t += strlen(s + t) + 1;
        return;
}

int init(void){
        compile_word(DEFINE);
        compile_word(IMMEDIATE);
        compile_word(COMPILE); /*defining _read, a non-immediate word*/
        w = *m;
        c=READ;
        c=RUN;
        I = *m;
        c=w;
        c=I - 1;
        for (w = LOAD; w < LAST_ENUM;)
                compile_word(COMPILE), c=w++;
        m[1] = *m;
        *m += 512;
        return I;
}

void run(void)
{
  while(1){
    x = m[I++];
    INNER:
            switch (m[x++]) {
            case PUSH:
                    *++S = f;
                    f = m[I++];
                    break;
            case COMPILE:
                    c=x;
                    break;
            case RUN:
                    m[++m[1]] = I;
                    I = x;
                    break;
            case DEFINE:
                    m[8]=1;
                    compile_word(1);
                    c=RUN;
                    break;
            case IMMEDIATE:
                    *m -= 2;
                    c=RUN;
                    break;
            case READ:
                    m[1]--;
                    for (w = scanf("%s", s) < 1 ? exit(0), 0 : L; strcmp(s, &s[m[w + 1]]); w = m[w]) ;
                    if(w-1){
                      x=w+2;
                      if(m[8]==0){
                        if(m[x]==COMPILE)
                          x++;
                      }
                      goto INNER;
                    } else {
                      if(m[8]!=0){
                        c=2; c=atoi(s);
                      } else {
                        *++S = f;
                        f = atoi(s);
                      }
                    }
                    break;
            case LOAD:
                    f = m[f];
                    break;
            case STORE:
                    m[f] = *S--;
                    f = *S--;
                    break;
            case SUBTRACT:
                    f = *S-- - f;
                    break;
            case ADD:
                    f = *S-- + f;
                    break;
            case MULTIPLY:
                    f *= *S--;
                    break;
            case DIVIDE:
                    f = *S-- / f;
                    break;
            case LESSZ:
                    f = 0 > f;
                    break;
            case EXIT:
                    I = m[m[1]--];
                    break;
            case EMIT:
                    putchar(f);
                    f = *S--;
                    break;
            case KEY:
                    *++S = f;
                    f = getchar();
                    break;
            case FROMR:
                    *++S = f;
                    f = m[m[1]--];
                    break;
            case TOR:
                    m[++m[1]] = f;
                    f = *S--;
                    break;
            case BRANCH:
                    I+=m[I];
                    break;
            case NBRANCH:
                    if(f==0){
                      I+=m[I] ;
                    } else {
                      I++;
                    } 
                    f = *S--;
                    break;
            case PRINTNUM:
                    printf("%d",f);
                    f = *S--;
                    break;
            case QUOTE:
                    *++S=f;
                    f = m[I++];
                    break;
            case COMMA:
                    c = f;
                    f = *S--;
                    break;
            case NOT:
                    f=!f;
                    break;
            case EQUAL:
                    f = *S-- == f;
                    break;
            case SWAP:
                    w = f;
                    f = *S--;
                    *++S = w;
                    break;
            case DUP:
                    *++S = f;
                    break;
            case DROP:
                    f = *S--;
                    break;
            case TAIL:
                    m[1]--;
                    break;
            default:
                    fprintf(stderr,"Unknown instruction\n");
                    exit(1);
            }
  }
}

int main(void)
{
 /* int i;
  stdin = fopen("third.fs","r");
  i = init();
  run();
  freopen("/dev/stdin","r",stdin);
  I = i;*/
  init();
  run();
  return 0;
}
