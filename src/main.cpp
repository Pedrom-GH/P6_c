#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <time.h>

// --- DATOS DEL WIFI YA PUESTOS ---
const char* ssid = "Mi_Red_ESP32"; 
const char* password = "password123";    

// --- PINES DEL ESP32 ---
#define SD_CS_PIN 5      // Pin CS para la tarjeta SD
#define RFID_CS_PIN 15   // Pin CS para el lector RFID
#define RFID_RST_PIN 22  // Pin Reset para el lector RFID

MFRC522 mfrc522(RFID_CS_PIN, RFID_RST_PIN);

// --- CONFIGURACIÓN DE LA HORA (ESPAÑA) ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;      // +1 hora
const int   daylightOffset_sec = 3600; // Horario de verano

// Función para descargar la hora del reloj mundial
String obtenerHora() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "Hora Desconocida";
  }
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%d/%m/%Y %H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  // TRUCO HARDWARE: Apagamos ambos módulos antes de empezar para que no choquen
  pinMode(SD_CS_PIN, OUTPUT);
  pinMode(RFID_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  digitalWrite(RFID_CS_PIN, HIGH);

  // --- CONEXIÓN AL WIFI ---
  Serial.println("\n--- INICIANDO SISTEMA DE REGISTRO ---");
  Serial.print("Conectando al WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi conectado.");
  
  // Sincronizamos el reloj interno del ESP32 con Internet
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // --- INICIAR BUS SPI ---
  SPI.begin();

  // --- INICIAR SD ---
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("❌ Error: No se encontro la tarjeta SD.");
  } else {
    Serial.println("✅ Tarjeta SD montada y lista.");
  }

  // --- INICIAR RFID ---
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Antena al máximo
  Serial.println("✅ Lector RFID listo.");
  Serial.println("\nESPERANDO TARJETAS... (Acerca el llavero)");
}

void loop() {
  // 1. Si hay una tarjeta y podemos leerla...
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    
    // 2. Miramos qué hora es
    String marcaDeTiempo = obtenerHora();

    // 3. Sacamos el código de la tarjeta (UID)
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    // 4. Preparamos la línea de texto que vamos a guardar
    String lineaRegistro = "[" + marcaDeTiempo + "] UID:" + uid + "\n";
    
    // Imprimimos por pantalla para que lo veas
    Serial.print("NUEVO FICHAJE: " + lineaRegistro);

    // 5. Guardamos físicamente en la SD (fichero.log)
    File file = SD.open("/fichero.log", FILE_APPEND);
    if(file) {
      file.print(lineaRegistro);
      file.close();
      Serial.println("   -> Guardado en la SD correctamente.");
    } else {
      Serial.println("   -> ❌ Fallo al intentar escribir en la SD.");
    }

    // 6. Pausamos la lectura
    mfrc522.PICC_HaltA();
    delay(1500); 
  }
}