#include <stdio.h>
#include "kversion.h"


int main(int argc, char* argv[])
{

  krnlver kvi;
    long kv = kversion_get(&kvi);


    fprintf(stderr,"Running on kernel %u.%u(%u)\n",
				kvi.major,kvi.minor,kvi.release);

    return 0;
}
