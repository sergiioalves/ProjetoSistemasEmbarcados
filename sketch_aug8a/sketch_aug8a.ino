#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

const char* ssid = "seuSsid";  
const char* password = "suaSenha"; 
const char* mqtt_server = "broker.hivemq.com";
const char* topic_status = "esp32/sensorChuva";
const char* topic_analog = "esp32/sensorChuva/analog";
const char* topic_comando = "esp32/controle";
const char* botToken = "seuBotToken"; 
const char* chatId = "seuchatId";    

WiFiClient espClient;
PubSubClient client(espClient);

#define CHUVA_DIGITAL_PIN 23
#define CHUVA_ANALOG_PIN 36
#define LED_PIN 2  

String estadoAnterior = "SECO";

void setup_wifi() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void enviarAlertaTelegram(const String& mensagem) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + String(botToken) + "/sendMessage?chat_id=" + String(chatId) + "&text=" + mensagem;
    Serial.print("Enviando alerta: ");
    Serial.println(mensagem);

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("Alerta enviado. Código HTTP: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro ao enviar alerta. Código: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Erro: WiFi desconectado.");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensagem);

  if (mensagem == "LIGAR") {
    Serial.println("LED ligado!");
    digitalWrite(LED_PIN, HIGH);
  } else if (mensagem == "DESLIGAR") {
    Serial.println("LED desligado!");
    digitalWrite(LED_PIN, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ESP32ClientHHCDSHCJ")) {
      Serial.println("Conectado.");
      client.subscribe(topic_comando); 
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(CHUVA_DIGITAL_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); 

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int statusDigital = digitalRead(CHUVA_DIGITAL_PIN);
  Serial.print("Leitura digital do sensor: ");
  Serial.println(statusDigital);
  String statusAtual = (statusDigital == HIGH) ? "SECO" : "CHUVA";
  Serial.print("Estado atual: ");
  Serial.println(statusAtual);
  client.publish(topic_status, statusAtual.c_str());

  int valorAnalogico = analogRead(CHUVA_ANALOG_PIN);
  Serial.print("Leitura analógica do sensor: ");
  Serial.println(valorAnalogico);
  client.publish(topic_analog, String(valorAnalogico).c_str());
  
  if (statusAtual != estadoAnterior && statusAtual == "CHUVA") {
    enviarAlertaTelegram("Está chovendo!");
  }

  estadoAnterior = statusAtual;

  delay(5000);
}




