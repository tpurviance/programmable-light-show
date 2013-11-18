#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define STRAND_LEN 160 // Number of LEDs on strand

static FILE *grb_fp;

void clear() {
  int i;
  for (i = 0; i < STRAND_LEN; i++) {
    fprintf(grb_fp, "0 0 0\n");
  }
}

void pattern1() {
  int i;
  unsigned char data[3];
  srand(time(NULL));

  for (i = 0; i < STRAND_LEN; i++) {
    data[0] = rand() % 0x7F;
    data[1] = rand() % 0x7F;
    data[2] = rand() % 0x7F;
    
    fprintf(grb_fp, "%d %d %d\n", data[0], data[1], data[2]);
    usleep(20000);
  }
}

void pattern2() {
  int i, j;
  unsigned char g, r, b;
  unsigned char data[STRAND_LEN * 3];
  srand(time(NULL));
  
  for (i = 0; i < 20; i++) {
    g = rand() % 0x7F;
    r = rand() % 0x7F;
    b = rand() % 0x7F;
    for (j = 0; j < STRAND_LEN * 3; j += 3) {
      data[j] = g;
      data[j+1] = r;
      data[j+2] = b;
    }
    for (j = 0; j < STRAND_LEN * 3; j += 3) {
      fprintf(grb_fp, "%d %d %d\n", data[j], data[j+1], data[j+2]);
    }
    usleep(200000);
  }
}

//signal handler that breaks program loop and cleans up
void signal_handler(int signo){
  if (signo == SIGINT) {
    printf("\n^C pressed, cleaning up and exiting..\n");
    fflush(stdout);
    fclose(grb_fp);
    exit(0);
  }
}

int main() { 
  grb_fp = fopen("/sys/firmware/lpd8806/device/grb", "w");
  setbuf(grb_fp, NULL);
  if (grb_fp == NULL) {
    return 1;
  }
  
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    printf("Error with SIGINT handler\n");
    return 1;
  }
  
  while (1) {
    clear();
    pattern1();
    clear();
    pattern2();
  }
}
