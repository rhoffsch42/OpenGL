#pragma once

#include <cstdlib>
#include <iostream>
#include <stdlib.h>

#define DEBUG_LEAF		false
#define DEBUG_LEAF_AREA	1
#define DEBUG_FILL_TOO	false
#define DEBUG_BUILD_TOO	true

class Pixel
{
public:
	Pixel() {
		r = 0;
		g = 0;
		b = 0;
	}
	Pixel(uint8_t red, uint8_t green, uint8_t blue) {
		r = red;
		g = green;
		b = blue;
	}
	uint8_t	r;
	uint8_t	g;
	uint8_t	b;
};

/*
_________
| A | B |
|---|---|
| C | D |
`````````
*/

/*
	result: https://docs.google.com/spreadsheets/d/17MNseuZ7234wSgKaGH4x13Qxni6oD9iySb00nfcebaU/edit?usp=sharing
	this can be improved by fusing areas on a second pass in the tree
*/

class QuadNode
{
public:
	QuadNode(Pixel** arr, int x, int y, int width, int height, unsigned int threshold);
	QuadNode(uint8_t* data, int baseWidth, int x, int y, int w, int h, unsigned int threshold);
	~QuadNode();
	bool		isLeaf() const;
	// template<class UnaryPredicate> void	browse(int threshold, UnaryPredicate p);

	QuadNode **		children;
	Pixel			pixel;
	double			detail;
	unsigned int	x;
	unsigned int	y;
	unsigned int	width;
	unsigned int	height;
	/*
		Obj3d *	cube;
		sera en fait un descriptif du cube et pas un ptr:
			texture
			is breakable
			...
	*/
private:
};