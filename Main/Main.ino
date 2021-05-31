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
    asm(
    "ldi	r20, 0x0A \n"
    "ldi	r21, 0x00 \n"
    "ldi	r22, 0x00 \n"
    "ldi	r23, 0x00 \n"
    "movw	r24, r28 \n"
    "adiw	r24, 0x1c \n"
    "call	0x1b94 \n");
    
    int offset = 0;
    asm(
    "std	Y+56, r1 \n"
    "std	Y+55, r1 \n");
    
    image.read(&offset, sizeof(offset));
    asm(
    "ldi	r20, 0x02 \n"
    "ldi	r21, 0x00 \n"
    "movw	r22, r28 \n"
    "subi	r22, 0xC9 \n"
    "sbci	r23, 0xFF \n"
    "movw	r24, r28 \n"
    "adiw	r24, 0x1c \n"
    "call	0x1c00 \n");

    image.seek(offset);
    asm(
    "ldd	r20, Y+55 \n"
    "ldd	r21, Y+56 \n"
    "mov	r0, r21 \n"
    "add	r0, r0 \n"
    "sbc	r22, r22 \n"
    "sbc	r23, r23 \n"
    "movw	r24, r28 \n"
    "adiw	r24, 0x1c \n"
    "call	0x1b94 \n");
    
    char img_byte;
    
    char text_byte = 0;
    
    for (int i = 0; i < 8; ++i) {
        image.read(&img_byte, sizeof(char));
        text_byte = text_byte | ((img_byte & 0x01) << i);

        if ((i + 1) % 3 == 0) {
            asm(
            "ldi	r24, 0x03 \n"
            "mov	r8, r24 \n"
            "mov	r9, r1 \n");
            
            image.seek(10);
            
            int offset = 0;
            
            image.read(&offset, sizeof(offset));

            image.seek(offset);
            
            while (image.available()) {
                asm(
                "movw	r24, r28 \n"
                "adiw	r24, 0x1c \n"
                "call	0x1bc0 \n"
                "or	r24, r25 \n"
                "brne	.+14 \n");
                error();
                return;
            }

            immagine.close();
            asm(
            "ldi	r24, 0x98 \n"
            "ldi	r25, 0x04 \n"
            "call	0x2298 \n");
            
            testo.close();
            asm(
            "ldi	r24, 0x7D \n"
            "ldi	r25, 0x04 \n"
            "rjmp	.-216 \n");
            
            offset = 0;
            
            image.read(&offset, sizeof(offset));

            image.seek(offset);
            
            while (image.available()) {
                setRgb(image.size(), image.position());
                text_byte = 0;
                asm("mov	r15, r1 \n");
                
                char img_byte;
                
                for (int i = 0; i < 8; ++i) {
                    asm(
                    "ldi	r17, 0x00 \n"
                    "ldi	r16, 0x00 \n");
                    
                    image.read(&img_byte, sizeof(char));
                    asm(
                    "ldi	r20, 0x01 \n"
                    "ldi	r21, 0x00 \n"
                    "movw	r22, r28 \n"
                    "subi	r22, 0xAE \n"
                    "sbci	r23, 0xFF \n"
                    "movw	r24, r28 \n"
                    "adiw	r24, 0x1c \n"
                    "call	0x1c00 \n");
                    
                    text_byte = text_byte | ((img_byte & 0x01) << i);
                    asm(
                    "adiw	r28, 0x13 \n"
                    "ldd	r24, Y+63 \n"
                    "sbiw	r28, 0x13 \n"
                    "bst	r24, 0 \n"
                    "eor	r10, r10 \n"
                    "bld	r10, 0 \n"
                    "mov	r11, r1 \n"
                    "movw	r24, r10 \n"
                    "mov	r0, r16 \n"
                    "rjmp	.+2 \n"
                    "add	r24, r24 \n"
                    "dec	r0 \n"
                    "brpl	.-6 \n"
                    "or	r15, r24 \n");

                    if ((i + 1) % 3 == 0) {
                        asm(
                        "subi	r16, 0xFF \n"
                        "sbci	r17, 0xFF \n"
                        "movw	r24, r16 \n"
                        "movw	r22, r8 \n"
                        "call	0x337a \n"
                        "or	r24, r25 \n"
                        "brne	.+8 \n");
                        
                        image.read();
                        asm(
                        "movw	r24, r28 \n"
                        "adiw	r24, 0x1c \n"
                        "call	0x1cc8 \n");

                        image.seek(offset);
                        
                        while (image.available()) {
                            
                            char text_byte = 0;
                            
                            char img_byte;
                            
                            for (int i = 0; i < 8; ++i) {
                                asm(
                                "cpi	r16, 0x08 \n"
                                "cpc	r17, r1 \n"
                                "brne	.-76 \n");

                                if ((i + 1) % 3 == 0) {
                                    image.read();
                                }
                            }
                            
                            image.read();
                            asm(
                            "movw	r24, r28 \n"
                            "adiw	r24, 0x1c \n"
                            "call	0x1cc8 \n");
                            
                            text.write(text_byte);
                            asm(
                            "mov	r22, r15 \n"
                            "movw	r24, r28 \n"
                            "adiw	r24, 0x01 \n"
                            "call	0x1c6e \n");

                            if (text_byte == 0) {
                                asm(
                                "cpse	r15, r1 \n"
                                "rjmp	.-130 \n"
                                "rjmp	.-120 \n");
                            }
                        }
                    }
                }
            }
        }
    }
}

void decrypt(File image, File text) {
    image.seek(10);
    int offset = 0;
    image.read(&offset, sizeof(offset));

    image.seek(offset);
    while (image.available()) {
        char text_byte = 0;
        char img_byte;
        for (int i = 0; i < 8; ++i) {
            image.read(&img_byte, sizeof(char));
            text_byte = text_byte | ((img_byte & 0x01) << i);
            
            if ((i + 1) % 3 == 0) {
                image.read();
            }
        }
        image.read();
        text.write(text_byte);

        if (text_byte == 0) {
            break;
        }
    }
}

void setRgb(int file_size, int current_position) {
    double x = ((double) current_position / (double) file_size) * 255.0 * 2.0;
    RGB_color(0, x >= 255 ? x - 255 : 0, 255 - abs(x - 255));
}

void complete() {
    stop(0, 255, 0);
}

void error() {
    completed = true;
    stop(255, 0, 0);
}

void stop(int red_light_value, int green_light_value, int blue_light_value) {
    while (completed) {
        RGB_color(0, 0, 0);
        delay(1000);
        RGB_color(red_light_value, green_light_value, blue_light_value);
        delay(1000);

        if (digitalRead(INTERRUPT_PIN) == HIGH) {
            completed = false;
        }
    }
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
}

void loop() {
    if (!SD.begin(CS_PIN)) {
        RGB_color(0, 0, 255);
        delay(1000);
        return;
    }
    RGB_color(0, 0, 0);

    //ENCRYPT
    if (digitalRead(ENCRYPT_DECRYPT_PIN) == HIGH/* && SD.exists("INPUT.BMP") && SD.exists("TEXT.TXT")*/) {
        immagine = SD.open("INPUT.BMP", FILE_READ);
        testo = SD.open("TEXT.TXT", FILE_READ);
        output = SD.open("OUTPUT.BMP", FILE_WRITE);

        if (!immagine || !testo || !output) {
            error();
            return;
        }
        encrypt(immagine, testo, output);

        immagine.close();
        testo.close();
        output.close();

        //SD.remove("INPUT.BMP");
        //SD.remove("TEXT.TXT");
        completed = true;
    }
    //DECRYPT
    else/* if (digitalRead(ENCRYPT_DECRYPT_PIN) == LOW && SD.exists("OUTPUT.BMP")) */{
        immagine = SD.open("OUTPUT.BMP", FILE_READ);
        testo = SD.open("OUTPUT.TXT", FILE_WRITE);
        if (!immagine || !testo) {
            error();
            return;
        }
        decrypt(immagine, testo);

        immagine.close();
        testo.close();

        //SD.remove("OUTPUT.BMP");
        completed = true;
    }
    complete();
}
