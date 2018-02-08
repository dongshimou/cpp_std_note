//
// Created by skadi on 2018/2/8.
//

#ifndef UNTITLED_XML_PARSER_HPP
#define UNTITLED_XML_PARSER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <functional>
#include <map>
#define log(x) std::cout<<x<<std::endl


struct xml_tag;
using xml_child=std::variant<std::string,xml_tag*,std::vector<xml_tag*>>;
using xml_attribute=std::map<std::string_view ,std::string_view >;
enum class ParserStatus{
    Parser_OpenBegin,
    Parser_OpenEnd,
    Parser_OpenTag,
    Parser_CloseTag,
    Parser_CloseBegin,
    Parser_CloseEnd,
    Parser_Context,
    Parser_Begin,
    Parser_End,
    Parser_Error,
};
struct xml_tag{
    xml_tag(){}
    std::string label;
    xml_child value;
    xml_attribute attribute;
    ParserStatus status;
    std::string raw;
};


void parser(const std::string& input){
    auto trim=[&input]() {
        auto str = std::string();
        auto i = 0;
        for (; i < input.size();) {
            if (isspace(input[i])){
                i++;
            }else{
                break;
            }
        }
        for (; i < input.size() - 1;) {
            if (isspace(input[i]) && isspace(input[i + 1])) {
                i++;
                continue;
            }
            str += input[i];
            i++;
        }
        if (!isspace(input[i])) {
            str += input[i];
        }
        return str;
    }();
    auto view=std::string_view(trim);

    log(view);

    auto ptr=0;
    std::function<xml_tag*(void)> parser_stack=[&view,&ptr,&parser_stack]()->xml_tag*{
        auto tag=new xml_tag;
        ParserStatus status=ParserStatus::Parser_OpenBegin;

        auto trim_space=[&](){
            while (ptr!=view.size()){
                if(isspace(view[ptr])){
                    ptr++;
                    continue;
                }
                break;
            }
        };

        auto xml_open_begin=[&](){
            trim_space();
                if (view[ptr]=='<'){
                    ptr++;
                    return ParserStatus ::Parser_OpenTag;
                }else{
                    return ParserStatus ::Parser_Error;
                }
        };
        auto xml_open_tag=[&](){
            auto parse_key=[&]()->std::tuple<std::string_view,ParserStatus>{
                auto p_s=ptr;
                while (ptr!=view.size()){
                    if(isalpha(view[ptr])){
                        ptr++;
                    }else if(view[ptr]=='='){
                        auto p_view=std::string_view(view.begin()+p_s,ptr-p_s);
                        ptr++;
                        return std::tuple(std::move(p_view),ParserStatus::Parser_OpenTag);
                    }else{
                        return std::tuple(std::string_view(),ParserStatus::Parser_Error);
                    }
                }
                return std::tuple(std::string_view(),ParserStatus::Parser_Error);
            };
            auto parse_value=[&]()->std::tuple<std::string_view,ParserStatus>{
                if(view[ptr]=='\''){
                    ptr++;
                }else{
                    return std::tuple(std::string_view(),ParserStatus::Parser_Error);
                }
                auto p_s=ptr;
                while(ptr!=view.size()){
                    if(isalnum(view[ptr])){
                        ptr++;
                    }else if(view[ptr]=='\''){
                        auto p_view=std::string_view(view.begin()+p_s,ptr-p_s);
                        ptr++;
                        return std::tuple(std::move(p_view),ParserStatus::Parser_OpenTag);
                    }else{
                        return std::tuple(std::string_view(),ParserStatus::Parser_Error);
                    }
                }
                return std::tuple(std::string_view(),ParserStatus::Parser_Error);
            };
            auto parse_attr=[&](){
                auto [key,key_status]=parse_key();
                if (key_status==ParserStatus::Parser_Error){
                    return key_status;
                }
                auto [value,value_status]=parse_value();
                if (value_status==ParserStatus::Parser_Error){
                    return value_status;
                }
                tag->attribute[key]=value;
            };
            trim_space();
            while (ptr!=view.size()){
                if(isalnum(view[ptr])){
                    tag->label+=view[ptr];
                    ptr++;
                }else {
                    break;
                }
            }
            trim_space();
            while (ptr<view.size()){
                if(isalpha(view[ptr])){
                    parse_attr();
                }else if(isspace(view[ptr])){
                    trim_space();
                    continue;
                }else{
                    if(view[ptr]=='/'&&ptr+1<view.size()&&view[ptr+1]=='>'){
                        ptr+=2;
                        return ParserStatus ::Parser_End;
                    }else if(view[ptr]=='>'){
                        return ParserStatus ::Parser_OpenEnd;
                    }else{
                        return ParserStatus ::Parser_Error;
                    }
                }
            }
        };
        auto xml_open_end=[&](){
            if(view[ptr]=='>'){
                ptr++;
                return ParserStatus ::Parser_Context;
            }
            return ParserStatus ::Parser_Error;
        };
        auto xml_context=[&](){
            if (ptr+1==view.size()){
                return ParserStatus ::Parser_Error;
            }
//            while (ptr!=view.size()){
//                if(isspace(view[ptr])){
//                    ptr++;
//                }
//            }
            if (view[ptr]=='<'&&ptr<view.size()-1&&view[ptr+1]!='/') {
                xml_tag *temp = nullptr;
                std::vector<xml_tag *> list;
                while (view[ptr] == '<'&&ptr<view.size()-1&&view[ptr+1]!='/') {
                    temp = parser_stack();
                    if (temp->status == ParserStatus::Parser_Error) {
                        return ParserStatus::Parser_Error;
                    }
                    list.emplace_back(temp);
                }
                if (list.size()>1) {
                    tag->value = std::move(list);
                }else{
                    tag->value=std::move(temp);
                }
            }else {
                std::string context;
                while (view[ptr] != '<') {
                    context += view[ptr];
                    ptr++;
                    if (ptr == view.size()) {
                        return ParserStatus::Parser_Error;
                    }
                }
                tag->value = std::move(context);
            }
            if(view[ptr]=='<'&&ptr<view.size()-1&&view[ptr+1]=='/'){
                return ParserStatus ::Parser_CloseBegin;
            }
            return ParserStatus ::Parser_Error;
        };
        auto xml_close_begin=[&](){
            if(view[ptr]=='<'&&ptr+1!=view.size()&&view[ptr+1]=='/'){
                ptr+=2;
                return ParserStatus ::Parser_CloseTag;
            }
            return ParserStatus ::Parser_Error;
        };
        auto xml_close_tag=[&](){
            auto p_s=ptr;
            while (true){
                if(ptr==view.size()){
                    return ParserStatus ::Parser_Error;
                }else if(isalnum(view[ptr])){
                    ptr++;
                }else if (view[ptr]=='>'){
                    break;
                }else{
                    return ParserStatus ::Parser_Error;
                }
            }
            std::string_view temp(view.begin()+p_s,ptr-p_s);
            if (temp!=tag->label){
                return ParserStatus ::Parser_Error;
            }
            return ParserStatus::Parser_CloseEnd;
        };
        auto xml_close_end=[&](){
            if(view[ptr]=='>'){
                ptr++;
                return ParserStatus ::Parser_End;
            }
            return ParserStatus ::Parser_Error;
        };

        while (status!=ParserStatus::Parser_Error) {
            switch (status) {
                case ParserStatus::Parser_Begin:
                    status = ParserStatus::Parser_OpenBegin;
                    break;
                case ParserStatus::Parser_OpenBegin:
                    status = xml_open_begin();
                    break;
                case ParserStatus::Parser_OpenTag:
                    status = xml_open_tag();
                    break;
                case ParserStatus::Parser_OpenEnd:
                    status = xml_open_end();
                    break;
                case ParserStatus::Parser_CloseBegin:
                    status = xml_close_begin();
                    break;
                case ParserStatus::Parser_CloseTag:
                    status = xml_close_tag();
                    break;
                case ParserStatus::Parser_CloseEnd:
                    status = xml_close_end();
                    break;
                case ParserStatus::Parser_Context:
                    status=xml_context();
                    break;
                case ParserStatus::Parser_End:
                    tag->status=status;
                    return tag;
                default:
                    tag->status=status;
                    return tag;
            }
        }
    };

    auto root=parser_stack();
//    root->raw=trim;
    std::function<void(xml_tag*)>show=[&show](xml_tag*root){
        log(root->label);
        for(auto[k,v]:root->attribute){
            std::cout<<k<<" = "<<v<<std::endl;
        }
        if (auto ptr=std::get_if<std::string>(&root->value)){
            log(*ptr);
        }else if(auto ptr=std::get_if<xml_tag*>(&root->value)){
            show(*ptr);
        }else if(auto ptr=std::get_if<std::vector<xml_tag*>>(&root->value)){
            for(const auto& v:*ptr){
                show(v);
            }
        }
    };
    if(root->status!=ParserStatus::Parser_Error)
    show(root);
}

#endif //UNTITLED_XML_PARSER_HPP
