/**
  ************************************* Copyright ******************************
  *                 (C) Copyright 2023,--,China, CUIT.
  *                            All Rights Reserved
  *                                 By(JCML)
  * FileName   : esp01s.h
  * Version    : v1.0
  * Author     : JCML
  * Date       : 2025/3/26
  * Description: ATָ������ESP01sģ��
******************************************************************************
 */
#ifndef ESP01S_H
#define ESP01S_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// ���ջ�������С
#define RX_BUFFER_SIZE 512
//��־���������Ҫɾ��printf����
#define Log(...) printf(__VA_ARGS__)
//�����շ�����ͨ��
#define UART huart3

typedef struct{
    const char* ssid;       // WiFi SSID
    const char* password;   // WiFi����

    const char* mqtt_server;     // MQTT��������ַ
    uint16_t mqtt_port;          // �˿ں�
    const char* mqtt_client_id;  // �ͻ���ID
    const char* mqtt_username;   // �û���(��ѡ)s
    const char* mqtt_password;   // ����(��ѡ)
    uint16_t keepalive;     // ��������ʱ��(��)
}esp01s_config_t;

// ��ʼ��ESP01sģ��
bool ESP01S_Init(void);
// ����ATָ��ȴ���Ӧ
static bool send_at_command(const char* cmd, const char* expected_response, uint32_t timeout);
// ������Ϣ��ָ������
bool MQTT_Publish(const char* topic, const char* payload, uint8_t qos, bool retain);
// ��������
bool MQTT_Subscribe(const char* topic, uint8_t qos);
// ȡ������
bool MQTT_Unsubscribe(const char* topic);
// ������յ�������(��Ҫ����ѭ���е���)
void MQTT_ProcessReceivedData(void);
// ���ڽ����жϴ�����(��Ҫ���жϷ��������е���)
void ESP01S_UART_RxCpltCallback(uint8_t byte);

extern uint8_t rxTemp;
extern char mqtt_payload_buffer[128];
extern volatile bool mqtt_new_msg_flag;

#endif // ESP01S_MQTT_H

