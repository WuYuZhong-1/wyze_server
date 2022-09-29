#ifndef MYBUFFEREVENT_H
#define MYBUFFEREVENT_H

#include "event2/bufferevent.h"

namespace wyze
{

    class MyBufferEvent {
    public:
        MyBufferEvent(struct bufferevent* bev);
        ~MyBufferEvent();

        void readEvent(struct bufferevent* bev);
        void writeEvent(struct bufferevent* bev);

    private:
        struct bufferevent* m_bev;
    };



} // namespace wyze




#endif // !MYBUFFEREVENT_H