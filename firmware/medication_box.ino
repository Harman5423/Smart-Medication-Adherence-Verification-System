#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "SD.h"
#include "SPI.h"
#include "driver/i2s.h"

// -------- WIFI --------
const char* ssid     = "Wifi's Name";
const char* password = "Wifi's Password";

// -------- DEEPGRAM API --------
const char* deepgramKey = "your_deepgram_key_here";

// -------- HIVEMQ --------
const char* mqttHost = "your-cluster-id.s1.eu.hivemq.cloud";
const char* mqttUser = "your_mqtt_username";
const char* mqttPass = "your_mqtt_password";

// -------- TLS CERTIFICATE (ISRG Root X1) --------
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// -------- OLED --------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// -------- PINS --------
int irPins[3]  = {15,  4, 16};
int ledPins[3] = {17, 21, 22};

// -------- SD --------
#define SD_CS 13

// -------- I2S MIC (INMP441) --------
#define I2S_WS  25
#define I2S_SD  27
#define I2S_SCK 26

// -------- RECORDING SETTINGS --------
#define SAMPLE_RATE    16000
#define RECORD_FRAMES  375

// -------- TIMEOUT --------
#define LID_OPEN_TIMEOUT_MS  300000UL   // 5 minutes

// -------- MQTT OBJECTS --------
WiFiClientSecure espClient;
PubSubClient     mqttClient(espClient);

// -------- STATE --------
int  activeCompartment = -1;
bool alertActive       = false;
bool waitingForClose   = false;
unsigned long closeStart     = 0;
unsigned long alertStartTime = 0;
bool closeTimer = false;

// ================================================================
// DISPLAY
// ================================================================
void show(String line1, String line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println(line1);
  if (line2 != "") {
    display.setCursor(0, 35);
    display.println(line2);
  }
  display.display();
}

// ================================================================
// MQTT PUBLISH HELPER
// ================================================================
void publishStatus(int compartment, const char* status) {
  char payload[64];
  snprintf(payload, sizeof(payload),
           "{\"compartment\":%d,\"status\":\"%s\"}", compartment, status);
  bool ok = mqttClient.publish("medreminder/status", payload, true);
  Serial.printf("MQTT publish [%s] → %s\n", ok ? "OK" : "FAIL", payload);
}

// ================================================================
// MQTT CALLBACK
// ================================================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  // Debug: confirm callback fired
  Serial.printf(">>> CALLBACK FIRED: topic=%s len=%d\n", topic, length);

  // Print raw payload
  Serial.print(">>> Raw payload: ");
  for (unsigned int i = 0; i < length; i++) Serial.print((char)payload[i]);
  Serial.println();

  // Null-terminate safely — buffer large enough for full web app payload
  char msg[512] = {0};
  memcpy(msg, payload, min((unsigned int)511, length));

  // Large enough JSON doc for full web app payload
  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, msg);
  if (err) {
    Serial.printf("MQTT: JSON parse failed: %s\n", err.c_str());
    return;
  }

  if (strcmp(topic, "medreminder/alarm")  == 0 ||
      strcmp(topic, "medreminder/manual") == 0) {

    if (alertActive) {
      Serial.println("MQTT: alert already active, ignoring");
      return;
    }

    int comp = doc["compartment"] | -1;
    Serial.printf("MQTT: compartment field = %d\n", comp);

    if (comp < 0 || comp > 2) {
      Serial.println("MQTT: invalid compartment (must be 0, 1 or 2)");
      return;
    }

    // Log medicine name if present
    String medName = doc["medicine_name"] | "Medicine";
    Serial.printf("MQTT: medicine = %s\n", medName.c_str());

    // Arm the alert — loop() picks it up next iteration
    activeCompartment = comp;
    alertActive       = true;
    waitingForClose   = false;
    closeTimer        = false;
    alertStartTime    = millis();

    Serial.printf("MQTT: armed compartment %d successfully\n", comp);

  } else {
    Serial.printf("MQTT: unknown topic '%s', ignoring\n", topic);
  }
}

// ================================================================
// I2S SETUP
// ================================================================
void setupI2S() {
  i2s_config_t config = {
    .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate          = SAMPLE_RATE,
    .bits_per_sample      = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format       = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count        = 8,
    .dma_buf_len          = 256
  };
  i2s_pin_config_t pins = {
    .bck_io_num   = I2S_SCK,
    .ws_io_num    = I2S_WS,
    .data_out_num = -1,
    .data_in_num  = I2S_SD
  };
  i2s_driver_install(I2S_NUM_0, &config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

// ================================================================
// WAV HEADER
// ================================================================
void writeWavHeader(File& file, int dataBytes) {
  int fileSize = dataBytes + 36;
  int byteRate = SAMPLE_RATE * 2;
  byte h[44]   = {0};
  memcpy(h, "RIFF", 4);
  h[4]=fileSize;     h[5]=fileSize>>8;  h[6]=fileSize>>16; h[7]=fileSize>>24;
  memcpy(h+8, "WAVEfmt ", 8);
  h[16]=16; h[20]=1; h[22]=1;
  h[24]=SAMPLE_RATE;     h[25]=SAMPLE_RATE>>8;
  h[26]=SAMPLE_RATE>>16; h[27]=SAMPLE_RATE>>24;
  h[28]=byteRate;        h[29]=byteRate>>8;
  h[30]=byteRate>>16;    h[31]=byteRate>>24;
  h[32]=2; h[34]=16;
  memcpy(h+36, "data", 4);
  h[40]=dataBytes;     h[41]=dataBytes>>8;
  h[42]=dataBytes>>16; h[43]=dataBytes>>24;
  file.seek(0);
  file.write(h, 44);
}

// ================================================================
// RECORD AUDIO
// ================================================================
void recordAudio(const char* path) {
  SD.remove(path);
  File file = SD.open(path, FILE_WRITE);
  if (!file) { Serial.println("SD open failed"); return; }

  for (int i = 0; i < 44; i++) file.write((byte)0);

  int32_t buf[256];
  size_t  bytesRead;
  int     totalBytes = 0;
  float   dcOffset   = 0.0f;

  // Discard startup transients
  for (int w = 0; w < 16; w++)
    i2s_read(I2S_NUM_0, buf, sizeof(buf), &bytesRead, portMAX_DELAY);

  for (int f = 0; f < RECORD_FRAMES; f++) {
    i2s_read(I2S_NUM_0, buf, sizeof(buf), &bytesRead, portMAX_DELAY);
    int samples = bytesRead / 4;
    for (int i = 0; i < samples; i++) {
      int32_t raw = buf[i] >> 8;
      float   s   = (float)(raw >> 8);
      dcOffset    = 0.995f * dcOffset + 0.005f * s;
      s          -= dcOffset;
      s          *= 3.0f;
      if      (s >  32767) s =  32767;
      else if (s < -32768) s = -32768;
      int16_t out = (int16_t)s;
      if (abs(out) < 30) out = 0;
      file.write((uint8_t*)&out, 2);
      totalBytes += 2;
    }
  }

  writeWavHeader(file, totalBytes);
  file.close();
  Serial.printf("Recorded %d bytes\n", totalBytes);
}

// ================================================================
// DEEPGRAM TRANSCRIPTION
// ================================================================
String transcribe(const char* path) {
  File file = SD.open(path);
  if (!file) { Serial.println("File open failed"); return ""; }

  HTTPClient http;
  http.begin("https://api.deepgram.com/v1/listen?model=nova-2&language=en-IN&punctuate=false");
  http.addHeader("Authorization", String("Token ") + deepgramKey);
  http.addHeader("Content-Type", "audio/wav");
  http.setTimeout(8000);

  int code = http.sendRequest("POST", &file, file.size());
  file.close();

  if (code != 200) {
    Serial.printf("Deepgram HTTP error: %d\n", code);
    http.end();
    return "";
  }

  String response = http.getString();
  http.end();

  DynamicJsonDocument doc(4096);
  if (deserializeJson(doc, response) != DeserializationError::Ok) {
    Serial.println("JSON parse error");
    return "";
  }

  String transcript = doc["results"]["channels"][0]["alternatives"][0]["transcript"].as<String>();
  transcript.toLowerCase();
  Serial.println("Heard: " + transcript);
  return transcript;
}

// ================================================================
// KEYWORD CHECK
// ================================================================
bool isMedicineTaken(String text) {
  return (text.indexOf("medicine") >= 0 && text.indexOf("taken") >= 0);
}

// ================================================================
// MQTT RECONNECT
// ================================================================
void reconnectMQTT() {
  if (mqttClient.connected()) return;

  Serial.printf("Connecting to MQTT %s:8883 as '%s'...\n", mqttHost, mqttUser);

  if (mqttClient.connect("ESP32MedBox", mqttUser, mqttPass)) {
    Serial.println("MQTT connected!");

    // QoS 1 — broker retries if ESP32 briefly misses it
    bool s1 = mqttClient.subscribe("medreminder/alarm",  1);
    bool s2 = mqttClient.subscribe("medreminder/manual", 1);
    Serial.printf("Subscribed alarm=%d manual=%d\n", s1, s2);

    show("MQTT Ready", "Waiting...");
  } else {
    int rc = mqttClient.state();
    Serial.printf("MQTT failed rc=%d\n", rc);
    char err[16];
    snprintf(err, sizeof(err), "rc=%d", rc);
    show("MQTT fail", err);
    delay(5000);
  }
}

// ================================================================
// SETUP
// ================================================================
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 3; i++) {
    pinMode(irPins[i],  INPUT);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  Wire.begin(32, 33);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(2);
  display.setTextColor(WHITE);
  show("Booting...", "");

  if (!SD.begin(SD_CS)) { show("SD failed!"); while (true); }

  setupI2S();

  WiFi.begin(ssid, password);
  show("WiFi...", "");
  while (WiFi.status() != WL_CONNECTED) delay(500);
  show("WiFi OK", WiFi.localIP().toString());
  Serial.printf("WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
  delay(1000);

  // Proper TLS verification using ISRG Root X1 certificate
  espClient.setCACert(root_ca);

  // setBufferSize BEFORE setServer — handles full web app payload
  mqttClient.setBufferSize(1024);
  mqttClient.setServer(mqttHost, 8883);
  mqttClient.setCallback(mqttCallback);

  reconnectMQTT();
}

// ================================================================
// LOOP
// ================================================================
void loop() {

  // Keep MQTT alive
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  // ---- IDLE: waiting for web app command ----
  static bool wasIdle = false;
  if (!alertActive) {
    if (!wasIdle) {
      show("Ready", "Waiting for app");
      wasIdle = true;
    }
    delay(50);
    return;
  }
  wasIdle = false;

  // ---- ALERT TRIGGERED ----
  static int lastArmedCompartment = -1;
  if (lastArmedCompartment != activeCompartment) {
    lastArmedCompartment = activeCompartment;
    digitalWrite(ledPins[activeCompartment], HIGH);
    String msg = "Take Med C" + String(activeCompartment + 1);
    show(msg, "Open & say phrase");
    publishStatus(activeCompartment, "triggered");
    Serial.println(msg);
  }

  // ---- MISSED TIMEOUT ----
  if (!waitingForClose &&
      millis() - alertStartTime > LID_OPEN_TIMEOUT_MS) {
    digitalWrite(ledPins[activeCompartment], LOW);
    show("Missed!", "No response");
    publishStatus(activeCompartment, "missed");
    activeCompartment    = -1;
    alertActive          = false;
    lastArmedCompartment = -1;
    delay(2000);
    return;
  }

  // ---- WAIT FOR LID OPEN, THEN RECORD ----
  if (!waitingForClose) {
    if (digitalRead(irPins[activeCompartment]) == HIGH) {

      show("Listening...", "Say: medicine taken");
      publishStatus(activeCompartment, "listening");
      delay(400);

      recordAudio("/med.wav");

      show("Checking...", "");
      String text = transcribe("/med.wav");

      if (text.length() < 2) {
        show("No audio", "Try again");
        publishStatus(activeCompartment, "no_audio");

      } else if (isMedicineTaken(text)) {
        show("Confirmed!", "Close the lid");
        publishStatus(activeCompartment, "confirmed");
        waitingForClose = true;

      } else {
        show("Wrong phrase", text.substring(0, 18));
        publishStatus(activeCompartment, "wrong_phrase");
      }

      delay(2000);
    }
  }

  // ---- WAIT FOR LID CLOSE (2s stable) ----
  if (waitingForClose) {
    if (digitalRead(irPins[activeCompartment]) == LOW) {
      if (!closeTimer) { closeStart = millis(); closeTimer = true; }

      if (millis() - closeStart > 2000) {
        digitalWrite(ledPins[activeCompartment], LOW);
        show("Done!", "");
        publishStatus(activeCompartment, "done");

        activeCompartment    = -1;
        alertActive          = false;
        waitingForClose      = false;
        closeTimer           = false;
        lastArmedCompartment = -1;
        delay(2000);
      }
    } else {
      closeTimer = false;
    }
  }
}
