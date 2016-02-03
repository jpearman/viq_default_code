# viq_default_code
Examples and simulations of the VexIQ driver control program

Three example programs that demonstrate how to control the VexIQ clawbot with sensors using ROBOTC.

The simulations become increasingly more accurate and complex.  One exception is the "exit" button which is owned by the VEX firmware and cannot be simulated.

**iqDefaultCodeBasic.c**  
Simple driver control using standard motor ports for the ClawBot.
No sensors are implemented, all motors must be present or an exception will be thrown.

**iqDefaultCodeSimple.c**  
Simulation of the VexIQ driver control program for motors only.
No sensors are implemented.
Motors are automatically detected.
Ports 1 and 7 are assigned to the left drive if they are installed.
Ports 6 and 12 are assigned to the right drive if they are installed.
Ports 4, 5, 10 & 11 are used as auxiliary motors if installed.
The configure menu allows motors to be reversed in the same way as the default driver control program, however, configuration is not stored after the program stops

**iqDefaultCode.c**  
Full simulation of the VexIQ driver control program.
Motors and sensors are automatically detected.
Ports 1 and 7 are assigned to the left drive if they are installed.
Ports 6 and 12 are assigned to the right drive if they are installed.
Ports 4, 5, 10 & 11 are used as auxiliary motors if installed.
configure menu allows motors to be reversed in the same way as the default driver control program.
The configuration is stored in SRAM to retain the settings after the program has stopped.
The configuration can be optionally stored in EEPROM to retain settings when the VexIQ is powered off.
Gyro, Color Sensor, Touch LED and distance sensor have the same behavior as the VexIQ default driver control program.
The LCD can display joystick, sensor and motor values in the same way as the default driver control code.

##screen shots
![home screen](https://raw.githubusercontent.com/jpearman/viq_default_code/master/images/home_screen.jpg "Home Screen")

![config screen](https://raw.githubusercontent.com/jpearman/viq_default_code/master/images/config_screen.jpg "Config Screen")

![run screen 1](https://raw.githubusercontent.com/jpearman/viq_default_code/master/images/run_screen_1.jpg "Run Screen")

![run screen 2](https://raw.githubusercontent.com/jpearman/viq_default_code/master/images/run_screen_2.jpg "Run Screen joystick")

![run screen 3](https://raw.githubusercontent.com/jpearman/viq_default_code/master/images/run_screen_3.jpg "Run Screen motors & sensors")

