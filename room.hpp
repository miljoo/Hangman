#include <SFML/Graphics.hpp>

#ifndef ROOM_HPP
#define ROOM_HPP

extern int SCREEN_W;
extern int SCREEN_H;

struct V2f{
	float x;
	float y;

	V2f(){
		x = 0;
		y = 0;
	}

	V2f(float _x,float _y){
		x = _x;
		y = _y;
	}
};

float hypotenuse(float x1, float y1, float x2, float y2);

sf::Vector2f relPosition(float relx, float rely, float offsetx, float offsety);

float centerX(float size);

float centerY(float size);

void drawRoom();

#endif
