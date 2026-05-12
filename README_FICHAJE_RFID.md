# Sistema de Fichaje RFID con Registro en SD

Sistema completo de registro de tarjetas RFID con timestamp sincronizado por internet. Guarda todos los registros en una tarjeta SD para posterior analisis. Ideal para control de asistencia, acceso y auditoria.

---

## Descripcion

Este proyecto implementa un sistema integrado que:

- Conecta a WiFi para sincronizar la hora con internet (NTP)
- Detecta tarjetas/llaveros RFID con lector MFRC522
- Obtiene el UID de cada tarjeta detectada
- Guarda registros con timestamp en una tarjeta SD
- Maneja multiples dispositivos SPI sin conflictos
- Genera logs detallados para auditoria

Perfecto para sistemas de control de asistencia, registro de acceso, control de inventario o auditoria de eventos.

---

## Componentes Necesarios

| Componente | Cantidad | Notas |
|:---|:---|:---|
| ESP32 | 1 | Soporte WiFi y SPI |
| Modulo RFID MFRC522 | 1 | Lector con antena |
| Modulo Tarjeta SD | 1 | Interfaz SPI |
| Tarjeta SD | 1 | Formateada en FAT32 |
| Tarjeta/Llavero RFID | 1+ | Compatible 13.56 MHz |
| Cables DuPont | 10+ | Para conexiones |
| Breadboard | 1 | Facilita montaje |
| Fuente 5V (opcional) | 1 | Si necesitas mas potencia |

---

## Conexiones

### ESP32 - Pines Principales

| Dispositivo | Funcion | Pin ESP32 |
|:---|:---|:---|
| SD Card | CS | GPIO 5 |
| SD Card | MOSI | GPIO 23 |
| SD Card | MISO | GPIO 19 |
| SD Card | SCK | GPIO 18 |
| RFID | CS | GPIO 15 |
| RFID | MOSI | GPIO 23 |
| RFID | MISO | GPIO 19 |
| RFID | SCK | GPIO 18 |
| RFID | RST | GPIO 22 |
| RFID | 3.3V | 3.3V |
| SD Card | 3.3V | 3.3V |
| Ambos | GND | GND |

**Diagrama de conexion:**

```
ESP32
├── GPIO 5  → SD CS
├── GPIO 15 → RFID CS
├── GPIO 22 → RFID RST
├── GPIO 23 → MOSI (compartido)
├── GPIO 19 → MISO (compartido)
├── GPIO 18 → SCK (compartido)
├── 3.3V    → 3.3V (compartido)
└── GND     → GND (compartido)
```

---

## Requisitos Previos

### Hardware

- ESP32 con WiFi integrado
- Tarjeta SD formateada en FAT32
- Red WiFi disponible
- Acceso a internet (para sincronizar hora)

### Software

- Arduino IDE instalado
- Soporte ESP32 en Arduino IDE
- Librerias instaladas:
  - MFRC522 (para RFID)
  - SD.h (incluida)
  - WiFi.h (incluida)

---

## Instalacion de Librerias

### 1. MFRC522 en Arduino IDE

1. Ve a Sketch → Include Library → Manage Libraries
2. Busca "MFRC522"
3. Instala "MFRC522 by GithubCommunity"

### 2. Configurar Credenciales WiFi

Abre main.cpp y modifica estas lineas:

```cpp
const char* ssid = "Mi_Red_ESP32";      // Tu nombre de WiFi
const char* password = "password123";    // Tu contraseña
```

### 3. Cargar el Codigo

1. Copia el contenido de main.cpp a Arduino IDE
2. Selecciona Board: ESP32 Dev Module
3. Selecciona Puerto: COM (tu puerto)
4. Haz clic en Upload

---

## Como Usar

### 1. Abre el Monitor Serie

- Arduino IDE → Tools → Serial Monitor
- Velocidad: 9600 baud

### 2. Esperaras ver:

```
--- INICIANDO SISTEMA DE REGISTRO ---
Conectando al WiFi: Mi_Red_ESP32
.....
Conectado al WiFi.
Tarjeta SD montada y lista.
Lector RFID listo.

ESPERANDO TARJETAS... (Acerca el llavero)
```

### 3. Acerca una tarjeta RFID

Veras algo como:

```
NUEVO FICHAJE: [12/05/2024 14:23:45] UID: 3A 1B 2C D5
   -> Guardado en la SD correctamente.

NUEVO FICHAJE: [12/05/2024 14:24:12] UID: 5F E2 8A 12
   -> Guardado en la SD correctamente.
```

### 4. El archivo se crea automaticamente

- Ubicacion: Raiz de la tarjeta SD
- Nombre: fichero.log
- Contenido: Un registro por linea con timestamp y UID

---

## Estructura del Archivo de Log

Cada linea del fichero.log tiene este formato:

```
[DD/MM/YYYY HH:MM:SS] UID: XX XX XX XX
```

Ejemplo de archivo completo:

```
[12/05/2024 14:23:45] UID: 3A 1B 2C D5
[12/05/2024 14:24:12] UID: 5F E2 8A 12
[12/05/2024 14:25:33] UID: 7E 9D 4A 21
[12/05/2024 14:26:05] UID: 3A 1B 2C D5
```

---

## Explicacion del Codigo

### Inicializacion (setup)

1. **Configurar pines CS:** Desactiva ambos dispositivos para evitar conflictos SPI
2. **Conectar WiFi:** Se conecta a la red especificada
3. **Sincronizar hora:** Descarga la hora actual de servidores NTP
4. **Iniciar SPI:** Bus compartido para SD y RFID
5. **Iniciar SD:** Monta la tarjeta SD
6. **Iniciar RFID:** Configura el lector RFID

### Bucle principal (loop)

1. **Detectar tarjeta:** Espera nueva tarjeta RFID
2. **Obtener hora:** Consulta el reloj interno sincronizado
3. **Leer UID:** Convierte codigo de la tarjeta a hexadecimal
4. **Crear registro:** Formatea linea con timestamp y UID
5. **Guardar en SD:** Anade la linea al fichero.log
6. **Mostrar resultado:** Imprime por Serial si se guardo correctamente
7. **Pausar lectura:** Espera 1.5 segundos antes de siguiente lectura

---

## Configuracion

### Credenciales WiFi

```cpp
const char* ssid = "Mi_Red_ESP32";
const char* password = "password123";
```

### Zona Horaria

Para cambiar la zona horaria, modifica estas constantes:

```cpp
const long  gmtOffset_sec = 3600;      // +1 hora (UTC+1)
const int   daylightOffset_sec = 3600; // Horario de verano
```

Ejemplos de otras zonas:

```cpp
// UTC+0 (Londres)
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

// UTC+2 (Rumania, Grecia)
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 3600;

// UTC-5 (Nueva York)
const long gmtOffset_sec = -18000;
const int daylightOffset_sec = 3600;
```

### Pines Personalizados

Si necesitas usar otros pines, modifica:

```cpp
#define SD_CS_PIN 5      // Pin CS para SD
#define RFID_CS_PIN 15   // Pin CS para RFID
#define RFID_RST_PIN 22  // Pin Reset RFID
```

---

## Solucion de Problemas

| Problema | Solucion |
|:---|:---|
| No conecta al WiFi | Verifica SSID y contraseña, comprueba que el ESP32 tenga antena WiFi |
| La hora es incorrecta | Verifica zona horaria, comprueba conectividad a internet |
| No detecta tarjetas RFID | Revisa conexiones SPI, verifica pin RST, acerca tarjeta mas |
| No escribe en SD | Formatea SD en FAT32, verifica pin CS, comprueba espacio libre |
| Conflictos SPI | Los pines CS evitan conflictos, no compartas CS entre dispositivos |
| No sincroniza hora | Necesita conexion a internet, espera 30 segundos despues de WiFi |
| Archivo no aparece | Verifica que SD este correctamente montada, intenta reiniciar |
| Caracteres raros en log | Asegurate que la SD este formateada en FAT32 |

---

## Mejoras Posibles

### 1. Validar UID contra lista blanca

```cpp
String uidsPermitidos[] = {"3A1B2CD5", "5FE28A12"};
bool esPermitido = false;
for(int i = 0; i < 2; i++) {
  if(uid == uidsPermitidos[i]) {
    esPermitido = true;
    break;
  }
}
```

### 2. Activar LED o buzzer con registro

```cpp
digitalWrite(LED_PIN, HIGH);
delay(200);
digitalWrite(LED_PIN, LOW);
```

### 3. Enviar datos a servidor web

```cpp
WiFiClient client;
if(client.connect("servidor.com", 80)) {
  client.println("GET /api/registro?uid=" + uid);
}
```

### 4. Mostrar informacion en pantalla LCD

```cpp
lcd.setCursor(0, 0);
lcd.print("UID: " + uid);
lcd.setCursor(0, 1);
lcd.print(marcaDeTiempo);
```

---

## Consideraciones Importantes

1. **Sincronizacion de hora:** El ESP32 debe estar conectado a internet para obtener la hora correcta. Los primeros registros pueden tener hora incorrecta si se hacen antes de sincronizar.

2. **Conflictos SPI:** SD y RFID comparten MOSI, MISO y SCK. Los pines CS los diferencian. Nunca conectes dos dispositivos al mismo CS.

3. **Tarjeta SD:** Debe estar formateada en FAT32. Las tarjetas de mas de 32GB pueden necesitar herramientas especiales.

4. **Consumo de potencia:** El WiFi consume bastante energia. Si usas bateria, considera modo sleep.

5. **Archivo de log:** Crece indefinidamente. Considera limpiar periodicamente o limitar su tamano.

6. **Zona horaria:** Cambiar zona horaria requiere reconexion WiFi o reinicio del ESP32.

7. **Duracion del registro:** El delay de 1.5 segundos evita registros duplicados. Ajusta segun necesidad.

---

## Recursos Utiles

- Documentacion ESP32: https://docs.espressif.com/
- Documentacion Arduino: https://www.arduino.cc/en/Reference/
- MFRC522 GitHub: https://github.com/miguelbalboa/rfid
- Servidor NTP: pool.ntp.org

---

## Licencia

Este proyecto es de codigo abierto y libre de usar para fines educativos y personales.

---

## Autor

Creado como practica integradora de sistemas embebidos, WiFi, RFID y almacenamiento en SD con ESP32.
