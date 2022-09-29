#ifndef MYTCPSERVEREVENT_H
#define MYTCPSERVEREVENT_H

#include "MySocketEvent.h"
#include "event2/listener.h"
#include <arpa/inet.h>

namespace wyze {

    class MyTcpServerEvent {
    public:
        MyTcpServerEvent();
        ~MyTcpServerEvent();

        void dispatch();
        void clientEvent(int cfd, struct sockaddr_in *addr);
    protected:
        static void clientConn( evconnlistener* listener, int cfd,
                                 sockaddr* sock, int socklen, void* arg);
    
        bool parseConfig();
        int selectSocketEvent(); //算法，算出哪个events挂在的数量最少,返回m_eventbase的下标
    private:
        struct event_base* m_base = nullptr;
        struct evconnlistener* m_listener = nullptr;
        MySocketEvent* m_eventbase = nullptr;

        std::string m_host;
        uint16_t m_port = 8080;
        int m_basesize = 1;
    };

}

#endif // !MYTCPSERVEREVENT_H