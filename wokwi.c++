#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Configurações - variáveis editáveis
const char* default_SSID = "Wokwi-GUEST"; // Nome da rede Wi-Fi
const char* default_PASSWORD = ""; // Senha da rede Wi-Fi
const char* default_BROKER_MQTT = "46.17.108.131"; // IP do Broker MQTT
const int default_BROKER_PORT = 1883; // Porta do Broker MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/sensor001/cmd"; // Tópico MQTT de escuta
const char* default_TOPICO_PUBLISH_1 = "/TEF/sensor001/attrs"; // Tópico MQTT de envio de informações combinadas
const char* default_TOPICO_TEMP = "/TEF/sensor001/temperature"; // Tópico MQTT para temperatura
const char* default_TOPICO_UMID = "/TEF/sensor001/humidity";    // Tópico MQTT para umidade
const char* default_TOPICO_LDR = "/TEF/sensor001/ldr";          // Tópico MQTT para LDR
const char* default_ID_MQTT = "fiware_sensor_001"; // ID MQTT
const int default_D4 = 2; // Pino do LED onboard

// Declaração da variável para o prefixo do tópico
const char* topicPrefix = "sensor001";

// Definição dos pinos
#define DHTPIN 15       // Pino conectado ao DHT22
#define DHTTYPE DHT22   // Tipo do sensor DHT

#define LDR_PIN 34      // Pino analógico do LDR (AO)

// Inicialização dos objetos
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD, colunas, linhas
WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';
unsigned long lastMsg = 0;

void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(default_SSID);
    Serial.println("Aguarde");
    reconectWiFi();
}

void initMQTT() {
    MQTT.setServer(default_BROKER_MQTT, default_BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

void setup() {
    initSerial();
    InitOutput();
    initWiFi();
    initMQTT();
    pinMode(LDR_PIN, INPUT);
    dht.begin();
    lcd.init();
    lcd.backlight();
    delay(5000);
    MQTT.publish(default_TOPICO_PUBLISH_1, "s|on");
}

void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return;
    WiFi.begin(default_SSID, default_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Conectado com sucesso na rede ");
    Serial.println(default_SSID);
    Serial.print("IP obtido: ");
    Serial.println(WiFi.localIP());

    // Garantir que o LED inicie desligado
    digitalWrite(default_D4, LOW);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        msg += c;
    }
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);

    // Forma o padrão de tópico para comparação
    String onTopic = String(topicPrefix) + "@on|";
    String offTopic = String(topicPrefix) + "@off|";

    // Compara com o tópico recebido
    if (msg.equals(onTopic)) {
        digitalWrite(default_D4, HIGH);
        EstadoSaida = '1';
    }

    if (msg.equals(offTopic)) {
        digitalWrite(default_D4, LOW);
        EstadoSaida = '0';
    }
}

void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT();
    reconectWiFi();
}

void EnviaEstadoOutputMQTT() {
    boolean success = false;
    if (EstadoSaida == '1') {
        success = MQTT.publish(default_TOPICO_PUBLISH_1, "s|on");
        Serial.println("- LED Ligado");
    } else if (EstadoSaida == '0') {
        success = MQTT.publish(default_TOPICO_PUBLISH_1, "s|off");
        Serial.println("- LED Desligado");
    }

    if (success) {
        Serial.println("- Estado do LED enviado ao broker com sucesso!");
    } else {
        Serial.println("- Falha ao enviar o estado do LED ao broker.");
    }
    Serial.println("-------------------------");
    delay(1000);
}

void InitOutput() {
    pinMode(default_D4, OUTPUT);
    digitalWrite(default_D4, HIGH);
    boolean toggle = false;

    for (int i = 0; i <= 10; i++) {
        toggle = !toggle;
        digitalWrite(default_D4, toggle);
        delay(200);
    }
    digitalWrite(default_D4, LOW); // Garante que o LED comece desligado
    EstadoSaida = '0';
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(default_BROKER_MQTT);
        if (MQTT.connect(default_ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(default_TOPICO_SUBSCRIBE);
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}

void handleSensors() {
    // Lê os valores dos sensores
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int ldrValue = analogRead(LDR_PIN);

    // Verifica se houve erro na leitura
    if (isnan(h) || isnan(t)) {
        Serial.println("Falha na leitura do DHT22!");
        return;
    }

    // Converte o valor do LDR para porcentagem (0 a 100)
    float ldrPercentage = (ldrValue / 4095.0) * 100.0;

    // Formata os dados em JSON
    char payload[250];
    snprintf(payload, sizeof(payload), "{\"temperature\":%.2f,\"humidity\":%.2f,\"ldr\":%.2f}", t, h, ldrPercentage);

    // Publica os dados no tópico MQTT (dados combinados)
    MQTT.publish(default_TOPICO_PUBLISH_1, payload);

    // Publica temperatura separadamente
    char tempPayload[20];
    snprintf(tempPayload, sizeof(tempPayload), "%.2f", t);
    MQTT.publish(default_TOPICO_TEMP, tempPayload);

    // Publica umidade separadamente
    char humPayload[20];
    snprintf(humPayload, sizeof(humPayload), "%.2f", h);
    MQTT.publish(default_TOPICO_UMID, humPayload);

    // Publica LDR separadamente
    char ldrPayload[20];
    snprintf(ldrPayload, sizeof(ldrPayload), "%.2f", ldrPercentage);
    MQTT.publish(default_TOPICO_LDR, ldrPayload);

    // Mostra os dados no display LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.print(t);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Umid:");
    lcd.print(h);
    lcd.print("%");

    // Imprime os dados no Serial Monitor
    Serial.println("Dados dos sensores:");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" °C");
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.println(" %");
    Serial.print("LDR: ");
    Serial.print(ldrPercentage);
    Serial.println(" %");
    Serial.println("-------------------------");
}

void loop() {
    VerificaConexoesWiFIEMQTT();
    EnviaEstadoOutputMQTT();
    MQTT.loop();

    unsigned long now = millis();
    if (now - lastMsg > 1000) { // Intervalo de 1 segundo
        lastMsg = now;
        handleSensors();
    }
}










