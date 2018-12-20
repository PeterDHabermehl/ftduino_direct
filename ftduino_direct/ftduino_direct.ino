/*
 * ftduino-direct 
 * A ftduino sketch to provide direct control of the hw i/o
 * via serial USB communication
 * (c) 2017 Peter Habermehl
 */

#include <EEPROM.h>
#include <Ftduino.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);

#define FTDUINODIRECTVERSION "1.2.1"
#define MAX_CMD 32
#define BURGER 0xdeadbeef

struct ftd_settings {
          uint32_t ftd_eeprom_marker;
          byte ftd_i2c_address;
          char ftd_name[17];
};

ftd_settings ftd_conf = { 0, 42, "ftDuino\0"};

char rxCur = 0;
char rxBuf[MAX_CMD+1]; // an array to store the received data + Nullbyte

// servo min & max pwm, < 4096
int smin = 200;
int smax = 510;

// pwm frequency in Hz
int freq = 60;


bool ok = true;


// die setup-Funktion wird einmal beim Start aufgerufen
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // init servo driver
  pwm1.begin();
  pwm1.setPWMFreq(freq);
  
  Serial.begin(115200);  // seriellen USB-Port öffnen

  // warte auf PC-Kommunikation
  while(!Serial) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(450);
  }

  bool cnfread = ftd_readconf();
  
  /* check & report, if config was read or created
  if (cnfread) {
    Serial.println("Config read!");
  }
  else {
    Serial.println("Config created!");     
  }
  */
  
  ftduino.init();
}

// die loop-Funktion wird immer wieder aufgerufen
void loop() {
  int success;
  
  // lies alle Bytes, die per USB/Serial empfangen wurden
  while(ok && Serial.available()) {
    char c = Serial.read();

    // "return" beendet ein Kommando
    if((c == '\n') || (c == '\r')) {
      if(rxCur > 0) {
        rxBuf[rxCur] = '\0';

        // separiere bei erstem leerzeichen, wenn vorhanden
        char *parm = NULL;        
        if(strchr(rxBuf, ' ')) {
          parm = strchr(rxBuf, ' ')+1;
          *strchr(rxBuf, ' ') = '\0';
        }

        success=-1;
        
        // below commands with case sensitive parameters
        
        String cmd = String(rxBuf);
        cmd.toLowerCase();
             
        if((cmd == "led_set") && parm) {
          int c = atoi(parm);
          if(c == 0) {
            digitalWrite(LED_BUILTIN, LOW);
            success=1;
          }
          else {
            digitalWrite(LED_BUILTIN, HIGH);
            success=1;
          }
        }

        else if(cmd == "ftduino_direct_get_version") {
           Serial.println(FTDUINODIRECTVERSION);
           success=2;
        }

        else if((cmd == "ftduino_id_set") && parm){
          strncpy(ftd_conf.ftd_name, parm, 17);
          ftd_conf.ftd_name[16]=0;
          EEPROM.put(0,ftd_conf);
          Serial.println(ftd_conf.ftd_name);
          success=2;
        }

        else if(cmd == "ftduino_id_get") {
           Serial.println(ftd_conf.ftd_name);
           success=2;
        }

        else if((cmd == "i2c_addr_set") && parm){
          ftd_conf.ftd_i2c_address=lowByte(atoi(parm));
          EEPROM.put(0,ftd_conf);
          Serial.println(ftd_conf.ftd_i2c_address);
          success=2;
        }

        else if(cmd == "i2c_addr_get") {
           Serial.println(ftd_conf.ftd_i2c_address);
           success=2;
        }

        // now only commands with case insensitive parameters
        
        String par=String(parm);
        par.toLowerCase();
        par.toCharArray(parm,par.length()+1);
        
        if((cmd == "motor_set") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
          uint8_t dir = 3;
          
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
              int pwm = min(max(0,atoi(parm3)),512)*Ftduino::MAX/512;

              if(parm[0]=='m'){                 
                uint8_t motor = parm[1]-49;
                
                switch (parm2[0]) {
                case 'l': dir=Ftduino::LEFT; break; 
                case 'r': dir=Ftduino::RIGHT; break;  
                case 'b': dir=Ftduino::BRAKE; break; 
                }            
                if ((motor>-1)&&(motor<4)) {
                  ftduino.motor_set(motor, dir, pwm);
                  success=1;
                }
              }
            }
          }       
        }
        
        else if((cmd == "motor_counter") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
          char *parm4 = NULL;
          uint8_t dir = 3;
          
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
              if(strchr(parm3, ' ')) {
                parm4 = strchr(parm3, ' ')+1;
                *strchr(parm3, ' ') = '\0';
                
                int pwm = min(max(0,atoi(parm3)),512)*Ftduino::MAX/512;
  
                if(parm[0]=='m'){                 
                  uint8_t motor = parm[1]-49;
                  
                  switch (parm2[0]) {
                  case 'l': dir=Ftduino::LEFT; break; 
                  case 'r': dir=Ftduino::RIGHT; break;  
                  case 'b': dir=Ftduino::BRAKE; break; 
                  }            
                  if ((motor>-1)&&(motor<4)) {
                    ftduino.motor_counter(motor, dir, pwm, atoi(parm4));
                    success=1;
                  }
                }
              }
            }
          }       
        }
        
        else if((cmd == "motor_counter_active") && parm) {
          if(parm[0]=='m'){                 
            uint8_t motor = parm[1]-49; 
            if ((motor>-1)&&(motor<4)) {
              if(ftduino.motor_counter_active(motor)) {
                Serial.println(1);
              }
              else {
                Serial.println(0);
              }
              success=2;  
            }
          }       
        }
        
        else if((cmd == "motor_counter_set_brake") && parm) {
          char *parm2 = NULL;
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            *strchr(parm2, ' ') = '\0';
            if(parm[0]=='m'){                 
              uint8_t motor = parm[1]-49;
              switch (parm2[0]) {
              case 't': ftduino.motor_counter_set_brake(motor, true); success=1; break; 
              case 'f': ftduino.motor_counter_set_brake(motor, false); success=1; break;  
              }            
            }
          }       
        }
        
        else if((cmd == "output_set") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
          uint8_t mode = 0;

          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
              int pwm = min(max(0,atoi(parm3)),512)*Ftduino::MAX/512;

              if(parm[0]=='o'){
                uint8_t output = parm[1]-49;
                
                if (parm2[0]=='1') {
                  mode=1;  
                }
                else if(parm2[0]=='2') {
                  mode=2;  
                }
                if ((output>-1)&&(output<8)) {
                  ftduino.output_set(output, mode, pwm);
                  success=1;
                }
              }
            }
          }        
        }

        else if((cmd == "input_set_mode") && parm) {
          char *parm2 = NULL;
  
          if(strchr(parm, ' ')) {
            uint8_t inp=parm[1]-49;
            uint8_t mode=0;
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            
            switch (parm2[0]) {
            case 'r': mode=0; break;
            case 'v': mode=1; break;
            case 's': mode=2; break;
            default: mode=99;
            }

            if((inp>-1) && (inp<8) && (mode != 99) && (parm[0]=='i')) {
              ftduino.input_set_mode(inp, mode);
              success=1;
            }           
          } 
        }
        
        else if((cmd == "input_get") && parm) {          
          uint8_t inp=parm[1]-49;
       
          if((inp>-1) && (inp<8) && (parm[0]=='i')) {
            Serial.println(ftduino.input_get(inp));
            success=2;
          }
        }


        else if((cmd == "counter_set_mode") && parm) {
          char *parm2 = NULL;
 
          if(strchr(parm, ' ')) {
            uint8_t inp=parm[1]-49;
            uint8_t mode=0;
            parm2 = strchr(parm, ' ')+1;
            // *strchr(parm, ' ') = '\0';
            
            switch (parm2[0]) {
            case 'n': mode=0; break;
            case 'r': mode=1; break;
            case 'f': mode=2; break;
            case 'a': mode=3; break;
            default: mode=99; 
            }

            if((inp>-1) && (inp<8) && (mode != 99) && (parm[0]=='c')) {
              ftduino.counter_set_mode(inp, mode);
              success=1;
            }           
          } 
        }
        
        else if((cmd == "counter_get") && parm) {          
          uint8_t inp=parm[1]-49;

          if((inp>-1) && (inp<4) && (parm[0]=='c')) {
            Serial.println(ftduino.counter_get(inp));
            success=2;
          }    
        }

        else if((cmd == "counter_clear") && parm) {          
          uint8_t inp=parm[1]-49;

          if((inp>-1) && (inp<4) && (parm[0]=='c')) {
            ftduino.counter_clear(inp);
            success=1;
          }

        }

        else if((cmd == "counter_get_state") && parm) {          
          uint8_t inp=parm[1]-49;

          if((inp>-1) && (inp<4) && (parm[0]=='c')) {
              Serial.println(ftduino.counter_get_state(inp));
              success=2;
            }
        }
        
        else if((cmd == "ultrasonic_enable") && parm) {          
         
          switch (parm[0]) {
          case 't': 
            ftduino.ultrasonic_enable(true);
            success=1; break;
          case 'f': 
            ftduino.ultrasonic_enable(false);
            success=1; break;          
          }  
        }

        else if(cmd == "ultrasonic_get") {
          Serial.println(ftduino.ultrasonic_get());
          success=2;
        }

        // I2C-Servo Shield Funktionen ab hier
        
        else if((cmd == "pwm_set") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
         
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
           
              pwm1.setPWM(max(min(atoi(parm),15),0),
                max(min(atoi(parm2),4096),0),
                max(min(atoi(parm3),4096),0) );
     
              success=1;
            }
          }
        }

        else if(cmd == "pwm_halt") {
          for (int i=0; i<15; i++) {
             pwm1.setPWM(i, 4096, 0);
          }
          success=1;
        }
        
        // Abschließende Rückmeldung
        
        if(success!=2) {
          Serial.println(success);
        }
                
        Serial.flush(); // warte auf Leeren des Schreibbuffers
        rxCur = 0;
      }
    } else if(rxCur < MAX_CMD) {
      rxBuf[rxCur++] = c;
    }
  }
}

// lese ftduino-settings aus EEPROM
bool ftd_readconf() {
    //EEPROM.write(0,0);
    EEPROM.get(0,ftd_conf);
    // prüfe, ob der Fleischmarker noch im EEPROM steht
    if (ftd_conf.ftd_eeprom_marker == BURGER) {
      return true;
    }
    else {
      ftd_conf.ftd_eeprom_marker=BURGER;
      ftd_conf.ftd_i2c_address=42;
      strcpy(ftd_conf.ftd_name, "ftDuino\0");
      EEPROM.put(0,ftd_conf);
      return false;
    }
}
