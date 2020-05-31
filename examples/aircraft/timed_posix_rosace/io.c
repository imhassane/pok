#include <stdio.h>
#include "types.h"
#include "io.h"

#define FMTREAL "%5.15f"
#define BASE_FREQUENCY 200.0

extern REAL_TYPE h_c;

void ROSACE_update_altitude_command(REAL_TYPE h_cons){
  h_c = h_cons;
}

void ROSACE_write_outputs(output_t* v){
  static int first=1;
  if (first) {
    printf("# %15s, %15s, %15s, %15s, %15s, %15s, %15s, %15s\n",
           "T","Va","az","q","Vz","h","delta_th_c","delta_e_c");
    first = 0;
  }
  //printf("%3.4f, ", (v->t_simu)/BASE_FREQUENCY); // The time is already in ms here
  printf("%3.4f, ", (v->t_simu)/1000.0);
  printf(FMTREAL", ", v->sig_outputs.Va);
  printf(FMTREAL", ", v->sig_outputs.az);
  printf(FMTREAL", ", v->sig_outputs.q);
  printf(FMTREAL", ", v->sig_outputs.Vz);
  printf(FMTREAL", ", v->sig_outputs.h);
  printf(FMTREAL", ", v->sig_delta_th_c);
  printf(FMTREAL"\n", v->sig_delta_e_c);
}
