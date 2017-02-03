#ifndef __STATIC_SHADER_H__
#define __STATIC_SHADER_H__

#include "shaders.h"

class Static_Shader : public Shader{
public:
	Static_Shader();

	virtual void Load() override;
	virtual void bind_attributes() override;
};

#endif // __STATIC_SHADER_H__
