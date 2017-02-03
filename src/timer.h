#ifndef __TIMER_H__
#define __TIMER_H__

#include <iostream>
#include <GLFW/glfw3.h>

struct Timer {
    double dt, now, last;
    double timer;
    long ticker;

    float fps;

    double last_time;
    int frames;

    Timer(){
        dt          = 0.0016f;
        now         = last = glfwGetTime();
        timer       = ticker = 0;
        last_time   = glfwGetTime();
        frames      = 0;
        fps         = 0;
    }

    void Update(){
        now = glfwGetTime();
        dt = now - last;
        last = now;

        timer += dt;
        ticker++;

        frames ++;
        double current_time = glfwGetTime();
        if (current_time - last_time >= 1.0){
            fps = (float)(frames);
            frames = 0;
            last_time += 1.0;
        }
    }
};

void start_time();
void stop_time(const char * prefix);

#endif //__TIMER_H__