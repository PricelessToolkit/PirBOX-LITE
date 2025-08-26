<img src="img/main.jpg"/>

🤗 Please consider subscribing to my [YouTube channel](https://www.youtube.com/@PricelessToolkit/videos)
Your subscription goes a long way in backing my work. If you feel more generous, you can buy me a coffee




[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/U6U2QLAF8)

## PirBOX-LITE and [PirBOX-MAX](https://github.com/PricelessToolkit/PirBOX-MAX)

**PirBOX-LITE** is an Open-Source, compact, low-power LoRa-based PIR motion sensor designed for long-range applications such as mailboxes, basements, underground "garages/parkings", and remote sheds. It is the improved, more affordable successor to the **[MailBox-Guard](https://github.com/PricelessToolkit/MailBoxGuard)** project.

<img src="img/ha.png"/>

#### 🛒 Can be purchased in my shop http://www.PricelessToolkit.com

### ✨ Key Features:
- Seamless integration with **Home Assistant MQTT**  
- **LoRa-based communication** for long-range, low-power use cases  
- Sends **( Detected** / **Cleared )** statuses to a LoRa gateway "**[CapiBridge](https://github.com/PricelessToolkit/CapiBridge) or [LilyGo LoRa Board](https://github.com/PricelessToolkit/TTGO_Gateway)**"  
- **Ultra-low power consumption**: ~11 µAh when No motion detected
- Powered by a **250 mAh battery** for long-lasting operation
- Perfect for remote monitoring where Wi-Fi or Zigbee range isn't enough

> [!NOTE]
> **Not intended** for close proximity use or in high-traffic areas, Zigbee devices are better suited for those applications.

### 📋 Power Consumption Table
Measured by "Power Profiler KIT 2"

| Condition                  | Current Draw      | Battery Life (approx.)         |
|---------------------------|-------------------|--------------------------------|
| No motion detected        | 11 µA             | ~2.6 years (250 mAh / 11 µA)   |
| Constant motion detected  | 25 µA             | ~1.1 years (250 mAh / 25 µA)   |
| Single status transmission (314 ms)           | 4.02 µAh          | ~62,188 sends (250,000 µAh / 4.02 µAh) |

> [!NOTE]
> Battery Life is only an approximate value. Actual battery life can vary depending on temperature, battery quality, configuration, and other factors.


### Required:
- [CapiBridge LoRa Gateway](https://github.com/PricelessToolkit/CapiBridge) has 2-Way communication and Encryption possibility, or the one-way more affordable version [TTGO_Gateway](https://github.com/PricelessToolkit/TTGO_Gateway) *no Encryption and future support is limited.
- Programmer [UNIProg](https://www.pricelesstoolkit.com/en/projects/33-uniprog-uartupdi-programmer-33v-0741049314412.html) or any other 3.3v UPDI Programmer
- LiPo Battery [1S 250mAh](https://www.pricelesstoolkit.com/en/products/47-battery-li-po-37v-250mah-ph-20mm-2-pin.html) or less (Connector PH2.0) Important! MAX SIZE "35x20x4mm"
- LiPo Charger [Micro-Charger](https://www.pricelesstoolkit.com/en/li-ion-li-poly/48-micro-charger-for-li-po-and-li-ion.html) or any other 4.2V lipo charger with (PH2.0)
- Pogo PIN Clamp 1x6 PIN "Only for convenience" https://s.click.aliexpress.com/e/_ooPke35

### Choosing a Gateway Hardware

1. [CapiBridge LoRa/ESP-NOW Gateway](https://github.com/PricelessToolkit/CapiBridge): I developed this project, featuring multi-protocol support, including LoRa and ESP-NOW. It supports 2-way communication with Encryption, making it possible to send commands to `LoRa` nodes as well as receive the sensor data. It will be compatible with my future LoRa and ESP-NOW sensor projects.
2. [LilyGo LoRa Board](https://github.com/PricelessToolkit/MailBoxGuard/tree/main?tab=readme-ov-file#select-ttgo_lora-board-version): This option is more affordable but only has LoRa and does not support Encryption.


### 📣 Updates, Bugfixes, and Breaking Changes
- 22.05.2025 - Breaking Change (XOR obfuscation "Encryption" for LoRa).
- - CapiBridge firmware needs to be updated.

### Configuration / Reflashing:
> [!NOTE]
> By default, it comes already flashed and tested with the default firmware, `GATEWAY_KEY "xy"` and `encryption_key { 0x4B, 0xA3, 0x3F, 0x9C }`. You need to change the keys!

1. Install MegaTinyCore in Arduino IDE "http://drazzy.com/package_drazzy.com_index.json"
2. Download the PirBOX-Lite project file; don't just copy and paste the code.
3. Open the PirBox-Lite.ino file in Arduino IDE. "It will include all necessary files and Radio Library."
4. In Config.h, change the sensor name, gateway key, and radio settings based on your gateway config

> [!IMPORTANT]  
> The PirBOX LoRa module uses the sync word `0x1424`, which is equivalent to the CapiBridge's `0x12` sync word.

```cpp
/////////////////////////// LoRa Gateway Key ///////////////////////////

#define GATEWAY_KEY "xy"                           // Separation Key "Keep it Short 2 letters is enough"
#define NODE_NAME "PirBoxL"                        // Sensor Name which will be visible in Home Assistant
#define Encryption true                            // Global Payload obfuscation (Encryption)
#define encryption_key_length 4                    // must match number of bytes in the XOR key array
#define encryption_key { 0x4B, 0xA3, 0x3F, 0x9C }  // Multi-byte XOR key (between 2–16 values).
                                                   // Use random-looking HEX values (from 0x00 to 0xFF).
                                                   // Must match exactly on both sender and receiver.
                                                   // Example: { 0x1F, 0x7E, 0xC2, 0x5A }  ➜ 4-byte key.

////////////////////////////// LORA CONFIG ////////////////////////////////////

#define BAND                      868E6     // 433E6 MHz or 868E6 MHz or 915E6 MHz
#define TX_OUTPUT_POWER           22       // dBm tx output power

//| Bandwidth | Supported Spreading Factors |
//|-----------|-----------------------------|
//| 125 kHz   | SF5 – SF9                   |
//| 250 kHz   | SF5 – SF10                  |
//| 500 kHz   | SF5 – SF11                  |

#define LORA_BANDWIDTH            5         // bandwidth 4: 125Khz, 5: 250KHZ, 6: 500Khz
#define LORA_SPREADING_FACTOR     10         // spreading factor 6-12 [SF5..SF11]
#define LORA_CODINGRATE           1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH      12         // Same for Tx and Rx
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


```




5. Select board configuration as shown below.

<img src="img/arduino_board_config.jpg"/>

6. Disconnect the Battery if connected. Then connect the UPDI programmer to the back of the PirBOX using the pogo pin clamp, or any method you prefer.


<table>
  <tr>
    <td><img src="img/updi.png" width="200" /></td>
    <td>

<!-- Markdown-style table inside HTML cell -->
  
| **PirBOX** | **Programmer** |
|------------|----------------|
| 3.3V       | 3.3V           |
| GND        | GND            |
| UPDI       | UPDI           |

</table>


7. In Arduino IDE, select the COM Port and programmer "SerialUPDI-230400 baud "Required UPDI Programmer"
8. Click "Upload Using Programmer" or "Ctrl + Shift + U", Done!

If your gateway and sensor are configured correctly, you should see under MQTT Devices "PIRBoxL" or the custom name you assigned in the config.h file. Once it's visible under MQTT Devices, the next step is to create an automation in Home Assistant to send a notification to your mobile phone.

Below is an example automation that sends a message along with an image. This example is configured for a mailbox, but you can easily adapt it to fit your specific needs.

"Automation" Notification [Manual](https://companion.home-assistant.io/docs/notifications/actionable-notifications/)

```yaml

alias: Pirbox
description: "MailBoxSensor"
triggers:
  - type: motion
    device_id: 5e37501c1847ff9e185f293757ff91b3
    entity_id: 0cc089df300ab037a78abc906b9736af
    domain: binary_sensor
    trigger: device
conditions: []
actions:
  - data:
      message: Mailbox is Full!
      title: New Mail!
      data:
        url: /lovelace/home
        importance: high
        channel: MailBox
        tag: mailbox
        image: /media/local/notify/mailbox.jpg
    action: notify.mobile_app_doogee_v20pro

```
PS: mailbox.jpg is included in the "img" folder.

____________

# 🛠️ Prefer to build it on your own?
This project is open-source and includes Source code, 3D Print files, and Gerber files, allowing you to order blank PCBs and assemble the PirBOX-Lite yourself. To help with manual assembly, I've included an Interactive HTML BOM in the PCB folder. This tool shows the placement and polarity of each component, helping to eliminate errors during soldering.

> [!NOTE]
>  Please note that CPL file or POS "Pick and Place" files, and KiCad source files are not included. These are intentionally omitted, as this project is intended for manual assembly. If you prefer a ready-to-use solution, you can purchase one directly from my shop: https://www.pricelesstoolkit.com.

## Schematic
<details>
  <summary>View schematic. Click here</summary>
<img src="PCB/PirBOX-Lite_Schematic.jpg"/>
</details>
