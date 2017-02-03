#ifndef __COLOR_H__
#define __COLOR_H__

struct Color {
	float r, g, b, a;
};

#define GETCOLOR(c) c.r, c.g, c.b, c.a

#endif //__COLOR_H__