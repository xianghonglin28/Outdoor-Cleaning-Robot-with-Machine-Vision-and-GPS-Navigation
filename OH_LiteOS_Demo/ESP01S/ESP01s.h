/**
  ************************************* Copyright ******************************
  *                 (C) Copyright 2023,--,China, CUIT.
  *                            All Rights Reserved
  *                                 By(JCML)
  * FileName   : esp01s.h
  * Version    : v1.0
  * Author     : JCML
  * Date       : 2025/3/26
  * Description: AT指令驱动ESP01s模块
******************************************************************************
 */
#ifndef ESP01S_H
#define ESP01S_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// 接收缓冲区大小
#define RX_BUFFER_SIZE 512
//日志输出，不需要删除printf即可
#define Log(...) printf(__VA_ARGS__)
//配置收发串口通道
#define UART huart3

typedef struct{
    const char* ssid;       // WiFi SSID
    const char* password;   // WiFi密码

    const char* mqtt_server;     // MQTT服务器地址
    uint16_t mqtt_port;          // 端口号
    const char* mqtt_client_id;  // 客户端ID
    const char* mqtt_username;   // 用户名(可选)s
    const char* mqtt_password;   // 密码(可选)
    uint16_t keepalive;     // 保持连接时间(秒)
}esp01s_config_t;

// 初始化ESP01s模块
bool ESP01S_Init(void);
// 发送AT指令并等待响应
static bool send_at_command(const char* cmd, const char* expected_response, uint32_t timeout);
// 发布消息到指定主题
bool MQTT_Publish(const char* topic, const char* payload, uint8_t qos, bool retain);
// 订阅主题
bool MQTT_Subscribe(const char* topic, uint8_t qos);
// 取消订阅
bool MQTT_Unsubscribe(const char* topic);
// 处理接收到的数据(需要在主循环中调用)
void MQTT_ProcessReceivedData(void);
// 串口接收中断处理函数(需要在中断服务例程中调用)
void ESP01S_UART_RxCpltCallback(uint8_t byte);

extern uint8_t rxTemp;
extern char mqtt_payload_buffer[128];
extern volatile bool mqtt_new_msg_flag;

#endif // ESP01S_MQTT_H

