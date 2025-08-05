#include "ESP01s.h"
#include "Network.h"
#include "MqttKit.h"
#include "usart.h"

#include "string.h"
#include "stdio.h"
#include "stdarg.h"





void Net_LinkSrever(void){
    char cmd[128] = {0};
    sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",MQTT_SERVER_IP,MQTT_PORT);
    while(Esp01s_SendCmd(cmd, "CONNECT",10))              DelayMS(100);
    LogPrintf("%s->Link Server OK\r\n",MQTT_SERVER_IP);
}

void Net_SendData(const char* topic, char *fmt,...){
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};		

    char cmd[256] = {0};
    int16_t len = 0;

    va_list args;
    va_start(args, fmt);
    len = vsprintf(cmd, fmt, args);
    va_end(args);
    if(len == 0 || len > 256){
        LogPrintf("Data Format Error\r\n");
        return;
    }    
    MQTT_PacketPublish(MQTT_PUBLISH_ID,topic,NULL,len,MQTT_QOS_LEVEL1,0,1,&mqttPacket);
    for(uint16_t i = 0; i < len; i++){
        mqttPacket._data[mqttPacket._len++] = cmd[i];
    }
    Esp01s_SendData(mqttPacket._data,mqttPacket._len);
    MQTT_DeleteBuffer(&mqttPacket);
}