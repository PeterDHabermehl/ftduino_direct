# ftduino_direct
An arduino sketch and Python3 library to access the ftDuino hardware

## What's new?

**sketch v1.3.2 (2019-01-11):** I2CRead and I2CWrite commands, see below.


**sketch v1.2 (2018-12-18):** added support for PCA9685 based I2C servo shields. See "Use" below for details.

## What is it?
The [ftDuino](https://ftduino.de) is an Arduino based Microcontroller intended to be used in combination with the [fischertechnik](http://www.fischertechnik.de/) robotics range of construction kits, as you already might know since you're here...

The ftDuino can be programmed using the Arduino IDE according to the instructions to be found at the ftDuino repository.

To access the ftDuino hardware via Python, the ftduino_direct project provides a ftDuino Arduino sketch (ftduino_direct.ino) and a Python module (ftduino_direct) to communicate to the ftduino via USB.

## Installation
### Arduino sketch

If you use the ftDuino in conjunction with a fischertechnik TXT controller running the [community firmware](https://cfw.ftcommunity.de/) or a [TX-Pi](https://github.com/harbaum/tx-pi), you simply might get the App ["ftDuinIO"](https://github.com/PeterDHabermehl/ftDuinIO) from the app store. It allows to flash a pre-compiled binary of the sketch. Just select it within the app.

**Alternately and without a device running the community firmware:**

*First*, follow the instructions found at the [ftDuino](https://ftduino.de) homepage to install the Arduino IDE and set it up to be used with the ftduino.

Also install the Adafruit PCA9685 PWM Servo Driver Library within the IDE:

Sketch -> Include Library -> Manage Library

Search for "adafruit servo" -> Install "Adafruit PWM Servo Driver Library"


*Second*, [download](https://github.com/PeterDHabermehl/ftduino_direct/raw/master/ftduino_direct/ftduino_direct.ino) the ftduino_direct.ino sketch.

*Third*, open the ftduino_direct.ino sketch into the ftduino IDE and upload it to your ftduino.

Your ftduino is now ready. It will indicate this by flashing the internal red LED with a frequency of 2Hz, until inital USB communication takes place.

You might open the Arduino IDE serial console and issue the command "LED_set 1" to switch on the internal red LED of the ftduino. "LED_set 0" turns the LED off again. Make sure to set the serial console to "CR" to issue a CR code on pressing Return or Enter, otherwise your commands will not be sent.

### Python module
*First* [download](https://github.com/PeterDHabermehl/ftduino_direct/raw/master/ftduino_direct-1.0.8.tar.gz) the Python Package and untar the file.

*Second* open a shell window, cd into the unpack directory and issue the command 'sudo python3 setup.py install'. The package will now be installed to your system.

*Third* get the [test.py](https://github.com/PeterDHabermehl/ftduino_direct/raw/master/python/test.py) Python script and run it. It will  switch on a motor connected to M1 of the ftduino and flash the red LED.

## Use

See test.py on how to use...

**Please be aware that ftduino_direct uses a pwm range of 0..512, as for the ft TXT controller, as opposed to the ftduino internal range of 0..64.** 

You might execute all functions provided by [Till Harbaum's sketch library](https://github.com/harbaum/ftduino/tree/master/ftduino/libraries/Ftduino) using the .comm method of ftduino_direct.py, namely:
    
    void input_set_mode(uint8_t ch, uint8_t mode);
    uint16_t input_get(uint8_t ch);

    void output_set(uint8_t port, uint8_t mode, uint8_t pwm);
    void motor_set(uint8_t port, uint8_t mode, uint8_t pwm);
    void motor_counter(uint8_t port, uint8_t mode, uint8_t pwm, uint16_t counter);
    bool motor_counter_active(uint8_t port);
    void motor_counter_set_brake(uint8_t port, bool on);

    void ultrasonic_enable(bool ena);
    int16_t ultrasonic_get();

    void counter_set_mode(uint8_t ch, uint8_t mode);
    uint16_t counter_get(uint8_t ch);
    void counter_clear(uint8_t ch);
    bool counter_get_state(uint8_t ch);

## Servo/Pwm control
Connect a PCA9685 based I2C servo shield to ftDuino's I2C port.

Only I2C address 0x40 supported, only one shield. pwm frequency is 60Hz fixed. 
The new command to address a PWM port: **pwm_set \<channel\> \<pulse high start\> \<pulse high end\>**


with

    <channel>           Number of the pwm/servo port of the shield, starts at 0 up to num.ports-1, so 0..15 for 16 channels.

    <pulse high start>  Start of the high pulse in x/4096, usually "0"
        
    <pulse high end>    End of the high pulse in x/4096
        

As the servo shield resolves a pwm pulse in 12 bit and the high level of the pwm signal usually starts at 0, a duty cycle of 5% on channel 0 could be set like:
    
    pwm_set 0 0 205 because 4096 * 0,05 = 204,8

Accordingly 12.5%:

    pwm_set 0 0 512 (4096 * 0.125 = 512)

To turn on a channel with 100%, issue
    
    pwm_set <channel> 4096 0

to turn off

    pwm_set <channel> 0 4096
    
**The command "pwm_halt" turns off all 16 channels at once.**

## I2C communication
To access the ftDuino I2C port, use

    I2CWrite <device address> <data byte 0> .. <data byte 31>
    
    I2CRead <device address> <number of bytes to read>
    
where all parameters are expected to be space-separated 8bit unsigned integer numbers.

I2CRead then returns the data also as a sequence of space-separated numbers.

