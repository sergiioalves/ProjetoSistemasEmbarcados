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

## 1. **Firmware no ESP32**

- Rode o código disponível no repositório utilizando a **Arduino IDE** ou o **VS Code** com **PlatformIO**.
- Configure as credenciais do Wi-Fi e o broker MQTT no código.
  
### Funcionalidades principais do firmware:

- **Conexão Wi-Fi + MQTT**: O ESP32 realiza conexão e reconexão automática ao broker MQTT utilizando um **clientId fixo**.
- **Telemetria**: O valor analógico do sensor é lido a cada 5 segundos (via `analogRead(36)`) e publicado no tópico `esp32/sensorChuva/analog` no formato de valores entre 0 e 4095.
- **Controle do LED**: O ESP32 assina o tópico `esp32/controle` para receber comandos de controle do LED. Com os comandos `"LIGAR"` e `"DESLIGAR"`, o LED pode ser ligado ou desligado.
- **Monitoramento de Intensidade de Chuva**: O firmware assina os tópicos `esp32/sensorChuva/intensidade/seco`, `esp32/sensorChuva/intensidade/leve`, `esp32/sensorChuva/intensidade/moderada`, e `esp32/sensorChuva/intensidade/forte`. 
    - Ao receber um valor indicando chuva, o LED é atualizado para refletir a intensidade e, em transições para chuva (diferente de `"seco"`), um alerta é enviado via **Telegram**.
- **Alertas via Telegram**: Quando ocorre uma transição para chuva (qualquer intensidade diferente de `"seco"`), o firmware envia uma requisição HTTP GET para a **Telegram Bot API** (`/sendMessage`) para enviar um alerta com a intensidade da chuva.

### Dados publicados:
- **Estado digital**:  
  `esp32/sensorChuva` → `"CHUVA"` ou `"SECO"`
  
- **Valor analógico**:  
  `esp32/sensorChuva/analog` → Valor de 0 a 4095 (indicação da intensidade do sensor).

### Controle:
- **Comandos de controle**:  
  O ESP32 assina o tópico `esp32/controle` para controlar o LED:
  - **Ligar o LED**: Comando `"LIGAR"`.
  - **Desligar o LED**: Comando `"DESLIGAR"`.


## 2. **Node-RED**

O **Node-RED** é uma plataforma de desenvolvimento de fluxo para conectar dispositivos, APIs e serviços online de forma visual. Para integrar o Node-RED com o seu sistema de monitoramento, siga os passos abaixo.

### Passo 1: Instalar o Node-RED

1. **Instalar o Node.js**:
   - Se você ainda não tem o **Node.js** instalado, baixe e instale a versão mais recente a partir da [página oficial do Node.js](https://nodejs.org/). O Node-RED é executado sobre o Node.js, então esse passo é essencial.
   
2. **Instalar o Node-RED**:
   - Após a instalação do Node.js, abra o terminal ou prompt de comando e digite o seguinte comando para instalar o Node-RED globalmente:
     ```bash
     npm install -g --unsafe-perm node-red
     ```
   - Esse comando irá instalar o Node-RED e suas dependências.

### Passo 2: Rodar o Node-RED

1. **Iniciar o Node-RED**:
   - Depois de instalar o Node-RED, inicie o editor digitando o comando:
     ```bash
     node-red
     ```
   - Isso irá iniciar o servidor do Node-RED na sua máquina local, geralmente na porta **1880**. No terminal, você verá uma saída parecida com:
     ```bash
     22 Oct 10:00:00 - [info] Node-RED version: v2.x.x
     22 Oct 10:00:00 - [info] Node.js  version: v16.x.x
     22 Oct 10:00:00 - [info] Linux 5.x.x x64 LE
     22 Oct 10:00:00 - [info] Loading palette nodes
     22 Oct 10:00:00 - [info] Settings file  : /home/usuario/.node-red/settings.js
     22 Oct 10:00:00 - [info] Flows file     : /home/usuario/.node-red/flows.json
     22 Oct 10:00:00 - [info] Starting flows
     22 Oct 10:00:00 - [info] Server now running at http://127.0.0.1:1880/
     ```

2. **Acessar o Editor do Node-RED**:
   - Abra um navegador da web e digite o seguinte endereço na barra de URL:
     ```
     http://localhost:1880
     ```
   - O **Node-RED Editor** será carregado, onde você poderá arranjar os fluxos de dados visualmente.

### Passo 3: Importar Fluxos no Node-RED

Agora que o editor está em funcionamento, é hora de importar os fluxos JSON disponíveis no repositório para integrar com o seu sistema.

1. **Importar Fluxos**:
   - No editor do Node-RED, clique no ícone de três **pontos** no canto superior direito (ao lado do botão de "Deploy").
   - Selecione **Importar** e depois **Escolher arquivo**.
   - Navegue até o local onde você armazenou os fluxos JSON e selecione os arquivos desejados para importar.

### Passo 4: Fluxos a serem Importados

Após importar os fluxos JSON, você terá as seguintes funcionalidades configuradas:

- **Receber dados MQTT**:
  - O Node-RED irá se conectar ao broker MQTT (como o que você configurou no ESP32) e receber dados de tópicos como `esp32/sensorChuva/analog` e `esp32/sensorChuva`. 
  - Use o nó **MQTT Input** para assinar os tópicos de interesse (exemplo: `esp32/sensorChuva/analog`), que irá trazer os valores publicados pelo ESP32.

- **Persistir no MongoDB**:
  - Os dados recebidos (como intensidade de chuva ou valor analógico) podem ser armazenados em um banco de dados **MongoDB** para persistência. 
  - Importe os fluxos que já configuram a conexão com o MongoDB, utilizando o nó **mongodb** para gravar os dados no banco.
  - Certifique-se de que o MongoDB esteja instalado e em execução, ou utilize uma instância em nuvem (como MongoDB Atlas).

- **Exibir no Dashboard (gauge + gráfico)**:
  - O Node-RED oferece um **Dashboard** interativo onde você pode visualizar os dados recebidos de forma gráfica.
  - Importe os fluxos que criam um **gauge (medidor de intensidade)** e um **gráfico** para mostrar a intensidade da chuva e os valores analógicos ao longo do tempo.
  - Use os nós de visualização, como **gauge** e **chart**, para criar essas interfaces.

- **Enviar alerta via Telegram**:
  - Caso haja uma mudança significativa nos dados de intensidade (por exemplo, a detecção de chuva), o fluxo pode enviar um alerta para o seu **Telegram Bot**.
  - O Node-RED já tem um nó **Telegram Sender** que irá enviar mensagens para o chat configurado com o bot, utilizando a **API do Telegram**.
  - O fluxo irá enviar mensagens como "Está chovendo!" sempre que a intensidade da chuva mudar para qualquer valor diferente de "seco".

### Passo 5: Configurações Finais

1. **Configurar o MQTT Broker**:
   - No Node-RED, ao importar os fluxos, você precisará configurar o **Broker MQTT**. Clique no nó de **MQTT Input** ou **MQTT Output** e insira o **endereço do broker** (por exemplo, `broker.hivemq.com` ou o broker que você estiver usando), o **port 1883** e as credenciais necessárias.

2. **Configurar o MongoDB**:
   - Para persistir os dados, configure o nó **mongodb** com a URL do seu servidor MongoDB e o banco de dados onde os dados serão armazenados.

3. **Configurar o Telegram**:
   - No nó **Telegram Sender**, insira o **token** e o **chat_id** que você obteve ao criar o bot no Telegram, para que o Node-RED possa enviar alertas.

### Passo 6: Deploy e Monitoramento

- Após importar e configurar todos os fluxos, clique em **Deploy** no canto superior direito do editor para salvar e ativar os fluxos.
- Agora você pode monitorar os dados de chuva, visualizar gráficos, controlar o LED via Node-RED e receber alertas no seu Telegram, tudo em tempo real!

---

### Observações

- Certifique-se de que o **Node-RED**, o **broker MQTT**, o **MongoDB** e o **Telegram** estejam funcionando corretamente para garantir a integração completa.
- O Node-RED é uma ferramenta visual e flexível, permitindo que você adicione, remova ou modifique fluxos facilmente conforme suas necessidades.

Com esses passos detalhados, agora você pode integrar o Node-RED no seu sistema para gerenciar dados de sensores, persistir informações e enviar alertas.

 

## 3. **MongoDB**

Para persistir os dados coletados, utilizamos o banco de dados **MongoDB**. Siga os passos abaixo para configurar o banco e a coleção, além de entender como os documentos serão armazenados.

### Passo 1: Criar o Banco de Dados e a Coleção

1. **Crie o Banco de Dados**:
   - O banco de dados a ser utilizado será o **`EstacaoMeteorologica`**. Você pode criar o banco de dados diretamente no MongoDB com o comando:
     ```bash
     use EstacaoMeteorologica
     ```

2. **Criar a Coleção**:
   - A coleção que armazenará os dados será **`chuva`**. Caso ela ainda não exista, ela será criada automaticamente quando o primeiro documento for inserido.
     ```bash
     db.createCollection("chuva")
     ```

### Passo 2: Estrutura dos Documentos

Cada documento na coleção **`chuva`** representa uma leitura do sensor de chuva, com informações sobre a intensidade da chuva, o valor analógico e a data/hora da leitura.

Exemplo de documento:

```json
{
  "sensor": "YL-83",               // Nome do sensor utilizado (ex: YL-83)
  "analogico": 3172,               // Valor analógico do sensor (0-4095)
  "intensidade": "moderada",       // Intensidade da chuva (ex: "seco", "leve", "moderada", "forte")
  "data": "2025-08-15T21:25:11.103Z" // Data e hora no formato ISO 8601 (UTC)
}
```

## 4. **Telegram Bot**

Para enviar alertas de chuva via Telegram, você precisa criar um bot e obter o **token** e o **chat_id**. Siga as etapas abaixo para configurar seu bot:

### Passo 1: Criar o Bot no Telegram

1. **Abrir o Telegram**:
   - Abra o aplicativo Telegram no seu celular ou desktop.
   
2. **Procurar pelo BotFather**:
   - Busque por **BotFather** usando a barra de pesquisa do Telegram. O BotFather é o bot oficial do Telegram para criar outros bots.

3. **Criar um novo Bot**:
   - Inicie uma conversa com o **BotFather**.
   - Digite o comando `/newbot` para criar um novo bot.
   - O BotFather vai te pedir para **dar um nome** para o seu bot (pode ser algo como "Bot da Chuva").
   - Em seguida, você precisará fornecer um **username** para o bot (que deve ser único e terminar com "bot", por exemplo: `botdachuva_bot`).
   
4. **Obter o Token do Bot**:
   - Após criar o bot, o BotFather irá te enviar um **token de autenticação**. Este token é essencial para se comunicar com a API do Telegram. O token será semelhante a isto:
     ```
     1234567890:ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890
     ```
   - **Copie o token** gerado.

### Passo 2: Obter o Chat ID

Para que o bot saiba para qual conversa ele deve enviar a mensagem, você precisa do **chat_id**. Esse ID é único para cada usuário ou grupo no Telegram.

#### Para um chat individual:

1. **Inicie uma conversa com o seu bot**:
   - Busque pelo seu bot (utilizando o username criado) e comece uma conversa com ele.
   
2. **Obter o chat_id**:
   - Para obter o `chat_id` da conversa, você pode acessar a URL abaixo no seu navegador (substituindo `TOKEN_DO_SEU_BOT` pelo token do bot que você copiou no Passo 1):
     ```
     https://api.telegram.org/botTOKEN_DO_SEU_BOT/getUpdates
     ```
   - Você verá uma resposta JSON no formato:
     ```json
     {
       "ok": true,
       "result": [
         {
           "update_id": 123456789,
           "message": {
             "message_id": 1234,
             "from": {
               "id": 12345678,
               "is_bot": false,
               "first_name": "SeuNome",
               "last_name": "Sobrenome",
               "username": "SeuUsername",
               "language_code": "pt"
             },
             "chat": {
               "id": 1234567890,
               "first_name": "SeuNome",
               "last_name": "Sobrenome",
               "username": "SeuUsername",
               "type": "private"
             },
             "date": 1633036800,
             "text": "Olá, bot!"
           }
         }
       ]
     }
     ```
   - O campo `"chat": { "id": 1234567890, ... }` contém o **chat_id** necessário. Copie esse número.

#### Para um grupo:

1. **Adicione seu bot a um grupo**.
2. **Envie uma mensagem para o grupo**.
3. **Obtenha o chat_id** com o mesmo procedimento anterior. Após enviar a mensagem para o grupo, a resposta JSON incluirá o `chat_id` do grupo.

### Passo 3: Configurar o Bot no Código do ESP32

Agora que você tem o **token** e o **chat_id**, você pode configurar o bot no código do ESP32.

1. **Abra o código no Arduino IDE ou PlatformIO**.
2. **Adicione o token e o chat_id no código**. Localize as variáveis `botToken` e `chatId` e substitua pelos valores que você obteve.

   ```cpp
   const char* botToken = "SEU_TOKEN_AQUI";
   const char* chatId = "SEU_CHAT_ID_AQUI";


5. **Testes**
   - Simule chuva tocando a placa sensora com água.
   - Verifique:
     - Publicação MQTT  
     - Visualização no Dashboard  
     - Inserção no MongoDB  
     - Alerta no Telegram  

---

### 🧪 Exemplos de Payloads

| Tópico                         | Payload                | Descrição                                                       |
|---------------------------------|------------------------|-----------------------------------------------------------------|
| `esp32/sensorChuva`            | `"CHUVA"` / `"SECO"`   | Estado digital do sensor (indica se está chovendo ou seco)     |
| `esp32/sensorChuva/analog`     | `0–4095`               | Valor analógico (ADC 12 bits) representando a intensidade do sensor de chuva |
| `esp32/controle`               | `{"led": true}`        | Comando para controlar o LED (ligar: `true`, desligar: `false`) |
| `esp32/sensorChuva/intensidade/seco`   | `"seco"`          | Intensidade da chuva: "seco" (sem chuva)                       |
| `esp32/sensorChuva/intensidade/leve`   | `"leve"`          | Intensidade da chuva: "leve" (chuva fraca)                     |
| `esp32/sensorChuva/intensidade/moderada`| `"moderada"`     | Intensidade da chuva: "moderada" (chuva moderada)               |
| `esp32/sensorChuva/intensidade/forte`  | `"forte"`         | Intensidade da chuva: "forte" (chuva intensa)                  |

### Observações Finais

- O ESP32 publica o **estado do sensor de chuva** no tópico `esp32/sensorChuva` com os valores `"CHUVA"` ou `"SECO"`.
- O **valor analógico** do sensor (0-4095) é enviado periodicamente no tópico `esp32/sensorChuva/analog`.
- O ESP32 monitora os tópicos de intensidade (`esp32/sensorChuva/intensidade/*`), atualizando o LED de acordo com a intensidade da chuva (ex: `"leve"`, `"moderada"`, `"forte"`, `"seco"`).
- O **controle do LED** é feito via tópico `esp32/controle`, onde o payload é um objeto JSON:
  - `{ "led": true }` para ligar o LED.
  - `{ "led": false }` para desligar o LED.
  
Esses tópicos garantem a comunicação em tempo real entre o ESP32, o MQTT Broker, e outros sistemas como o Node-RED e o Telegram, permitindo a automação do monitoramento de chuva e envio de alertas.


---
