#ifndef __INSTANCED_SHADER_H__
#define __INSTANCED_SHADER_H__

#include "shaders.h"

class Instanced_Shader : public Shader{
public:
    Instanced_Shader();

    virtual void Load() override;
    virtual void bind_attributes() override;
};

#endif //__INSTANCED_SHADER_H__