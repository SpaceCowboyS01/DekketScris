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

//unsigned long time = 0;

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
        char to_write;
            to_write = ((img_byte & 0xFE) | ((text_byte & x) >> i));
            x <<= 1;
      /*asm(
      //encrytp TO DO
      :::
      );*/
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

void unveiling(File image, File text) {
image.seek(10);
int offset = 0;
image.read(&offset, sizeof(offset));

image.seek(offset);
while (image.available()) {
    setRgb(image.size(), image.position());
    
    
    
    for ( i = 0; i < 8; ++i) {
        image.read(&img_byte, sizeof(char));
        //text_byte = text_byte | ((img_byte & 0x01) << i);
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

void setRgb(unsigned long file_size, unsigned long current_position) {
double x = ((double) current_position / (double) file_size) * 255.0 * 2.0;
RGB_color(0, (x >= 255 ? x - 255 : 0), 255 - x);
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

//Serial.begin(9600);
}

void loop() {
if (!SD.begin(CS_PIN)) {
if (digitalRead(ENCRYPT_DECRYPT_PIN) == HIGH) {
RGB_color(200, 60, 0); //Rellow
} else {
RGB_color(143, 0, 255); //Blurple
}
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
    //time = millis();
    hiding(immagine, testo, output);
    //Serial.println(millis() - time);

    immagine.close();
    testo.close();
    output.close();

    //TODO
    //SD.remove("INPUT.BMP");
    //SD.remove("TEXT.TXT");
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
    //time = millis();
    unveiling(immagine, testo);
    //Serial.println(millis() - time);

    immagine.close();
    testo.close();

    //TODO
    //SD.remove("OUTPUT.BMP");
    completed = true;
}
complete();
}
