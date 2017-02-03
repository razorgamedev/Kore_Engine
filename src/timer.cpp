#include "timer.h"

static double start, end;

void start_time(){
    start = glfwGetTime();
}

void stop_time(const char * prefix){
    end = glfwGetTime();
    std::cout << prefix << end - start << std::endl;
}