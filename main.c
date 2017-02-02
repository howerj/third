#include <stdio.h>
#include "third.h"

int main(void)
{
	third_obj_t *tobj = third_initialize(stdin, stdout);
	if(!tobj)
		return -1;
	third_run(tobj);
	third_free(tobj);
	return 0;
}

