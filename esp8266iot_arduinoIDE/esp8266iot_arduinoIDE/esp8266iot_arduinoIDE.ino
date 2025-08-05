#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/******************************* 硬件配置 ************************************/
#define LEDPIN 0               // LED引脚（低电平触发）

/******************************* WiFi配置 ************************************/
#define WIFI_SSID     "clare"
#define WIFI_PASSWORD "xhlxhlxhl"

/******************************* MQTT配置 ***********************************/
#define device_id  "6814519e84adf27cda549bd5_car_armwifi"
#define secret     "car123456"    
#define ClientId   "6814519e84adf27cda549bd5_car_armwifi_0_0_2025050209"
#define Username   "6814519e84adf27cda549bd5_car_armwifi"
#define Password   "5dd35aa635c080e72123dfe454bda6ea184d118aadc9d2d564e449d39ba70d8d"
#define MQTT_Address "ca61807ba4.st1.iotda-device.cn-north-4.myhuaweicloud.com"
#define MQTT_Port 1883

/******************************* MQTT Topic *********************************/
#define Iot_link_MQTT_Topic_Report "$oc/devices/" device_id "/sys/properties/report"
#define Iot_link_MQTT_Topic_Commands "$oc/devices/" device_id "/sys/commands/#" 
#define Iot_link_MQTT_Topic_CommandsRes "$oc/devices/" device_id "/sys/commands/response/request_id="

/******************************* 全局变量 ***********************************/
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastReportTime = 0;
const unsigned long reportInterval = 1000; // 5秒上报间隔
int last_reset = -1;          // 记录上次执行的reset值
int current_reset = -1;       // 当前正在处理的reset值
int reset_result = -1;        // reset执行结果（来自STM32）
uint32_t lastActionTime = 0;  // 上次执行动作的时间
const uint32_t actionCooldown = 500; // 动作冷却时间(ms)
String current_request_id = ""; // 当前处理的请求ID

// GPS和OpenMV数据
float gps_lat = 0.0;
float gps_lon = 0.0;
int openmv_distance = 0;
int openmv_x = 0;
int openmv_y = 0;

/******************************* 函数声明 ***********************************/
void WIFI_Init();
void MQTT_Init();
void maintainConnections();
void MQTT_POST();
void callback(char* topic, byte* payload, unsigned int length);
void sendResponse(const char* topic, int code, const String& message);
void parseSerialData();
void parseDebugCommands();
void printSystemStatus();
void processReset(int reset_value);
void sendToSTM32(int reset_value);
void reportResetResult(int result);

//==========================================================================
// 初始化
//==========================================================================
void setup() {
  Serial.begin(115200);  // 调试串口
  Serial1.begin(115200); // 硬件UART1 (TX=GPIO2) - 用于发送命令给STM32

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH); // 初始化LED熄灭
  
  WIFI_Init();  // 连接WiFi
  MQTT_Init();  // 连接MQTT

  Serial.println("\n[System Ready]");
  Serial.println("Available commands:");
  Serial.println("  0 - Simulate RESET=0 action");
  Serial.println("  1 - Simulate RESET=1 action");
  Serial.println("  2 - Simulate RESET=2 action");
  Serial.println("  3 - Simulate RESET=3 action");
  Serial.println("  r - Force report status to cloud");
  Serial.println("  s - Print system status");
}

//==========================================================================
// 主循环
//==========================================================================
void loop() {
  //1. 维持网络连接
  maintainConnections();

  // 2. 处理MQTT消息
  if (client.connected()) {
    client.loop();
  }

  // 3. 处理串口数据（来自STM32）
  parseSerialData();
  
  // 4. 处理调试命令（来自串口监视器）
  parseDebugCommands();

  // 5. 定时上报状态
  if (millis() - lastReportTime >= reportInterval) {
    MQTT_POST();
    lastReportTime = millis();
  }

  delay(10); // 防止 watchdog 触发
}

//==========================================================================
// 处理调试命令（来自串口监视器）
//==========================================================================
void parseDebugCommands() {
  if (Serial.available()) {
    char command = Serial.read();
    
    switch (command) {
      case '0': // 模拟RESET=0动作
        processReset(0);
        break;
        
      case '1': // 模拟RESET=1动作
        processReset(1);
        break;
        
      case '2': // 模拟RESET=2动作
        processReset(2);
        break;
        
      case '3': // 模拟RESET=3动作
        processReset(3);
        break;
        
      case 'r': // 强制上报状态
        Serial.println("[Debug] Forcing status report to cloud");
        MQTT_POST();
        lastReportTime = millis(); // 重置定时器
        break;
        
      case 's': // 打印系统状态
        printSystemStatus();
        break;
        
      case '\n': // 忽略换行符
      case '\r':
        break;
        
      default:
        Serial.println("[Debug] Unknown command. Available: 0,1,2,3,r,s");
        break;
    }
  }
}

//==========================================================================
// 处理reset命令
//==========================================================================
void processReset(int reset_value) {
  uint32_t currentTime = millis();
  
  // 检查冷却时间
  if (currentTime - lastActionTime >= actionCooldown) {
    lastActionTime = currentTime;
    
    // 更新reset状态
    last_reset = reset_value;
    current_reset = reset_value;
    
    Serial.printf("[Reset] Processing reset: %d\n", reset_value);
    
    // 发送reset指令给STM32
    sendToSTM32(reset_value);
    
    // 立即上报状态
    MQTT_POST();
  } else {
    Serial.println("[WARN] Action too frequent! Ignored.");
  }
}

//==========================================================================
// 发送reset指令给STM32
//==========================================================================
void sendToSTM32(int reset_value) {
  // 只发送有效reset值 (0,1,2,3)
  if (reset_value >= 0 && reset_value <= 3) {
    Serial1.write(reset_value); // 直接发送字节值
    Serial.printf("[Reset] Sending to STM32: 0x%02X (%d)\n", reset_value, reset_value);
    
    // 闪烁LED指示动作执行
    digitalWrite(LEDPIN, LOW);
    delay(100);
    digitalWrite(LEDPIN, HIGH);
  }
}

//==========================================================================
// WiFi初始化
//==========================================================================
void WIFI_Init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LEDPIN, !digitalRead(LEDPIN)); // 闪烁LED
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LEDPIN, HIGH); // 连接成功后关闭LED
}

//==========================================================================
// MQTT初始化
//==========================================================================
void MQTT_Init() {
  client.setServer(MQTT_Address, MQTT_Port);
  client.setCallback(callback);

  if (client.connect(ClientId, Username, Password)) {
    Serial.println("MQTT Connected!");
    
    // 订阅命令Topic
    bool subResult = client.subscribe(Iot_link_MQTT_Topic_Commands);
    Serial.printf("Subscribe to commands topic: %s\n", subResult ? "Success" : "Failed");
    
    // 打印订阅的Topic
    Serial.printf("Subscribed to: %s\n", Iot_link_MQTT_Topic_Commands);
    
    // LED指示连接成功
    for (int i = 0; i < 3; i++) {
      digitalWrite(LEDPIN, LOW);
      delay(100);
      digitalWrite(LEDPIN, HIGH);
      delay(100);
    }
  } else {
    Serial.print("MQTT Failed, rc=");
    Serial.println(client.state());
    digitalWrite(LEDPIN, LOW); // 连接失败时亮灯
  }
}

//==========================================================================
// 维持网络连接
//==========================================================================
void maintainConnections() {
  // WiFi重连
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    digitalWrite(LEDPIN, LOW); // 重连时亮灯
    
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      digitalWrite(LEDPIN, !digitalRead(LEDPIN)); // 闪烁LED
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi reconnected!");
      digitalWrite(LEDPIN, HIGH);
    } else {
      Serial.println("\nWiFi reconnect failed!");
    }
  }
  
  // MQTT重连
  if (!client.connected()) {
    Serial.println("MQTT disconnected! Reconnecting...");
    digitalWrite(LEDPIN, LOW); // 重连时亮灯
    
    MQTT_Init(); // 重新初始化MQTT连接
    
    if (client.connected()) {
      digitalWrite(LEDPIN, HIGH);
    }
  }
}

//==========================================================================
// MQTT数据上报
//==========================================================================
void MQTT_POST() {
  // 构建上报数据
  char report[512];
  snprintf(report, sizeof(report),
   "{\"services\":["
    "{\"service_id\":\"arm_control\",\"properties\":{"
    "\"last_reset\":%d,"
    "\"reset_result\":%d}},"
    "{\"service_id\":\"gps\",\"properties\":{"
    "\"lat\":%.6f,"
    "\"lon\":%.6f}},"
    "{\"service_id\":\"openmv\",\"properties\":{"
    "\"distance\":%d,"
    "\"x\":%d,"
    "\"y\":%d}}"
    "]}", 
    last_reset, 
    reset_result,
    gps_lat,
    gps_lon,
    openmv_distance,
    openmv_x,
    openmv_y);

  bool result = client.publish(Iot_link_MQTT_Topic_Report, report);
  
  // 输出上报结果
  if (result) {
    Serial.printf("[Cloud] Status reported: last_reset=%d, reset_result=%d\n", 
                  last_reset, reset_result);
    Serial.printf("[Cloud] GPS: lat=%.6f, lon=%.6f\n", gps_lat, gps_lon);
    Serial.printf("[Cloud] OpenMV: distance=%d, x=%d, y=%d\n", 
                  openmv_distance, openmv_x, openmv_y);
  } else {
    Serial.println("[Cloud] Failed to report status!");
  }
}

//==========================================================================
// 上报reset结果到云端
//==========================================================================
void reportResetResult(int result) {
  // 确保有当前请求ID
  if (current_request_id == "") {
    Serial.println("[WARN] No request ID for result reporting");
    return;
  }
  
  // 构建响应Topic
  String responseTopic = String(Iot_link_MQTT_Topic_CommandsRes) + current_request_id;
  
  // 构建响应内容
  char response[256];
  snprintf(response, sizeof(response),
    "{\"result_code\":0,\"result_desc\":\"OK\",\"paras\":{\"reset0\":%d}}",
    result);
  
  Serial.println("\n[Reporting Result]");
  Serial.printf("Topic: %s\n", responseTopic.c_str());
  Serial.printf("Content: %s\n", response);
  
  // 发送响应
  bool pubResult = client.publish(responseTopic.c_str(), response);
  Serial.printf("Publish result: %s\n", pubResult ? "Success" : "Failed");
  
  // 清空请求ID
  current_request_id = "";
}

//==========================================================================
// MQTT回调函数 - 处理云端命令
//==========================================================================
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n[MQTT Message]");
  Serial.print("Topic: ");
  Serial.println(topic);

  // 检查是否是指令Topic
  if (String(topic).indexOf("/sys/commands/") != -1) {
    Serial.println("Valid command topic detected");
  } else {
    Serial.println("Unknown topic type");
    return;
  }

  // 解析payload为字符串
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Payload: ");
  Serial.println(msg);

  // 提取request_id
  int requestIdStart = String(topic).lastIndexOf('=') + 1;
  current_request_id = String(topic).substring(requestIdStart);
  Serial.printf("Extracted request_id: %s\n", current_request_id.c_str());

  // 解析JSON
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.print("JSON Parse Error: ");
    Serial.println(error.c_str());
    sendResponse(topic, 400, "Invalid JSON: " + String(error.c_str()));
    return;
  }

  // 检查命令名称是否为"arm"
  if (!doc.containsKey("command_name")) {
    sendResponse(topic, 401, "Missing command_name");
    return;
  }
  
  String cmd = doc["command_name"].as<String>();
  if (cmd != "arm") {
    sendResponse(topic, 401, "Invalid command_name: " + cmd);
    return;
  }
  
  // 检查参数
  if (!doc["paras"].containsKey("reset")) {
    sendResponse(topic, 402, "Missing reset parameter");
    return;
  }
  
  // 先发送"命令已接收"的快速响应
  sendResponse(topic, 0, "Command received");

  // 获取reset值
  int reset_value = doc["paras"]["reset"].as<int>();
  
  // 检查reset值是否有效
  if (reset_value < 0 || reset_value > 3) {
    Serial.printf("[ERROR] Invalid reset value: %d\n", reset_value);
    return;
  }

  // 处理reset命令
  processReset(reset_value);
}

//==========================================================================
// 发送MQTT响应
//==========================================================================
void sendResponse(const char* topic, int code, const String& message) {
  // 提取request_id
  int requestIdStart = String(topic).lastIndexOf('=') + 1;
  String request_id = String(topic).substring(requestIdStart);
  
  // 构建响应Topic
  String responseTopic = String(Iot_link_MQTT_Topic_CommandsRes) + request_id;
  
  // 构建响应内容
  String response = "{\"result_code\":" + String(code) + ",\"result_desc\":\"" + message + "\"}";
  
  Serial.println("\n[Sending Response]");
  Serial.printf("Response Topic: %s\n", responseTopic.c_str());
  Serial.printf("Response Content: %s\n", response.c_str());
  
  // 发送响应
  bool pubResult = client.publish(responseTopic.c_str(), response.c_str());
  Serial.printf("Publish result: %s\n", pubResult ? "Success" : "Failed");
  
  // 检查MQTT状态
  if (!pubResult) {
    Serial.printf("MQTT Publish Error: %d\n", client.state());
  }
}

//==========================================================================
// 解析串口数据（来自STM32）- 接收执行结果
//==========================================================================
void parseSerialData() {
  static String inputString = "";
  static uint32_t lastReceivedTime = 0;
  static uint8_t lastReceivedValue = 0xFF;

  while (Serial1.available()) {
    char received = (char)Serial1.read();
    // 在parseSerialData中收到字符时打印
Serial.print("Received char: ");
Serial.println(received); // 查看是否有数据到达ESP8266
    // ========= 1. 处理 JSON 行 =========
    if (received == '\n') {
      // 检查是否是 JSON 格式
      if (inputString.startsWith("{")) {
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, inputString);
        if (error) {
          Serial.print("[JSON Error] ");
          Serial.println(error.c_str());
        } else {
          // 判断是 GPS 还是 OpenMV
          if (doc.containsKey("lat") && doc.containsKey("lon")) {
            gps_lat = doc["lat"];
            gps_lon = doc["lon"];
            Serial.printf("[GPS] lat: %.6f, lon: %.6f\n", gps_lat, gps_lon);
          } else if (doc.containsKey("distance")) {
            openmv_distance = doc["distance"];
            openmv_x = doc["x"];
            openmv_y = doc["y"];
            Serial.printf("[OpenMV] distance: %d, x: %d, y: %d\n", openmv_distance, openmv_x, openmv_y);
          } else {
            Serial.println("[JSON] Unknown content:");
            Serial.println(inputString);
          }
        }
      }

      inputString = ""; // 清空行缓冲
    }
    // 累加 JSON 数据
    else if (received >= 32 && received <= 126) { // 只接收可见字符
      inputString += received;
    }

    // ========= 2. 处理单字节命令（0-3） =========
    else if ((uint8_t)received >= 0 && (uint8_t)received <= 3) {
      uint8_t val = (uint8_t)received;

      if (val == lastReceivedValue && (millis() - lastReceivedTime < 500)) {
        Serial.printf("[Filter] Ignored duplicate value: %d\n", val);
        continue;
      }

      // 保存并处理结果
      reset_result = val;
      lastReceivedValue = val;
      lastReceivedTime = millis();
      Serial.printf("[Result] Received from STM32: %d\n", val);

      reportResetResult(val);
      last_reset = current_reset;
      current_reset = -1;
      MQTT_POST();
    }

    // ========= 3. 打印未知字符（调试用） =========
    else if ((uint8_t)received > 3) {
      Serial.printf("[Status] Received byte: 0x%02X (%d)\n", received, received);
    }
  }
}

//==========================================================================
// 打印系统状态（调试用）
//==========================================================================
void printSystemStatus() {
  Serial.println("\n--- System Status ---");
  Serial.print("WiFi: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "OK" : "Disconnected");
  Serial.print("MQTT: ");
  Serial.println(client.connected() ? "OK" : "Disconnected");
  Serial.printf("Last Reset: %d\n", last_reset);
  Serial.printf("Current Reset: %d\n", current_reset);
  Serial.printf("Reset Result: %d\n", reset_result);
  Serial.printf("GPS Data: lat=%.6f, lon=%.6f\n", gps_lat, gps_lon);
  Serial.printf("OpenMV Data: distance=%d, x=%d, y=%d\n", openmv_distance, openmv_x, openmv_y);
  Serial.printf("Current Request ID: %s\n", current_request_id.c_str());
  Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
  Serial.println("---------------------");
}