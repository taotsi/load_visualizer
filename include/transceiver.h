#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include "hostgui.h"

/* message format:
    [[channel name][ ][N][ ][string 1][ ][string 2][]...[string N]]
*/

enum class MsgType{
    kOut,
    kUniv,
    kPtr,
    kInt,
    kFloat
};

class MsgChannel{
public:
    MsgChannel(MsgType mt, std::string channel)
        : type_{mt}, channnel_{channel}{}
    MsgChannel(const MsgChannel&) = default;
    MsgChannel(MsgChannel&&) = default;
    MsgChannel& operator=(const MsgChannel&) = default;
    MsgChannel& operator=(MsgChannel&&) = default;
    ~MsgChannel() = default;

    friend bool operator<(const MsgChannel &l, const MsgChannel &r){
        if(l.type() < r.type()){
            return true;
        }else if(l.type() == r.type()){
            if(l.channel() < r.channel()){
                return true;
            }
        }
        return false;
    }

    MsgType type() const {return type_;}
    std::string channel() const {return channnel_;}
private:
    MsgType type_ = MsgType::kUniv;
    std::string channnel_;
};

class Transceiver{
public:
    Transceiver();
    Transceiver(const Transceiver&) = default;
    Transceiver(Transceiver&&) = default;
    Transceiver& operator=(const Transceiver&) = default;
    Transceiver& operator=(Transceiver&&) = default;
    ~Transceiver();

    void TurnOn();
    void TurnOff();
    bool is_on();

    void AddSubscriber(MsgChannel mc, std::shared_ptr<Component> cpn);
    void AddPublisher(MsgChannel mc, std::shared_ptr<Component> cpn);

private:
    std::thread thread_;
    void ThreadMain();
    std::mutex mtx_;
    std::atomic<bool> is_on_;
    std::map<MsgChannel, std::vector<std::shared_ptr<Component>>> subscribers_;
    std::map<MsgChannel, std::vector<std::shared_ptr<Component>>> publishers_;

    std::mutex mtx_out_;
    std::queue<std::string> out_trans_;
    std::queue<std::string> out_receiv_;
    std::thread thread_out_trans_;
    std::thread thread_out_receiv_;
    void ThreadOutTrans();
    void ThreadOutReceiv();
    bool CheckFormat(std::string msg, std::string &channel, unsigned int &n, std::string &content);
    void TransmitToOut(std::string &msg);
    bool ReceiveFromOut(std::string &msg);
    void OutTransmit(std::string &msg);
    bool OutReceive(std::string &msg);
};

#endif // TRANSCEIVER_H_