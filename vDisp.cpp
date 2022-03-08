#include "vDisp.h"

#include "utility.cpp"

#include <iostream>
#include <cmath>


vDisp::vDisp(SDL_Renderer* renderer, const uint16_t& worldScale, const uint16_t& windowWidth, const uint16_t& windowHeight) 
	:renderer(renderer), worldScale(worldScale), windowWidth(windowWidth), windowHeight(windowHeight)
{
	viewport = { 0, 0, windowWidth / worldScale,  windowHeight / worldScale};
	pixel = { 1, 1, worldScale, worldScale };
	keyboardStateArray = SDL_GetKeyboardState(NULL);
}


inline SDL_Point vDisp::screenCordsToWorldCords(const int& x, const int& y)
{
	int tempx, tempy;
	tempx = (x + viewport.x)/worldScale;
	tempy = (y + viewport.y)/worldScale;
	if(signbit((float)tempx)) tempx--;
	if (signbit((float)tempy)) tempy--;
	return { tempx, tempy };
}

inline SDL_Point vDisp::worldCordsToScreenCords(const int& x, const int& y)
{
	SDL_Point screenCords;
	screenCords.x = x*worldScale - viewport.x;
	screenCords.y = y*worldScale - viewport.y;
	return screenCords;
}

void vDisp::DrawGrid() 
{
	int offsetX = -MOD(viewport.x, worldScale);
	int offsetY = -MOD(viewport.y, worldScale);
	int linesX = (windowWidth - offsetX) / worldScale;
	int linesY = (windowHeight - offsetY) / worldScale ;
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	for (int i = 0; i <= linesX; i++)
		SDL_RenderDrawLine(renderer, i * worldScale + offsetX, 0, i * worldScale + offsetX, windowHeight);
	for (int i = 0; i <= linesY; i++)
		SDL_RenderDrawLine(renderer, 0, i*worldScale + offsetY, windowWidth, i*worldScale + offsetY);
}

inline void vDisp::DrawPixel(const int& x, const int& y, const Color& color)
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_Point pos = worldCordsToScreenCords(x, y);
	pixel.x = pos.x +1;
	pixel.y = pos.y+1;
	SDL_RenderFillRect(renderer, &pixel);
}

void vDisp::DrawLine(const aLine& line)
{
	SDL_Point startPoint = line.startPoint, endPoint = line.endPoint;
	bool qd = true;
	float dx = endPoint.x - startPoint.x, dy = endPoint.y - startPoint.y;
	
	if (std::fabs(dx) > std::fabs(dy)) {
		int x_step = signbit(dx) ? -1 : 1;
		int y_step = signbit(dy) ? -1 : 1;
		dx = dx * x_step;
		dy = dy * y_step;
		float d = dy - (dx * 0.5f);
		int x = startPoint.x, y = startPoint.y;
		DrawPixel(x, y, line.color);
		while (x != endPoint.x)
		{
			x = x + x_step;
			if (d < 0)
				d = d + dy;
			else {
				d = d + dy - dx;
				y = y + y_step;
			}
			DrawPixel(x, y, line.color);
		}
	}
	else {
		int x_step = signbit(dx) ? -1 : 1;
		int y_step = signbit(dy) ? -1 : 1;
		dx = dx * x_step;
		dy = dy * y_step;
		float d = dx - (dy / 2.f);
		int x = startPoint.x, y = startPoint.y;
		DrawPixel(x, y, line.color);
		while (y != endPoint.y)
		{
			y = y + y_step;
			if (d < 0)
				d = d + dx;
			else {
				d = d + dx - dy;
				x = x + x_step;
			}
			DrawPixel(x, y, line.color);
		}
	}



}

void vDisp::Move(const int& x, const int& y)
{
	viewport.x += x * ceil(worldScale / 5.f);
	viewport.y += y * ceil(worldScale / 5.f);
}

void vDisp::Zoom(const float& zoomDir, const int& mouseX, const int& mouseY)
{
	SDL_Point worldPos = screenCordsToWorldCords(mouseX, mouseY);
	viewport.x -= worldPos.x*worldScale;
	viewport.y -= worldPos.y*worldScale;
	int oldScale = worldScale;
	worldScale +=  zoomDir*CLAMP(std::floor(worldScale/10.f), 1, 50);
	worldScale = CLAMP(worldScale, 1, MIN(windowHeight/2, windowWidth/2));
	viewport.w = windowWidth/worldScale;
	viewport.h = windowHeight/worldScale;
	viewport.x += worldPos.x * worldScale;
	viewport.y += worldPos.y * worldScale;
	//new pos of vPixel
	
	pixel.w = worldScale; 
	pixel.h = worldScale;
}

void vDisp::Render() 
{
	for (auto& x : pixelArray)
		for (auto& y : x.second)
			DrawLine(aLine({ 0,0 }, { x.first,y.first }, y.second));
			//DrawPixel(x.first, y.first, y.second);
	//DrawGrid();
}

void vDisp::EventHandler(SDL_Event& event)
{	
	int xIn = 0, yIn = 0;
	switch (event.type)
	{
	case SDL_KEYDOWN:
		if (keyboardStateArray[SDL_SCANCODE_W])
			yIn--;
		if (keyboardStateArray[SDL_SCANCODE_S])
			yIn++;
		if (keyboardStateArray[SDL_SCANCODE_A])
			xIn--;
		if (keyboardStateArray[SDL_SCANCODE_D])
			xIn++;
		Move(xIn, yIn);

		if (event.key.keysym.sym == SDLK_LCTRL)
			pixelArray.clear();
		
		break;
	case SDL_MOUSEBUTTONDOWN:
		//std::cout << "int event\n";
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);
			SDL_Point worldCord = screenCordsToWorldCords(mouseX, mouseY);
			if (pixelArray[worldCord.x][worldCord.y].empty)
				pixelArray[worldCord.x][worldCord.y] = { 255,0,255,255 };
			else
				pixelArray[worldCord.x].erase(worldCord.y);
		}
		break;
	}
}