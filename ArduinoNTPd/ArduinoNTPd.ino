/*
 * File: arduino-ntpd.ino
 * Description:
 *   Main application code.
 * Author: Mooneer Salem <mooneer@gmail.com>
 * License: New BSD License
 */

#if defined(ARDUINO)

#include "Arduino.h"
#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#include "config.h"
#include "NTPServer.h"
#include "SerialDataSource.h"
#include "GPSTimeSource.h"
#include "NTPPacket.h"
#include "HTTPServer.h"
#include "TimeUtilities.h"

#include "HtmlStrings.h"

SerialDataSource dataSource;
GPSTimeSource timeSource(dataSource);
NtpServer timeServer(timeSource);

void rootPage(HttpServer *server)
{
    server->responseRedirect("/time");
}

static void zeroPrepend(HttpServer *server, int val)
{
    if (val < 10)
    {
        server->print("0");
    }
    server->print(val);
}

void timePage(HttpServer *server)
{
    server->responseOK();
    uint32_t secs, fract;
    timeSource.now(&secs, &fract);
    
    uint32_t year, month, day, hour, minute, second;
    TimeUtilities::dateFromNumberOfSeconds(secs, &year, &month, &day, &hour, &minute, &second);
    
    server->print(COMMON_PAGE_HEADER);
    server->print(TIME_PAGE_HEADER);
    server->print(year);
    server->print("-");
    zeroPrepend(server, month);
    server->print("-");
    zeroPrepend(server, day);
    server->print(" ");
    zeroPrepend(server, hour);
    server->print(":");
    zeroPrepend(server, minute);
    server->print(":");
    zeroPrepend(server, second);
    server->print(TIME_PAGE_FOOTER);
    server->print(COMMON_PAGE_FOOTER);
}

void positionPage(HttpServer *server)
{
    server->responseOK();
    timeSource.now(NULL, NULL);
    int32_t latitude = timeSource.latitude();
    int32_t longitude = timeSource.longitude();
    
    server->print(COMMON_PAGE_HEADER);
    server->print(POSITION_PAGE_HEADER);
    server->print(latitude / 100000);
    server->print(".");
    server->print(abs(latitude % 100000));
    server->print(", ");
    server->print(longitude / 100000);
    server->print(".");
    server->print(abs(longitude % 100000));
    server->print(POSITION_PAGE_FOOTER);
    server->print(COMMON_PAGE_FOOTER);
}

UrlHandler handlers[] = {
    UrlHandler("/", rootPage),
    UrlHandler("/time", timePage),
    UrlHandler("/location", positionPage)
};

HttpServer httpServer(handlers, 3);

void setup()
{
    // Print banner.
    Serial.begin(115200);
    Serial.println("ArduinoNTPd starting.");
    
    // Set up network.
    Ethernet.begin(macAddress, ipAddress);
    dataSource.begin();
    timeServer.beginListening();
    httpServer.beginListening();

    // Enable GPS interrupts.
    timeSource.enableInterrupts();
}

void loop()
{
    timeServer.processOneRequest();
    httpServer.processOneRequest();
}

#endif // defined(ARDUINO)
