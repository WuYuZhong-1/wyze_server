#include "MyBufferEvent.h"
#include <stdio.h>
#include <string.h>
#include "MyLoggerManager.h"
#include <ctype.h>


namespace wyze
{
    void MyBufferEvent::readEvent(bufferevent* bev)
    {
        memset(m_buf, 0, sizeof(m_buf));
        m_len = bufferevent_read(bev, m_buf, sizeof(m_buf));
        INFO("client:{}", m_buf);
        for(int i = 0; i < m_len; ++i) {
            m_buf[i] = toupper(m_buf[i]);
        }
        bufferevent_write(bev, m_buf, m_len);
    }

    void MyBufferEvent::writeEvent(bufferevent* bev)
    {
        // printf("bev=%p\n",bev);
    }



} // namespace wyze
