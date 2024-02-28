#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
double global_sqrt_sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Prototype for calculating square root function according the given method as arguments
void *calculate_sqrt(void *arg);


int main(int argc, char *argv[]) {

    if (argc != 5) {
        printf("Usage: %s <start> <end> <threads> <method>\n", argv[0]);
        return 1;
    }

    unsigned long long start = strtoull(argv[1], NULL, 10);
    unsigned long long end = strtoull(argv[2], NULL, 10);
    int num_threads = atoi(argv[3]);
    int method = atoi(argv[4]);

    if (start >= end || num_threads <= 0 || (method != 1 && method != 2 && method != 3)) {
        printf("Invalid input.\n");
        return 1;
    }

    pthread_t threads[num_threads];
    unsigned long long subrange = (end - start + 1) / num_threads;

    /*
     * Dividing the whole difference between start and end arguments
     * to subranges into all threads
     */
    int i;
    for (i = 0; i < num_threads; i++) {
        unsigned long long *params = malloc(3 * sizeof(unsigned long long));
        params[0] = start + (i * subrange);
        params[1] = (i < num_threads - 1) ? params[0] + subrange - 1 : end;
        params[2] = method;

        pthread_create(&threads[i], NULL, calculate_sqrt, (void *)params);
    }

    //Wait until all threads finish their execution
    int d;
    for (d = 0; d < num_threads; d++) {
        pthread_join(threads[d], NULL);
    }

    printf("Sum of square roots: %.5e\n", global_sqrt_sum);

    pthread_mutex_destroy(&mutex);
    return 0;
}

void *calculate_sqrt(void *arg) {
    unsigned long long *params = (unsigned long long *)arg;
    unsigned long long start = params[0];
    unsigned long long end = params[1];
    int method = params[2];
    /*
     * Calculating sum of sqrt's between given arguments
     * According to methods, behaviour of the threads are considered
     *
     */
    double local_sqrt_sum = 0;
    unsigned long long i;
    for (i = start; i <= end; i++) {
        if (method == 1) {
            global_sqrt_sum += sqrt(i);
        } else if (method == 2) {
            pthread_mutex_lock(&mutex);
            global_sqrt_sum += sqrt(i);
            pthread_mutex_unlock(&mutex);
        } else if (method == 3) {
            local_sqrt_sum += sqrt(i);
        }
    }

    if (method == 3) {
        pthread_mutex_lock(&mutex);
        global_sqrt_sum += local_sqrt_sum;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}