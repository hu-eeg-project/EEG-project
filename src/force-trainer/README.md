# Force Trainer

This directory contains the code for the Force Trainer. The code currently uses the ESP32 as the platform, but it can easily be ported to another platform.

## Connect ESP32
Connect the Tx from the Force Trainer headset to pin 16 on the ESP32. The Tx output of the headset can be found on the back of the PCB inside the headset.
Also connect the ground from the headset to a ground pin on the ESP32. The negative terminal of the battery holder in the headset can ben used as ground.

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


