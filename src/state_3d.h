#ifndef __STATE_3D_H__
#define __STATE_3D_H__

#include "timer.h"
#include "renderer.h"

class State_3D{
public:
    State_3D(){};
    ~State_3D(){};
    virtual void Load()                         = 0;
    virtual void Update(Timer       & timer)    = 0;
    virtual void Render(Renderer    & renderer) = 0;
protected:
private:
}

#endif //__STATE_3D_H__