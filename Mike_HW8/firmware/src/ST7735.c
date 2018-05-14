
// functions to operate the ST7735 on the PIC32
// adapted from https://github.com/sumotoy/TFT_ST7735
// and https://github.com/adafruit/Adafruit-ST7735-Library

// pin connections:
// VCC - 3.3V
// GND - GND
// CS - B7
// RESET - 3.3V
// A0 - B15
// SDA - A1
// SCK - B14
// LED - 3.3V

// B8 is turned into SDI1 but is not used or connected to anything

#include <xc.h>
#include "ST7735.h"

void SPI1_init() {
  SDI1Rbits.SDI1R = 0b0100; // B8 is SDI1
  RPA1Rbits.RPA1R = 0b0011; // A1 is SDO1
  TRISBbits.TRISB7 = 0; // SS is B7
  LATBbits.LATB7 = 1; // SS starts high

  // A0 / DAT pin
  ANSELBbits.ANSB15 = 0;
  TRISBbits.TRISB15 = 0;
  LATBbits.LATB15 = 0;

  SPI1CON = 0; // turn off the spi module and reset it
  SPI1BUF; // clear the rx buffer by reading from it
  SPI1BRG = 0; // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0; // clear the overflow bit
  SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1; // master operation
  SPI1CONbits.ON = 1; // turn on spi1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    LATBbits.LATB15 = 0; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(com);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat>>8);
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_init() {
    SPI1_init();
  int time = 0;
  LCD_command(ST7735_SWRESET);//software reset
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2/2) {}

	LCD_command(ST7735_SLPOUT);//exit sleep
  time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {}

	LCD_command(ST7735_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_FRMCTR2);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_FRMCTR3);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	LCD_data(0x01);
	LCD_data(0x2C);
	LCD_data(0x2D);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_INVCTR);//display inversion
	LCD_data(0x07);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_PWCTR1);
	LCD_data(0x0A);//4.30 - 0x0A
	LCD_data(0x02);//0x05
	LCD_data(0x84);//added auto mode
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_PWCTR2);
	LCD_data(0xC5);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR3);
	LCD_data(0x0A);
	LCD_data(0x00);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR4);
	LCD_data(0x8A);
	LCD_data(0x2A);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command( ST7735_PWCTR5);
	LCD_data(0x8A);
	LCD_data(0xEE);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_VMCTR1);
	LCD_data(0x0E);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_INVOFF);

	LCD_command(ST7735_MADCTL);
	LCD_data(0xC8);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_COLMOD);
	LCD_data(0x05);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_CASET);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x7F);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_RASET);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x9F);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_GMCTRP1);
	LCD_data(0x02);
	LCD_data(0x1C);
	LCD_data(0x07);
	LCD_data(0x12);
	LCD_data(0x37);
	LCD_data(0x32);
	LCD_data(0x29);
	LCD_data(0x2D);
	LCD_data(0x29);
	LCD_data(0x25);
	LCD_data(0x2B);
	LCD_data(0x39);
	LCD_data(0x00);
	LCD_data(0x01);
	LCD_data(0x03);
	LCD_data(0x10);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_GMCTRN1);
	LCD_data(0x03);
	LCD_data(0x1D);
	LCD_data(0x07);
	LCD_data(0x06);
	LCD_data(0x2E);
	LCD_data(0x2C);
	LCD_data(0x29);
	LCD_data(0x2D);
	LCD_data(0x2E);
	LCD_data(0x2E);
	LCD_data(0x37);
	LCD_data(0x3F);
	LCD_data(0x00);
	LCD_data(0x00);
	LCD_data(0x02);
	LCD_data(0x10);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {}

	LCD_command(ST7735_NORON);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/100) {}

	LCD_command(ST7735_DISPON);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/10) {}

	LCD_command(ST7735_MADCTL); // rotation
    LCD_data(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // check boundary
  LCD_setAddr(x,y,x+1,y+1);
  LCD_data16(color);
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
  LCD_command(ST7735_CASET); // Column
  LCD_data16(x0);
	LCD_data16(x1);

	LCD_command(ST7735_RASET); // Page
	LCD_data16(y0);
	LCD_data16(y1);

	LCD_command(ST7735_RAMWR); // Into RAM
}

void LCD_clearScreen(unsigned short color) {
  int i;
  LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
	for (i = 0;i < _GRAMSIZE; i++){
		LCD_data16(color);
	}
}

void drawCross(unsigned short x, unsigned short y, unsigned short h, signed short lenx1, signed short leny1, unsigned short len, unsigned short c1, unsigned short c2){
    int realH = (2*h+1), fillH = realH - 2;
    int i = 0, j = 0, k = 0, oS = (realH-1)/2, oS_Fill = oS-1;
    
    // Bar Borders:
    for(i = 0; i<=len; i++){
    LCD_drawPixel(x+oS,y+i,c1);
    LCD_drawPixel(x-oS,y+i,c1);
    LCD_drawPixel(x+i,y-oS,c1);
    LCD_drawPixel(x+i,y+oS,c1);
    LCD_drawPixel(x+oS,y-i,c1);
    LCD_drawPixel(x-oS,y-i,c1);
    LCD_drawPixel(x-i,y-oS,c1);
    LCD_drawPixel(x-i,y+oS,c1);
    if(i==0 | i == len){
        for(j = 0; j < realH; j++){
            LCD_drawPixel(x-oS+j,y+i,c1);
            LCD_drawPixel(x-oS+j,y-i,c1);
            LCD_drawPixel(x+i,y-oS+j,c1);
            LCD_drawPixel(x-i,y-oS+j,c1);
            }
        }
    }
    
    // X Bar Fill:
    if(lenx1>=0){
        for(i = 0; i<len; i++){
            for(j=0;j<fillH;j++){
                if(i<lenx1){
                    LCD_drawPixel(x+i, y-oS_Fill+j,c1); 
                }else{
                    LCD_drawPixel(x+i, y-oS_Fill+j,c2); 
                }
                LCD_drawPixel(x-i,y-oS_Fill+j,c2);
            }
        }
    }else if(lenx1<0) {
        for(i = 0; i>(-len); i--){
            for(j=0;j<fillH;j++){
                if(i>lenx1){
                    LCD_drawPixel(x+i, y-oS_Fill+j,c1);
                }else{
                    LCD_drawPixel(x+i, y-oS_Fill+j,c2);
                }
                LCD_drawPixel(x-i,y-oS_Fill+j,c2);    
            }
        }
    }
    
    // Y Bar Fill:
    if(leny1>=0){
        for(i = 0; i<len; i++){
            for(j=0;j<fillH;j++){
                if(i<leny1){
                    LCD_drawPixel(x-oS_Fill+j, y+i,c1);
                }else{
                    LCD_drawPixel(x-oS_Fill+j, y+i,c2);
                }
                LCD_drawPixel(x-oS_Fill+j,y-i,c2);
            }
        }
    }else{
        for(i = 0; i>(-len); i--){
            for(j=0;j<fillH;j++){
                if(i>leny1){
                    LCD_drawPixel(x-oS_Fill+j, y+i,c1);
                }else{
                    LCD_drawPixel(x-oS_Fill+j, y+i,c2);
                }
                LCD_drawPixel(x-oS_Fill+j,y-i,c2);
            }
        }
    }
}

// clearLCD: colors entire screen black
void clearLCD(void){
    int i = 0, j = 0;
    for(i = 0; i<128;i++){
        for (j = 0; j<160;j++){
            LCD_drawPixel(i,j,BLACK);
        }   
    }
}

// drawChar: draws a single character at (x,y) w/ foreground color c1 and
//           background color c2
void drawChar(unsigned short x, unsigned short y, char mes, unsigned short c1, unsigned short c2){
    char row = mes - 0x20;         // subtract for missing ascii command characters
    int col = 0;
    for (col = 0; col < 5; col++){
        char pixels = ASCII[row][col];
        int j = 0;
        for(j = 0; j < 8; j++){
            if((pixels>>j)&1==1){
                LCD_drawPixel(x+col,y+j,c1);
            }else{
                LCD_drawPixel(x+col,y+j,c2);
            }
        }
    }
    
}

// drawString: writes a sprintf character array at (x,y) w/ foreground color c1
//             and background color c2
void drawString(unsigned short x, unsigned short y, char *message, unsigned short c1, unsigned short c2){
   int i = 0;
   while (message[i]){
       drawChar(x+6*i,y,message[i],c1,c2);
       i++;
   }
}

// progBar: draws a progress bar at (x,y) w/ height h,
//          fill length len1 (0 to 100), total length len2, 
//          foreground color c1, and background color c2
void progBar(unsigned short x, unsigned short y, unsigned short h, unsigned short len1, unsigned short c1, unsigned short len2, unsigned short c2){
    int i = 0, j = 0, k = 0;
    if(len1 > 100){
        len1 = 100;
    }else if(len1 < 0){
        len1 = 0;
    }
    int fill = (int) (((((float) len1)/100)*len2)-1);
    for(j = 0; j < h; j++){
        LCD_drawPixel(x,y+j,c1);
        LCD_drawPixel(x+len2,y+j,c1);
    }
    for(i = 0; i <= len2; i++){
        LCD_drawPixel(x+i,y,c1);
        LCD_drawPixel(x+i,y+h,c1);
    }
    for(k = 0; k < fill; k++){
        for(j = 0; j < h; j++){
            LCD_drawPixel(x+1+k, y+1+j, c1);
        }
    }
}