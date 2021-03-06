/**

  Barrier implementation.

**/

#include <pthread.h>
#include <stdio.h> // printf
#include <semaphore.h> // sem_t
#include <errno.h> // errno
#include <assert.h>

int enter_barrier(int id, pthread_mutex_t *b_mtx, pthread_cond_t *b_cond_var, int *b_counter, const int thresh, int *b_is_closed) {
  assert(b_mtx);
  assert(b_cond_var);
  assert(b_counter);

///!!!! This is a double walled barrier where are the first Wall is initially
///!!!! open and the second wall is initially closed. the Walls of The barrier are
///!!!! always in opposite States, the state of the walls Flip each time all
///!!!! threats are inside the barrier.

    // main lock
  if(pthread_mutex_lock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  if(*b_counter == 0) { // 1st wall open close
    printf("#%d: Broadcast >> FIRST. Closing barrier. b_counter = %d.\n", id, *b_counter);
    *b_is_closed = 1;
    if(pthread_cond_broadcast(b_cond_var) != 0) {
      printf("%s\n", strerror(errno));
      assert(0);
    }
  }

  // main unlock
  if(pthread_mutex_unlock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  // main lock
  if(pthread_mutex_lock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  while (!*b_is_closed) { // 1st wall
    printf("#%d: Barrier is already open, waiting for next round to complete... *b_counter = %d.\n", id, *b_counter);
    // cond. var. wait.
    if (pthread_cond_wait(b_cond_var, b_mtx) != 0) {
      printf("%s\n", strerror(errno));
      assert(0);
    }

  }

  // main unlock
  if(pthread_mutex_unlock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }


///!!!!!

  // main lock
  if(pthread_mutex_lock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  printf("#%d: BEFORE inc. b_counter = %d.\n", id, *b_counter);

  *b_counter += 1; // t counter update

  printf("#%d: AFTER inc. b_counter = %d.\n", id, *b_counter);

  // main unlock
  if(pthread_mutex_unlock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  return 0;
}

int exit_barrier(int id, pthread_mutex_t *b_mtx, pthread_cond_t *b_cond_var, int *b_counter, const int thresh, int *b_is_closed) {
  assert(b_mtx);
  assert(b_cond_var);
  assert(b_counter);
  /// WAIT for the barrier to open. ///

  // main lock
  if(pthread_mutex_lock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  printf("#%d: I'm at the barrier wall.\n", id);


  while ((*b_counter < thresh) && *b_is_closed) { // 2nd wall open close
    printf("#%d: Waiting for barrier to open... %d < %d.\n", id, *b_counter, thresh);
    // cond. var. wait.
    if (pthread_cond_wait(b_cond_var, b_mtx) != 0) {
      printf("%s\n", strerror(errno));
      assert(0);
    }

  }

  printf("#%d: THE BARRIER IS DOWN!\n", id);

  // main unlock
  if(pthread_mutex_unlock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  /// The last T does the first signal, then all subsequent signals the guy behind him.

  // main lock
  if(pthread_mutex_lock(b_mtx) != 0) { // Needed?
    printf("%s\n", strerror(errno));
    assert(0);
  }

  if(*b_counter == thresh) { // 2nd wall open close
    printf("#%d: Broadcast >> Opening barrier!!! b_counter = %d.\n", id, *b_counter);
    *b_is_closed = 0;
    if(pthread_cond_broadcast(b_cond_var) != 0) {
      printf("%s\n", strerror(errno));
      assert(0);
    }
  }

  if(!*b_is_closed && *b_counter > 0) { // t Count update out
    *b_counter -= 1;
    printf("#%d: Decrementing b_counter = %d.\n", id, *b_counter);
  }

  if(*b_counter == 0) {
    printf("#%d: LAST. b_counter = %d.\n", id, *b_counter);
  }

  // main unlock
  if(pthread_mutex_unlock(b_mtx) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  return 0;
}

// Sync. decls.
static sem_t entry_gate_sem;

static pthread_mutex_t mutex;
static pthread_cond_t  cond_var;
static int is_closed;

static int inside_barrier_count; // Counts num. threads inside the barrier walls.
static int barrier_threshold_count; // Num. of threads at which the barrier is released/opened.

/**

**/
int barrier_point(int id) {

  printf("#%d: IN TURNSTILE!\n", id);

  if (sem_wait(&entry_gate_sem) != 0) { // Exactly N T's make it inside the barrier walls at a time.
    printf("%s\n", strerror(errno));
    assert(0);
  }

  printf("#%d: At entry!\n", id);
  enter_barrier(id, &mutex, &cond_var, &inside_barrier_count, barrier_threshold_count, &is_closed);
  exit_barrier(id, &mutex, &cond_var, &inside_barrier_count, barrier_threshold_count, &is_closed);

  printf("\n#%d: OUT TURNSTILE!\n", id);

  if (sem_post(&entry_gate_sem) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  return 0;
}

// Cleanup state before terminating.
void cleanup_state (void) {

  if(pthread_mutex_destroy(&mutex) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  if(pthread_cond_destroy(&cond_var) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  if (sem_destroy(&entry_gate_sem) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

}

int init(int N) {

  assert(N > 0);

  is_closed = 1;
  inside_barrier_count = 0;
  barrier_threshold_count = N;

  // Init. mutex
  if(pthread_mutex_init(&mutex, NULL) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  if (pthread_cond_init(&cond_var, NULL) != 0) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  // Create an "unnamed" semaphore, flags = 0, init. value = NUM_THREADS.
  if (sem_init(&entry_gate_sem, 0, N) == -1) {
    printf("%s\n", strerror(errno));
    assert(0);
  }

  return 0;
}