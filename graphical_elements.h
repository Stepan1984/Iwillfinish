#pragma once
#include <SDL.h>
#include <string>
#include "globals.h"
#include "dynamic_element.h" // заголовочный файл с определением абстрактного базового класса движущегося элемента (на самом деле просто интерфейс и одно поле) хи-хи
class static_graphical_element //статический графический элемент (абстрактный базовый класс)
{
public:
	virtual void SetX(const int) = 0; // установить позицию по X на рендере
	virtual void SetY(const int) = 0; // установить позицию по Y на рендере
	virtual void SetSize(const int, const int) = 0; // установить размер спрайта на рендере (width, heigh)
	virtual void SetTexture(SDL_Texture *) = 0; // установить картинку (SDL_Texture pointer)
	virtual void SetFlip(SDL_RendererFlip newFlip) = 0; // установить flip
	virtual void LoadTexture(std::string path, SDL_Renderer* gRenderer) = 0; // загрузка текстуры из файла
	virtual int GetLeftX() = 0; // получить X левой границы картинки на рендере
	virtual int GetRightX() = 0; // получить X правой границы картинки на рендере
	virtual int GetUpperY() = 0; // получить Y верхней границы картинки на рендере
	virtual int GetLowerY() = 0; // получить Y нижней границы картинки на рендере
	virtual int GetWidth() = 0;
	virtual int GetHeight() =0;
	virtual SDL_Texture* GetTexture() = 0; // получить картинку
	virtual SDL_RendererFlip GetFlip() = 0; // получить flip 
	virtual void Render(SDL_Renderer * gRenderer) = 0; // отобразить на рендере
	virtual void FreeTexture() = 0; // сброс указателя на текстуру
	
};

class rectangle : static_graphical_element // класс "прямоугольник"
{
protected:
	SDL_Rect rect; // размеры и позиция на рендере
	SDL_Texture* sprite; // картинка
	SDL_RendererFlip flip;
public:
	//rectangle();//конструктор
	rectangle(SDL_Rect rect, std::string path, SDL_Renderer* render); // констурктор с параметрами
	~rectangle(); // деструктор
	rectangle(const rectangle& other); // конструктор копирования
	void SetX(const int x) override; // установить позицию по X на рендере
	void SetY(const int y) override; // установить позицию по Y на рендере
	void SetSize(const int w, const int h) override; // установить размер спрайта на рендере (width, heigh)
	void SetTexture(SDL_Texture* newImage) override; // установить картинку
	void SetFlip(SDL_RendererFlip newFlip) override; //установить 
	void LoadTexture(std::string path, SDL_Renderer* gRenderer) override; // загрузка текстуры из файла
	int GetLeftX() override; // получить X левой границы картинки на рендере
	int GetRightX() override; // получить X правой границы картинки на рендере
	int GetUpperY() override; // получить Y верхней границы картинки на рендере
	int GetLowerY() override; // получить Y нижней границы картинки на рендере
	int GetWidth() override;
	int GetHeight() override;
	SDL_Texture* GetTexture() override; // получить указатель на картинку
	SDL_RendererFlip GetFlip() override; // получить flip 
	void Render(SDL_Renderer* gRenderer) override; // отобразить на рендере
	void FreeTexture() override; // сброс картинки
};

class dynamic_graphical_element : public rectangle, dynamic_element // динамический графический элементы
{
public:
	//dynamic_graphical_element(); // конструктор
	dynamic_graphical_element(SDL_Renderer* render, SDL_Rect newRect, std::string newTexturePath, int newXSpeed, int newYspeed); // конструктор с параметрами
	~dynamic_graphical_element(); // деструктор
	dynamic_graphical_element(dynamic_graphical_element& other); // констурктор копирования
	void Move(const int XStepsAmount, const int YStepsAmount) override; // подвинуть на количество шагов по осям X и Y
	void SetXSpeed(const int speed) override; // установить размер шага по оси X
	void SetYSpeed(const int speed) override; // установить размер шага по оси Y
	int GetXSpeed();
	int GetYSpeed();
};

class ship :public dynamic_graphical_element // класс - корбаль
{
private:
	int points;
	bool shot;
public:
	ship(); // констурктор
	ship(SDL_Renderer* render, SDL_Rect newRect, std::string newTexturePath, int newXSpeed, int newYspeed, int newPoints);
	~ship(); // деструктор
	void SetPoints(int points); // установить количество очков за корабль
	void SetShot(bool flag);
	void Move(const int XStepsAmount, const int YStepsAmount) override; // подвинуть на количество шагов по осям X и Y
	bool GetShot();
	int GetPoints(); // получить количество очков за уничтожение корабля
	bool CheckCollision(int OtherLeftX, int OtherRightX);
};

class torpedo: public dynamic_graphical_element
{
private:
	bool launched;
public:
	torpedo(SDL_Renderer* render, SDL_Rect newRect, std::string newTexturePath, int newXSpeed, int newYspeed);
	bool GetLaunched();
	void SetLaunched(bool);
};





