#pragma once
#include "graphical_elements.h"
#include <string>
#include <iostream>

//rectangle::rectangle() // конструктор 
//{
//	rect.x = rect.y = rect.w = rect.h = 0; sprite = NULL;
//	flip = SDL_FLIP_NONE;
//}

rectangle::rectangle(SDL_Rect newRect = {0,0,0,0}, std::string path = "\0", SDL_Renderer* render = gRenderer)
{
	rect = newRect; // устанавливаем размеры
	flip = SDL_FLIP_NONE;
	LoadTexture(path.c_str(), render); // загружаем картинку
}

rectangle::~rectangle() // деструктор
{
	this->FreeTexture();
}

rectangle::rectangle(const rectangle &other) // конструктор копирования
{
	this->rect.x = other.rect.x;
	this->rect.y = other.rect.y;
	this->rect.w = other.rect.w;
	this->rect.h = other.rect.h;
	this->sprite = other.sprite;
}

void rectangle::SetX(const int x) // установить позицию по Х на рендере
{
	rect.x = x;
}

void rectangle::SetY(const int y)
{
	rect.y = y;
}

void rectangle::SetSize(const int w, const int h) // установить размеры спрайта на рендере
{
	rect.w = w;
	rect.h = h;
}

void rectangle::SetTexture(SDL_Texture* newImage) // установить картинку
{
	this->sprite = newImage;
}

void rectangle::SetFlip(SDL_RendererFlip newFlip)
{
	flip = newFlip;
}

void rectangle::LoadTexture(std::string path, SDL_Renderer * gRenderer)
{
	SDL_Texture* newTexture = NULL; // создаём указатель на новую текстуру

	SDL_Surface* loadedSurface = IMG_Load(path.c_str()); // выгружаем картинку в поврехность
	if (loadedSurface == NULL)
	{
		std::cerr << IMG_GetError() << std::endl;
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); // преобразуем картинку в текстуру
		if (newTexture == NULL)
		{
			std::cerr << SDL_GetError() << std::endl;
		}

		SDL_FreeSurface(loadedSurface); // удаляем картинку
	}

	sprite = newTexture; // сохраняем текстуру
}

int rectangle::GetLeftX() // получить X левой границы картинки на рендере
{
	return rect.x;
}

int rectangle::GetRightX() // получить X правой границы картинки на рендере
{
	return rect.x + rect.w;
}

int rectangle::GetUpperY() // получить Y верхней границы картинки на рендере
{
	return rect.y;
}

int rectangle::GetLowerY() // получить Y нижней границы картинки на рендере 
{
	return rect.y + rect.h;
}

int rectangle::GetWidth()
{
	return rect.w;
}

int rectangle::GetHeight()
{
	return rect.h;
}

SDL_Texture* rectangle::GetTexture()
{
	return sprite;
}

SDL_RendererFlip rectangle::GetFlip()
{
	return flip;
}

void rectangle::Render(SDL_Renderer* gRenderer)
{
	SDL_RenderCopyEx(gRenderer, sprite, NULL, &rect, 0.0, NULL,GetFlip());//
}

void rectangle::FreeTexture() // сброс указателя на текстуру
{
	SDL_DestroyTexture(sprite);
	sprite = NULL;
}

//dynamic_graphical_element::dynamic_graphical_element()
//{
//	XSpeed = 0;
//	YSpeed = 0;
//}

dynamic_graphical_element::dynamic_graphical_element(SDL_Renderer* render = gRenderer, SDL_Rect newRect = {0, 0, 0, 0}, std::string newTexturePath = "\0", int newXSpeed = 0, int newYspeed = 0)
{
	rect = newRect;
	LoadTexture(newTexturePath.c_str(), render);
	XSpeed = newXSpeed;
	YSpeed = newYspeed;
}

dynamic_graphical_element::~dynamic_graphical_element()
{
	FreeTexture();
}

dynamic_graphical_element::dynamic_graphical_element( dynamic_graphical_element& other)
{
	this->SetSize(other.GetRightX() - other.GetLeftX(), other.GetLowerY() - other.GetUpperY());
	this->SetTexture(other.GetTexture());
	this->XSpeed = other.XSpeed;
	this->YSpeed= other.YSpeed;
	this->SetX(other.GetLeftX());
	this->SetY(other.GetUpperY());
}

void dynamic_graphical_element::Move(const int XStepsAmount, const int YStepsAmount)
{
	if(XStepsAmount != 0)
		SetX(GetLeftX() + XSpeed * XStepsAmount);
	if(YStepsAmount != 0)
		SetY(GetUpperY() + YSpeed* YStepsAmount);
	if (XStepsAmount < 0) // если двигаем налево
		SetFlip(SDL_FLIP_HORIZONTAL); // отразить по вертикали
	if (XStepsAmount > 0) // если двигаем направо
		SetFlip(SDL_FLIP_NONE); // не отражать по вретикали
}

void dynamic_graphical_element::SetXSpeed(const int speed)
{
	XSpeed = speed;
}

void dynamic_graphical_element::SetYSpeed(const int speed)
{
	YSpeed = speed;
}

int dynamic_graphical_element::GetXSpeed()
{
	return XSpeed;
}

int dynamic_graphical_element::GetYSpeed()
{
	return YSpeed;
}

ship::ship() : dynamic_graphical_element()
{
	SetY(240); // устанавливаем высоту спавна
	points = 0;
}

ship::~ship()
{
	FreeTexture();
}

ship::ship(SDL_Renderer* render, SDL_Rect newRect = {0,0,0,0}, std::string newTexturePath = "\0", int newXSpeed = 10, int newYspeed = 10, int newPoints = 1) : dynamic_graphical_element(render, newRect, newTexturePath, newXSpeed, newYspeed)
{
	points = newPoints;
	shot = false;
}

//ship::ship(const ship& other) : dynamic_graphical_element(other)
//{
//	this->points = other.points;
//}

void ship::SetPoints(int newPoints)
{
	points = newPoints;
}

int ship::GetPoints()
{
	return points;
}

void ship::SetShot(bool flag)
{
	shot = flag;
}

bool ship::GetShot()
{
	return shot;
}

void ship::Move(const int XStepsAmount, const int YStepsAmount)
{
	if (XStepsAmount != 0)
		SetX(GetLeftX() + GetXSpeed()  * XStepsAmount);
	if (YStepsAmount != 0)
		SetY(GetUpperY() + GetYSpeed() * YStepsAmount);
	if (XStepsAmount < 0) // если двигаем налево
		SetFlip(SDL_FLIP_HORIZONTAL); // отразить по вертикали
	if (XStepsAmount > 0) // если двигаем направо
		SetFlip(SDL_FLIP_NONE); // не отражать по вретикали
}

bool ship::CheckCollision(int OtherLeftX, int OtherRightX)
{
	if(GetLeftX() >= OtherLeftX && GetLeftX() <= OtherRightX || GetRightX() <= OtherRightX && GetRightX() >= OtherLeftX || GetLeftX() <= OtherLeftX && GetRightX() >= OtherRightX)
		return true;
	return false;
}

bool torpedo::GetLaunched()
{
	return launched;
}

void torpedo::SetLaunched(bool flag)
{
	launched = flag;
}

torpedo::torpedo(SDL_Renderer* render = gRenderer, SDL_Rect newRect = {0,0,50,50}, std::string newTexturePath = "images/torpedo.png", int newXSpeed = 0, int newYspeed = 0) : dynamic_graphical_element(render, newRect,newTexturePath, newXSpeed, newYspeed)
{
	launched = false;
}

//CheckCollision(SDL_Rect& wall)
//{
//	//Move the dot left or right
//	mPosX += mVelX;
//	mCollider.x = mPosX;
//
//	//If the dot collided or went too far to the left or right
//	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH) || checkCollision(mCollider, wall))
//	{
//		//Move back
//		mPosX -= mVelX;
//		mCollider.x = mPosX;
//	}
//
//	//Move the dot up or down
//	mPosY += mVelY;
//	mCollider.y = mPosY;
//
//	//If the dot collided or went too far up or down
//	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT) || checkCollision(mCollider, wall))
//	{
//		//Move back
//		mPosY -= mVelY;
//		mCollider.y = mPosY;
//	}
//}

//int move(int hy, bool z) //1 - смогли двинуться; 0 - дошли до края, пора удалять; -1 - поймали
//{
//	if (rect.x + rect.w  > 0 && rect.x + rect.w - HWW < 40)
//	{
//		if (hooked)
//			this->setY(hy);
//		if (!hooked && !z && fabs(hy - (coord.y + coord.h / 2)) < 30)
//		{
//			rav = hy - coord.y;
//			hooked = true;
//			return -1;
//		}
//	}
//	if (!hooked) coord.x += speed;
//	if (coord.x > WW || coord.y <= WL) return 0;
//	return 1;
//}
