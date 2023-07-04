#pragma once
using namespace std;

#include"constants.h"


class staticGraficalElement //
{
protected:
	SDL_Rect coordinates;
	SDL_Texture* texture;
	bool flag;
public:
	staticGraficalElement(SDL_Rect newCoords = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, string path = "images/background.png", SDL_Renderer* render = NULL)
	{
		texture = IMG_LoadTexture(render, &path[0]);
		coordinates = newCoords;
		flag = false;
		SDL_RenderCopy(render, texture, &coordinates, &coordinates);
	}
	~staticGraficalElement()
	{
		SDL_DestroyTexture(texture);
	}
	staticGraficalElement(const staticGraficalElement& other)
	{
		flag = other.flag;
		coordinates = other.coordinates;
		texture = other.texture;
	}
	staticGraficalElement& operator = (const staticGraficalElement& other)
	{
		if (this == &other)
			return *this;
		coordinates = other.coordinates;
		flag = other.flag;
		SDL_DestroyTexture(texture);
		texture = other.texture;
	}
	bool render(SDL_Renderer* rend)
	{
		if (SDL_RenderCopy(rend, texture, NULL, &coordinates) < 0) return false;
		return true;
	}
};

class dynamicElement 
{
protected:
	SDL_Rect coordinates;
	SDL_Rect part;
	SDL_Texture* texture;
	int speed;
public:

	dynamicElement(SDL_Rect newCoords = { 0, 0, 100, 100 }, SDL_Texture* newTexture = NULL, int newSpeed= 0)
	{
		coordinates = newCoords;
		texture = newTexture;
		speed = newSpeed;
		part = { 0, 0, 0, 0 };
	}
	virtual ~dynamicElement() { SDL_DestroyTexture(texture); }
	dynamicElement& operator = (const dynamicElement& other)
	{
		if (this == &other)
			return *this;
		coordinates = other.coordinates;
		SDL_DestroyTexture(texture);
		texture = other.texture;
		speed = other.speed;
	}
	void SetSpeed(int newSpeed)
	{
		speed = newSpeed;
	}
	int GetSpeed()
	{
		return speed;
	}
	void SetY(int y)
	{
		coordinates.y = y;
	}
	void SetX(int x)
	{
		coordinates.x = x;
	}
	int GetY()
	{
		return coordinates.y;
	}
	int GetX()
	{
		return coordinates.x;
	}
	int GetW()
	{
		return coordinates.w;
	}
	int GetH()
	{
		return coordinates.h;
	}
	
	bool render(SDL_Renderer* rend)
	{
		if (SDL_RenderCopy(rend, texture, &part, &coordinates) < 0) return false;
		return true;
	}
};

class ship : public dynamicElement
{
protected:
	bool touched; 
	int points;

public:
	ship(SDL_Rect c = {0, 0, 180, 90}, SDL_Texture* newTexture = NULL, int newSpeed = 0, int newPoints = 0)
	{
		touched = false;
		coordinates = c;
		texture = newTexture;
		speed = newSpeed;
		points = newPoints;

	}
	 ~ship()
	{
		SDL_DestroyTexture(texture);
	}
	void MakeTouched()
	{
		touched = touched ? false : true;
	}
	int GetPoint()
	{
		return touched? points : 0;
	}
	virtual int Move(int tx, int ty, int tw, int th, int ts, bool z) = 0;
};

class Rship : public ship //корабль, плывущий направо
{
public:

	Rship(SDL_Rect c = { 0, 0, 100, 100 }, SDL_Texture * newTexture = NULL, int newSpeed = 0, int newPoints = 0)
	{
		touched = false;
		coordinates = c;
		texture = newTexture;
		speed = newSpeed;
		points = newPoints;
		part = {180, 0, 180, 90}; 
	}
	 ~Rship()
	{
		SDL_DestroyTexture(texture);
	}
	int Move(int tx, int ty, int tw, int th, int ts, bool z) override 
	{
			if (!touched && 
				!z && 
				(ty <= WATER_LINE + 35 && ty >= WATER_LINE - coordinates.h) &&
				(tx + tw <= coordinates.x + coordinates.w && tx >= coordinates.x))
			{
				
				touched = true;
				return -1;
			}
		if (!touched) coordinates.x += speed;
		if (coordinates.x > WINDOW_WIDTH ) return 0;
		return 1;
	}
};

class Lship : public ship // корабль, плывущий налево
{
public:

	Lship(SDL_Rect c = { 0, 0, 100, 100}, SDL_Texture* newTexture = NULL, int newSpeed = 0, int newPoints = 0)
	{
		touched = false;
		coordinates = c;
		texture = newTexture;
		speed = newSpeed;
		points = newPoints;
		part = { 0, 0, 180, 90};
	}
	 ~Lship()
	{
		SDL_DestroyTexture(texture);
	}
	int Move(int tx, int ty, int tw, int th, int ts,  bool z) 
	{
			if (!touched && 
				!z && 
				(ty <= WATER_LINE + 30 && ty >= WATER_LINE - coordinates.h) &&
				(tx + tw <= coordinates.x + coordinates.w && tx >= coordinates.x))  
			{
				touched = true;
				return -1;
			}
		if (!touched) coordinates.x -= speed; 
		if (coordinates.x + coordinates.w < 0 ) return 0; 
		return 1;
	}
};

class rocket : public dynamicElement
{
protected:
	bool collision;
	bool launched;
public:
	rocket(SDL_Texture* newTexture = NULL, int newSpeed = 40)
	{
		collision = false;
		coordinates = { WINDOW_WIDTH/2 - 40, WINDOW_HEIGHT-225, 70, 70 }; 
		texture = newTexture; // спрайт
		speed = newSpeed;
		part = { 0, 0, 512, 512 }; // часть картикни под рендер
		launched = false;
	}
	rocket(const rocket& other) // конструктор копирования
	{
		collision = other.collision;
		coordinates = other.coordinates;
		texture = other.texture;
		speed = other.speed;
		part = other.part;
	}
	 ~rocket() // деструктор
	{
		SDL_DestroyTexture(texture);
	}

	void SetCollision(bool flag)
	{
		collision = flag;
	}

	bool GetLaunched()
	{
		return launched;
	}

	void SetLaunched(bool flag)
	{
		launched = flag;
	}

	bool MoveU()
	{
		if (coordinates.y + coordinates.h <= 0)
			return true;
			
		launched = true;
		if (collision) // если соприкосновение 
		{
			collision = false;
			return true;
		}
		else
		{
			coordinates.y -= speed;
		}
		
		return false;
	}

	bool GetCollision()
	{
		return collision;
	}

	
};

class submarine: public dynamicElement
{
public:
	submarine(SDL_Rect c = { 0, 0, 180, 90 }, SDL_Texture* exem = NULL, int sp = 0)
	{
		coordinates = c;
		texture = exem;
		speed = sp;
		part = { 0,0,683, 1294 };
	}
	~submarine()
	{
		SDL_DestroyTexture(texture);
	}

	void MoveR()
	{
		if (WINDOW_WIDTH - coordinates.x + coordinates.w >= speed)
			coordinates.x += speed;
	}

	void MoveL()
	{
		if ( coordinates.x >= speed)
			coordinates.x -= speed;
	}
};