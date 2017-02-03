#ifndef __GUI_ELEMENTS_H__
#define __GUI_ELEMENTS_H__

#include <glm/glm.hpp>
#include <vector>

struct Element{
    glm::vec2 Position;
    glm::vec2 Size;

    bool hidden;
    Element(glm::vec2 pos, glm::vec2 size){
        Position = pos;
        Size = size;
        hidden = false;
    }

    std::vector<Element> children;
    bool Contains(glm::vec2 pos){
        return (pos.x > Position.x && pos.x < Position.x + Size.x && pos.y > Position.y && pos.y < Position.y + Size.y);
    }
};

struct Button : public Element{
    Button(glm::vec2 pos, glm::vec2 size) : Element(pos, size){

    }


};



#endif //__GUI_ELEMENTS_H__