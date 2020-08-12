#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <direct.h>
#include <io.h>

#include <cqcppsdk/cqcppsdk.h>
#include <yaml-cpp/yaml.h>
#include "json.hpp"

#ifndef HEADERS

using json = nlohmann::json;
using node = YAML::Node;

using cq::utils::ansi;

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

#define HEADERS

#endif

bool Register(const int &group_id,const int &user_id) {
    json jsonfile;
    try { //读取数据
        ifstream jsonFile(ansi(dir::app()+"data.json"));
        jsonfile=json::parse(jsonFile);
    } catch (ApiError &err) {
        logging::warning("加载数据","读取应用数据失败！错误码："+to_string(err.code));
        return false;
    } catch (nlohmann::detail::parse_error &err) { //json不存在
        logging::warning("加载数据","读取应用数据失败，请检查目录是否存在data.json！");
        return false;
    }

    srand(time(NULL));
    Message response;
    try {
        json fortune=jsonfile["fortune"];
        int id=rand()%fortune.size();
        response+=MessageSegment::at(user_id)+"签到成功！\n今日运势："+fortune[id].get<string>()+"\n";
    } catch (nlohmann::detail::type_error &err) { //不存在fortune键值
        logging::warning("加载数据","读取应用数据失败，请data.json是否存在fortune项！");
        return false;
    }

    time_t t=time(NULL);
    tm *nowt=localtime(&t);
    response+="当前时间："+to_string(nowt->tm_year+1900)+"年"+to_string(nowt->tm_mon+1)+"月"+to_string(nowt->tm_mday)+"日"+to_string(nowt->tm_hour)+"时"+to_string(nowt->tm_min)+"分"+to_string(nowt->tm_sec)+"秒\n";

    try {
        json characters=jsonfile["characters"];
        int id=rand()%characters.size();
        response+="今日幸运角色："+characters[id].get<string>()+"\n";
    } catch (nlohmann::detail::type_error &err) { //不存在characters键值
        logging::warning("加载数据","读取应用数据失败，请data.json是否存在characters项！");
        return false;
    }

    int suitid;
    try {
        json suitable=jsonfile["suitable"];
        suitid=rand()%suitable.size();
        int id=suitid;
        for(json::iterator it = suitable.begin(); it != suitable.end(); ++it) {
            if(id==0) {
                response+="宜："+it.key()+"："+it.value().get<string>()+"\n";
                break;
            }
            id--;
        }
    } catch (nlohmann::detail::type_error &err) { //不存在suitable键值
        logging::warning("加载数据","读取应用数据失败，请data.json是否存在suitable项！");
        return false;
    }

    try {
        json unsuitable=jsonfile["unsuitable"];
        int id=rand()%unsuitable.size();
        if(id==suitid)id=(id+1)%unsuitable.size();
        for(json::iterator it = unsuitable.begin(); it != unsuitable.end(); ++it) {
            if(id==0) {
                response+="忌："+it.key()+"："+it.value().get<string>()+"\n";
                break;
            }
            id--;
        }
    } catch (nlohmann::detail::type_error &err) { //不存在unsuitable键值
        logging::warning("加载数据","读取应用数据失败，请data.json是否存在unsuitable项！");
        return false;
    }

    int h=rand()%24,m=rand()%60,s=rand()%60;
    response+="抽卡加成时间："+to_string(h)+"时"+to_string(m)+"分"+to_string(s)+"秒\n";

    try {
        json position=jsonfile["position"];
        int id=rand()%position.size();
        response+="抽卡加成方向："+position[id].get<string>()+"\n";
    } catch (nlohmann::detail::type_error &err) { //不存在position键值
        logging::warning("加载数据","读取应用数据失败，请data.json是否存在position项！");
        return false;
    }

    try {
        json actions=jsonfile["actions"];
        int id=rand()%actions.size();
        response+="抽卡加成动作："+actions[id].get<string>()+"\n";
    } catch (nlohmann::detail::type_error &err) { //不存在actions键值
        logging::warning("加载数据","读取应用数据失败，请data.json是否存在actions项！");
        return false;
    }

    send_group_message(group_id,response);
    return true;
}