/*
 * File: NTPServer.h
 * Description:
 *   NTP server implementation.
 * Author: Mooneer Salem <mooneer@gmail.com>
 * License: New BSD License
 */
 
#ifndef NTP_SERVER_H
#define NTP_SERVER_H

#include "ITimeSource.h"

class NtpServer
{
public:
    NtpServer(ITimeSource &source)
        : timeSource_(source)
    {
        // empty
    }
    
    void beginListening(void);
    void processOneRequest(void);
    
private:
    ITimeSource &timeSource_;
    EthernetUDP timeServerPort_;
};

#endif // NTP_SERVER_H