#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H


extern unsigned long AP_timelimit;
//=================================GENERAL CONNECTION =================================================================================================================
void connectToWiFi();
long signalStrength();
void connectToAP();
void disconnect();

#endif