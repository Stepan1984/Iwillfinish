#pragma once

//better-vcr_0.ttf
//DejaVuMarkup.ttf

#define WW 940
#define WH 730
#define HWH 315
#define HWW 470

class wwt
{
protected:
	SDL_Color color;
	TTF_Font* font;
	string text;
	int size;
	int lsize;
	int ssize;
	int letters;
	int stroks;
	int maxw = 20000;

	SDL_Texture* texture;
	SDL_Rect coord;
	SDL_Rect* part;
	SDL_Surface* surface = NULL;
	SDL_Renderer* rend = NULL;
	void crtexture()
	{
		SDL_DestroyTexture(texture);
		surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), color, min(letters * lsize, maxw)+200 );
		texture = SDL_CreateTextureFromSurface(rend, surface);
		SDL_FreeSurface(surface);
	}
public:
	wwt(string intext = "\0", int insize = 0, int x = 0, int y = 0, SDL_Renderer* inrender = NULL, SDL_Surface* insurface = NULL)
	{
		stroks = 1;
		lsize = insize * 0.6;
		letters = 0;
		color = { 255, 255, 255 };
		size = insize;
		text = intext;
		font = TTF_OpenFont("better-vcr_0.ttf", size);
		coord.x = x;
		coord.y = y;
		coord.h = size;
		part = NULL;
		int lenght = 0;
		for (int i = 0; i < text.length(); i++)
		{
			if ((int)text[i] != -48 && (int)text[i] != -47) letters++;
			if (text[i] == '\n')
			{
				ssize = --lenght > ssize ? lenght : ssize;
				letters--;
				lenght = 0;
				stroks++;
			}
			lenght++;
		}
		coord.h = size * stroks + 0.5*size*(stroks-1);
		coord.w = letters * lsize;
		surface = insurface;
		rend = inrender;
		crtexture();
	}
	virtual ~wwt()
	{
		surface = NULL;
		rend = NULL;
		SDL_DestroyTexture(texture);
		TTF_CloseFont(font);
	}
	void setY(int x)
	{
		coord.y = x;
	}
	void setcenter(int x)
	{
		coord.x = HWW - coord.w / 2 + x;
	}
	void setcoord(int x, int y)
	{
		coord.x = x;
		coord.y = y;
	}
	void setcolor(SDL_Color incolor)
	{
		color = incolor;
		crtexture();
	}
	void settext(string intext)
	{
		text.clear();
		text = intext;
		letters = 0;
		stroks = 1;
		int lenght = 0;
		for (int i = 0; i < text.length(); i++)
		{
			if ((int)text[i] != -48 && (int)text[i] != -47) letters++;
			if (text[i] == '\n')
			{
				ssize = --lenght > ssize ? lenght : ssize;
				letters--;
				lenght = 0;
				stroks++;
			}
			lenght++;
		}
		coord.h = size * stroks + 0.5 * size * (stroks - 1);
		coord.w = letters * lsize;
		crtexture();
	}
	void makeparted(int w, int h)
	{
		part = new SDL_Rect;
		part->x = 0;
		part->y = 0;
		part->h = h;
		part->w = w;
	}
	void setmaxW(int w)
	{
		maxw = w;
		cout << maxw << endl;
	}
	string gettext()
	{
		return text;
	}
	int getlength()
	{
		return text.length();
	}
	bool render(SDL_Renderer* rendering)
	{
		SDL_RenderCopy(rendering, texture, part, &coord);
		return true;
	}
};

//изменяемый (в т.ч. пользователем) текст
class chatext : public wwt
{
protected:
	int maxlet = 10;
	bool centred = false;
public:
	chatext(int insize = 0, int x = 0, int y = 0, SDL_Renderer* inrender = NULL, SDL_Surface* insurface = NULL) : wwt("", insize, x, y, inrender, insurface)
	{
		text.clear();
	}
	~chatext()
	{ }
	void setcentred()
	{
		centred = true;
	}
	void write(SDL_Event* event)
	{
		if (letters < maxlet)
		{
			text.append(event->text.text);
			coord.w += lsize;
			letters++;
			if (centred) setcenter(0);
			crtexture();
		}
	}
	void backspace()
	{
		if (text.length() > 0)
		{
			letters--;
			text.pop_back();
			coord.w -= lsize;
			if (!text.empty() && (text[text.length() - 1] == -48 || text[text.length() - 1] == -47)) text.pop_back();
			if (centred) setcenter(0);
			crtexture();
		}
	}
};

//Таймер
class Timer : public wwt
{
protected:
	bool paused;
	bool started;
	Uint32 startt;
	Uint32 fromstrt;
	Uint32 pauset;

	Uint32 startp;
	Uint32 maxtime;
public:
	Timer(Uint32 maxseconds = 120, int insize = 0, int x = 0, int y = 0, SDL_Renderer* inrender = NULL, SDL_Surface* insurface = NULL) : wwt("\0", insize, x, y, inrender, insurface)
	{
		paused = false;
		started = false;
		startt = 0;
		startp = 0;
		pauset = 0;
		fromstrt = 0;
		maxtime = maxseconds;

		crtexture();
	}
	~Timer()
	{
		text.clear();
		surface = NULL;
		rend = NULL;
	}
	void start()
	{
		started = true;
		paused = false;
		startt = SDL_GetTicks();
		fromstrt = 1;
		pauset = 0;
		startp = 0;
	}
	void stop()
	{
		started = false;
		paused = false;
		fromstrt = startt = 0;
		pauset = 0;
		startp = 0;
	}
	void pause()
	{
		if (paused)
		{
			pauset += SDL_GetTicks() - startp;
		}
		else
		{
			startp = SDL_GetTicks();
		}
		paused = !paused;
	}
	bool ispaused()
	{
		return paused;
	}
	bool trender(SDL_Renderer* rendering)
	{
		if (!paused && fromstrt != 0 && (SDL_GetTicks() / 1000))
		{
			text.clear();
			fromstrt = maxtime - (SDL_GetTicks() - startt - pauset) / 1000;
			text.append(to_string(fromstrt / 60));
			text.append(":");
			text.append(to_string(fromstrt % 60));
			coord.w = 0.7 * size * text.length();
			crtexture();
		}
		SDL_RenderCopy(rendering, texture, NULL, &coord);
		return fromstrt ? true : false;
	}
};

//Кнопки (кликабельные объекты)
class button : public wwt
{
protected:
	int index = 0;			//Номер кнопки
	SDL_Texture* btexture;	//Текстура подложки для текста
	SDL_Rect bcoord;		//Координаты подложки/кликабельная часть
public:
	button(string intext = "\0", int insize = 0, int x = 0, int y = 0, SDL_Renderer* inrender = NULL, SDL_Surface* insurface = NULL) : wwt(intext, insize, x, y, inrender, insurface)
	{
		btexture = NULL;
		bcoord.x = x;
		bcoord.y = y;
		bcoord.w = coord.w;
		bcoord.h = coord.h;
	}
	~button()
	{
		SDL_DestroyTexture(btexture);
	}
	void settexture(const char* texture, int w, int h)
	{
		btexture = IMG_LoadTexture(rend, texture);
		bcoord = { bcoord.x, bcoord.y, w < coord.w ? coord.w : w, h < coord.h ? coord.h : h };
	}
	void setcenter(int x)
	{
		coord.x = HWW - coord.w / 2 + x;
		bcoord.x = HWW - bcoord.w / 2 + x;
	}
	bool setindex(int iindex) //Функция получения индекса возвращает false если индеск не был присвоен (попытка присвоить 0вой или отрицательный индекс)
 	{
		if (iindex < 0) return false;
		index = iindex;
		return true;
	}
	int getindex()
	{
		return index;
	}
	int getclick() //при нажатии возвращает индекс кнопки, в остальных случаях -1
	{
		int q, w;
		SDL_GetMouseState(&q, &w);
		if ((q > bcoord.x && w > bcoord.y) && (q < bcoord.x + bcoord.w && w < bcoord.y + bcoord.h))
		{
			cout << index << endl;
			return index;
		}
		cout << -1 << endl;
		return -1;
	}
	void setpart(int x, int y)
	{
		if (part)
		{
			part->x = x;
			part->y = y;
		}
	}
	bool render(SDL_Renderer* rendering)
	{
		SDL_RenderCopy(rendering, btexture, part, &bcoord);
		SDL_RenderCopy(rendering, texture, NULL, &coord);
		return true;
	}
};