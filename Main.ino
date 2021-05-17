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
#define ERROR_PIN 8

typedef char byte;

File immagine, testo, output;

//TODO Debug
void encrypt(File image, File text, File out) {
    image.seek(10);
    int offset = 0;
    offset += image.read() << 24;
    offset += image.read() << 16;
    offset += image.read() << 8;
    offset += image.read();

    image.seek(2);
    int image_length = 0; // = image.size(); ??
    image_length += image.read() << 24;
    image_length += image.read() << 16;
    image_length += image.read() << 8;
    image_length += image.read();

    //image.seek(offset);
    long text_length = text.size();

    image.seek(0);
    for (int j = 0; j < offset; ++j) {
        out.write(image.read());
    }

    while (image.available()) {
        if (text.available()) {
            byte text_byte = text.read();
            byte to_write;

            byte x = 0x01;
            for (int j = 0; j < 8; ++j) {
                byte img_byte = image.read();
                // to_write = ((img_byte >> 1) << 1) | (text_byte & x) >> j;
                to_write = (img_byte & 0xFE) | (text_byte & x) >> j;
                x <<= 1;
                out.write(to_write);
                if ((j + 1) % 4 == 0) { //(i + 1) % 3 ??
                    out.write(image.read());
                }
            }
            out.write(image.read());
        } else {
            out.write(image.read());
        }
    }
}

void decrypt(File image, File text) {
    //TODO Finish implementation
    image.seek(10);
    int offset = 0;
    offset += image.read() << 24;
    offset += image.read() << 16;
    offset += image.read() << 8;
    offset += image.read();

    image.seek(offset); //offset - 1 ??
    while (image.available()) {
        byte text_byte = 0;

        for (int i = 0; i < 8; ++i) {
            byte img_byte = image.read();
            if ((i + 1) % 4 != 0) { //(i + 1) % 3 ??
                text_byte += (img_byte | 0x01) << i; //&= instead of += ??

                //text_byte &= (img_byte | 0xFE)

                /*
                 * text_byte += (img_byte | 0x01);
                 * text_byte << 1;
                 */
            }
        }
    }
}

void complete() {
    digitalWrite(PROCESS_COMPLETE_PIN, HIGH);
    while (1) {
        delay(1000);
    }
}

void error() {
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
    if (digitalRead(ENCRYPT_DECRYPT_PIN) == HIGH) {
        immagine = SD.open("image.bmp", FILE_READ);
        testo = SD.open("text.txt", FILE_READ);
        output = SD.open("output.bmp", FILE_WRITE);

        if (!immagine || !testo || testo.size() * 3 > immagine.size()) { //TODO test for last condition
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
    else {
        immagine = SD.open("output.bmp", FILE_READ);
        testo = SD.open("text.txt", FILE_WRITE);
        if (!immagine) {
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
