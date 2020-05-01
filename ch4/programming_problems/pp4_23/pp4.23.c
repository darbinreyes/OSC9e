/**

# Programming Problem 4.23.

## Start: From the Book

> Repeat Exercise 4.22, but instead of using a separate thread to generate random
> points, use OpenMP to parallelize the generation of points. Be careful not to
> place the calculcation of   in the parallel region, since you want to calculcate
> only once.

## End


* Plan:
  * Use /Users/darbinreyes/dev/private_dev/osc_textbook/OSC9e/ch4/openmp.c as guide for using openmp.

**/

#include <pthread.h>
#include <unistd.h> // sleep()
#include <stdio.h> // printf
#include <stdlib.h> // rand()
//#include <semaphore.h> // sem_t
#include <string.h> // strerr()
#include <errno.h> // errno
#include <assert.h>
#include <omp.h>

// Thread definitions.
#define NUM_RAND_POINTS (1 << 16)
void Worker_thread_func(void);
static int inside_circle_count;
static double total_points = (double) 0.0;

// Cleanup state before terminating.
void cleanup_state (void) {
}

// Init. program state.
int init_state(void) {
  FILE *fh;
  long my_seed = 0;

  //
  // Init. the random num. generator with a fresh seed.
  //
  fh = fopen("/dev/random", "r");

  if(fh == NULL) {
    assert(0);
    return -1;
  }

  if(fread((void*)&my_seed, sizeof(my_seed), 1, fh) != 1) {
    assert(0);
  }

  printf("Seed = %ld.\n", my_seed);

  srand48(my_seed);

  if(fclose(fh) != 0) {
    assert(0);
    return -1;
  }

  inside_circle_count = 0;

  return 0;
}

static double monte_carlo_estimate_pi(double num_inside, double num_total) {
  assert(num_inside >= 0.0 && num_total >= 0.0);

  return (4.0*num_inside/num_total);
}

int main(void) {
  int i;

  if(init_state() != 0) {
    return -1;
  }

  /**

  "When this thread has exited, the parent thread will calculate and output the estimated value of Pi."

  **/

  Worker_thread_func();

  printf("Main: Inside/Total = %d/%f. Pi estimate= %f. peace out.\n", inside_circle_count, total_points, monte_carlo_estimate_pi((double)inside_circle_count, total_points));
  cleanup_state();

  printf("Main. peace out.\n");

  return 0;
}

static void monte_carlo_get_rand_point(double *x, double *y) {

  // drand48() Rand. number in [0.0-1.0)

  *x = (drand48() - 0.5) * 2.0;
  *y = (drand48() - 0.5) * 2.0;
}

static int monte_carlo_is_point_in_circle(double x, double y) { // returns 1 if the given point is inside the unit circle, 0 otherwise.
  double distance;
  // Compute distance from origin. If < 1 then its in the circle.

  distance = x * x + y * y;

  if(distance < 1)
    return 1;

  return 0;
}

/**
 * The thread will begin control in this function
 */
void Worker_thread_func(void)
{
  #pragma omp parallel
  {
  //pthread_t tid;
  unsigned long id = 0;
  double x, y;
  int i, my_inside_count;

  printf("I am a parallel region\n");

  /*

  "Write a multithreaded version of this algorithm that creates a separate
  thread to generate a number of random points. "

   "The thread will count the number of points that occur within the circle
   and store that result in a global variable."

  */

  // Use the tid address to uniquely identify a thread's prints.

  printf("#%lu: Hello bro, monte carlo.\n", id);

  my_inside_count = 0;

  for(i = 0; i < NUM_RAND_POINTS; i++) {

    monte_carlo_get_rand_point(&x, &y); // Generate a random point inside the unit square.

    if(monte_carlo_is_point_in_circle(x, y)) {
      my_inside_count += 1;
      //printf("#%lu: INSIDE. (%f,%f). Count = %d.\n", id, x, y, my_inside_count);
    } else {
      //printf("#%lu: OUTSIDE. (%f,%f). Count = %d.\n", id, x, y, my_inside_count);
    }

  }

  inside_circle_count += my_inside_count;
  total_points += NUM_RAND_POINTS;

  printf("#%lu: Goodbye bro. my_inside_count = %d.\n", id, my_inside_count);
  }
}
