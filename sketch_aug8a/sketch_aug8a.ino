#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <string.h>

const char* ssid = "meussid";
const char* password = "minhasenha";
const char* mqtt_server = "broker.hivemq.com";
const char* topic_analog = "esp32/sensorChuva/analog";
const char* topic_comando = "esp32/controle";
const char* topic_seco = "esp32/sensorChuva/intensidade/seco";
const char* topic_leve = "esp32/sensorChuva/intensidade/leve";
const char* topic_moderada = "esp32/sensorChuva/intensidade/moderada";
const char* topic_forte = "esp32/sensorChuva/intensidade/forte";
const char* botToken = "meubottoken";
const char* chatId = "meuchatid";

WiFiClient espClient;
PubSubClient client(espClient);

#define LED_PIN 2
String intensidadeAtual = "";
String intensidadeAnterior = "";

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
    String url = "https://api.telegram.org/bot" + String(botToken) +
                 "/sendMessage?chat_id=" + String(chatId) +
                 "&text=" + mensagem;

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
  String mensagem = "";
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(mensagem);

  if (strcmp(topic, topic_comando) == 0) {
    if (mensagem == "LIGAR") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ligado!");
    } else if (mensagem == "DESLIGAR") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED desligado!");
    }
  } else if (
    strcmp(topic, topic_forte) == 0 ||
    strcmp(topic, topic_moderada) == 0 ||
    strcmp(topic, topic_leve) == 0 ||
    strcmp(topic, topic_seco) == 0
  ) {
    intensidadeAtual = mensagem;

    Serial.print("Intensidade recebida: ");
    Serial.println(intensidadeAtual);

    if (intensidadeAtual == "forte" || intensidadeAtual == "moderada") {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }

    if (intensidadeAtual != intensidadeAnterior) {
      if (intensidadeAtual != "seco") {
        String alerta = "Chuva detectada! Intensidade: " + intensidadeAtual;
        enviarAlertaTelegram(alerta);
      }
      intensidadeAnterior = intensidadeAtual;
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ESP32ClientHHCDSHCJ")) {
      Serial.println("Conectado.");
      client.subscribe(topic_comando);
      client.subscribe(topic_seco);
      client.subscribe(topic_leve);
      client.subscribe(topic_moderada);
      client.subscribe(topic_forte);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
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

  int valorAnalogico = analogRead(36);
  Serial.print("Leitura analógica do sensor: ");
  Serial.println(valorAnalogico);
  client.publish(topic_analog, String(valorAnalogico).c_str());

  delay(5000);
}
