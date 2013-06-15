#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*Current dictionary pointer is at m[0].*/
#define c m[m[0]++]

char s[2500];
int m[2500] = { 32 }, L = 1, I, T[500], *S = T, t = 64, w, f, x;

struct third_obj{
  FILE *input, *output;
  char s[2500];
  int m[2500],L,I,T[500],*S,t,w,f,x;
};

typedef struct third_obj third_obj_t;

enum primitives{
  PUSH,COMPILE,RUN,DEFINE,IMMEDIATE,READ,LOAD,STORE,SUBTRACT,ADD,
  MULTIPLY,DIVIDE,LESSZ,EXIT,EMIT,KEY,FROMR,TOR,BRANCH,NBRANCH,PRINTNUM,
  QUOTE,COMMA,NOT,EQUAL,SWAP,DUP,DROP,TAIL,LAST_ENUM
};

enum bool{false,true};

void compile_word(third_obj_t *tobj, int code, int flag, char *str)
{
        c=L;
        L = *m - 1;
        c=t;
        c=code;
        if(flag)
          strcpy(s+t,str);
        else
          fscanf(tobj->input,"%s", s + t);
        t += strlen(s + t) + 1;
        return;
}

int init(third_obj_t *tobj, FILE *input, FILE *output){
        tobj->input = input;
        tobj->output = output;

        m[0]=32;
        L=1;
        I=0;
        S=T;
        t=64;
        w=0;
        f=0;
        x=0;

        compile_word(tobj,DEFINE,true,":");
        compile_word(tobj,IMMEDIATE,true,"immediate");
        compile_word(tobj,COMPILE,true,"read");
        w = *m;
        c=READ;
        c=RUN;
        I = *m;
        c=w;
        c=I - 1;
        w = LOAD;

        compile_word(tobj,COMPILE,true,"@"); c = w++;
        compile_word(tobj,COMPILE,true,"!"); c = w++;
        compile_word(tobj,COMPILE,true,"-"); c = w++;
        compile_word(tobj,COMPILE,true,"+"); c = w++;
        compile_word(tobj,COMPILE,true,"*"); c = w++;
        compile_word(tobj,COMPILE,true,"/"); c = w++;
        compile_word(tobj,COMPILE,true,"<0"); c = w++;
        compile_word(tobj,COMPILE,true,"exit"); c = w++;
        compile_word(tobj,COMPILE,true,"emit"); c = w++;
        compile_word(tobj,COMPILE,true,"key"); c = w++;
        compile_word(tobj,COMPILE,true,"r>"); c = w++;
        compile_word(tobj,COMPILE,true,">r"); c = w++;
        compile_word(tobj,COMPILE,true,"jmp"); c = w++;
        compile_word(tobj,COMPILE,true,"jmpz"); c = w++;
        compile_word(tobj,COMPILE,true,"."); c = w++;
        compile_word(tobj,COMPILE,true,"'"); c = w++;
        compile_word(tobj,COMPILE,true,","); c = w++;
        compile_word(tobj,COMPILE,true,"not"); c = w++;
        compile_word(tobj,COMPILE,true,"="); c = w++;
        compile_word(tobj,COMPILE,true,"swap"); c = w++;
        compile_word(tobj,COMPILE,true,"dup"); c = w++;
        compile_word(tobj,COMPILE,true,"drop"); c = w++;
        compile_word(tobj,COMPILE,true,"tail"); c = w++;

        m[1] = *m;
        *m += 512;
        return I;
}

void run(third_obj_t *tobj)
{
  while(true){
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
                    compile_word(tobj,COMPILE,false,NULL);
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
                    fputc(f,tobj->output);
                    f = *S--;
                    break;
            case KEY:
                    *++S = f;
                    f = fgetc(tobj->input);
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
                    fprintf(tobj->output,"%d",f);
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
  third_obj_t *tobj = calloc(1,sizeof(struct third_obj));
  init(tobj,stdin,stdout);
  run(tobj);
  return 0;
}
