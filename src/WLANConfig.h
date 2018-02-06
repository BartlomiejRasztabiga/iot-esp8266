#ifndef WLANConfig_H
#define WLANConfig_H

struct WLANConfig
{
    static const char* ssid;
    static const char* password;
};

const char* WLANConfig::ssid = "nie_zgaduj_hasla";
const char* WLANConfig::password = "#aLlEnDoR@5_2013#";

#endif // WLANConfig_H
