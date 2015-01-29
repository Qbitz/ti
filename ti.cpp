// # includes

#include <cstdio>
#include <ctime>
#include <cmath>

#include <thread>
#include <mutex>

// # defines

#define DEFAULT_THREAD_NO 17
#define DEFAULT_STEPS_NO 1000000
#define BOUNDS_LOWER 0.0
#define BOUNDS_UPPER M_PI
#define FUNCTION(x) sin(x)

// #defines helpers for displaying value of FUNCTION macro

#define STR_VALUE(arg)      #arg
#define FUNCTION_NAME(name) STR_VALUE(name)
#define PRINT_FUNCTION      FUNCTION_NAME(FUNCTION(x))

// global variable declarations

std::mutex guardianMutex;

// function declarations

void trapezoidalRule(double a, double b, double step, double *sum);

// bootstrap

int main(int argc, char* argv[]) {
    clock_t startClock = clock();

    printf("Integration of 'f(x)=%s' with bounds <%.3lf, %.3lf> and %d of steps\n", PRINT_FUNCTION, BOUNDS_LOWER, BOUNDS_UPPER, DEFAULT_STEPS_NO);

    double sum = 0.0;
    double step = (BOUNDS_UPPER - BOUNDS_LOWER) / DEFAULT_STEPS_NO;
    int threadSize = (int)ceil((double)DEFAULT_STEPS_NO/(double)DEFAULT_THREAD_NO);

    printf("> dispatching job to %d threads\n", DEFAULT_THREAD_NO);
    std::thread **threads = new std::thread*[DEFAULT_THREAD_NO];
    for(int i=0; i<DEFAULT_THREAD_NO; i++) {
        double threadMin = BOUNDS_LOWER + i*threadSize*step;
        double threadMax = threadMin + threadSize*step > BOUNDS_UPPER ? BOUNDS_UPPER : threadMin + threadSize*step;

        threads[i] = new std::thread(&trapezoidalRule, threadMin, threadMax, step, &sum);
    }

    printf("> running threads\n");

    for(int i=0; i<DEFAULT_THREAD_NO; i++) {
        threads[i]->join();
        delete threads[i];
    }
    delete threads;
    printf("> threads joined\n");

    printf("Integration result: %.10lf\n", sum);

    printf("\nFinished in %.2fs\n", ((float)clock() - (float)startClock) / CLOCKS_PER_SEC);
    return 0;
}

// function definitions

void trapezoidalRule(double a, double b, double step, double *sum) {
    double partialSum = 0.0;
    for(double i=a; i<b-step; i+=step) {
        partialSum += 0.5 * (FUNCTION(i) + FUNCTION(i+step)) * step;
    }

    std::lock_guard<std::mutex> lock(guardianMutex);
    *sum += partialSum;
}
