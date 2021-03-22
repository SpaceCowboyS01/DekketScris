/*
    SD card attached to SPI bus as follows:
    MOSI - pin 11
    MISO - pin 12
    CLK - pin 13
    CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <SPI.h>
#include <SD.h>

#define CS_PIN 4
#define SERIAL_FREQUENCY 9600
#define ENCRYPT_DECRYPT_PIN 13
#define PROCESS_COMPLETE_PIN 12
#define ERROR_PIN 11

File immagine, testo, output;

void encrypt(File image, File text, File out){
  //TODO implement
  byte img_byte = image.read();
  byte text_byte = text.read();
  byte to_write;
  out.write(to_write);
}

void decrypt(File image, File text){
  //TODO implement
}

void complete(){
  digitalWrite(PROCESS_COMPLETE_PIN, HIGH);
  while (1){
    delay(1000);
  }  
}

void error(){
  digitalWrite(ERROR_PIN, HIGH);
  complete();
}

void setup() {
  pinMode(PROCESS_COMPLETE_PIN, OUTPUT);
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(ENCRYPT_DECRYPT_PIN, INPUT);
  digitalWrite(PROCESS_COMPLETE_PIN, LOW);
  digitalWrite(ERROR_PIN, LOW);

  if (!SD.begin(CS_PIN)) {
    error();
  }
  
  //ENCRYPT
  if(digitalRead(ENCRYPT_DECRYPT_PIN)==HIGH){
    immagine = SD.open("image.bmp", FILE_READ);
    testo = SD.open("text.txt", FILE_READ);
    output = SD.open("output.bmp", FILE_WRITE);
    //TODO test for last condition
    if(!immagine || !testo || testo.size()*3>immagine.size()){
      error();
    }
    encrypt(immagine, testo, output);
    
    immagine.close();
    testo.close();
    output.close();

    SD.remove("image.bmp");
    SD.remove("text.txt");
  }
  //DECRYPT
  else{
    immagine = SD.open("output.bmp", FILE_READ);
    testo = SD.open("text.txt", FILE_WRITE);
    if(!immagine){
      error();
    }
    decrypt(immagine, testo);

    immagine.close();
    testo.close();

    SD.remove("output.bmp");
  }
  complete();
}

void loop() {}
