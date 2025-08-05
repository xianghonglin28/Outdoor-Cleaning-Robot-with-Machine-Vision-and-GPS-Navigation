#ifndef NETWORK_H
#define NETWORK_H

#include "main.h"
#include "MqttKit.h"

void Net_LinkSrever(void);
void Net_SendData(const char* topic, char *fmt,...);

#endif // !ESP01S_H

