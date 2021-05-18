/*
    SD card attached to SPI bus as follows:
    MOSI - pin 11
    MISO - pin 12
    CLK - pin 13
    CS - pin 10 (for MKRZero SD: SDCARD_SS_PIN)
*/

#include <SPI.h>
#include <SD.h>

#define CS_PIN 10
#define RGB_RED_PIN A0
#define RGB_GREEN_PIN 5
#define RGB_BLUE_PIN 6
#define ENCRYPT_DECRYPT_PIN 2
#define INTERRUPT_PIN 3

File immagine, testo, output;
bool completed = false;
bool encrypting = true;


void encrypt(File image, File text, File out) {
    image.seek(10);
    int offset = 0;
    image.read(&offset, sizeof(offset));

    int image_length = image.size() - offset;
    
    image.seek(0);
    for (int j = 0; j < offset; ++j) {
        byte x;
        image.read(&x, sizeof(byte));
        out.write(x);
        delay(1);
    }

    while (image.available()) {
        setRgb(image.size(), image.position());
        if (text.available()) {
            byte text_byte = text.read();
            byte to_write;

            byte x = 0x01;
            for (int j = 0; j < 8; ++j) {
                byte img_byte = image.read();
                to_write = (img_byte & 0xFE) | (text_byte & x) >> j;
                x <<= 1;
                out.write(to_write);
                
                if ((j + 1) % 3 == 0) {
                    byte x = image.read();
                    out.write(x);
                }
            }
            byte y = image.read();
            out.write(y);
        } else {
            byte x = image.read();
            x &= 0xFE;
            out.write(x);
        }
    }
}

void decrypt(File image, File text) {
    image.seek(10);
    int offset = 0;
    image.read(&offset, sizeof(offset));

    image.seek(offset);
    while (image.available()) {
        byte text_byte = 0;

        for (int i = 0; i < 8; ++i) {
            byte img_byte;
            image.read(&img_byte, sizeof(byte));
            text_byte |= (img_byte & 0x01) << i;
            
            if ((i + 1) % 3 != 0) {
                image.read();
            }
        }
        text.write(text_byte);
        
        if (text_byte == 0) {
            break;
        }
    }
}

void setRgb(int file_size, int current_position) {
    int x = (current_position * 255 * 2) / file_size;
    if (x < 255) {
        RGB_color(x, 0, 0);
    } else {
        RGB_color(255, x % 255, 0);
    }
}

void complete() {
    RGB_color(0, 255, 0);
    stop();
    RGB_color(0, 0, 0);
}

void stop() {
    while (completed) {
        delay(1000);
    }
}

void error() {
    completed = true;
    RGB_color(255, 0, 0);
    stop();
    RGB_color(0, 0, 0);
}

void changeComplete() {
    completed = false;
}

void encryptDecrypt() {
    encrypting = !encrypting;
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value) {
    analogWrite(RGB_RED_PIN, red_light_value);
    analogWrite(RGB_GREEN_PIN, green_light_value);
    analogWrite(RGB_BLUE_PIN, blue_light_value);
}


void setup() {
    pinMode(RGB_RED_PIN, OUTPUT);
    pinMode(RGB_GREEN_PIN, OUTPUT);
    pinMode(RGB_BLUE_PIN, OUTPUT);
    pinMode(ENCRYPT_DECRYPT_PIN, INPUT);
    pinMode(INTERRUPT_PIN, INPUT);
    RGB_color(0, 0, 0);

    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), changeComplete, RISING);
    //attachInterrupt(digitalPinToInterrupt(ENCRYPT_DECRYPT_PIN), encryptDecrypt, RISING);
}

void loop() {
    if (!SD.begin(CS_PIN)) {
        RGB_color(255, 0, 0);
        delay(1000);
        return;
    }
    RGB_color(0, 0, 0);

    //ENCRYPT
    if (digitalRead(ENCRYPT_DECRYPT_PIN) == HIGH) {
        immagine = SD.open("image.bmp", FILE_READ);
        testo = SD.open("text.txt", FILE_READ);
        output = SD.open("output.bmp", FILE_WRITE);

        if (!immagine || !testo) {
            error();
        }
        encrypt(immagine, testo, output);

        immagine.close();
        testo.close();
        output.close();

        //SD.remove("image.bmp");
        //SD.remove("text.txt");
        completed = true;
    }
    //DECRYPT
    else {
        immagine = SD.open("output.bmp", FILE_READ);
        testo = SD.open("text.txt", FILE_WRITE);
        if (!immagine) {
            error();
        }
        decrypt(immagine, testo);

        immagine.close();
        testo.close();

        //SD.remove("output.bmp");
        completed = true;
    }
    complete();
}
