#include "MyTcpServerEvent.h"
#include "MyConfig.h"
#include <signal.h>

namespace wyze {

    MyTcpServerEvent::MyTcpServerEvent() 
    {
        if(!parseConfig()) {
            ERROR("libevent error");
            return;
        }

        if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            return;


        do {
            m_base = event_base_new();
            if(m_base == nullptr) {
                ERROR("create listen base error");
                break;
            }

            struct sockaddr_in sin;
            memset(&sin, 0, sizeof(sin));
            sin.sin_family = AF_INET;
            sin.sin_port = htons(m_port);

            if(m_host != "0.0.0.0") {
                inet_pton(AF_INET, m_host.c_str(), &sin.sin_addr.s_addr);
            }

            m_listener = evconnlistener_new_bind(m_base, &MyTcpServerEvent::clientConn, (void*)this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                                                    -1, (struct sockaddr*)&sin, sizeof(sin));
            if(m_listener == nullptr) {
                ERROR("create listen event error");
                break;
            }

            m_basesize = (m_basesize <= 0 ? 1: m_basesize);
            m_eventbase = new MySocketEvent[m_basesize];
            if(m_eventbase == nullptr) {
                ERROR("create m_eventbase error");
                break;
            }

            return;
        } while(0);

        if(m_base)
            event_base_free(m_base);

        if(m_listener)
            evconnlistener_free(m_listener);
        
    }

    MyTcpServerEvent::~MyTcpServerEvent()
    {
        if(m_base)
            event_base_free(m_base);

        if(m_listener)
            evconnlistener_free(m_listener);

        if(m_eventbase)
            delete[] m_eventbase;
    }

    void MyTcpServerEvent::dispatch()
    {
        event_base_dispatch(m_base);
    }

    void MyTcpServerEvent::clientEvent(int cfd, struct sockaddr_in *addr)
    {
        // 这里可能存在多线程崩溃
        // m_eventbase[selectSocketEvent()].addEvent(cfd, addr);
        m_eventbase[selectSocketEvent()].addfd(cfd);
    }

    void MyTcpServerEvent::clientConn(evconnlistener* listener, int cfd,
                                 sockaddr* sock, int socklen, void* arg)
    {
        //这里将数据保存，在文件中
        struct sockaddr_in* sin = (struct sockaddr_in*)sock;
        char ip[32] = {0};
        INFO("client connect ip={}, port={}", inet_ntop(AF_INET,&sin->sin_addr.s_addr,ip,sizeof(ip)), ntohs(sin->sin_port));
        MyTcpServerEvent* p = (MyTcpServerEvent*)arg;
        p->clientEvent(cfd, sin);
    }

    bool MyTcpServerEvent::parseConfig()
    {
        auto confman = MySinglePtr<MyConfigManager>::getInstance();
        auto var = confman->lookup<StEvent>(CONFIG_UTIL_LIBEVENT);

        if(var) {
            INFO("libevent: {}",var->toString());
            StEvent event = var->getValue();
            m_host = event.host;
            m_port = event.port;
            m_basesize = event.basesize;
            return true;
        }

        return false;
    }

    int MyTcpServerEvent::selectSocketEvent()
    {
        int countArr[m_basesize]= {0};
        for(int i =0; i < m_basesize; i++) {
            countArr[i] = m_eventbase[i].getEventCount();
        }

        //算法算出最小的数
        int index = 0;
        int minCount = countArr[index];
        for(int i = 1; i < m_basesize; i++) {
            if( minCount > countArr[i]) {   //目前最小的数大于当前比较的数
                minCount = countArr[i];
                index = i;
            }
        }

        return index;
    }
}