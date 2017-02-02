#ifndef THIRD_H
#define THIRD_H

#ifdef __cplusplus
extern "C" {
#endif

struct third_obj;
typedef struct third_obj third_obj_t;

third_obj_t *third_initialize(FILE * input, FILE * output);
int third_run(third_obj_t * tobj);
void third_free(third_obj_t * tobj);

#ifdef __cplusplus
}
#endif

#endif
