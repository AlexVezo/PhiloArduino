/*
 * @title: Hedonistic Calculator
 * @author of script: AlexVezo
 * @used library: LiquidCrystal (http://www.arduino.cc/en/Tutorial/LiquidCrystal)
 * @version: v1.0.2 (beta)
 * @date: 06.07.2016
 * -----------------------------------------------------------------------------------------------  
 * The circuit:
 *  - LCD RS pin to digital pin 12
 *  - LCD Enable pin to digital pin 11
 *  - LCD D4 pin to digital pin 5
 *  - LCD D5 pin to digital pin 4
 *  - LCD D6 pin to digital pin 3
 *  - LCD D7 pin to digital pin 2
 *  - 10K resistor:
 *  - ends to +5V and ground
 *  - wiper to LCD VO pin (pin 3)
 * -----------------------------------------------------------------------------------------------   
 *  - D6 pin to button 1 - "go / power on"    - 
 *  - D7 pin to button 2 - "sadness"          - oben links
 *  - D8 pin to button 3 - "happyness"        - oben rechts
 *  - D9 pin to button 4 - "next"             - unten rechts
 */

// include the library code:
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "Adafruit_MCP23008.h"

// initialize the library with the numbers of the interface pins
Adafruit_LiquidCrystal lcd(12,11,5,4,3,2);

// 6 - oben links
// 7 - oben rechts 
// 8 = 9 - unten

// Used buttons:
int pin2            = 6;        // top    left
int pin1            = 8;        // buttom left
int pin3            = 7;        // top    right
int pin4            = 9;        // buttom right

// used variables:
const int options   = 2;        // How many alternative things are given to choose from?
const int questions = 5;        // How many aspects/kategories are given?

int skala           = 7;        // Skala for ranking goes from 0 to [skala]
int sadness         = 0;        // [-] - negative value
int happyness       = 0;        // [+] - positive value

int wait_time       = 0;        // When not used: How long not used? -> Turn off display to save power
int limit_time      = 30000;    // When not used: How long should the Arduino till display off mode? [1000 = 1 sec]

String quality;                 // variable showing whether good or bad conditions to decide are given
String antwort[2];              // Sum of all anwsers of inputs
int result[options];            // Temporary saved results of sum 
int summe[options][questions];  // Adding all values of given answers
String ae = String(char(225));  // 

String question[questions] = {"   Intensit"+ae+"t  ", 
                              "  N"+ae+"he / Ferne ", 
                              " Folgetr"+ae+"chtig?", 
                                " (Un)Gewissheit ", 
                                "    Reinheit    "
                             };

// ============================================================================= //
void reset_values(){
  memset(antwort, 0, sizeof(antwort));
  memset(result, 0, sizeof(result));
  memset(summe, 0, sizeof(summe));
}

void lcd_refresh() {
   lcd.setCursor(0, 1);
   lcd.print("Leid:");
   lcd.print(sadness);
   lcd.print(" Freude:");
   lcd.print(happyness);
   lcd.print(" ");
}

void lcd_show( String value_0, String value_1 ) {
   lcd.setCursor(0, 0);
   lcd.print(value_0);
   lcd.setCursor(0, 1);
   lcd.print(value_1);
   delay(2000);
}

void lcd_clear(){
   lcd_show("                ","                ");
}
void setup() {
  
  // declare buttons as inputs
  pinMode(pin1, INPUT); // on 
  pinMode(pin2, INPUT); // -
  pinMode(pin3, INPUT); // +
  pinMode(pin4, INPUT); // >>

  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);

  // Print a nice welcoming message
  lcd_show("Hallo, kann ich", "weiterhelfen?");
}

void loop() {
  // (note: line 1 is the second row, since counting begins with 0):
  // print the number of seconds since reset:
  // lcd.print(millis()/1000);

  // For proceeding, pressing the on / power button is needed
  do {
    delay(100);
    wait_time += 100;  
    if (wait_time > limit_time) {
      lcd.noDisplay(); // show nowthing
    }    
  } while (digitalRead(pin1) != 0);

  // show text again after hiding text in loop with lcd.noDisplay
  lcd.display(); 
  
  // Start using
  if (digitalRead(pin1) == 0) {
     delay(1000);
     reset_values();
     
  // If nothing happens: wait
  } else {
     delay(100);
  }

   // For each option and ...
   for (int now_option = 0; now_option < 2; now_option++) {

     // Display: this option is to think about
     lcd_clear();
     lcd_show("W"+ae+"hle nun aus: ", String(now_option) );

      // ... for each kategory do following: 
      for (int kategory = 0; kategory < 5; kategory++) { 

         // Show kategory
         lcd.setCursor(0, 0);
         lcd.print(question[kategory]);    

         // Show values of happyness and sadness
         lcd_refresh();

         // Wait in each kategory till button "next" is pressed.
         // Till then do:
         while (digitalRead(pin4) != 0) {

            // Read digital input: Button 2
            // Increase value by one
            if (digitalRead(pin2) == 0) {
               delay(200);
               if (sadness % skala == 0) {
                  sadness = 0;  
               }  
               sadness--;

            // Refresh display and values
            lcd_refresh();
   
            // Read digital input: Button 3
            // Increase value by one
            } if (digitalRead(pin3) == 0) {
               delay(200);
               if (happyness % skala == 0) {
                  happyness = 0;  
               }  
               happyness++;
            }

            // Refresh display and values
            lcd_refresh();
         }

         delay(500);

         // Sum up all values now, reset values
         summe[now_option][kategory] = happyness + sadness;
         sadness = 0;
         happyness = 0;
      }
   }

   // Now input gathering came to an end.
   // The results are to calculate now
   // Typically, there should be two of them 
   // for two given options

      // Calculate ...
      for (int i = 0; i < 5; i++) {
         for (int j = 0; j < 2; j++) {
            result[j] += summe[j][i]; 
         }
      }

      // Transform values for display
      antwort[0] += "Option 1: ";
      antwort[0] += result[0];
      antwort[0] += "              ";
      antwort[1] += "Option 2: ";
      antwort[1] += result[1];
      antwort[1] += "              ";

      // Display answers //
      lcd_show(antwort[0], antwort[1]);
      delay(4000);
      
      // Give a answer //
      if (result[0] == result[1]) {
         if (result[0] < 0) {
            quality = "schlecht";   
         } else {
            quality = "gut";
         };
         lcd_show("Beides ist ", "gleich " + quality);

      } else if (result[0] > result[1]) {
         lcd_show("Option 1 ist    ", "empfehlenswerter");

      } else if (result[0] < result[1]) {
         lcd_show("Option 2 ist    ", "empfehlenswerter");         
     
      } else {
         lcd_show("huppala, I made", "a mistake. Sorry :( ");  
      }      


     if (digitalRead(pin4) == 0) {
        lcd_show(antwort[0], antwort[1]);
    }
}


