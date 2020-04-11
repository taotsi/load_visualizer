#include "transceiver.h"
#include <sstream>

Transceiver::Transceiver(){
    thread_ = std::thread{&Transceiver::ThreadMain, this};
    thread_out_trans_ = std::thread{&Transceiver::ThreadOutTrans, this};
    thread_out_receiv_ = std::thread(&Transceiver::ThreadOutReceiv, this);
}

Transceiver::~Transceiver(){
    if(thread_out_trans_.joinable()){
        thread_out_trans_.join();
    }
    if(thread_out_receiv_.joinable()){
        thread_out_receiv_.join();
    }
    if(thread_.joinable()){
        thread_.join();
    }
}

bool Transceiver::is_on(){
    return static_cast<bool>(is_on_);
}
void Transceiver::TurnOn(){
    is_on_ = true;
}

void Transceiver::TurnOff(){
    is_on_ = false;
}

void Transceiver::TransmitToOut(std::string &msg){
    std::lock_guard<std::mutex> lg{mtx_out_};
    out_receiv_.push(msg);
}
bool Transceiver::ReceiveFromOut(std::string &msg){
    std::lock_guard<std::mutex> lg{mtx_out_};
    if(!out_trans_.empty()){
        msg = out_trans_.front();
        out_trans_.pop();
        return true;
    }
    return false;
}
void Transceiver::OutTransmit(std::string &msg){
    std::lock_guard<std::mutex> lg{mtx_out_};
    out_trans_.push(msg);
}
bool Transceiver::OutReceive(std::string &msg){
    std::lock_guard<std::mutex> lg{mtx_out_};
    if(!out_receiv_.empty()){
        // NOTE: std::cout is an example
        std::cout << out_receiv_.front() << "\n";
        out_receiv_.pop();
        return true;
    }
    return false;
}

void Transceiver::AddSubscriber(MsgChannel mc, std::shared_ptr<Component> cpn){
    subscribers_[mc].push_back(cpn);
}
void Transceiver::AddPublisher(MsgChannel mc, std::shared_ptr<Component> cpn){
    publishers_[mc].push_back(cpn);
}

// NOTE: n might be 0, which means raw string and no align check
bool Transceiver::CheckFormat(std::string msg, std::string &channel, unsigned int &n, std::string &content){
    std::stringstream ss{msg};
    if(ss.good()){
        ss >> channel;
    }else{
        std::cout << "wrong format of message! need channen name\n";
        return false;
    }
    if(ss.good()){
        ss >> n;
        if(n>10){
            std::cout << "WARNING: is N too big?\n";
        }
    }else{
        std::cout << "wrong format of message! need number of data units\n";
        return false;
    }
    if(!ss.good()){
        std::cout << "wrong format of message! need content\n";
        return false;
    }
    content = msg.substr(msg.find_first_not_of(" \t"));        // remove leading spaces
    content = content.substr(content.find_first_of(" \t")+1);  // remove the first word
    content = content.substr(content.find_first_of(" \t")+1);  // remove the second word

    return true;
}

void Transceiver::ThreadMain(){
    std::string line;
    while(is_on_){
        /* receive messages from publishers and forward them to subscribers */
        std::string raw_msg;
        std::string msg_channel;
        unsigned int n;
        std::string msg_content;
        if(ReceiveFromOut(raw_msg) && CheckFormat(raw_msg, msg_channel, n, msg_content)){
            std::cout << "got msg from out\n";
            MsgChannel mc{MsgType::kOut, msg_channel};
            if(subscribers_.find(mc) != subscribers_.end()){
                std::cout << "found kOut subs\n";
                for(auto &it : subscribers_[mc]){
                    std::cout << "send msg to kOut sub once\n";
                    it->Write(msg_content);
                }
            }
        }
        // for(auto& [mc_pub, pubs] : publishers_){
        for(auto& p : publishers_){
            if(p.first.type() == MsgType::kOut){
                for(auto &it : p.second){
                    if(it->Read(raw_msg) && CheckFormat(raw_msg, msg_channel, n, msg_content)){
                        TransmitToOut(msg_content);
                    }
                }
            }else if(p.first.type() == MsgType::kUniv){
                MsgChannel mc_sub{MsgType::kUniv, p.first.channel()};
                if(subscribers_.find(mc_sub) != subscribers_.end()){
                    for(auto &it : p.second){
                        if(it->Read(raw_msg) && CheckFormat(raw_msg, msg_channel, n, msg_content)){
                            for(auto &it : subscribers_[mc_sub]){
                                it->Write(msg_content);
                            }
                        }
                    }
                }
            }else{
                // TODO:
            }
        }
    }
    TurnOff();
}

void Transceiver::ThreadOutTrans(){
    std::string line;
    while(getline(std::cin, line) && is_on_){
        if(line == "exit"){
            TurnOff();
            break;
        }
        OutTransmit(line);
        std::cout << "out trans one line\n";
    }
    TurnOff();
}
void Transceiver::ThreadOutReceiv(){
    std::string msg;
    while(is_on_){
        if(OutReceive(msg)){
            std::cout << "out receiv one line\n";
        }
    }
}