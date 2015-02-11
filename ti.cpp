// # includes

#include <cstdio>
#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>

// # defines

#define MAX_THREAD_NO 30
#define MAX_ITERATIONS 100
#define DEFAULT_STEPS_NO 1000000
#define BOUNDS_LOWER 0.0
#define BOUNDS_UPPER M_PI
#define FUNCTION(x) (sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x)*sin(x))

// #defines helpers for displaying value of FUNCTION macro

#define STR_VALUE(arg)      #arg
#define FUNCTION_NAME(name) STR_VALUE(name)
#define PRINT_FUNCTION      FUNCTION_NAME(FUNCTION(x))

// global variable declarations

std::mutex guardianMutex;

// function declarations

void trapezoidalRule(double a, double b, double step, double *sum);
double dispatchThreads(int threadNo);

// bootstrap

int main(int argc, char* argv[]) {
    std::chrono::time_point<std::chrono::system_clock> startRealTime = std::chrono::system_clock::now();

    FILE* fp = fopen("report/speadup.dat", "w");

    double speadupP1Base = 1.0;
    for(int threadNo = 1; threadNo <= MAX_THREAD_NO; threadNo++) {
        printf("Iterating for %3d threads\t", threadNo);
        fflush(stdout);

        double *iterationRealTimes = new double[MAX_ITERATIONS];

        for(int i = 0; i < MAX_ITERATIONS; i++) {
            iterationRealTimes[i] = dispatchThreads(threadNo);

            if(((int)ceil(100.0*i/MAX_ITERATIONS))%10==0) {
                printf(".");
                fflush(stdout);
            }
        }

        double avg = 0.0;
        for(int i = 0; i < MAX_ITERATIONS; i++) {
            avg += iterationRealTimes[i];
        }
        avg /= MAX_ITERATIONS;

        if(threadNo==1) {
            speadupP1Base = avg;
        }

        double stdDev = 0.0;
        for(int i = 0; i < MAX_ITERATIONS; i++) {
            stdDev += pow(avg - iterationRealTimes[i], 2.0);
        }
        stdDev = sqrt(stdDev / MAX_ITERATIONS);

        delete iterationRealTimes;

        printf("\tdone with avg real time %.6lf (std dev %.6lf) [speadup %.3lf]\n", avg, stdDev, speadupP1Base/avg);
        fprintf(fp, "%d\t%.6lf\t%.6lf\t%.3lf\n", threadNo, avg, stdDev, speadupP1Base/avg);
    }

    fclose(fp);

    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - startRealTime;
    printf("\nFinished in %.6fs\n", elapsed_seconds.count());
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

double dispatchThreads(int threadNo) {
    double sum = 0.0;
    double step = (BOUNDS_UPPER - BOUNDS_LOWER) / DEFAULT_STEPS_NO;
    int threadSize = (int)floor((double)DEFAULT_STEPS_NO/(double)threadNo);

    std::chrono::time_point<std::chrono::system_clock> dispatchToJoinRealTimeStart = std::chrono::system_clock::now();

    std::thread **threads = new std::thread*[threadNo];
    for(int i=0; i<threadNo; i++) {
        double threadMin = BOUNDS_LOWER + i*threadSize*step;
        double threadMax = threadMin + threadSize*step > BOUNDS_UPPER ? BOUNDS_UPPER : threadMin + threadSize*step;

        threads[i] = new std::thread(&trapezoidalRule, threadMin, threadMax, step, &sum);
    }

    for(int i=0; i<threadNo; i++) {
        threads[i]->join();
    }

    std::chrono::duration<double> dispatchToJoinRealTime = std::chrono::system_clock::now() - dispatchToJoinRealTimeStart;

    for(int i=0; i<threadNo; i++) {
        delete threads[i];
    }
    delete threads;

    return dispatchToJoinRealTime.count();
}
