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
  * Description: ATָ������ESP01sģ��,�Ѿ�ʵ��MQTT�ķ�����Ϣ�Ͷ�����Ϣ�Ļ�������
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

//����ESP01s������Ϣ
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

// ���ջ�����
uint8_t rx_buffer[RX_BUFFER_SIZE]; 
uint16_t rx_index = 0;
bool cmd_ready = false;
uint8_t rxTemp;

/*
* @brief: MQTT��Ϣ������,���ڶԽ��յ�����Ϣ���д���
            �˴�ֻ����Log���
* @param topic: ��������
* @param payload: ��Ϣ����
* @return: ��
*/

char mqtt_payload_buffer[128]; // ȫ�ֱ��������� payload
volatile bool mqtt_new_msg_flag = false; // ��־λ���յ�����Ϣʱ��Ϊ true

void mqtt_message_handler(const char* topic, const char* payload) {
    Log("Received message - Topic: %s, Payload: %s\n", topic, payload);
	
	  strncpy(mqtt_payload_buffer, payload, sizeof(mqtt_payload_buffer) - 1);
    mqtt_payload_buffer[sizeof(mqtt_payload_buffer) - 1] = '\0'; // ȷ���ַ�����β
    mqtt_new_msg_flag = true;
//		printf("Current MQTT Payload: %s\n", mqtt_payload_buffer);
}




/*
* @brief: �ײ㴮�ڷ��ͺ������˴���������ӿ�
* @param str: ���͵��ַ���
* @param len: �ַ�������
* @return: ��
*/
void uart_send(const char* str, uint16_t len) {
    HAL_UART_Transmit(&UART, (uint8_t*)str, len, 1000);
}

/*
* @brief: �����жϺ��������ڽ������ݣ�����Ķ�
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
* @brief: ������Ϣ��ָ������
* @param topic: ��������
* @param payload: ��Ϣ����
* @param qos: QoS����(0, 1, 2)Ĭ��Ϊ0����
                    QoS 0 �C ��ཻ��һ��
                    QoS 1 �C ���ٽ���һ��
                    QoS 2 �C ֻ����һ��
* @param retain: �Ƿ�����ϢĬ��Ϊfalse����
* @return: �������
*/
bool MQTT_Publish(const char* topic, const char* payload, uint8_t qos, bool retain) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=0,\"%s\",\"%s\",%d,%d\r\n",
            topic, payload, qos, retain ? 1 : 0);
    printf("[DEBUG] AT CMD: %s\n", cmd);

    return send_at_command(cmd, "OK", 3000);
}

/*
* @brief: ��������
* @param topic: ��������
* @param qos: QoS����(0, 1, 2)Ĭ��Ϊ1����
                    QoS 0 �C ��ཻ��һ��
                    QoS 1 �C ���ٽ���һ��
                    QoS 2 �C ֻ����һ��
* @return: �������
*/
bool MQTT_Subscribe(const char* topic, uint8_t qos) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=0,\"%s\",%d\r\n", topic, qos);
    return send_at_command(cmd, "OK", 3000);
}

/* 
* @brief: ȡ����������
* @param topic: ��������
* @return: �������
*/
bool MQTT_Unsubscribe(const char* topic) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "AT+MQTTUNSUB=0,\"%s\"\r\n", topic);
    return send_at_command(cmd, "OK", 3000);
}

/* 
* @brief: ������յ�������
* @param data: ���յ�������
* @param len: ���ݳ���
* @return: ��
*/
void MQTT_ProcessReceivedData(void) {
    if(cmd_ready) {
        rx_buffer[rx_index] = '\0';
        
        // ����Ƿ���MQTT��Ϣ
        char* msg_start = strstr((char*)rx_buffer, "+MQTTSUBRECV:");
        if(msg_start) {
					// Log("%s\n",msg_start);
            // ������Ϣ��ʽ: +MQTTSUBRECV:0,topic,payload_len,payload
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
* @brief: ��ʼ��ESP01sģ��
* @param: ��
* @return: ��
*/
bool ESP01S_Init(void) {
    char cmd[256];
    HAL_Delay(2000);
    HAL_UART_Receive_IT(&huart3, &rxTemp,1);
    rx_index = 0;
    cmd_ready = false;
    
    // ����ATָ��
    Log("1:Test AT command...  ");
    while(!send_at_command("AT\r\n", "OK", 1000)) {
        Log("Failed to send AT command, retrying...\n");
        HAL_Delay(1000);
    }
    Log("OK\n");

    // ���ñ�����һ��δ�ϵ絼�µ�����
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
   //mqtt����
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
* @brief: ����ATָ��ȴ���Ӧ
* @param: cmd: ATָ��
* @param: expected_response: ��������Ӧ
* @param: timeout: ��ʱʱ��(����)
* @return: �������
*/
static bool send_at_command(const char* cmd, const char* expected_response, uint32_t timeout) {
    uint32_t start_time = HAL_GetTick();
    
    // ��ս��ջ�����
    rx_index = 0;
    cmd_ready = false;
     
    // ����ATָ��
    uart_send(cmd, strlen(cmd));
    // �ȴ���Ӧ
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
* @brief: ������Ϣ�ص�����
* @param: callback: �ص�����
* @return: ��
*/
void ESP01S_UART_RxCpltCallback(uint8_t byte) {
    // ��黺�����Ƿ�����
    if(rx_index >= RX_BUFFER_SIZE - 1) {
        rx_index = 0; // ���������������
    }
    
    // �洢���յ����ֽ�
    rx_buffer[rx_index++] = byte;
    
    // ����Ƿ��յ���������(��\r\n��β)
    if(rx_index >= 2 && rx_buffer[rx_index-2] == '\r' && rx_buffer[rx_index-1] == '\n') {
        cmd_ready = true;
    }
}

