#pragma once
#include <stdlib.h>
#include <SDL.h>
#include <map>
#include <vector>
#include <iostream>

#define PI 3.14159265

enum EditMode
{
	line = 0,
	polygon,
};


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

struct Transform
{
	SDL_Point location;
	SDL_Point rotation;
	SDL_Point scale;
	
	Transform()
		:location({ 0,0 }),rotation({ 0,0 }), scale({ 1,1 }) {}
};

struct aLine {
	SDL_Point startPoint;
	SDL_Point endPoint;
	Color color;
	bool alreadyExisted;
	Transform transform;
	aLine(SDL_Point startPoint, SDL_Point endPoint, Color color) : startPoint(startPoint), endPoint(endPoint), color(color), alreadyExisted(true) {};
	aLine() :color({ 0,0,0,255 }), alreadyExisted(false), startPoint({ 0,0 }), endPoint({ 0,0 }) {};
};

struct aPolygon {
	int noOfPoints;
	std::vector<SDL_Point> vertices;
	SDL_Color fillColor;
	bool fill;
	bool borders;
	bool alreadyExisted;
	Transform transform;
	SDL_Point origin;
	aPolygon(int n, std::vector<SDL_Point> points, SDL_Color color, bool fill) : noOfPoints(n), vertices(points), fillColor(color), fill(fill), alreadyExisted(true), borders(false) {
		UpdateOrigin();
	}
	aPolygon() :noOfPoints(3), vertices({ {0,0},{10,0},{0,10} }), fillColor({ 0,0,0,255 }), fill(false), alreadyExisted(false), borders(false) 
	{
		UpdateOrigin();
	}
	void UpdateOrigin()
	{
		float sumX = 0, sumY = 0;
		for (auto& v : vertices)
			sumX += v.x, sumY += v.y;
		origin.x = sumX / noOfPoints, origin.y = sumY/noOfPoints;
	}
	void translate(const float& x, const float& y)
	{
		for (auto& v : vertices)
			v.x += x, v.y += y;
	}
	void rotate()
	{
		translate(-origin.x, -origin.y);
		double theta = (double)transform.rotation.x * PI / 180.f;
		SDL_Point temp;
		for (auto& v : vertices)
		{
			temp.x = ceil(((double)v.x * cos(theta)) - ((double)v.y * sin(theta)));
			temp.y = ceil(((double)v.x * sin(theta)) + ((double)v.y * cos(theta)));
			v = temp;
		}
		
		translate(origin.x, origin.y);
	}
	void scale()
	{
		translate(-origin.x, -origin.y);
		for (auto& v : vertices)
			v.x = v.x * transform.scale.x, v.y = v.y * transform.scale.y;
		translate(origin.x, origin.y);
	}
};

class vDisp
{
private:
	uint16_t windowWidth=0, windowHeight=0;
	uint16_t worldScale=0;
	SDL_Rect viewport={0,0,0,0};
	SDL_Renderer* renderer=nullptr;
	SDL_Rect pixel={0,0,0,0};
	int linesArraySize = 0, polygonArraySize = 0;
	Color emptyColor = { 0,0,0,0 };
	const Uint8* keyboardStateArray;

	bool showGrid = false;

	std::map<int,std::map<int, Color>> pixelArray;
	std::map<int,aLine> linesArray;
	std::map<int, aPolygon> polygonArray;

	uint16_t editMode;

	bool isRecording = false, firstPoint = true, editingPolygon = false, recordingPolygon = false;
	int currEditing;

public:
	vDisp(SDL_Renderer* renderer, const uint16_t& worldScale, const uint16_t& windowWidth, const uint16_t& windowHeight);
	vDisp();
	void init(SDL_Renderer* render, const uint16_t& worldscale, const uint16_t& windowwidth, const uint16_t& windowheight);
	void Zoom(const float& zoomDir, const int& mouseX, const int& mouseY);
	void Move(const int& x, const int& y);
	SDL_Point screenCordsToWorldCords(const int& x, const int& y);
	SDL_Point worldCordsToScreenCords(const int& x, const int& y);
	void DrawGrid();
	void Render();
	void DrawPixel(const int& x, const int& y, const Color& color);
	void DrawLine(const aLine& line);
	void DrawPolygon(const aPolygon& polygon);
	void EventHandler(SDL_Event& event);
	void imgui();
};
