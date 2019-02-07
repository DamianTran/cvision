/** ////////////////////////////////////////////////////////////////

    *** EZ-C++ - A simplified C++ experience ***

        Yet (another) open source library for C++

        Original Copyright (C) Damian Tran 2019

        By aiFive Technologies, Inc. for developers

    Copying and redistribution of this code is freely permissible.
    Inclusion of the above notice is preferred but not required.

    This software is provided AS IS without any expressed or implied
    warranties.  By using this code, and any modifications and
    variants arising thereof, you are assuming all liabilities and
    risks that may be thus associated.

////////////////////////////////////////////////////////////////  **/

#include "EZC/toolkit/html.hpp"
#include "EZC/algorithm.hpp"

#include "EZC/toolkit/charsurf.hpp"
#include "EZC/toolkit/string.hpp"

using namespace EZC;

HTML_tree::HTML_tree(const char* str){
    read_text(str);
}

HTML_tree::HTML_tree(){ }

bool HTML_BEGIN(const char* text_ptr){
    if((*text_ptr == '<') && (*(text_ptr + 1) != '/')){
        while(*text_ptr){
            if(*text_ptr == '>') return true;
            ++text_ptr;
        }
    }
    return false;
}

bool HTML_END(const char* text_ptr){
    if(ptr_at_string(text_ptr, "</")){
        while(*text_ptr){
            if(*text_ptr == '>') return true;
            ++text_ptr;
        }
    }
    return false;
}

bool HTML_DECL(const char* text_ptr){
    return ptr_at_string(text_ptr, "<!DOCTYPE html>") ||
            ptr_at_string(text_ptr, "<!DOCTYPE HTML>");
}

bool HTML_COMMENT(const char* text_ptr){
    if(ptr_at_string(text_ptr, "<!--")){
        text_ptr += 4;
        while(*text_ptr){
            if(ptr_at_string(text_ptr, "-->")) return true;
            ++text_ptr;
        }
    }
    return false;
}

bool HTML_NOCONTENT(const char* text_ptr){
    if(HTML_BEGIN(text_ptr)){
        std::string itemType;
        ++text_ptr;
        const char* c = text_ptr;
        while(*text_ptr && (*text_ptr != ' ') && (*text_ptr != '>')){
            ++text_ptr;
        }
        itemType.assign(c, text_ptr);
        itemType.insert(itemType.begin(), '/');
        itemType.insert(itemType.begin(), '<');
        itemType.insert(itemType.end(), '>');

        while(*text_ptr){
            if(ptr_at_string(text_ptr, itemType.c_str())) return false;
            ++text_ptr;
        }

        return true;
    }

    return false;
}

int HTML_FORWARD(const char* text_ptr){
    int distance = 0;

    if(*text_ptr == '<'){
        ++distance;

        bool bQuote = false;
        char quoteType;

        while(*text_ptr && (bQuote || (*text_ptr != '>'))){

            if(!bQuote && (isCharType(*text_ptr, "'\""))){
                bQuote = true;
                quoteType = *text_ptr;
            }
            else if(*text_ptr == quoteType){
                bQuote = false;
            }

            ++text_ptr;
            ++distance;
        }
    }

    return distance;
}

std::string HTML_NAME(const char* text_ptr){
    std::string output;
    if(HTML_BEGIN(text_ptr)){
        ++text_ptr;
        const char* c = text_ptr;
        while(*text_ptr && (*text_ptr != ' ') && (*text_ptr != '>')){
            ++text_ptr;
        }
        output.assign(c, text_ptr);
    }
    return output;
}

std::string HTML_ATTR(const char* text_ptr, const char* attribute_name){
    std::string output;
    if(HTML_BEGIN(text_ptr)){
        bool bQuote = false;
        char quoteType;

        while(*text_ptr && !ptr_at_string(text_ptr, attribute_name)){
            ++text_ptr;
        }
        text_ptr += strlen(attribute_name);

        while(*text_ptr && isCharType(*text_ptr, " \t\r\n=")) ++ text_ptr;

        const char* c = text_ptr;
        while(*text_ptr && (bQuote || (*text_ptr != ' ')) && (*text_ptr != '>')){
            if(!bQuote && isCharType(*text_ptr, "'\"")){
                bQuote = true;
                quoteType = *text_ptr;
            }
            else if(*text_ptr == quoteType){
                bQuote = false;
            }

            ++text_ptr;
        }
        output.assign(c, text_ptr);
        trim(output, " \"'\t\n\r");
    }
    return output;
}

std::string HTML_CLASS(const char* text_ptr){

    std::string output;

    if(HTML_BEGIN(text_ptr)){
        ++text_ptr;
        const char* c = text_ptr;
        while(*text_ptr && (*text_ptr != ' ') && (*text_ptr != '>')) ++text_ptr;

        output.assign(c, text_ptr);
        trim(output, "\t\r\n ");

    }

    return output;

}

void get_HTML_attributes(const char* text_ptr,
                         std::map<std::string, std::string>& output){

    while(*text_ptr){

        if(HTML_BEGIN(text_ptr)){
            std::string attr, value;
            const char* c = NULL;
            bool bQuote = false;
            char quoteType;

            // Skip the class ID

            while(*text_ptr != ' '){
                if(*text_ptr == '>'){
                    return; // No attributes
                }
                ++text_ptr;
            }

            while(*text_ptr){

                if(attr.empty()){

                    if(!c){
                        while(*text_ptr && isCharType(*(text_ptr + 1), " \r\t\n")) ++text_ptr;
                        c = text_ptr;
                    }

                    if(*text_ptr == '>') return;

                    if(*text_ptr == '='){
                        attr.assign(c, text_ptr);
                        trim(attr, " \"'\t\n\r");
                        c = NULL;
                    }

                }
                else if(value.empty()){
                    if(!c){
                        c = text_ptr;
                    }

                    if(!bQuote && isCharType(*text_ptr, "'\"")){
                        bQuote = true;
                        quoteType = *text_ptr;
                    }
                    else if(*text_ptr == quoteType){
                        bQuote = false;
                    }

                    if(!bQuote && ((*text_ptr == ' ') || (*text_ptr == '>'))){
                        value.assign(c, text_ptr);
                        trim(value, " \"'\t\n\r");

                        output[attr] = value;
                        while(*text_ptr && isCharType(*(text_ptr + 1), " =\t\n\r")) ++text_ptr;
                        c = NULL;
                        value.clear();
                        attr.clear();
                    }
                }

                if(!bQuote && (*text_ptr == '>')) return;
                ++text_ptr;
            }

            return;
        }

        ++text_ptr;
    }
}

bool sub_HTML_content(std::string& text){

    size_t i = 0, j = 0,
        beginIdx,
        subIdx,
        level = 0;
    bool bQuote = false,
        bStatus = false;
    char quoteType;
    std::string content;

    while(i < text.size()){

        if(HTML_BEGIN(&text.c_str()[i])){

            if(HTML_NOCONTENT(&text.c_str()[i])){

                size_t s = i;
                i += HTML_FORWARD(&text.c_str()[i]);

                if(i < text.size())
                    text.erase(text.begin() + s, text.begin() + i);
                else text.clear();
                i = s;

            }
            else{

                beginIdx = i;
                i += HTML_FORWARD(&text.c_str()[i]);
                if(!level) subIdx = i;
                ++level;

            }

        }
        else if(HTML_END(&text.c_str()[i])){

            --level;

            if(!level)
                content.assign(text.begin() + subIdx, text.begin() + i);

            i += HTML_FORWARD(&text.c_str()[i]);

            if(!level){
                j = i - beginIdx - content.size();

                text.replace(text.begin() + beginIdx, text.begin() + i,
                             content.begin(), content.end());

                i -= j;

                bStatus = true;
            }

        }
        else ++i;

    }

    return bStatus;

}

bool HTML_tree::read_text(const char* text){

    itemClass.clear();
    ID.clear();
    attributes.clear();

//    std::cout << "SECTION BEGIN: =======================\n\n"
//            << text << "\n\nSECTION END: ========================\n\n";

    size_t L = strlen(text), i = 0;
    bool bBracket = false, bSubsection = false;
    std::string subBuf;
    unsigned int beginIdx, subIdx, level = 0;

    if(HTML_DECL(&text[i])) i += HTML_FORWARD(&text[i]);

    for(; i < L;){
        if(HTML_COMMENT(&text[i])){
            i += HTML_FORWARD(&text[i]);
        }
        else if(HTML_NOCONTENT(&text[i])){
            i += HTML_FORWARD(&text[i]);
        }
        else if(HTML_BEGIN(&text[i])){
            if(!level){
                beginIdx = i + HTML_FORWARD(&text[i]);
                get_HTML_attributes(&text[i], attributes);
                itemClass = HTML_CLASS(&text[i]);
//                std::cout << itemClass << '\n';
            }
            else if(level == 1){
                subIdx = i;
            }
            ++level;
            i += HTML_FORWARD(&text[i]);
        }
        else if(HTML_END(&text[i])){
            --level;
            if(!level){
                content.assign(text + beginIdx, text + i);
            }
            else if(level == 1){
                subBuf.assign(text + subIdx, text + i + HTML_FORWARD(&text[i]));
                branches.emplace_back(subBuf.c_str());
            }

            i += HTML_FORWARD(&text[i]);
        }
        else ++i;
    }

    return true;
}

bool HTML_tree::read_text(const std::string& text){
    return read_text(text.c_str());
}

bool HTML_tree::read_file(const std::string& filename){

    if(access(filename.c_str(), F_OK)){
        std::cout << "HTML tree: error reading file\n";
        return false;
    }

    FILE* inFILE = fopen(filename.c_str(), "rb");

    fseek(inFILE, 0, SEEK_END);
    size_t fileSIZE = ftell(inFILE);
    fseek(inFILE, 0, SEEK_SET);

    char* text = new char[fileSIZE];

    fread(text, fileSIZE, sizeof(char), inFILE);
    fclose(inFILE);

    bool status = read_text(text);

    delete[] (text);

    return status;

}

const std::string& HTML_tree::getClass() const{
    return itemClass;
}

const std::string& HTML_tree::getID() const{
    try{
        return attributes.at("id");
    }catch(...){
        return std::string();
    }
}

const std::string& HTML_tree::getContent() const{
    return content;
}

bool HTML_tree::isClass(const std::string& class_type) const{
    return itemClass == class_type;
}

bool HTML_tree::isID(const std::string& id) const{
    return getID() == id;
}

std::string HTML_tree::getAttribute(const std::string& attr) const{
    try{
        return attributes.at(attr);
    }catch(...){
        return std::string();
    }
}

bool HTML_tree::hasAttribute(const std::string& attr) const{
    try{
        return !attributes.at(attr).empty();
    }catch(...){ }
    return false;
}

bool HTML_tree::getTreeContent(std::vector<std::string>& output,
                        const std::vector<std::string>& class_types,
                        const std::map<std::string, std::string>& attribute_filters) const{

    bool status = false, match = false;

    if(anyEqual(getClass(), class_types)){

        if(!attribute_filters.empty()){
            try{
                for(auto& attr : attributes){
                    for(auto& o_attr : attribute_filters){
                        if((attr.first == o_attr.first) &&
                           (attr.second == o_attr.second)){
                                match = true;
                                goto match_found;
                        }
                    }
                }
            }catch(...){ }
        }
        else match = true;

    }

    match_found:;

    if(match){
        output.emplace_back(getContent());
        status = true;
    }

    for(auto& tree : branches){
        status |= tree.getTreeContent(output, class_types, attribute_filters);
    }

    return status;

}

bool HTML_tree::getTreeText(std::vector<std::string>& output,
                            const std::map<std::string, std::string>& attribute_filters) const{

    getTreeContent(output,
          { "p", "h1", "h2", "h3", "h4", "h4", "h6" },
          attribute_filters);
    char code_apostrophe[] = { -30, -128, -103, 0 };

    size_t i;

    for(size_t j = 0; j < output.size();){

        while(sub_HTML_content(output[j])){ }

        for(i = 0; i < output[j].size();){

            if(ptr_at_string(&output[j].c_str()[i], "&nbsp;")){
                output[j].erase(output[j].begin() + i, output[j].begin() + i + 5);
                output[j][i] = ' ';
                ++i;
            }
            else if(ptr_at_string(&output[j].c_str()[i], "&lt;")){
                output[j].erase(output[j].begin() + i, output[j].begin() + i + 3);
                output[j][i] = ' ';
                ++i;
            }
            else if(ptr_at_string(&output[j].c_str()[i], "&amp")){
                output[j].erase(output[j].begin() + i, output[j].begin() + i + 3);
                output[j][i] = ' ';
                ++i;
            }
            else if(ptr_at_string(&output[j].c_str()[i], code_apostrophe)){
                output[j].erase(output[j].begin() + i, output[j].begin() + i + 2);
                output[j][i] = '\'';
                ++i;
            }
            else if(!isTypeChar(output[j][i])){
                output[j][i] = ' ';
            }
            else{
                ++i;
            }
        }

        if(output[j].empty()){
            output.erase(output.begin() + j);
        }
        else{
            ++j;
        }

    }

    return !output.empty();
}
