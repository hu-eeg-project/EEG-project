# OpenEEG
This directory contains the code for converting analog signal from the OpenEEG amplifier to digital data and sending these to a PC. Like the Force Trainer this code uses an ESP32 and the ESP-IDF framework.

## Connect ESP32
See [this schamatic](https://github.com/stefan9090/EEG-project/blob/develop/docs/images/openeeg-schematic.png) for the necessary connections.

## Compiling
The code uses the ESP-IDF as the framework, for extra information about this framework see [this](https://docs.espressif.com/projects/esp-idf/en/latest/) link.

To compile, flash and open a serial monitor, run the following command: 
```
make flash monitor
```

It might be necessary to change settings inside the ESP-IDF (e.g. COM port, baudrate, etc.). 
This can be done through a menu, which can be accesed by running the following command:
```
make menuconfig
```

# ATTENTION
Only use channel 2 of the OpenEEG amp. The amplifier was shipped with a wrong resistor (gain resistor of opamp) in channel 1, which causes this channel to malfunction.
