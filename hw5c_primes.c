#include <stdio.h>
#include <string.h>
#include "mythreads.h"

//
// simple synchronizer: allows one thread to wait for another
// structure "synchronizer_t" has all the needed data
// methods are:
//   init (called by one thread)
//   wait (to wait for a thread)
//   done (to indicate thread is done)
//
typedef struct __synchronizer_t {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int done;
} synchronizer_t;

synchronizer_t s;
int isPrime(long int x);

pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;
int k; int n;
int maxNum; // keeping track of highest prime
int totalCount; // keeping track of total amount of primes

void signal_init(synchronizer_t *s) {
    pthread_mutex_init(&s->lock, NULL);
    pthread_cond_init(&s->cond, NULL);
    s->done = 0;
}

void signal_done(synchronizer_t *s) {
    pthread_mutex_lock(&s->lock);
    s->done = 1;
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->lock);
}

void signal_wait(synchronizer_t *s) {
    pthread_mutex_lock(&s->lock);
    while (s->done == 0)
    	pthread_cond_wait(&s->cond, &s->lock);
    pthread_mutex_unlock(&s->lock);
}
typedef struct {
  int num;
} prime_struct;

void * worker(void * a) {
  prime_struct * arg = a;
  int threadNo = arg->num;
  int count = 0;
  int div = n / k;
  int max = threadNo * div;
  for(int i = (max - div) + 1; i <= max; i++) {
    if (isPrime(i)) {
      count++;
      if (i > maxNum)
        maxNum = i;
    }
  }
  return (void *) count;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
  	fprintf(stderr, "Usage: ./a.out <max> <# threads>\n");
	  return 0;
  }
  n = atoi(argv[1]);
  k = atoi(argv[2]);
  if (k > 20) {
    printf("setting k = 20...\n");
    k = 20;
  }
  totalCount = 0;
  int i;
  pthread_t id[k];
  void * status;
  for (i = 0; i < k; i++) {
    prime_struct * arg = malloc(sizeof *arg);
    arg->num = i+1;
    Pthread_create(&id[i], NULL, worker, arg);
  }
  for (i = 0; i < k; i++) {
    Pthread_join(id[i], &status);
    pthread_mutex_lock(&counter_lock);
    totalCount += (int)status;
    pthread_mutex_unlock(&counter_lock);
  }

  printf("highest prime - %d\n", maxNum);
  printf("total number of primes <= %d - %d\n", n, totalCount);
  return 0;
}

int isPrime(long int x) {
  if (x < 2) return 0;
  if (x == 2) return 1;
  if (x % 2 == 0) return 0;
  for(long int i=3; i*i <= x; i += 2)
    if (x % i == 0) return 0;
  return 1;
}

