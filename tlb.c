#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define PAGESIZE 4096

struct timeval start, stop;
long micros;

int main(int argc, char * argv[]) {
  struct timeval start, stop;
  start.tv_sec = 0;
  stop.tv_sec = 0;

  int numPagesToTouch = atoi(argv[1]);
  int numTrials = atoi(argv[2]);
  int totalCost = 0;

  if (argc < 3) { // not enough arguments from user
    fprintf(stderr, "Usage: ./a.out numPagesToTouch numTrials [silent]\n");
    printf("  silent suppresses all output except the numPagesToTouch, numTrials, total_s, avg access time in ns.\n");
    return 0;
  }

  else if (argc > 2) { // argument count is good
      int page = getpagesize();
      printf("Note: assuming page size of %d\n", PAGESIZE);
      printf("Run the following to confirm: getconf PAGESIZE\n");
      printf("Compile with -O0 flag to disable optimization.\n\n");

      printf("Allocating %d pages of %d bytes each\n", numPagesToTouch, PAGESIZE);

      int jump = PAGESIZE / sizeof(int); // distance to each next page
      int * x = calloc(numPagesToTouch, PAGESIZE);

		  for (int j = 1; j <= numTrials; j++) {  // going through the amount of trials
      	if (-1 == gettimeofday(&start,NULL)) { // starting timer, checking for error
        	fprintf(stderr, "Error starting the timer.\n");
        	return 0;
      	}
        for (int i = 0; i < numPagesToTouch * jump; i += jump) {
          x[i] += 1;
        }
      	if (-1 == gettimeofday(&stop, NULL)) { // stopping timer, checking for error
        	fprintf(stderr, "Error stopping the timer.\n");
        	return 0;
      	}
        micros = (((stop.tv_sec - start.tv_sec) * 1000000) + stop.tv_usec - start.tv_sec);
			}

      if (argc == 3) // normal printing case
        printf("testing\n");
      free(x);
      if (argc == 4) // silent printing case
        printf("%d, %d, x, %ld\n", numPagesToTouch, numTrials, micros); // x will be total_s
  }
  return 0;
}

