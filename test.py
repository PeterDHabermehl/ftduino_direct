#! /usr/bin/python3

import ftduinoDirect as ftd
import time,sys


print("Scanne nach ftduinos...")
print("Gefundene ftduinos:")
print(ftd.ftduino_scan())
print("")



meinftdname="PetersFtd"

print("Versuche, mich mit dem ftduino '"+meinftdname+"' zu verbinden.")

# suche einen ftduino nach Namen
f=ftd.ftduino_find_by_name(meinftdname)

if f == None:
    print("ftduino mit dem Namen "+meinftdname+" nicht gefunden")
    print("Versuche, andere ftduinos zu öffnen")
    myftd=ftd.ftduino()   # Verbinde mit dem erstbesten ftduino
else:
    myftd=ftd.ftduino(f)  # Verbinde mit dem "gewünschten" ftduino


if myftd.getDevice() != None:
    print("Habe den ftduino mit Namen '" + myftd.comm("ftduino_id_get") + "' gefunden!")
    
    maxpwm = myftd.comm("ftduino_pwm_get_max")  
    print("Maximal möglicher pwm-Wert:", maxpwm)
    
    print("Motor M1 links mit voller Leistung")
    myftd.comm("motor_set M1 left "+str(maxpwm))
          
    for n in range(0,3):
        myftd.comm("LED_set 1")
        time.sleep(0.5)
        myftd.comm("LED_set 0")
        time.sleep(0.5)
    
    print("Motor M1 rechts mit voller Leistung")
    myftd.comm("motor_set M1 right "+str(maxpwm))
    
    for n in range(0,3):
        myftd.comm("LED_set 1")
        time.sleep(0.5)
        myftd.comm("LED_set 0")
        time.sleep(0.5)
    
    print("Motor M1 stop")
    myftd.comm("motor_set M1 brake 0")
    print("...und Ende")
    myftd.close()

    
else:
    print("Verbindungsfehler")
    
    
