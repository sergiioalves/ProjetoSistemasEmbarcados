# 🌧️ Sistema Embarcado de Detecção de Chuva com ESP32

## 📌 Descrição do Projeto

Este projeto consiste em um sistema embarcado para detecção de chuva em tempo real utilizando:

- **ESP32** como microcontrolador principal  
- **Sensor de chuva YL-83** com saídas digital e analógica  
- **Comunicação MQTT** para envio de dados  
- **Node-RED** para orquestração, visualização e persistência  
- **MongoDB** para armazenamento dos dados  
- **Telegram Bot** para envio de alertas instantâneos  

O objetivo é oferecer uma solução de baixo custo e fácil implementação para monitoramento climático, com possibilidade de automação de decisões.

---

## 🚀 Como Executar o Projeto

### 🔧 Ferramentas Necessárias

- ESP32 (placa com Wi-Fi integrado)  
- Sensor YL-83 (módulo + placa sensora)  
- LED + resistor (indicador visual)  
- Broker MQTT (Mosquitto local ou HiveMQ Cloud)  
- Node-RED com Dashboard  
- MongoDB (local ou Atlas)  
- Telegram Bot (token e chat_id configurados)  
- Software de monitoramento serial (ex: Arduino IDE)  
- MQTTX (para testes de publicação/assinatura MQTT)  

---

### 📥 Passo a Passo para Configuração

1. **Firmware no ESP32**
   - Rode o código disponível no repositório usando Arduino IDE ou VS Code com PlatformIO.
   - Configure as credenciais Wi-Fi e o broker MQTT.
   - O ESP32 publica:
     - Estado digital: `esp32/sensorChuva` → `"CHUVA"` ou `"SECO"`
     - Valor analógico: `esp32/sensorChuva/analog` → `0–4095`
     - Recebe comandos no tópico `esp32/controle` para controle do LED.

2. **Node-RED**
   - Se ainda não tiver, instale o Node-red na pagina oficial do **Node.js**;
   - Após isso, no seu terminal, digite **node-red** para rodar o editor;
   - Após rodar no navegador, clique na aba de três pontos > importar e importe os Jsons disponíveis nesse repositório.
   - Importe os fluxos para:
     - Receber dados MQTT  
     - Persistir no MongoDB  
     - Exibir no Dashboard (gauge + gráfico)  
     - Enviar alerta via Telegram  
 

3. **MongoDB**
   - Crie o banco `EstacaoMeteorologica` e a coleção `chuva`.
   - Os documentos armazenam:
     ```json
     {
       "estado": "CHUVA",
       "analogico": 812,
       "data": "2025-08-15T20:12:34.000Z"
     }
     ```

4. **Telegram Bot**
   - Crie um bot via [BotFather](https://t.me/BotFather)
   - Configure o token e chat_id no código do ESP32
   - Mensagem enviada: `"Está chovendo!"`

5. **Testes**
   - Simule chuva tocando a placa sensora com água.
   - Verifique:
     - Publicação MQTT  
     - Visualização no Dashboard  
     - Inserção no MongoDB  
     - Alerta no Telegram  

---

### 🧪 Exemplos de Payloads

| Tópico                     | Payload         | Descrição                     |
|---------------------------|------------------|-------------------------------|
| `esp32/sensorChuva`       | `"CHUVA"` / `"SECO"` | Estado digital do sensor      |
| `esp32/sensorChuva/analog`| `0–4095`         | Valor analógico (ADC 12 bits) |
| `esp32/controle`          | `{"led":true}`   | Comando para LED              |

---
