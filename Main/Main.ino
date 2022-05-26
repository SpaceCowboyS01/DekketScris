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

volatile int i;
volatile char img_byte;
volatile char text_byte = 0;

void hiding(File image, File text, File out) {
    image.seek(10);
    int offset = 0;
    image.read(&offset, sizeof(offset));

    image.seek(0);

    for (int j = 0; j < offset; ++j) {
        char x;
        image.read(&x, sizeof(char));
        out.write(x);
    }

    image.seek(offset);

    while (image.available()) {
        setRgb(image.size(), image.position());

        if (text.available()) {
            char text_byte = text.read();
            char x = 0x01;

            for (int i = 0; i < 8; ++i) {

                char img_byte;

                image.read(&img_byte, sizeof(char));
                char to_write = ((img_byte & 0xFE) | ((text_byte & x) >> i));

                x <<= 1;

                asm(
                        "lds r24, (img_byte) \n"
                        "lds r26, (text_byte) \n"
                        "and r24, 0xFE \n"
                        "and r24, 0x01 \n"
                        "lds r25, (i) \n"
                        "loop:"
                        "lsr r26"
                        "dec r25 \n"
                        "brne loop \n"
                        "or r24, r26 \n"
                        "lds r28, (to_write) \n"
                        "mov r28, r24 \n"
                        :: :"r26", "r24", "r25", "r26"
                );

                x <<= 1;
                out.write(to_write);

                if ((i + 1) % 3 == 0) {
                    char x = image.read();
                    out.write(x);
                }
            }

            char y = image.read();
            out.write(y);

        } else {
            char x = image.read();
            x &= 0xFE;
            out.write(x);
        }
    }
}
/**
 * This method unveil the message hided on image.
 *
 * @param image
 * @param text
 */
void unveiling(File image, File text) {
    image.seek(10);
    int offset = 0;
    image.read(&offset, sizeof(offset));

    image.seek(offset);
    while (image.available()) {
        setRgb(image.size(), image.position());

        for ( i = 0; i < 8; ++i) {
            image.read(&img_byte, sizeof(char));

            asm(
            "lds r24, (img_byte) \n"
            "lds r26, (text_byte) \n"
            "and r24, 0x01 \n"
            "lds r25, (i) \n"
            "loop:"
            "lsl r24 \n"
            "dec r25 \n"
            "brne loop \n"
            "or r26, r24 \n"
            :::"r24", "r26", "r25"
            );

            if ((i + 1) % 3 == 0)
                image.read();
        }

        image.read();
        text.write(text_byte);

        if (text_byte == 0)
            break;
    }
}
/**
 * This method will turn the LED from BLUE to GREEN by fade.
 * It will indicate the status of the process.
 *
 * @param file_size
 * @param current_position
 */
void setRgb(unsigned long file_size, unsigned long current_position) {
    double position_color = ((double) current_position / (double) file_size) * 255.0 * 2.0;
    if (position_color <= 255)
        RGB_color(0, position_color, 255);
    else
        RGB_color(0, 255, (255 * 2) - position_color);
}
/**
 * This method will turn the LED in GREEN color and blink.
 * It will signal the end of the process, then you can remove the SD card.
 */
void complete() {
    stop(0, 255, 0);
}

/**
 * This method will turn the LED in RED color and blink.
 * It will report a generic error or an error in the input files.
 */
void error() {
    completed = true;
    stop(255, 0, 0);
}

/**
 *  This method will cause the LED to blink.
 *
 * @param red_light_value
 * @param green_light_value
 * @param blue_light_value
 */
void stop(int red_light_value, int green_light_value, int blue_light_value) {
    while (digitalRead(INTERRUPT_PIN) == HIGH) {
        RGB_color(0, 0, 0);
        delay(1000);
        RGB_color(red_light_value, green_light_value, blue_light_value);
        delay(1000);
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
        if (digitalRead(ENCRYPT_DECRYPT_PIN) == HIGH)
            RGB_color(200, 60, 0); //Rellow
        else
            RGB_color(143, 0, 255); //Blurple

        delay(1000);
        return;
    }

    RGB_color(0, 0, 0);

    //HIDING
    if (digitalRead(ENCRYPT_DECRYPT_PIN) == HIGH) {
        immagine = SD.open("INPUT.BMP", FILE_READ);
        testo = SD.open("TEXT.TXT", FILE_READ);
        output = SD.open("OUTPUT.BMP", FILE_WRITE);

        if (!immagine || !testo || !output) {
            error();
            return;
        }

        hiding(immagine, testo, output);

        immagine.close();
        testo.close();
        output.close();
        completed = true;
    }

    //UNVEILING
    else {
        immagine = SD.open("OUTPUT.BMP", FILE_READ);
        testo = SD.open("OUTPUT.TXT", FILE_WRITE);
        if (!immagine || !testo) {
            error();
            return;
        }

        unveiling(immagine, testo);

        immagine.close();
        testo.close();
        completed = true;
    }

    complete();
}