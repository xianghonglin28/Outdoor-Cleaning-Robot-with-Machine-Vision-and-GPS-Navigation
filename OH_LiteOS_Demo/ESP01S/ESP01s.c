/* esp01s.c */
/**
  ************************************* Copyright ******************************
  *                 (C) Copyright 2023,--,China, CUIT.
  *                            All Rights Reserved
  *                                 By(JCML)
  * FileName   : esp01s.c
  * Version    : v1.0
  * Author     : JCML
  * Date       : 2025/3/26
  * Description: AT指令驱动ESP01s模块,已经实现MQTT的发布消息和订阅消息的基础功能
******************************************************************************
 */
#include "esp01s.h"
#include "usart.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "ax_openmv.h"
#include "gpio.h"
#include "oled.h"

//配置ESP01s连接信息
esp01s_config_t config = {
    .ssid = "MUN",
    .password = "123456789wu",
    .mqtt_server = "192.168.112.79",/*broker.emqx.io*/
    .mqtt_port = 1883,
    .mqtt_client_id = "ESP01s",
    .mqtt_username = NULL,
    .mqtt_password = NULL,
    .keepalive = 60
};

// 接收缓冲区
uint8_t rx_buffer[RX_BUFFER_SIZE]; 
uint16_t rx_index = 0;
bool cmd_ready = false;
uint8_t rxTemp;

/*
* @brief: MQTT消息处理函数,用于对接收到的消息进行处理
            此处只是做Log输出
* @param topic: 主题名称
* @param payload: 消息内容
* @return: 无
*/

char mqtt_payload_buffer[128]; // 全局变量，保存 payload
volatile bool mqtt_new_msg_flag = false; // 标志位，收到新消息时设为 true

void mqtt_message_handler(const char* topic, const char* payload) {
    Log("Received message - Topic: %s, Payload: %s\n", topic, payload);
	
	  strncpy(mqtt_payload_buffer, payload, sizeof(mqtt_payload_buffer) - 1);
    mqtt_payload_buffer[sizeof(mqtt_payload_buffer) - 1] = '\0'; // 确保字符串结尾
    mqtt_new_msg_flag = true;
//		printf("Current MQTT Payload: %s\n", mqtt_payload_buffer);
}




/*
* @brief: 底层串口发送函数，此处定义输出接口
* @param str: 发送的字符串
* @param len: 字符串长度
* @return: 无
*/
void uart_send(const char* str, uint16_t len) {
    HAL_UART_Transmit(&UART, (uint8_t*)str, len, 1000);
}

/*
* @brief: 串口中断函数，用于接收数据，按需改动
*/
uint8_t c;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart == &UART){
        ESP01S_UART_RxCpltCallback(rxTemp);
        rxTemp = 0x00;
        HAL_UART_Receive_IT(&huart3, &rxTemp, 1);
    }
		else if(huart->Instance == USART2)
			{
        OpenMV_Receive_Data(c);
        HAL_UART_Receive_IT(&huart2, &c, 1);
    }
}




/*
* @brief: 发布消息到指定主题
* @param topic: 主题名称
* @param payload: 消息内容
* @param qos: QoS级别(0, 1, 2)默认为0即可
                    QoS 0 – 最多交付一次
                    QoS 1 – 至少交付一次
                    QoS 2 – 只交付一次
* @param retain: 是否保留消息默认为false即可
* @return: 操作结果
*/
bool MQTT_Publish(const char* topic, const char* payload, uint8_t qos, bool retain) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=0,\"%s\",\"%s\",%d,%d\r\n",
            topic, payload, qos, retain ? 1 : 0);
    printf("[DEBUG] AT CMD: %s\n", cmd);

    return send_at_command(cmd, "OK", 3000);
}

/*
* @brief: 订阅主题
* @param topic: 主题名称
* @param qos: QoS级别(0, 1, 2)默认为1即可
                    QoS 0 – 最多交付一次
                    QoS 1 – 至少交付一次
                    QoS 2 – 只交付一次
* @return: 操作结果
*/
bool MQTT_Subscribe(const char* topic, uint8_t qos) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=0,\"%s\",%d\r\n", topic, qos);
    return send_at_command(cmd, "OK", 3000);
}

/* 
* @brief: 取消订阅主题
* @param topic: 主题名称
* @return: 操作结果
*/
bool MQTT_Unsubscribe(const char* topic) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "AT+MQTTUNSUB=0,\"%s\"\r\n", topic);
    return send_at_command(cmd, "OK", 3000);
}

/* 
* @brief: 处理接收到的数据
* @param data: 接收到的数据
* @param len: 数据长度
* @return: 无
*/
void MQTT_ProcessReceivedData(void) {
    if(cmd_ready) {
        rx_buffer[rx_index] = '\0';
        
        // 检查是否是MQTT消息
        char* msg_start = strstr((char*)rx_buffer, "+MQTTSUBRECV:");
        if(msg_start) {
					// Log("%s\n",msg_start);
            // 解析消息格式: +MQTTSUBRECV:0,topic,payload_len,payload
            int link_id, topic_len, payload_len;
            char topic[128], payload[256];
            if(sscanf(msg_start, "+MQTTSUBRECV:%d,%[^,],%d,%[^\r\n]", 
                     &link_id, topic, &payload_len, payload) == 4) {
                        mqtt_message_handler(topic, payload);
            }
        }
        
        cmd_ready = false;
        rx_index = 0;
    }
}

/*
* @brief: 初始化ESP01s模块
* @param: 无
* @return: 无
*/
bool ESP01S_Init(void) {
    char cmd[256];
    HAL_Delay(2000);
    HAL_UART_Receive_IT(&huart3, &rxTemp,1);
    rx_index = 0;
    cmd_ready = false;
    
    // 测试AT指令
    Log("1:Test AT command...  ");
    while(!send_at_command("AT\r\n", "OK", 1000)) {
        Log("Failed to send AT command, retrying...\n");
        HAL_Delay(1000);
    }
    Log("OK\n");

    // 重置避免上一次未断电导致的问题
    Log("2:RESTORE...          ");
    if(!send_at_command("AT+RESTORE\r\n", "OK", 1000)) {
        Log("Failed to disable echo, retrying...\n");
    }
    Log("OK\n");
    HAL_Delay(1000);//need to wait for the module to restart
    Log("3:Set CWMODE...       ");
    while(!send_at_command("AT+CWMODE=1\r\n", "OK", 5000)) {
        Log("Failed to set CWMODE, retrying...\n");
        HAL_Delay(1000);
    }
    Log("OK\n");
    
   Log("4:Connect to WiFi...  ");
   memset(cmd, 0, sizeof(cmd));
   sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", config.ssid, config.password);
   while(!send_at_command(cmd, "OK", 5000)) {
       Log("Failed to connect to WiFi, retrying...\n");
       HAL_Delay(1000);
   }
   Log("OK\n");
   //mqtt部分
    Log("5:MQTT user config... ");
    memset(cmd, 0, sizeof(cmd));
    if(config.mqtt_username != NULL && config.mqtt_password != NULL){
        sprintf(cmd,"AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0\r\n", config.mqtt_client_id, config.mqtt_username, config.mqtt_password);
    }else{
        sprintf(cmd,"AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"\r\n", config.mqtt_client_id);
    }
    while(!send_at_command(cmd, "OK", 5000)) {
        Log("Failed to set MQTT user config, retrying...\n");
        HAL_Delay(3000);
    }
    Log("OK\n");
		
		
    Log("6:MQTT Connect...     ");
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "AT+MQTTCONN=0,\"%s\",%d,0\r\n", config.mqtt_server, config.mqtt_port);
    while(!send_at_command(cmd, "OK", 5000)) {
        Log("Failed to connect to MQTT server, retrying...\n");
        HAL_Delay(3000);
    }
		Log("OK\n");
    return true;
}

/*
* @brief: 发送AT指令并等待响应
* @param: cmd: AT指令
* @param: expected_response: 期望的响应
* @param: timeout: 超时时间(毫秒)
* @return: 操作结果
*/
static bool send_at_command(const char* cmd, const char* expected_response, uint32_t timeout) {
    uint32_t start_time = HAL_GetTick();
    
    // 清空接收缓冲区
    rx_index = 0;
    cmd_ready = false;
     
    // 发送AT指令
    uart_send(cmd, strlen(cmd));
    // 等待响应
    while(HAL_GetTick() - start_time < timeout) {
        if(cmd_ready) {
            rx_buffer[rx_index] = '\0';
            if(strstr((char*)rx_buffer, expected_response) != NULL) {
								memset(rx_buffer, 0, sizeof(rx_buffer));
								rx_index = 0;
                return true;
            }
            cmd_ready = false;
            rx_index = 0;
        }
    }
		// Log("AT Recive: %s\n", rx_buffer);//for Debug
    return false;
}

/*
* @brief: 设置消息回调函数
* @param: callback: 回调函数
* @return: 无
*/
void ESP01S_UART_RxCpltCallback(uint8_t byte) {
    // 检查缓冲区是否已满
    if(rx_index >= RX_BUFFER_SIZE - 1) {
        rx_index = 0; // 缓冲区溢出，重置
    }
    
    // 存储接收到的字节
    rx_buffer[rx_index++] = byte;
    
    // 检查是否收到完整命令(以\r\n结尾)
    if(rx_index >= 2 && rx_buffer[rx_index-2] == '\r' && rx_buffer[rx_index-1] == '\n') {
        cmd_ready = true;
    }
}

