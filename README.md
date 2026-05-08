# MideaUART

Library for controlling Midea home appliances over UART. Supports both **Arduino** and **ESP-IDF** frameworks, and ships with an **ESPHome external component**.

Control requires a custom dongle. You can build one yourself following numerous guides online, or buy a ready-made one from the [Tindie Shop](https://www.tindie.com/products/24607/).

A far from complete list of supported brands:
1. [Midea](https://www.midea.com/)
2. [Electrolux](https://www.electrolux.ru/)
3. [Qlima](https://www.qlima.com/)
4. [Artel](https://www.artelgroup.com/)
5. [Carrier](https://www.carrier.com/)
6. [Comfee](http://www.comfee-russia.ru/)
7. [Inventor](https://www.inventorairconditioner.com/)
8. [Dimstal/Simando](https://www.simando24.de/)

---

## ESPHome

This repo includes an ESPHome external component. Add it to your configuration and it works out of the box on both Arduino and ESP-IDF frameworks.

### YAML

```yaml
external_components:
  - source: github://remcom/MideaUART@main
    components: [midea]

uart:
  id: uart_bus
  tx_pin: GPIO17
  rx_pin: GPIO16
  baud_rate: 9600

climate:
  - platform: midea
    name: "Air Conditioner"
    uart_id: uart_bus
    # Optional settings
    period: 1s
    timeout: 2s
    num_attempts: 3
    beeper: false       # beep on control command
    autoconf: true      # auto-detect AC capabilities via 0xB5 query
    supported_modes:
      - HEAT_COOL
      - COOL
      - HEAT
      - DRY
      - FAN_ONLY
    supported_swing_modes:
      - VERTICAL
      - HORIZONTAL
      - BOTH
    supported_presets:
      - ECO
      - BOOST
      - SLEEP
    custom_presets:
      - FREEZE_PROTECTION
    custom_fan_modes:
      - SILENT
      - TURBO
    # Optional sensors
    outdoor_temperature:
      name: "Outdoor Temperature"
    power_usage:
      name: "Power Usage"
    humidity_setpoint:
      name: "Humidity Setpoint"
```

### Actions

The component exposes these automation actions:

| Action | Description |
|---|---|
| `midea_ac.power_on` | Turn on |
| `midea_ac.power_off` | Turn off |
| `midea_ac.power_toggle` | Toggle power |
| `midea_ac.beeper_on` | Enable beeper feedback |
| `midea_ac.beeper_off` | Disable beeper feedback |
| `midea_ac.display_toggle` | Toggle display light |
| `midea_ac.swing_step` | Step swing position (IR) |
| `midea_ac.follow_me` | Send Follow Me temperature (IR) |

---

## Arduino / PlatformIO

Add the library to `platformio.ini`:

```ini
lib_deps =
  https://github.com/remcom/MideaUART
```

### Usage

1. Create an `AirConditioner` instance.
2. Set the serial stream to `9600 8N1`.
3. Call `setup()` and `loop()` from the global functions.
4. Control the device via `control()`.
5. Optionally register a state-change callback.

```cpp
#include <Arduino.h>
#include <Appliance/AirConditioner/AirConditioner.h>

using namespace dudanov::midea::ac;

AirConditioner ac;

void onStateChange() {
  ac.getTargetTemp();
  ac.getIndoorTemp();
  ac.getMode();
  ac.getPreset();
  ac.getSwingMode();
  ac.getFanMode();
}

void setup() {
  Serial.begin(9600);
  ac.setStream(&Serial);
  ac.addOnStateCallback(onStateChange);
  ac.setup();
}

void loop() {
  ac.loop();
}
```

### Controlling

```cpp
// Change mode
Control control;
control.mode = Mode::MODE_COOL;
ac.control(control);

// Change mode and temperature together
Control control;
control.mode = Mode::MODE_AUTO;
control.targetTemp = 22.0f;
ac.control(control);

// Power
ac.setPowerState(true);
ac.setPowerState(false);
ac.togglePowerState();
```

---

## ESP-IDF

The library compiles under ESP-IDF without Arduino. Implement the `Stream` abstract class to wrap your UART driver, then use the same API as above.

```cpp
#include <Appliance/AirConditioner/AirConditioner.h>
#include "driver/uart.h"

class MyUARTStream : public Stream {
 public:
  int available() override {
    size_t len;
    uart_get_buffered_data_len(UART_NUM_1, &len);
    return (int)len;
  }
  int read() override {
    uint8_t byte;
    return uart_read_bytes(UART_NUM_1, &byte, 1, 0) == 1 ? byte : -1;
  }
  size_t write(const uint8_t *data, size_t size) override {
    return uart_write_bytes(UART_NUM_1, data, size);
  }
};

extern "C" void app_main() {
  // ... uart driver init at 9600 8N1 ...

  static MyUARTStream stream;
  static dudanov::midea::ac::AirConditioner ac;
  ac.setStream(&stream);
  ac.setup();

  while (true) {
    ac.loop();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
```

---

## Thanks

to the following people for their contributions to reverse engineering the UART protocol:

* [Mac Zhou](https://github.com/mac-zhou/midea-msmart)
* [Rene Klootwijk](https://github.com/reneklootwijk/midea-uart)
* [NeoAcheron](https://github.com/NeoAcheron/midea-ac-py)

If this project was useful to you, you can [buy me](https://paypal.me/dudan0v) a Cup of coffee :)
