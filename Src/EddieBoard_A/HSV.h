/*
 * hsv.h
 *
 * Created: 2014-10-27 22:40:48
 *  Author: Christopher
 */ 


#ifndef HSV_H_
#define HSV_H_

#include <stdint.h>

/*typedef struct RgbColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RgbColor;

typedef struct HsvColor
{
	unsigned char h;
	unsigned char s;
	unsigned char v;
} HsvColor;*/

class HsvColor;

class RgbColor {
	public:
		uint8_t r;
		uint8_t g;
		uint8_t b;
	
		RgbColor( uint8_t r, uint8_t g, uint8_t b );
		RgbColor() : RgbColor( 255, 255, 255 ) {};
	
	
		HsvColor ToHSV();
};
class HsvColor {
	public:
		uint8_t h;
		uint8_t s;
		uint8_t v;
	
		HsvColor( uint8_t h, uint8_t s, uint8_t v );
		HsvColor() : HsvColor( 0, 255, 255 ) {};
	
		RgbColor ToRGB();
};

RgbColor HsvToRgb(HsvColor hsv);
HsvColor RgbToHsv(RgbColor rgb);




#endif /* HSV_H_ */