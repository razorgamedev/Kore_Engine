#include "kaml_parser.h"

// @refactor, we should move this to its own seperate string handling file
std::vector<std::string> tokenize_string(std::string str, std::string del, std::string bad){
    std::vector<std::string> tokens;

    std::string word;
    for (unsigned int i = 0; i < str.size(); i++){
        auto ch = str[i];
        if (del.find(ch) != std::string::npos){
            if (word.size() > 0)
                tokens.push_back(word);
            word = "";
            if (bad.find(ch) == std::string::npos)
                tokens.push_back(std::string{ch});
        }else
            if (bad.find(ch) == std::string::npos)
                word += std::string{ch};

    }
    if (word.size() > 0)tokens.push_back(word);

    return tokens;
}

kaml_object * parse_kaml_file(const char * file_path){
    kaml_object * obj =  (kaml_object*)malloc(sizeof(kaml_object));
 
    obj->type = kaml_types::TABLE;

    std::ifstream file;
    file.open(file_path);

    if (!file){
        printf("ERROR::PARSE_KAML_FILE:: cannot find file: %s\n", file_path);
        return obj;
    }

    std::string code_string, line;
    while(std::getline(file, line))
        code_string += line;

    auto tokens = tokenize_string(code_string, "[]: \n\t", " \n\t");   

    for (auto t : tokens)
        printf("%s\n", t.c_str());

    file.close();

    return obj;
}

kaml_object * compile_token_list_to_kaml_object(std::vector<std::string> &tokens){



}