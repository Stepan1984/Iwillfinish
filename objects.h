#pragma once
#include <stdio.h>

using namespace std;

#define WW 940 
#define HWW 470
#define WH 730
#define WL 120


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
	virtual int move(int hy, bool z) = 0;
	virtual bool moveU() = 0;
	virtual bool moveD() = 0;
	bool render(SDL_Renderer* rend)
	{
		if (SDL_RenderCopy(rend, texture, &part, &coord) < 0) return false;
		return true;
	}
};

class fish : public movavaibleelem
{
protected:
	//true - ������, false - �� ������
	bool destroyed; 
	int points;
	int rav;
public:
	fish(SDL_Rect c = {0, 0, 100, 100}, SDL_Texture* exem = NULL, int sp = 0, int point = 0)
	{
		destroyed = false;
		coord = c;
		texture = exem;
		speed = sp;
		points = point;
		rav = 0;
	}
	 ~fish()
	{
		SDL_DestroyTexture(texture);
	}
	void makehooked()
	{
		destroyed = destroyed ? false : true;
	}
	void setY(int y) override
	{
		coord.y = y - rav;
	}
	int getpoint()
	{
		return destroyed? points : 0;
	}
	bool moveU() override
	{
		if (!destroyed) return true;
		coord.y -= speed;
		if (coord.y <= WL) return false;
		return true;
	}
	bool moveD() override
	{
		if (!destroyed) return true;
		if (coord.y + coord.h + speed <= WH) coord.y += speed;
		return true;
	}
};

class Rfish : public fish
{
public:

	Rfish(SDL_Rect c = { 0, 0, 100, 100 }, SDL_Texture * exem = NULL, int sp = 0, int point = 0)
	{
		destroyed = false;
		coord = c;
		texture = exem;
		speed = sp;
		points = point;
		part = {180, 0, 180, 90};
	}
	 ~Rfish()
	{
		SDL_DestroyTexture(texture);
	}
	int move(int hy, bool z) override //1 - ������ ���������; 0 - ����� �� ����, ���� �������; -1 - �������
	{
		if (coord.x + coord.w - HWW > 0 && coord.x + coord.w - HWW < 40)
		{
			if (destroyed)
				this->setY(hy);
			if (!destroyed && !z && fabs(hy - (coord.y + coord.h / 2)) < 30)
			{
				rav = hy - coord.y;
				destroyed = true;
				return -1;
			}
		}
		if (!destroyed) coord.x += speed;
		if (coord.x > WW || coord.y <= WL) return 0;
		return 1;
	}
};

class Lfish : public fish
{
public:

	Lfish(SDL_Rect c = { 0, 0, 100, 100 }, SDL_Texture* exem = NULL, int sp = 0, int point = 0)
	{
		destroyed = false;
		coord = c;
		texture = exem;
		speed = sp;
		points = point;
		part = { 0, 0, 180, 90 };
	}
	 ~Lfish()
	{
		SDL_DestroyTexture(texture);
	}
	int move(int hy, bool z) override //1 - ������ ���������; 0 - ����� �� ����, ���� �������; -1 - �������
	{
		if (coord.x - HWW < 0 && coord.x - HWW > -40)
		{
			if (destroyed)
				this->setY(hy);
			if (!destroyed && !z && fabs(hy - (coord.y + coord.h / 2)) < 30)
			{
				rav = hy - coord.y;
				destroyed = true;
				return -1;
			}
		}
		if (!destroyed) coord.x -= speed;
		if (coord.x + coord.w < 0 || coord.y <= WL) return 0;
		return 1;
	}
};

class hook : public movavaibleelem
{
protected:
	bool zacep;
public:
	hook(SDL_Texture* exem = NULL, int sp = 10)
	{
		zacep = false;
		coord = { HWW - 24, WL, 41, 57 };
		texture = exem;
		speed = sp;
		part = { 0, 0, 40, 57 };
	}
	void DEBUFINFO()
	{
		cout << zacep << "/n" << coord.x << " " << coord.y << "/n" << coord.w << " " << coord.h << endl;
		cout << texture << "/n" << endl;
	}
	hook(const hook& other)
	{
		zacep = other.zacep;
		coord = other.coord;
		texture = other.texture;
		speed = other.speed;
		part = other.part;
	}
	 ~hook()
	{
		SDL_DestroyTexture(texture);
	}
	bool setzacep()
	{
		if (zacep) return false;
		part = { 40, 0, 40, 57 };
		speed = 5;
		zacep = true;
		return true;
	}
	bool moveU() override
	{
		if (coord.y + coord.h + 42 > WL)
			coord.y -= speed;
		if (zacep && coord.y + coord.h / 2 < WL) {
			part = { 0, 0, 40, 57 };
			zacep = false;
			speed = 6;
			return true;
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
		return;
	}
	int move(int y, bool z) override
	{
		return 1;
	}
	int getY()
	{
		return coord.y;
	}
	bool getzac()
	{
		return zacep;
	}
};

class dynamite : public fish
{
protected:
	bool* flag;
public:
	dynamite(SDL_Rect c = { 0, 0, 100, 100 }, SDL_Texture* exem = NULL, int sp = 0, bool* fl = NULL)
	{
		destroyed = false;
		coord = c;
		coord.x = 0;
		texture = exem;
		speed = sp;
		rav = 0;
		points = 0;
		flag = fl;
		part = { 0, 0, c.w, c.h };
	}
	 ~dynamite()
	{
		flag = NULL;
		SDL_DestroyTexture(texture);
	}
	int move(int hy, bool z) override //1 - ������ ���������; 0 - ����� �� ����, ���� �������; -1 - �������
	{
		if (coord.x > WW) return 0;
		if (destroyed)
		{
			if (coord.y <= WL) { *flag = true; return 0; }
			this->setY(hy);
		}
		else
		{
			if (!z && coord.x + coord.w - HWW > 0 && coord.x + coord.w - HWW < 40 && fabs(hy - (coord.y + coord.h / 2)) < 30)
			{
				rav = hy - coord.y;
				destroyed = true;
				return -1;
			}
			coord.x += speed;
		}
		return 1;
	}
};

class ship : public movavaibleelem
{
protected:
	//true - ���������, false - �� ���������
	bool destroyed;
	int points;
	int rav;
public:
	fish(SDL_Rect c = { 0, 0, 100, 100 }, SDL_Texture* exem = NULL, int sp = 0, int point = 0)
	{
		destroyed = false;
		coord = c;
		texture = exem;
		speed = sp;
		points = point;
		rav = 0;
	}
	~fish()
	{
		SDL_DestroyTexture(texture);
	}
	void makehooked()
	{
		destroyed = destroyed ? false : true;
	}
	void setY(int y) override
	{
		coord.y = y - rav;
	}
	int getpoint()
	{
		return destroyed ? points : 0;
	}
	bool moveU() override
	{
		if (!destroyed) return true;
		coord.y -= speed;
		if (coord.y <= WL) return false;
		return true;
	}
	bool moveD() override
	{
		if (!destroyed) return true;
		if (coord.y + coord.h + speed <= WH) coord.y += speed;
		return true;
	}
};