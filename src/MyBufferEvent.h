#ifndef MYBUFFEREVENT_H
#define MYBUFFEREVENT_H

#include "event2/bufferevent.h"

namespace wyze
{

    class MyBufferEvent {
    public:
        virtual void readEvent(bufferevent* bev);
        virtual void writeEvent(bufferevent* bev);

    protected:
        char m_buf[1024]{0};
        int m_len = 0;
    };



} // namespace wyze




#endif // !MYBUFFEREVENT_H