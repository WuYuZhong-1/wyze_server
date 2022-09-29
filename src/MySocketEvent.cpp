#include "MySocketEvent.h"
#include "MyLoggerManager.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <thread>
#include <iostream>
#include <sstream>


namespace wyze {

    MySocketEvent::MySocketEvent():m_eventcount(0)
    {
        m_base = event_base_new();
        if(m_base == nullptr) {
            WARN("create event_base error");
            return;
        }

        int pipefd[2];
        int ret = pipe(pipefd);
        if(ret == -1) {
            WARN("pipe error:{}", strerror(errno));
            return;
        }
        
        m_readfd = pipefd[0];
        m_writefd = pipefd[1];

        //设置读管道
        m_evfifo = event_new(m_base, m_readfd, EV_READ | EV_PERSIST, [](int fd, short what, void* arg){}, this);
        if(!m_evfifo) {
            WARN("evfifo error");
            return;
        }
        event_add(m_evfifo, nullptr);
        m_t = new std::thread([this](){
            this->dispatch();
        });
        m_t->detach();
    }

    MySocketEvent::~MySocketEvent()
    {
        if(m_base) {
            event_base_free(m_base);
        }

        if(m_evfifo) {
            event_free(m_evfifo);
        }

        if(m_readfd == -1) {
            close(m_readfd);
        }

        if(m_writefd == -1) {
            close(m_writefd);
        }

        if(m_t)
            delete m_t;
    }

    void MySocketEvent::dispatch() 
    {
        // std::cout << "start dispatch threadid =" << std::this_thread::get_id() << std::endl;
        m_thredId = pthread_self();
        DEBUG("start dispatch threadid = {}", m_thredId);
        event_base_dispatch(m_base);
        DEBUG("end dispatch threadid = {}",  m_thredId);
        // std::cout << "end dispatch threadid =" << std::this_thread::get_id() << std::endl;
    }


    void MySocketEvent::loopbreak()
    {
        event_base_loopbreak(m_base);
    }


    bool MySocketEvent::addEvent(int cfd, struct sockaddr_in* addr)
    {
        //设置监听，创建 MyBufferEvent
        char ip[32] ={0};
        DEBUG("client ip = {}, port = {} add to thread = {}", evutil_inet_ntop(AF_INET, &addr->sin_addr.s_addr, ip, sizeof(ip)),
                                                                addr->sin_port, m_thredId);
        struct bufferevent* bev = bufferevent_socket_new(m_base, cfd, BEV_OPT_CLOSE_ON_FREE);
        if(!bev) {
            ERROR("create bufferevent error");
            close(cfd);
            return false;
        }

        //设置回调函数
        bufferevent_setcb(bev, 
            [](struct bufferevent* bev, void* arg){
                MySocketEvent* p = (MySocketEvent*)arg;
                p->readEvent(bev);
            },
            [](struct bufferevent* bev, void* arg){
                MySocketEvent* p = (MySocketEvent*)arg;
                p->writeEvent(bev);
            },
            [](struct bufferevent* bev,short events, void* arg){
                MySocketEvent* p = (MySocketEvent*)arg;
                p->errorEvent(bev,events);
            },
            this
        );
        bufferevent_enable(bev, EV_READ | EV_WRITE);

        m_bufferEvents_map.insert(std::make_pair(cfd,new MyBufferEvent(bev)));
        m_eventcount++;

        return true;
    }

    void MySocketEvent::readEvent(struct bufferevent* bev)
    {
        auto mybev = m_bufferEvents_map[bufferevent_getfd(bev)];
        mybev->readEvent(bev);
    }

    void MySocketEvent::writeEvent(struct bufferevent* bev)
    {
        auto mybev = m_bufferEvents_map[bufferevent_getfd(bev)];
        mybev->writeEvent(bev);
    }

    void MySocketEvent::errorEvent(struct bufferevent* bev,short events)
    {
        auto mybev = m_bufferEvents_map[bufferevent_getfd(bev)];

        if(events & BEV_EVENT_EOF) {
            WARN("connection closed");
        }
        else if(events & BEV_EVENT_ERROR) {
            WARN("Got an error on the connection: {}", strerror(errno));
        }

        m_bufferEvents_map.erase(bufferevent_getfd(bev));
        delete mybev;
    }

}