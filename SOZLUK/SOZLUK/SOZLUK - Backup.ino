/*
 * DICTIONARY
 * build for Arduino NANO only
 * Other Arduino may not be supported
 * Any porting must pay attention with the pin
 * 
 * Using SD Card for the source, Speaker for the audio, and LCD to display the text
 * 
 * Project started on March 16, 2019
 * SÖZLÜK Muhammad Naufal Hilmy Makarim tarafından
 * 
 * License: MIT
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>

/** <FUNCTION>  */
void dictScreen();
void input();
void wordList();
void blinkPointer();
void audioPlay();
/** </FUNCTION> */

/** <VARIABLE>  */
bool sdcard(0), pointer(0);
uint8_t xpointer(0), ypointer(0);
unsigned long time1(0), time2(0);
char inputChar, reader, *dictionary;
String inputWord, language, audioFile;
/** </VARIABLE> */

/** <CHARACTER>   */
#define BACKSPACE   0x00
#define RIGHT_ARROW 0x00
#define LEFT_ARROW  0x00
#define UP_ARROW    0x00
#define DOWN_ARROW  0x00
/** </CHARACTER>  */

/** <LCD> */
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
LiquidCrystal_I2C lcd(0x27, 20, 4);    // (LCD_ADDRESS, 20 CHARS, 4 LINES)
/** </LCD>  */

/** <SD CARD>   */
// pin needed for SD Card attached to SPI bus
#define SS    10
#define MOSI  11
#define MISO  12
#define SCK   13

File sDict;
/** </SD CARD>  */

/** <AUDIO>   */
#define audioPin    9
#define audioVolume 6

TMRpcm  sAudio;
/** </AUDIO>  */

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin();
  lcd.backlight();
  
  // open serial communications and wait for port to open:
  Serial.begin(9600);
  
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  do {
    sdcard = SD.begin(SS);
    if(!sdcard) {
      lcd.clear();
      lcd.setCursor(3,2);
      lcd.print("Initialization");
      lcd.setCursor(7,3);
      lcd.print("failed");
      delay(2000);
      lcd.clear();
      lcd.setCursor(2,2);
      lcd.print("Re-initializing");
      lcd.setCursor(5,3);
      lcd.print("SD Card...");
      delay(2000);
    }
  }while(!sdcard);

  // set audio pin and volume
  sAudio.speakerPin = audioPin;
  sAudio.setVolume(audioVolume);

  // welcome screen
  lcd.clear();
  lcd.setCursor(3,2);
  lcd.print("MARHABA");
  delay(2000);
}

void loop() {
  dictScreen();
  blinkPointer();
}

void dictScreen() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print('>');
  lcd.setCursor(1,0);
  input();
  lcd.setCursor(0,1);
  wordList();
}

void input() {
  if(Serial.available() > 0) {
    inputChar = Serial.read();
  }
  if(inputChar == BACKSPACE && xpointer > 1 && xpointer < 20 && ypointer == 0) {
    memmove(&inputWord[xpointer - 2], &inputWord[xpointer - 1], inputWord.length() - xpointer - 2);
    --xpointer;
  }
  else if(inputChar == RIGHT_ARROW && xpointer < inputWord.length() + 1) {
    ++xpointer; 
  }
  else if(inputChar == LEFT_ARROW && xpointer > 1) {
    --xpointer;
  }
  else if(inputChar == UP_ARROW && ypointer > 0) {
    --ypointer;
  }
  else if(inputChar == DOWN_ARROW && ypointer < 4) {
    ++ypointer;
  }
  else if(inputChar != BACKSPACE || inputChar != RIGHT_ARROW || inputChar != LEFT_ARROW || inputChar != UP_ARROW || inputChar != DOWN_ARROW) {
    inputWord += inputChar;
    ++xpointer;
  }
}

void wordList() {
  int row(1), column(0);
  sDict = SD.open(("/dictionary/" + language).c_str(), FILE_READ);
  do {
   reader = sDict.read();
   /* # Separate dictionary
    * $ First language
    * % Second language
    * ^ Endline
    */
   if(reader = '$') {
    while(reader != '^') {
      reader = sDict.read();
      lcd.setCursor(column,row);
      lcd.print(reader);
      ++column;
    }
    ++row;
   }
  }while(row < 4);
}

void blinkPointer() {
  time1 = time2;
  time2 = millis();
  lcd.setCursor(xpointer,ypointer);
  if(time2 - time1 > 500 && (pointer & 1)) {
    lcd.print('_');
    pointer = 0;
  }
  else if(time2 - time1 > 500) {
    lcd.print(' ');
    pointer = 1;
  }
}

void audioPlay() {
  // convert String to char *
  // append ".wav" for audio format
  // just accept pointer variable
  sAudio.play(("/audio/" + audioFile + ".wav").c_str());
}
