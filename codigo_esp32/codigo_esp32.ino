#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// --- 1. CREDENCIAIS DE REDE (Ajuste para a sua rede)
const char* ssid = "SEU_WIFI_AQUI";
const char* password = "SUA_SENHA_AQUI";

// --- 2. CONFIGURAÇÃO DO BROKER MQTT ---
// (IP da sua máquina ou endereço do broker (nuvem) 
const char* mqtt_server = "IP_OU_ENDERECO_DO_BROKER"; 

WiFiClient espClient;
PubSubClient client(espClient);

// --- 3. CONFIGURAÇÃO NTP (Data e Hora para o campo "measuredAt")
WiFiUDP ntpUDP;
// -10800 é o fuso horário de Brasília (UTC-3 em segundos)
NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop até reconectar ao broker MQTT
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("ESP32_CO2_Client")) {
      Serial.println("Conectado ao Broker!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializa o Wi-Fi
  setup_wifi();
  
  // Configura o servidor MQTT (porta padrão 1883)
  client.setServer(mqtt_server, 1883);
  
  // Inicializa o cliente NTP para sincronizar a hora
  timeClient.begin();
}

void loop() {
  // Garante que a conexão MQTT está ativa
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Atualiza a hora da internet
  timeClient.update();

  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  
  // Envia um pacote de dados a cada 5 segundos
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Pega a data e hora formatada no padrão exigido pelo contrato
    String formattedDate = timeClient.getFormattedDate(); // Ex: 2026-10-15T19:20:00

    // Valores simulados (quando o sensor chegar, você substitui pelas leituras reais do SCD30)
    int co2Value = 850;
    float tempValue = 24.5;
    float humValue = 45.2;

    // Monta o JSON estritamente de acordo com o contrato CT1-publishReading.md
    char jsonBuffer[256];
    snprintf(jsonBuffer, sizeof(jsonBuffer),
      "{\"deviceId\":\"ESP32-SALA-01\",\"co2\":%d,\"temperature\":%.1f,\"humidity\":%.1f,\"measuredAt\":\"%s\"}",
      co2Value, tempValue, humValue, formattedDate.c_str()
    );

    // Publica no tópico oficial definido no contrato (/sensor/data)
    client.publish("/sensor/data", jsonBuffer);
    
    Serial.print("Mensagem enviada para /sensor/data: ");
    Serial.println(jsonBuffer);
  }
}