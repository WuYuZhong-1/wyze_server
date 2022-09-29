#include "MyBufferEvent.h"
#include <stdio.h>


namespace wyze
{
    MyBufferEvent::MyBufferEvent(struct bufferevent* bev):m_bev(bev)
    {
    }   

    MyBufferEvent::~MyBufferEvent()
    {
        bufferevent_free(m_bev);
    }

    void MyBufferEvent::readEvent(struct bufferevent* bev)
    {
        printf("bev=%p\n",bev);
        printf("m_bev=%p\n",m_bev);
    }

    void MyBufferEvent::writeEvent(struct bufferevent* bev)
    {
        printf("bev=%p\n",bev);
        printf("m_bev=%p\n",m_bev);
    }



} // namespace wyze
