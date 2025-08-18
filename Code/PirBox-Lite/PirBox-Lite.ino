/***************************************************************
 * PirBox by PricelessToolkit — Interrupt-driven, binary-safe send
 * Requirements:
 *  - config.h (your version posted)
 *  - Ra01S / SX126x driver ("Ra01S.h")
 ***************************************************************/

#include "config.h"
#include <SPI.h>
#include "Ra01S.h"

// -------------------- PIN ASSIGNMENTS -------------------- //
#define PIR_PIN          PIN_PC0   // Motion sensor input
#define OFF_PIN          PIN_PB0   // Pull HIGH for Power OFF (latches power off)
#define BATT_PIN         PIN_PB4   // Battery sense pin

// RA-01SH / SX126x pins
#define RADIO_RESET_PIN  PIN_PA6
#define RADIO_NSS_PIN    PIN_PA4
#define RADIO_BUSY_PIN   PIN_PC2
#define RADIO_DIO1_PIN   PIN_PC3

SX126x lora(RADIO_NSS_PIN, RADIO_RESET_PIN, RADIO_BUSY_PIN);

// -------------------- STATE -------------------- //
volatile bool sensorTriggered = false;
volatile unsigned long lastInterruptTime = 0;   // for debounce
bool firstRun = true;

// -------------------- UTILS -------------------- //
/*
// XOR cipher using multi-byte key defined in config.h
static inline String xorCipher(const String& input) {
  const byte key[] = encryption_key;            // from config.h
  const int keyLength = encryption_key_length;  // from config.h

  String out;
  out.reserve(input.length());
  for (size_t i = 0; i < (size_t)input.length(); ++i) {
    byte keyByte = key[i % keyLength];
    out += char(uint8_t(input[i]) ^ keyByte);
  }
  return out;
}

*/


// -------------------- XOR+BAS64----------------------//

// config.h has:
// #define encryption_key_length 4
// #define encryption_key { 0x4B, 0xA3, 0x3F, 0x9C }

String xorCipher(String in) {
  // 1) Use an ARRAY, not a pointer, for the macro initializer
  static const uint8_t key[] = encryption_key;
  const int K = encryption_key_length;

  auto isB64 = [](const String& s)->bool{
    if (s.length() % 4) return false;
    for (size_t i=0;i<s.length();++i){
      char c=s[i];
      if (!((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='+'||c=='/'||c=='=')) return false;
    }
    return true;
  };

  auto b64enc = [](const uint8_t* b, size_t n)->String{
    static const char T[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String o; o.reserve(((n+2)/3)*4);
    for(size_t i=0;i<n;i+=3){
      uint32_t v = ((uint32_t)b[i] << 16)
                 | ((i+1<n ? (uint32_t)b[i+1] : 0) << 8)
                 |  (i+2<n ? (uint32_t)b[i+2] : 0);
      o += T[(v>>18)&63]; o += T[(v>>12)&63];
      o += (i+1<n) ? T[(v>>6)&63] : '=';
      o += (i+2<n) ? T[v&63]      : '=';
    }
    return o;
  };

  auto b64val = [](char c)->int{
    if(c>='A'&&c<='Z') return c-'A';
    if(c>='a'&&c<='z') return 26+c-'a';
    if(c>='0'&&c<='9') return 52+c-'0';
    if(c=='+') return 62;
    if(c=='/') return 63;
    return -1;
  };

  auto b64dec = [&](const String& s)->String{
    String o; o.reserve((s.length()/4)*3);
    for(size_t i=0;i<s.length(); i+=4){
      int a=b64val(s[i]), b=b64val(s[i+1]);
      int c = (s[i+2]=='=') ? -1 : b64val(s[i+2]);
      int d = (s[i+3]=='=') ? -1 : b64val(s[i+3]);

      // 2) Upcast before shifting (AVR int is 16-bit)
      uint32_t v = ((uint32_t)(a & 63) << 18)
                 | ((uint32_t)(b & 63) << 12)
                 | ((uint32_t)((c < 0 ? 0 : (c & 63))) << 6)
                 |  (uint32_t)((d < 0 ? 0 : (d & 63)));

      o += (char)((v >> 16) & 0xFF);
      if (c >= 0) o += (char)((v >> 8) & 0xFF);
      if (d >= 0) o += (char)(v & 0xFF);
    }
    return o;
  };

  if (isB64(in)) {                 // decrypt: Base64 -> XOR -> JSON
    String bytes = b64dec(in);
    String out; out.reserve(bytes.length());
    for (size_t i=0;i<bytes.length();++i)
      out += (char)(((uint8_t)bytes[i]) ^ key[i % K]);
    return out;
  } else {                         // encrypt: JSON -> XOR -> Base64
    String x; x.reserve(in.length());
    for (size_t i=0;i<in.length();++i)
      x += (char)(((uint8_t)in[i]) ^ key[i % K]);
    return b64enc((const uint8_t*)x.c_str(), x.length());
  }
}


//------------------ XOR END -------------------------------//







// Power latch off (your hardware should cut power after this line)
static inline void powerOFF() {
  delay(100);
  digitalWrite(OFF_PIN, HIGH); // Powering off
}

// Battery level (0–100%), using 2.5V ref and divider = x2
static inline int batt() {
  float totalVolts = 0.0f;
  for (int i = 0; i < 5; i++) {
    float volts = analogReadEnh(BATT_PIN, 12) * (2.5f / 4096.0f) * 2.0f;
    totalVolts += volts;
    delay(2);
  }
  float averageVolts = totalVolts / 5.0f;

  // Map ~3.3V..4.1V to 0..100%
  float percentage = ((averageVolts - 3.3f) / (4.1f - 3.3f)) * 100.0f;
  if (percentage < 0)   percentage = 0;
  if (percentage > 100) percentage = 100;
  return (int)(percentage + 0.5f);
}

// -------------------- RADIO -------------------- //
static inline void initLoRaRadio() {
  int16_t ret = lora.begin(BAND, TX_OUTPUT_POWER);
  if (ret != ERR_NONE) {
    // Halt if init fails
    while (1) { }
  }

  // If you want to set a custom SX126x sync word manually, you can:
  // lora.SetSyncWord(LORA_SYNC_WORD);

  lora.LoRaConfig(
    LORA_SPREADING_FACTOR,
    LORA_BANDWIDTH,
    LORA_CODINGRATE,
    LORA_PREAMBLE_LENGTH,
    LORA_PAYLOADLENGTH,   // 0 => explicit header (variable length)
    LORA_CRC_ON,
    false,                // IQ inversion
    LORA_SYNC_WORD        // must match CapiBridge's effective sync (0x1424 -> 0x12)
  );
}

// -------------------- ISR -------------------- //
void sensorISR() {
  unsigned long now = millis();
  // Debounce (~80 ms to tame PIR chatter on wake)
  if (now - lastInterruptTime > 80) {
    sensorTriggered = true;
    lastInterruptTime = now;
  }
}

// -------------------- ARDUINO -------------------- //
void setup() {
  pinMode(OFF_PIN, OUTPUT);
  digitalWrite(OFF_PIN, LOW);      // keep device powered

  pinMode(PIR_PIN, INPUT);
  pinMode(RADIO_DIO1_PIN, INPUT);

  // Optional serial for debug (avoid printing encrypted payloads!)
  // Serial.begin(9600);

  initLoRaRadio();

  // Use 2.5V internal reference for stable battery measurement
  analogReference(INTERNAL2V5);

  // Interrupt on rising edge only (ON events)
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), sensorISR, RISING);
}

void loop() {
  // Trigger on first boot or when PIR goes HIGH
  if (sensorTriggered || firstRun) {
    sensorTriggered = false;
    if (firstRun) firstRun = false;

    // Read PIR state (true = motion)
    const bool pirOn = (digitalRead(PIR_PIN) == HIGH);

    // Build compact JSON
    String payload;
    payload.reserve(64);
    payload  = "{\"k\":\""; payload += GATEWAY_KEY;
    payload += "\",\"id\":\""; payload += NODE_NAME; payload += "\"";
    payload += ",\"m\":\""; payload += (pirOn ? "on" : "off"); payload += "\"";
    payload += ",\"b\":"; payload += batt();
    payload += "}";

    // Obfuscate after JSON is fully built
    #if Encryption
      payload = xorCipher(payload);
    #endif

    // Send as raw bytes (binary-safe)
    uint8_t* buf = (uint8_t*)payload.c_str();
    uint8_t len  = (uint8_t)payload.length();

    if (lora.Send(buf, len, SX126x_TXMODE_SYNC)) {
      // Success — do not print encrypted data
      delay(20);
    } else {
      // Optional: brief retry/backoff if needed
      // delay(30);
      // lora.Send(buf, len, SX126x_TXMODE_SYNC);
    }

    // Cut power (hardware should latch off)
    powerOFF();
  }

  // Idle; device likely powers off after send
}
