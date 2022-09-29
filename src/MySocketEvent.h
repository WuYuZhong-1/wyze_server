#ifndef MYSOCKETEVENT_H
#define MYSOCKETEVENT_H

#include "event2/event.h"
#include <thread>
#include <atomic>
#include <map>
#include "MyBufferEvent.h"

namespace wyze {

    class MySocketEvent {
    public:
        MySocketEvent();
        ~MySocketEvent();

        void dispatch();
        void loopbreak();
        int getEventCount() const { return m_eventcount; }
        bool addEvent(int cfd, struct sockaddr_in* addr);

        void readEvent(struct bufferevent* bev);
        void writeEvent(struct bufferevent* bev);
        void errorEvent(struct bufferevent* bev,short events);
    private:
        struct event_base* m_base = nullptr;
        struct event* m_evfifo = nullptr;
        std::atomic<int> m_eventcount;
        int  m_readfd = -1;
        int m_writefd = -1;
        std::thread* m_t;
        pthread_t m_thredId = 0;
        std::map<int,MyBufferEvent*> m_bufferEvents_map;
    };

}


#endif // !MYSOCKETEVENT_H
