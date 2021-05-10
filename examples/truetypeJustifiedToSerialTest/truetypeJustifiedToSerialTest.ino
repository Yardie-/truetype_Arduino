#include "FS.h"
#include "SPIFFS.h"
#include "truetype_Arduino.h"
/*
 * This example simply creates a bitmap 
 * from a ttf file you supply
 * Then prints the buffer contents to Serial
 * as you would send it to your display. 
 * 
 * This is a good routine to test fonts
 * for compatiblity.
 * It is easiest to see the results 
 * if you use 1 bit per pixel. 
 * It works just the same for 4 and 8
 * 
 * It rotates the text in all four directions 
 * 
 * For copyright reasons no actual ttf fonts
 * are distiributed here.
 * 
 * Dont forget to upload your ttf to SPIFFS
 * Warning some fonts are not supported!
 * 
 * SPIFFS upload
   * create a sub folder in your sketch folder called data
   * add the ttf to that folder 
   * for this example rename it to FONTFILE.ttf
   * nb. SPIFFS has name length limitations so the
   * upload it to SPIFFS 
   * upload will not work while Serial Terminal is connected
 *
 * #define is used here to help clarify the
 * variables that you would normally set 
 * and how they intereract.
 * Also to help clarify some terms
 * and ideas
 * They are not required by the class
 * 
 * This adds justification to the base example
 *  
*/

// just to be clear as some are redundant 

#define WIDTH_PIXELS      64 // this needs to be a multiple of 8
#define HEIGHT_PIXELS     40
#define BITS_PER_PIXEL     1 // either 1, 4, or 8

#define DISPLAY_HEIGHT    HEIGHT_PIXELS
#define DISPLAY_WIDTH     WIDTH_PIXELS 

#define WIDTH_BYTES       (DISPLAY_WIDTH * BITS_PER_PIXEL) / 8

#define FRAMEBUFFER_SIZE  WIDTH_BYTES * DISPLAY_HEIGHT


#define MY_TTF "/FONTFILE.ttf"

//TrueType class declaration
truetypeClass truetype = truetypeClass();
uint8_t *framebuffer;

void print_bitmap(uint8_t *framebuffer, uint16_t width_in_bytes, uint16_t height_in_pixels) {
  for (int i = 0; i < (width_in_bytes * height_in_pixels); i++) {
    if ((i % width_in_bytes) == 0)
      Serial.println();
    for (uint8_t _bit = 8; _bit > 0; _bit--) {
      if (_BV(_bit - 1) & framebuffer[i])
        Serial.print("*"); //set bit
      else
        Serial.print("_");
    }
    // Serial.print(" "); // uncomment to show individual bytes
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  //Prepare a frame buffer

  framebuffer = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
  if (!framebuffer) {
    Serial.println("alloc memory failed !!!");
    while (1);
  }
  Serial.print("The image buffer is ");
  Serial.print(FRAMEBUFFER_SIZE);
  Serial.println(" bytes");
  //Read TrueType file
  //Example in SPIFFS
  //I think that SD, FATFS and other codes will be almost the same
  SPIFFS.begin(true);
  File fontFile = SPIFFS.open(MY_TTF, "r");
  Serial.print("The using ttf file ");
  Serial.print(fontFile.name());
  Serial.print(" which is ");
  Serial.print(fontFile.size());
  Serial.println(" bytes");

  //Set framebuffer array in TrueType class
  //Pay attention to the format of the framebuffer
  truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, BITS_PER_PIXEL, ROTATE_0, framebuffer);
  //Initial reading of ttf files
  if (!truetype.setTtfFile(fontFile)) {
    Serial.println("read ttf failed");
  } else {
    //TrueType class string parameter settings
    truetype.setCharacterSize(20);
    truetype.setCharacterSpacing(0);
    truetype.breakLine = false; // force one line only no wrapping
    // As we are rotated these change
    truetype.setTextBoundary(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    truetype.setTextColor(0xFF, 0xFF); // only the relevant bit/s will be set
    String str = "Hello";
    uint16_t width = truetype.getStringWidth(str);
    int16_t to_centre = DISPLAY_WIDTH / 2 - width / 2;
    int16_t to_right = DISPLAY_WIDTH - width;
    
    //Write a string to the framebuffer
    truetype.textDraw(to_centre, 0, str);
    truetype.textDraw(to_right, 20, str);
    //Export framebuffer to screen
    //FLASH_TO_SCREEN();
    Serial.println("The buffer looks like this.");
    print_bitmap(framebuffer, WIDTH_BYTES, HEIGHT_PIXELS);

    //end
    truetype.end();
  }
}

void loop() {
}
