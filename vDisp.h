#pragma once
#include <stdlib.h>
#include <SDL.h>
#include <map>

struct Color
{
	Uint8 r, g, b, a;
	bool empty;
	Color() 
		:empty(true),r(0),g(0),b(0),a(0)
	{}
	Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
		:r(r), g(g), b(b), a(a), empty(false)
	{}
};

struct aLine {
	SDL_Point startPoint;
	SDL_Point endPoint;
	Color color;
	bool alreadyExisted;
	aLine(SDL_Point startPoint, SDL_Point endPoint, Color color) : startPoint(startPoint), endPoint(endPoint), color(color), alreadyExisted(true) {};
	aLine() :color({ 0,0,0,255 }), alreadyExisted(false), startPoint({ 0,0 }), endPoint({ 0,0 }) {};
};

class vDisp
{
private:
	uint16_t windowWidth, windowHeight;
	uint16_t worldScale;
	SDL_Rect viewport;
	SDL_Renderer* renderer;
	SDL_Rect pixel;
	std::map<int,aLine> linesArray;
	std::map<int,std::map<int, Color>> pixelArray;
	Color emptyColor;
	const Uint8* keyboardStateArray;

public:
	vDisp(SDL_Renderer* renderer, const uint16_t& worldScale, const uint16_t& windowWidth, const uint16_t& windowHeight);
	void Zoom(const float& zoomDir, const int& mouseX, const int& mouseY);
	void Move(const int& x, const int& y);
	SDL_Point screenCordsToWorldCords(const int& x, const int& y);
	SDL_Point worldCordsToScreenCords(const int& x, const int& y);
	void DrawGrid();
	void Render();
	void DrawPixel(const int& x, const int& y, const Color& color);
	void DrawLine(const aLine& line);
	void EventHandler(SDL_Event& event);
};

