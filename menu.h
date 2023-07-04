#pragma once

#include"constants.h"

class textWindow
{
protected:
	SDL_Color color;
	TTF_Font* font;
	string text;
	int size;
	int letterSize;
	int lineSize;
	int letters;
	int lines;
	int maxWidth = 20000;

	SDL_Texture* texture;
	SDL_Rect coordinates;
	SDL_Rect* part;
	SDL_Surface* surface = NULL;
	SDL_Renderer* rend = NULL;
	void CreateTexture()
	{
		SDL_DestroyTexture(texture);
		surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, min(letters * letterSize, maxWidth)+200 );
		texture = SDL_CreateTextureFromSurface(rend, surface);
		SDL_FreeSurface(surface);
	}
public:
	textWindow(string newText = "\0", int newSize = 0, int x = 0, int y = 0, SDL_Renderer* newRenderer = NULL, SDL_Surface* newSurface = NULL)
	{
		lines = 1;
		letterSize = newSize * 0.6;
		letters = 0;
		color = { 255, 255, 255 };
		size = newSize;
		text = newText;
		font = TTF_OpenFont("better-vcr_0.ttf", size);
		coordinates.x = x;
		coordinates.y = y;
		coordinates.h = size;
		part = NULL;
		int lenght = 0;
		for (int i = 0; i < text.length(); i++)
		{
			if ((int)text[i] != -48 && (int)text[i] != -47) letters++;
			if (text[i] == '\n')
			{
				lineSize = --lenght > lineSize ? lenght : lineSize;
				letters--;
				lenght = 0;
				lines++;
			}
			lenght++;
		}
		coordinates.h = size * lines + 0.5*size*(lines-1);
		coordinates.w = letters * letterSize;
		surface = newSurface;
		rend = newRenderer;
		CreateTexture();
	}
	virtual ~textWindow()
	{
		surface = NULL;
		rend = NULL;
		SDL_DestroyTexture(texture);
		TTF_CloseFont(font);
	}
	void SetY(int y)
	{
		coordinates.y = y;
	}
	void SetCenterOffset(int x)
	{
		coordinates.x = HALF_WINDOW_WIDTH - coordinates.w / 2 + x;
	}
	void SetColor(SDL_Color newColor)
	{
		color = newColor;
		CreateTexture();
	}
	void SetText(string newText)
	{
		text.clear();
		text = newText;
		letters = 0;
		lines = 1;
		int lenght = 0;
		for (int i = 0; i < text.length(); i++)
		{
			if ((int)text[i] != -48 && (int)text[i] != -47) letters++;
			if (text[i] == '\n')
			{
				lineSize = --lenght > lineSize ? lenght : lineSize;
				letters--;
				lenght = 0;
				lines++;
			}
			lenght++;
		}
		coordinates.h = size * lines + 0.5 * size * (lines - 1);
		coordinates.w = letters * letterSize;
		CreateTexture();
	}
	void SetRenderingPart(int w, int h)
	{
		part = new SDL_Rect;
		part->x = 0;
		part->y = 0;
		part->h = h;
		part->w = w;
	}
	void SetMaxWidth(int w)
	{
		maxWidth = w;
	}
	string GetText()
	{
		return text;
	}
	int GetLength()
	{
		return text.length();
	}
	bool render(SDL_Renderer* renderer)
	{
		SDL_RenderCopy(renderer, texture, part, &coordinates);
		return true;
	}
};

class dynamicTextWindow : public textWindow
{
protected:
	int maxLetters = 10;
	bool centered = false;
public:
	dynamicTextWindow(int newSize = 0, int x = 0, int y = 0, SDL_Renderer* newRenderer = NULL, SDL_Surface* newSurface = NULL) : textWindow("", newSize, x, y, newRenderer, newSurface)
	{
		text.clear();
	}
	~dynamicTextWindow()
	{ }
	void SetCentered()
	{
		centered = true;
	}
	void Write(SDL_Event* event)
	{
		if (letters < maxLetters)
		{
			text.append(event->text.text);
			coordinates.w += letterSize;
			letters++;
			if (centered) SetCenterOffset(0);
			CreateTexture();
		}
	}
	void DeleteSymbol()
	{
		if (text.length() > 0)
		{
			letters--;
			text.pop_back();
			coordinates.w -= letterSize;
			if (!text.empty() && (text[text.length() - 1] == -48 || text[text.length() - 1] == -47)) text.pop_back();
			if (centered) SetCenterOffset(0);
			CreateTexture();
		}
	}
};

class button : public textWindow
{
protected:
	int index = 0;			
	SDL_Texture* bTexture;	
	SDL_Rect bCoord;		
public:
	button(string newText = "\0", int newSize = 0, int x = 0, int y = 0, SDL_Renderer* newRenderer = NULL, SDL_Surface* newSurface = NULL) : textWindow(newText, newSize, x, y, newRenderer, newSurface)
	{
		bTexture = NULL;
		bCoord.x = x;
		bCoord.y = y;
		bCoord.w = coordinates.w;
		bCoord.h = coordinates.h;
	}
	~button()
	{
		SDL_DestroyTexture(bTexture);
	}
	void SetTexture(const char* texture, int w, int h)
	{
		bTexture = IMG_LoadTexture(rend, texture);
		bCoord = { bCoord.x, bCoord.y, w < coordinates.w ? coordinates.w : w, h < coordinates.h ? coordinates.h : h };
	}
	void SetCenterOffset(int x)
	{
		coordinates.x = HALF_WINDOW_WIDTH - coordinates.w / 2 + x;
		bCoord.x = HALF_WINDOW_WIDTH - bCoord.w / 2 + x;
	}
	bool SetIndex(int newIndex) 
 	{
		if (newIndex < 0) 
			return false;
		index = newIndex;
		return true;
	}
	int GetIndex()
	{
		return index;
	}
	int GetClick() 
	{
		int q, w;
		SDL_GetMouseState(&q, &w);
		if ((q > bCoord.x && w > bCoord.y) && (q < bCoord.x + bCoord.w && w < bCoord.y + bCoord.h))
		{
			return index;
		}
		return -1;
	}
	void SetPart(int x, int y)
	{
		if (part)
		{
			part->x = x;
			part->y = y;
		}
	}
	bool render(SDL_Renderer* renderer)
	{
		SDL_RenderCopy(renderer, bTexture, part, &bCoord);
		SDL_RenderCopy(renderer, texture, NULL, &coordinates);
		return true;
	}
};