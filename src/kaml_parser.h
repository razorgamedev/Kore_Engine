#ifndef __KAML_PARSER_H__
#define __KAML_PARSER_H__

#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <map>

struct kaml_object;

enum kaml_types{
    NONE,
    STRING,
    NUMBER,
    BOOL,
    TABLE,
    NUM_TYPES
};

union kaml_value{
    float number;
    std::string string;
    bool boolean;
    std::vector<kaml_object> table;
};

struct kaml_object{
    std::string name;
    kaml_types type;
    kaml_value value;
    std::map<std::string, kaml_object> objects;
};

kaml_object * parse_kaml_file(const char * file_path);
kaml_object * compile_token_list_to_kaml_object(std::vector<std::string> &tokens);

#endif //__KAML_PARSER_H__