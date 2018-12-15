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
    int goodOrBad;
    int done;
    char password_attempt[100];
} synchronizer_t;

synchronizer_t s;

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
    while (s->done == 0)  //
    	pthread_cond_wait(&s->cond, &s->lock);
    pthread_mutex_unlock(&s->lock);
}

void* worker(void * arg) {
    char pass[100];

    printf("Password: ");
    scanf("%99s", pass);

    pthread_mutex_lock(&(s.lock));
    strcpy(s.password_attempt, pass);
    pthread_mutex_unlock(&(s.lock));
    signal_done(&s);

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p;
    signal_init(&s);
    while (s.goodOrBad == 0) {
      Pthread_create(&p, NULL, worker, NULL);
      Pthread_join(p, NULL);
      signal_wait(&s);
      if (strcmp(s.password_attempt, "crispy-bacon") == 0) {
        s.goodOrBad = 1;
        printf(" %s is correct, logging in...\n", s.password_attempt);
      }
      else
        printf(" %s is incorrect.\n", s.password_attempt);
    }

    return 0;
}
