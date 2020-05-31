#include <stdlib.h>
#include "threads.h"

/* The only purpose of this file is to do 
 * some configuration if required before
 * calling the actual rosace main function
 */

int main(int argc, char* argv[]){
  long tsimu=300*200;
  if (argc>1) {
    tsimu = atoi(argv[1])*200;
  }
  max_step_simu = tsimu;
  return run_rosace();
}

