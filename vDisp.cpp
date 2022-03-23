#include "vDisp.h"

#include "utility.cpp"

#include <iostream>
#include <cmath>
#include <string>
#include <limits>

#include "imgui.h"




vDisp::vDisp(SDL_Renderer* renderer, const uint16_t& worldScale, const uint16_t& windowWidth, const uint16_t& windowHeight) 
	:renderer(renderer), worldScale(worldScale), windowWidth(windowWidth), windowHeight(windowHeight)
{
	viewport = { 0, 0, windowWidth / worldScale,  windowHeight / worldScale};
	pixel = { 1, 1, worldScale, worldScale };
	keyboardStateArray = SDL_GetKeyboardState(NULL);
}

vDisp::vDisp()
{
	renderer = nullptr;
	keyboardStateArray = nullptr;
	editMode = line;
}

void vDisp::init(SDL_Renderer* render, const uint16_t& worldscale, const uint16_t& windowwidth, const uint16_t& windowheight)
{
	showGrid = false;
	renderer = render;
	worldScale = worldscale;
	windowWidth = windowwidth;
	windowHeight = windowheight;
	viewport = { 0, 0, windowWidth / worldScale,  windowHeight / worldScale };
	pixel = { 1, 1, worldScale, worldScale };
	keyboardStateArray = SDL_GetKeyboardState(NULL);
}

inline SDL_Point vDisp::screenCordsToWorldCords(const int& x, const int& y)
{
	int tempx, tempy;
	tempx = SDL_floor((x + viewport.x)/(float)worldScale);
	tempy = SDL_floor((y + viewport.y)/(float)worldScale);
	//if (tempx < 0) tempx--;
	//if (tempy < 0) tempy--;
	/*std::cout << "Mouse: " << x << " " << y << std::endl;
	std::cout << "World: " << tempx << " " << tempy << std::endl;*/
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
	pixel.x = pos.x;
	pixel.y = pos.y;
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

void vDisp::DrawPolygon(const aPolygon& polygon)
{
	aPolygon temp(polygon);
	temp.scale();
	temp.rotate();
	int end = recordingPolygon ? temp.noOfPoints - 1 : temp.noOfPoints;

	for (int i = 0; i < end; i++)
	{
		DrawLine(aLine(temp.vertices[MOD(i, temp.noOfPoints)], temp.vertices[MOD(i+1, temp.noOfPoints)], { 255,0,255,255 }));
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
	for (auto& line : linesArray)
		DrawLine(line.second);
	for (auto& polygon : polygonArray)
		DrawPolygon(polygon.second);
	if (showGrid)
		DrawGrid();
}

void vDisp::imgui()
{
	ImGui::Begin("Editor");
	if (ImGui::Button("Line Edit"))
		editMode = line;
	ImGui::SameLine();
	if (ImGui::Button("Polygon Edit"))
		editMode = polygon;
	std::string message;
	switch (editMode)
	{
	case(line):
		if (ImGui::Button("Add Line"))
			linesArraySize++;
		if (linesArraySize > linesArray.size())
			for (int i = 0; i < linesArraySize; i++)
				if (!linesArray[i].alreadyExisted && linesArray.size() == linesArraySize)
					break;
		for (auto& i : linesArray)
		{
			message = "[" + std::to_string(i.first) + "]";
			ImGui::Text(message.c_str());
			ImGui::SameLine();
			if (isRecording && currEditing == i.first)
			{
				message = "Editing..##" + std::to_string(i.first);
				if (ImGui::Button(message.c_str()))
				{
					isRecording = false;
				}
				
			}
			else
			{
				message = "Edit##" + std::to_string(i.first);
				if (ImGui::Button(message.c_str()))
				{
					isRecording = true;
					firstPoint = true;
					currEditing = i.first;
				}
				ImGui::SameLine();
			}

			message = "Delete##" + std::to_string(i.first);
			if (ImGui::Button(message.c_str()))
			{
				isRecording = false;
				linesArray.erase(i.first);
				linesArraySize--;
			}
		}
		break;
	case(polygon):
		if (ImGui::Button("Add Polygon"))
			polygonArraySize++;
		if (polygonArraySize > polygonArray.size())
			for (int i = 0; i < polygonArraySize; i++)
				if (!polygonArray[i].alreadyExisted && polygonArray.size() == polygonArraySize)
					break;
		for (auto& i : polygonArray)
		{
			message = "[" + std::to_string(i.first) + "]";
			ImGui::Text(message.c_str());
			ImGui::SameLine();
			message = "Edit##" + std::to_string(i.first);
			if (ImGui::Button(message.c_str()))
			{
				currEditing = i.first;
				editingPolygon = true;
			}

		}
		break;
	default:
		break;
	}
	ImGui::End();
	if (editingPolygon)
	{
		message = "Editing polygon #" + std::to_string(currEditing);
		ImGui::Begin(message.c_str());
		message = recordingPolygon ? "Listening..." : "Record vertices";
		if (ImGui::Button(message.c_str()))
		{
			recordingPolygon = true;
			polygonArray[currEditing].vertices.clear();
			polygonArray[currEditing].noOfPoints = 1;
		}
		float temp[2], temp1, temp2[2];
		SDL_Point curr = polygonArray[currEditing].transform.location;
		ImGui::Text("Location:");
		ImGui::SameLine();
		temp[0] = curr.x;
		temp[1] = curr.y;
		//ImGui::InputFloat2("", temp);
		ImGui::DragFloat2("", temp,1,1,1,"%.f",1);
		if (curr.x != temp[0] || curr.y != temp[1])
		{
			polygonArray[currEditing].translate(temp[0] - curr.x, temp[1] - curr.y);
			polygonArray[currEditing].UpdateOrigin();
		}
		polygonArray[currEditing].transform.location.x = temp[0];
		polygonArray[currEditing].transform.location.y = temp[1];
		ImGui::Text("Rotation:");
		ImGui::SameLine();
		temp1 = polygonArray[currEditing].transform.rotation.x;
		ImGui::DragFloat("##1", &temp1, 1, 1, 1, "% .f", 1);
		//if (temp1 != polygonArray[currEditing].transform.rotation.x)
		//	polygonArray[currEditing].rotate(polygonArray[currEditing].transform.rotation.x - temp1);
		polygonArray[currEditing].transform.rotation.x = temp1;
		
		ImGui::Text("Scale");
		temp2[0] = polygonArray[currEditing].transform.scale.x;
		temp2[1] = polygonArray[currEditing].transform.scale.y;
		ImGui::DragFloat2("##2", temp2);
		polygonArray[currEditing].transform.scale = { (int)temp2[0], (int)temp2[1] };
		if (ImGui::Button("Close"))
		{
			editingPolygon = false;
		}
		ImGui::End();
	}
}

void vDisp::EventHandler(SDL_Event& event)
{	
	int xIn = 0, yIn = 0;
	int mouseX, mouseY;
	SDL_Point worldPos;
	SDL_GetMouseState(&mouseX, &mouseY);
	worldPos = screenCordsToWorldCords(mouseX, mouseY);
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
		
		if (event.key.keysym.sym == SDLK_g)
			showGrid = !showGrid;


		break;
	case SDL_MOUSEBUTTONDOWN:
		//std::cout << "int event\n";
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			if (isRecording)
			{
				if (firstPoint)
				{
					linesArray[currEditing].startPoint = { worldPos.x, worldPos.y };
					linesArray[currEditing].color = { 255,0,255,255 };
					firstPoint = false;
				}
				else
				{
					linesArray[currEditing].endPoint = { worldPos.x, worldPos.y };
					isRecording = false;
				}
			}
		}
		

		break;
	}
	if (!firstPoint && isRecording)
	{
		linesArray[currEditing].endPoint = { worldPos.x, worldPos.y };
	}
	if (recordingPolygon)
	{
		

	}


}
