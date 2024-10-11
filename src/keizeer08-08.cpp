#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Definições de pinos
#define SS_PIN 5
#define RST_PIN 2
#define DHTPIN 13 // Pino do sensor DHT
#define DHTTYPE DHT22 // Ou DHT11
#define mqtt_topic "Estacionatech/circuito1"
#define mqtt_topic "Estacionatech/circuito2"

// Criar instâncias dos objetos
MFRC522 mfrc522(SS_PIN, RST_PIN); // Instância do MFRC522
Adafruit_SSD1306 display(128, 64, &Wire, -1); // Instância do OLED
DHT_Unified dht(DHTPIN, DHTTYPE); // Instância do DHT

// Prototipos
unsigned long lerRFID(void);
void mostrarSensor();

void setup() {
  Serial.begin(9600); // Iniciar comunicação serial
  SPI.begin();        // Iniciar SPI
  mfrc522.PCD_Init(); // Iniciar MFRC522

  // Iniciar o display OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 0x3C é o endereço I2C padrão para a maioria dos displays OLED
  display.clearDisplay(); // Limpar o display
  display.setTextSize(1); // Tamanho do texto
  display.setTextColor(WHITE); // Cor do texto

  // Iniciar o sensor DHT
  dht.begin();
}

void loop() {
  unsigned long uid = lerRFID();

  if (uid > 1) {
    // Limpar o display antes de escrever
    display.clearDisplay();

    // Exibir UID no display OLED
    display.setCursor(0, 0); // Definir o cursor na posição (0,0)
    display.print("UID: ");
    display.println(uid);
    display.display(); // Atualizar o display

    // Mostrar informações do sensor
    mostrarSensor();
  }

  delay(1000); // Atraso entre leituras
}

unsigned long lerRFID() {
  // Procura por cartões RFID
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }

  // Seleciona um dos cartões
  if (!mfrc522.PICC_ReadCardSerial()) {
    return 1;
  }

  unsigned long numericUID = 0;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    numericUID = numericUID * 256 + mfrc522.uid.uidByte[i];
  }

  return numericUID;
}

void mostrarSensor() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (event.temperature) {
    display.setCursor(0, 20);
    display.print("Temp: ");
    display.print(event.temperature);
    display.println(" C");
  } else {
    display.setCursor(0, 20);
    display.println("Temp: Erro");
  }

  dht.humidity().getEvent(&event);
  if (event.relative_humidity) {
    display.setCursor(0, 40);
    display.print("Umid: ");
    display.print(event.relative_humidity);
    display.println(" %");
  } else {
    display.setCursor(0, 40);
    display.println("Umid: Erro");
  }

  display.display(); // Atualizar o display
}