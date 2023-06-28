#pragma once
#include <stdio.h>

using namespace std;

#define WW 940 
#define HWW 470
#define WH 730
#define WL 345


class staticgrafelem //
{
protected:
	SDL_Rect coord;
	SDL_Texture* texture;
	bool flag; //������������� ���� �������, true - ��� ������ ���������
public:
	staticgrafelem(SDL_Rect c = { 0, 0, WW, WH }, string path = "fon.png", SDL_Renderer* render = NULL)
	{
		texture = IMG_LoadTexture(render, &path[0]);
		coord = c;
		flag = false;
		SDL_RenderCopy(render, texture, &coord, &coord);
	}
	~staticgrafelem()
	{
		SDL_DestroyTexture(texture);
	}
	staticgrafelem(const staticgrafelem& other)
	{
		flag = other.flag;
		coord = other.coord;
		texture = other.texture;
	}
	staticgrafelem& operator = (const staticgrafelem& other)
	{
		if (this == &other)
			return *this;
		coord = other.coord;
		flag = other.flag;
		SDL_DestroyTexture(texture);
		texture = other.texture;
	}
	bool render(SDL_Renderer* rend)
	{
		if (SDL_RenderCopy(rend, texture, NULL, &coord) < 0) return false;
		return true;
	}
};

class movavaibleelem //������� ����� "���������� ������������ �� ������ �������"
{
protected:
	SDL_Rect coord;
	SDL_Rect part;
	SDL_Texture* texture;
	int speed;
public:

	movavaibleelem(SDL_Rect c = { 0, 0, 100, 100 }, SDL_Texture* exem = NULL, int sp = 0)
	{
		coord = c;
		texture = exem;
		speed = sp;
		part = { 0, 0, 0, 0 };
	}
	virtual ~movavaibleelem() { SDL_DestroyTexture(texture); }
	movavaibleelem& operator = (const movavaibleelem& other)
	{
		if (this == &other)
			return *this;
		coord = other.coord;
		SDL_DestroyTexture(texture);
		texture = other.texture;
		speed = other.speed;
	}
	void set_speed(int sp)
	{
		speed = sp;
	}
	virtual void setY(int y) = 0;
	virtual int move(int ty, int tx, bool z) = 0;
	virtual bool moveU() = 0;
	virtual bool moveD() = 0;
	bool render(SDL_Renderer* rend)
	{
		if (SDL_RenderCopy(rend, texture, &part, &coord) < 0) return false;
		return true;
	}
};

class ship : public movavaibleelem
{
protected:
	//true - ������, false - �� ������
	bool hooked; 
	int points;

public:
	ship(SDL_Rect c = {0, 0, 180, 90}, SDL_Texture* exem = NULL, int sp = 0, int point = 0)
	{
		hooked = false;
		coord = c;
		texture = exem;
		speed = sp;
		points = point;

	}
	 ~ship()
	{
		SDL_DestroyTexture(texture);
	}
	void makehooked()
	{
		hooked = hooked ? false : true;
	}
	void setY(int y) override
	{
		coord.y = y;
	}
	int getpoint()
	{
		return hooked? points : 0;
	}
	bool moveU() override
	{
		if (!hooked) return true;
		coord.y -= speed;
		if (coord.y <= WL) return false;
		return true;
	}
	bool moveD() override
	{
		if (!hooked) return true;
		if (coord.y + coord.h + speed <= WH) coord.y += speed;
		return true;
	}
};

class Rship : public ship //�������, �������� �������
{
public:

	Rship(SDL_Rect c = { 0, 0, 100, 100 }, SDL_Texture * exem = NULL, int sp = 0, int point = 0)
	{
		hooked = false;
		coord = c;
		texture = exem;
		speed = sp;
		points = point;
		part = {180, 0, 180, 90}; // 180, 0 , 180 ,90
	}
	 ~Rship()
	{
		SDL_DestroyTexture(texture);
	}
	int move(int ty, int tx, bool z) override //1 - ������ ���������; 0 - ����� �� ����, ���� �������; -1 - ��������
	{
		if (coord.x + coord.w - HWW > 0 && coord.x - HWW < HWW + coord.w)
		{
			if (!hooked && 
				!z && 
				(ty <= WL + 30 && ty >= WL - coord.h) &&
				(tx + 70 <= coord.x + coord.w && tx >= coord.x))
			{
				
				hooked = true;
				return -1;
			}
		}
		if (!hooked) coord.x += speed;
		if (coord.x > WW ) return 0;
		return 1;
	}
};

class Lship : public ship // �������, �������� ������
{
public:

	Lship(SDL_Rect c = { 0, 0, 100, 100}, SDL_Texture* exem = NULL, int sp = 0, int point = 0)
	{
		hooked = false;
		coord = c;
		texture = exem;
		speed = sp;
		points = point;
		part = { 0, 0, 180, 90};
	}
	 ~Lship()
	{
		SDL_DestroyTexture(texture);
	}
	int move(int ty, int tx, bool z) override //1 - ������ ���������; 0 - ����� �� ����, ���� �������; -1 - ��������
	{
		if (coord.x - HWW < 0 && coord.x - HWW > -coord.w + 1) //���� ������� ����� � ������ ������
		{
			if (!hooked && // ���� ������� �� �����
				!z && // � ������� � ������� ��������������
				(ty <= WL + 30 && ty >= WL - coord.h) &&
				(tx + 70 <= coord.x + coord.w && tx >= coord.x)) // � 
			{
				hooked = true;
				return -1;
			}
		}
		if (!hooked) coord.x -= speed; // ���� �� �������
		if (coord.x + coord.w < 0 ) return 0; // ���� ����� �� ���� - �������
		return 1;
	}
};

class hook : public movavaibleelem
{
protected:
	bool zacep;
	bool launched;
public:
	hook(SDL_Texture* exem = NULL, int sp = 40)
	{
		zacep = false;
		coord = { WW/2 - 40, WH-225, 70, 70 }; //41 57
		texture = exem; // ������
		speed = sp;
		part = { 0, 0, 512, 512 }; // ����� �������� ��� ������
		launched = false;
	}
	void DEBUFINFO()
	{
		cout << zacep << "/n" << coord.x << " " << coord.y << "/n" << coord.w << " " << coord.h << endl;
		cout << texture << "/n" << endl;
	}
	hook(const hook& other) // ����������� �����������
	{
		zacep = other.zacep;
		coord = other.coord;
		texture = other.texture;
		speed = other.speed;
		part = other.part;
	}
	 ~hook() // ����������
	{
		SDL_DestroyTexture(texture);
	}

	void setzacep(bool flag)
	{
		zacep = flag;
	}

	bool getlaunched()
	{
		return launched;
	}

	void setlaunched(bool flag)
	{
		launched = flag;
	}

	void setcoords(int x, int y)
	{
		coord.x = x;
		coord.y = y;
	}
	bool moveU() override
	{
		if (coord.y + coord.h <= 0)
			return true;
			
		launched = true;
		if (zacep) // ���� ����� 
		{
			zacep = false;
			return true;
		}
		else
		{
			coord.y -= speed;
		}
		
		return false;
	}
	bool moveD() override
	{
		if (coord.y + coord.h < WH)
		{
			coord.y += speed;
			return true;
		}
		return false;
	}
	void setY(int y) override
	{
		coord.y = y;
	}
	int move(int ty, int tx, bool z) override
	{
		return 1;
	}
	int getY()
	{
		return coord.y;
	}
	int getX()
	{
		return coord.x;
	}
	bool getzac()
	{
		return zacep;
	}
};