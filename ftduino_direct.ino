/*
 * ftduino-direct 
 * A ftduino sketch to provide direct control of the hw i/o
 * via serial USB communication
 * (c) 2017 Peter Habermehl
 */

#include <EEPROM.h>
#include <Ftduino.h>

#define FTDUINODIRECTVERSION "0.9.4"
#define MAX_CMD 32
#define BURGER 0xdeadbeef

struct ftd_settings {
          uint32_t ftd_eeprom_marker;
          byte ftd_i2c_address;
          char ftd_name[17];
};

ftd_settings ftd_conf = { 0, 42, "ftduino\0"};

char rxCur = 0;
char rxBuf[MAX_CMD+1]; // an array to store the received data + Nullbyte


bool ok = true;


// die setup-Funktion wird einmal beim Start aufgerufen
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
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

        else if((cmd == "motor_set") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
          uint8_t dir = 3;

          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
              int pwm = min(max(0,atoi(parm3)),512)/512*Ftduino::MAX;

              if(parm[0]=='M' or parm[0]=='m'){
                char n[1];
                n[0]=parm[1]; n[1]="/0";
                 
                uint8_t motor = min(max(1,atoi(n)),4)-1;
                
                if (parm2[0]=='l' or parm2[0]=='L') {
                  dir=Ftduino::LEFT;  
                }
                else if(parm2[0]=='r' or parm2[0]=='R') {
                  dir=Ftduino::RIGHT;  
                }
                else if(parm2[0]=='b' or parm2[0]=='B') {
                  dir=Ftduino::BRAKE;  
                }            
                
                ftduino.motor_set(motor, dir, pwm);
                success=1;
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
              int pwm = min(max(0,atoi(parm3)),512)/512*Ftduino::MAX;

              if(parm[0]=='O' or parm[0]=='o'){
                char n[1];
                n[0]=parm[1]; n[1]="/0";
                
                uint8_t motor = min(max(1,atoi(n)),8)-1;
                
                if (parm2[0]=='1') {
                  mode=1;  
                }
                else if(parm2[0]=='2') {
                  mode=2;  
                }
                
                ftduino.output_set(motor, mode, pwm);
                success=1;
              }
            }
          }        
        }

        else if((cmd == "input_set_mode") && parm) {
          char *parm2 = NULL;
          
          if(strchr(parm, ' ')) {
            uint8_t inp=0;
            uint8_t mode=0;
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            
            switch (parm[1]) {
            case '1': inp=0; break;
            case '2': inp=1; break;
            case '3': inp=2; break;
            case '4': inp=3; break;
            case '5': inp=4; break;
            case '6': inp=5; break;
            case '7': inp=6; break;
            case '8': inp=7; break;
            default: inp=99;
            }
            
            switch (parm2[0]) {
            case 'r': mode=0; break;
            case 'v': mode=1; break;
            case 's': mode=2; break;
            case 'R': mode=0; break;
            case 'V': mode=1; break;
            case 'S': mode=2; break;
            default: mode=99;
            }

            if((inp!=99) && (mode != 99) && ((parm[0]=='i') || (parm[0]=='I') )) {
              ftduino.input_set_mode(inp, mode);
              success=1;
            }           
          } 
        }
        
        else if((cmd == "input_get") && parm) {          
          uint8_t inp=0;          

          switch (parm[1]) {
          case '1': inp=0; break;
          case '2': inp=1; break;
          case '3': inp=2; break;
          case '4': inp=3; break;
          case '5': inp=4; break;
          case '6': inp=5; break;
          case '7': inp=6; break;
          case '8': inp=7; break;
          default: inp=99;
          }

          if((inp!=99) && ((parm[0]=='i') || (parm[0]=='I') )) {
            Serial.println(ftduino.input_get(inp));
            success=2;
          }
        }


        else if((cmd == "counter_set_mode") && parm) {
          char *parm2 = NULL;
          
          if(strchr(parm, ' ')) {
            uint8_t inp=0;
            uint8_t mode=0;
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            
            switch (parm[1]) {
            case '1': inp=0; break;
            case '2': inp=1; break;
            case '3': inp=2; break;
            case '4': inp=3; break;
            default: inp=99; success=0;
            }
            
            switch (parm2[0]) {
            case 'n': mode=0; break;
            case 'r': mode=1; break;
            case 'f': mode=2; break;
            case 'a': mode=3; break;
            default: mode=99; success=0;
            }

            if((inp!=99) && (mode != 99) && ((parm[0]=='c') || (parm[0]=='C'))) {
              ftduino.counter_set_mode(inp, mode);
              success=1;
            }           
          } 
        }
        
        else if((cmd == "counter_get") && parm) {          
          uint8_t inp=0;
   
          switch (parm[1]) {
          case '1': inp=0; break;
          case '2': inp=1; break;
          case '3': inp=2; break;
          case '4': inp=3; break;
          default: inp=99;
          }

          if((inp!=99) && ((parm[0]=='c') || (parm[0]=='C'))) {
            Serial.println(ftduino.counter_get(inp));
            success=2;
          }    
        }

        else if((cmd == "counter_clear") && parm) {          
          uint8_t inp=0;
          
          switch (parm[1]) {
          case '1': inp=0; break;
          case '2': inp=1; break;
          case '3': inp=2; break;
          case '4': inp=3; break;
          default: inp=99;
          }

          if((inp!=99) && ((parm[0]=='c') || (parm[0]=='C'))) {
            ftduino.counter_clear(inp);
            success=1;
          }

        }

        else if((cmd == "counter_get_state") && parm) {          
          uint8_t inp=0;
          
            switch (parm[1]) {
            case '1': inp=0; break;
            case '2': inp=1; break;
            case '3': inp=2; break;
            case '4': inp=3; break;
            default: inp=99;
            }
  
            if((inp!=99) && ((parm[0]=='c') || (parm[0]=='C'))) {
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
      strcpy(ftd_conf.ftd_name, "ftduino\0");
      EEPROM.put(0,ftd_conf);
      return false;
    }
}
