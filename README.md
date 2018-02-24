# ftduino_direct
An arduino sketch and Python3 library to access the ftduino hardware

## What is it?
The [ftduino](https://github.com/harbaum/ftduino) is an Arduino based Microcontroller intended to be used in combination with the [fischertechnik](http://www.fischertechnik.de/) robotics range of construction kits, as you already might know since you're here...

The ftduino can be programmed using the Arduino IDE according to the instructions to be found at the ftduino repository.

To access the ftduino hardware via Python, the ftduino_direct project provides a ftduino Arduino sketch (ftduino_direct.ino) and a Python module (ftduino_direct) to communicate to the ftduino via USB.

## Installation
### Arduino sketch
*First*, follow the instructions found at the [ftduino](https://github.com/harbaum/ftduino) homepage to install the Arduino IDE and set it up to be used with the ftduino.

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

