/////////////////////////// LoRa Gateway Key ///////////////////////////

#define GATEWAY_KEY "xy"                           // Separation Key "Keep it Short 2 letters is enough"
#define NODE_NAME "PirBoxL"                        // Sensor Name which will be visible in Home Assistant
#define Encryption true                            // Global Payload obfuscation (Encryption)
#define encryption_key_length 4                    // must match number of bytes in the XOR key array
#define encryption_key { 0x4B, 0xA3, 0x3F, 0x9C }  // Multi-byte XOR key (between 2–16 values).
                                                   // Use random-looking HEX values (from 0x00 to 0xFF).
                                                   // Must match exactly on both sender and receiver.
                                                   // Example: { 0x1F, 0x7E, 0xC2, 0x5A }  ➜ 4-byte key.

/////////////////////////// LORA RADIO CONFIG ///////////////////////////

// For sending 6 bytes ("6 characters") of data using different Spreading Factors (SF), the estimated time on air (ToA)
//
// SF7: Approximately 0.027 seconds (27.494 milliseconds)
// SF8: Approximately 0.052 seconds (52.224 milliseconds)
// SF9: Approximately 0.100 seconds (100.147 milliseconds)
// SF10: Approximately 0.193 seconds (193.413 milliseconds)
// SF11: Approximately 0.385 seconds (385.297 milliseconds)
//
// These calculations demonstrate how the time on air increases with higher Spreading Factors
// due to the decreased data rate, which is a trade-off for increased communication range and signal robustness.

#define BAND                      868E6     // 433E6 MHz or 868E6 MHz or 915E6 MHz
#define TX_OUTPUT_POWER           20        // dBm tx output power

//| Bandwidth | Supported Spreading Factors |
//|-----------|-----------------------------|
//| 125 kHz   | SF5 – SF9                   |
//| 250 kHz   | SF5 – SF10                  |
//| 500 kHz   | SF5 – SF11                  |

#define LORA_BANDWIDTH            4         // bandwidth 4: 125Khz, 5: 250KHZ, 6: 500Khz
#define LORA_SPREADING_FACTOR     8         // spreading factor 6-12 [SF5..SF11]
#define LORA_CODINGRATE           1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH      6         // Same for Tx and Rx
#define LORA_PAYLOADLENGTH        0         // 0: Variable length packet (explicit header),  1..255 for Fixed length packet (implicit header)
#define LORA_CRC_ON               true
#define LORA_SYNC_WORD            0x1424    // The 0x1424 private sync word is equivalent to the CapiBridge 0x12 sync word.


// LoRa Sync Word Compatibility Table
//
// This table shows how 16-bit sync words used on "PirBOX" SX126x-based modules (like Ra-01SH) correspond to the 8-bit sync words used on older SX127x-based modules.
//
// | Type    | SX126x Sync Word | SX127x Equivalent | Notes                                                               |
// |---------|------------------|-------------------|---------------------------------------------------------------------|
// | Public  | `0x3444`         | `0x34`            | Default for LoRaWAN. Reserved, do **not** use for private networks. |
// | Private | `0x1424`         | `0x12`            | Recommended for custom/private networks.                            |
// | Private | `0x1437`         | `0x37`            | Custom private network sync word.                                   |
// | Private | `0x14A9`         | `0xA9`            | Custom private network sync word.                                   |
// | Private | `0x14D5`         | `0xD5`            | Custom private network sync word.                                   |
//
// Notes:
// - SX126x uses a 16-bit sync word, always starting with `0x14` for compatibility.
// - SX127x uses an 8-bit sync word, derived from the lower byte (`0xYZ` from `0x14YZ`).
// - Avoid `0x3444` (`0x34`) in private networks — it's reserved for "LoRaWAN public" use.
// - Choose a unique `0x14YZ` value for a private network to avoid collisions and improve isolation.
