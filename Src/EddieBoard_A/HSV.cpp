/*
 * HSV.c
 *
 * Created: 2014-10-27 22:38:44
 *  Code from http://stackoverflow.com/a/22120275/4204058
 */ 

#include "HSV.h"

/* Create a RGB object */
RgbColor::RgbColor( uint8_t r, uint8_t g, uint8_t b )
{
	this->r = r;
	this->g = g;
	this->b = b;
}
/* Create a HSV object */
HsvColor::HsvColor( uint8_t h, uint8_t s, uint8_t v )
{
	this->h = h;
	this->s = s;
	this->v = v;
}

/* Convert the HSV to RGB */
RgbColor
HsvColor::ToRGB()
{
	RgbColor rgb;
	unsigned char region, p, q, t;
	unsigned int h, s, v, remainder;

	if (this->s == 0)
	{
		rgb.r = this->v;
		rgb.g = this->v;
		rgb.b = this->v;
		return rgb;
	}

	// converting to 16 bit to prevent overflow
	h = this->h;
	s = this->s;
	v = this->v;

	region = h / 43;
	remainder = (h - (region * 43)) * 6;

	p = (v * (255 - s)) >> 8;
	q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	switch (region)
	{
		case 0:
		rgb.r = v;
		rgb.g = t;
		rgb.b = p;
		break;
		case 1:
		rgb.r = q;
		rgb.g = v;
		rgb.b = p;
		break;
		case 2:
		rgb.r = p;
		rgb.g = v;
		rgb.b = t;
		break;
		case 3:
		rgb.r = p;
		rgb.g = q;
		rgb.b = v;
		break;
		case 4:
		rgb.r = t;
		rgb.g = p;
		rgb.b = v;
		break;
		default:
		rgb.r = v;
		rgb.g = p;
		rgb.b = q;
		break;
	}

	return rgb;
}

/* Convert the RGB to HSV */
HsvColor
RgbColor::ToHSV()
{
	HsvColor hsv;
	unsigned char rgbMin, rgbMax;

	rgbMin = this->r < this->g ? (this->r < this->b ? this->r : this->b) : (this->g < this->b ? this->g : this->b);
	rgbMax = this->r > this->g ? (this->r > this->b ? this->r : this->b) : (this->g > this->b ? this->g : this->b);

	hsv.v = rgbMax;

	hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;

	if (rgbMax == this->r)
		hsv.h = 0 + 43 * (this->g - this->b) / (rgbMax - rgbMin);
	else if (rgbMax == this->g)
		hsv.h = 85 + 43 * (this->b - this->r) / (rgbMax - rgbMin);
	else
		hsv.h = 171 + 43 * (this->r - this->g) / (rgbMax - rgbMin);

	return hsv;
}