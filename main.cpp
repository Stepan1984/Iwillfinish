/*
*Возможности
	заставка, меню, справка, средства для управления программой
	заставка (название, имя автора, номер группы) -> ввод имени игрока -> меню
	меню
	  +|- Запусить - запуск раунда
	  +|- Справка - информация с правилами, и т.п.
	  +|- Выход - закрытие программы
	  +/-|- Рекорды - вывод на экран содержимого файла с рекордами - изначально бинарный файл (-> в текстовый для печати), возможность его очистки
														|- десять лучших результатов с именами игроков
	  +|- Смена игорка - ввод нового имени
	  +|- Каталог рыболова (некоторый бестиарий рыб, где указаны очки за них, может сделаю, может нет)
	Справка во время игры - с управлением, актуальной информацией
	+Управление мышь + клавиатура
	+Обновлять экран достаточно часто
	+Пауза - выход, продолжить

*Реализация
	Две иерархии:
	1. Меню и диалоговые окна
		База - "непрозрачное окно с текстовой строкой, выводимое в определенном месте экрана" - откуда один два уровня классов
	2. графический элемент
		База - абстрактный "статический графический элемент"
		База 2 - абстрактный "управляемо перемещаемый по экрану элемент"
	Иерархия в свой файл
	
	Разнотипные объекты одной иерархии должны быть объединены в общий
	массив или список. В простых случаях допускается использовать одну
	простую или ссылочную переменную для работы с разными объектами.

	 Для организации корректной перерисовки все отображаемые на экране
	объекты игрового процесса (перемещаемые и не перемещаемые) должны
	храниться в списке, отсортированном в требуемом порядке перерисовки
	(как правило, первым будет объект на заднем плане – фон, последним –
	объект на переднем плане, чаще всего – управляемый пользователем).


	"Рыбалка"
	+В реке на разной глубине - рыбы и мусор
	+рыбы + очки
	+мусор - очки

	+При попадании на крючок - висит там пока не поднимет
	+На крючке одновременно только один объект
	
	Время две минуты

*/

#include <windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "objects.h"
#include "menu.h"

#define WW 940
#define WH 730
#define HWH 315
#define WL 130
#define LH 100

using namespace std;

//Указатель на окно
SDL_Window* window = NULL;
//Указатель на поверхность
SDL_Surface* screenSurface = NULL;
//Указатель на рендер
SDL_Renderer* render = NULL;
//Флаг на динамит
bool flag_d_used = false;	//Что динамит был использован (если да, то проигрывается плохая концовка)
bool flag_d_catch = false;	//Что динамит был пойман (доставлен на сушу и готов к использования)

const Uint8* keys = SDL_GetKeyboardState(NULL);

//Для организации списка лидеров
typedef struct 
{
	short int points;
	string username;
} sdata;

typedef struct {
	short int scores;
	string usrername;
	bool quit;
} info;

//Функции
//Запуск СДЛ
bool start();
//Создание нового объекта в реке
fish* makenew(const char*[], const int[], SDL_Texture*);
//Работа с файлом
bool makefile(sdata**, int);
sdata** readfile(int);
bool showrecords(sdata**, staticgrafelem*);

//Игровые функции
info startm();
void menu(string, sdata**);
info game();

int main(int argc, char* argv[]) 
{    
	sdata** tlist = readfile(10);
	if (start())
	{
		printf("Failed to initialize!\n");
		return 1;
	}
	info ufo;
	ufo = startm(); //Cтартовое меню
	
	if (ufo.quit) menu(ufo.usrername, tlist);
	makefile(tlist, 10);
	return 0;
}

bool start()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return true;
	if (TTF_Init() == -1) return true;

	window = SDL_CreateWindow("Fish&ca", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WW, WH, SDL_WINDOW_SHOWN);
	if (window == NULL) return true;
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (render == NULL) return true;
	screenSurface = SDL_GetWindowSurface( window );
	if (screenSurface == nullptr) return true;
	return false;
}

//Работа со списком рекордов
bool makefile(sdata** dlist, int nop)
{
	ofstream file("f.dat");
	if (!file.is_open())
	{
		cout << "Ошибка открытия файла" << endl;
		return false;
	}
	cout << "DEBUGINFO" << endl;
	for (int i = 0; i < nop && dlist[i]; i++)
	{
		file << dlist[i]->points;
		file << dlist[i]->username;
		file << " ";
		cout << dlist[i]->points << " - " << dlist[i]->username << endl;
	}
	file << "\n";
	file.close();
	return true;
}
sdata** readfile(int nop = 0)
{
	ifstream file("f.dat");
	if (!file.is_open())
	{
		cout << "Ошибка чтения файла" << endl;
		return NULL;
	}
	sdata** dlist = new sdata*[10];
	for (int i = 0; i < 10; i++)
	{
		dlist[i] = NULL;
	}
	cout << "DEBUGINFO" << endl;
	char tmp;
	int i;
	file >> i;
	for (i = 0; i < nop && !file.eof(); i++)
	{
		tmp = '0';
		dlist[i] = new sdata;
		file >> dlist[i]->points;
		for (int g = 0; g < 10 && tmp != ' ' && !file.eof(); g++)
		{
			file >> tmp;
			if (tmp != ' ') dlist[i]->username.push_back(tmp);
		}
		cout << dlist[i]->points << " - " << dlist[i]->username << endl;
	}
	file.close();
	return dlist;
}
void sortnadd(sdata** tlist, sdata tmp)
{
	sdata* tmp1 = new sdata;
	tmp1->points = tmp.points;
	tmp1->username = tmp.username;
	int i = -1;
	while (++i < 10 && tlist[i] && tlist[i]->points > tmp.points);
	if (i == 10) return;
	cout << "FUCK YEAH" << endl;
	sdata* temp = tlist[i];
	tlist[i] = tmp1;
	while (++i < 10 && tlist[i-1])
	{
		tmp1 = tlist[i];
		tlist[i] = temp;
		temp = tmp1;
	}
	for (i = 0; i < 10 && tlist[i]; i++)
	{
		cout << i << " " << tlist[i]->username << endl;
	}
}

bool showrecords(sdata** tlist, staticgrafelem* fon)
{
	staticgrafelem b({ HWW - 766/2, 100, 766, 492 }, "textures/r.png", render);
	button exit("Назад", 50, 0, 490, render, screenSurface);
	exit.setcolor({ 0,0,0 });
	exit.setcenter(0);

	SDL_Event even;
	string records;
	int i = 0;
	int j, r;
	stringstream ss;
	while (i < 10 && tlist[i])
	{
		for (r = j = 0; j < tlist[i]->username.length(); j++)
			if (tlist[i]->username[j] == -47 || tlist[i]->username[j] == -48) r++;
		ss << setw(2) << i << ". " << setw(10+r) << tlist[i]->username << " | " << tlist[i]->points << endl;
		i++;
	}
	records = ss.str();
	wwt rec(records, 17, HWW - 766 / 2 + 90, 155, render, screenSurface);
	rec.setcolor({ 0,0,0 });
	bool quit = true,
		start = true;
	while (quit && start)
	{
		while (SDL_PollEvent(&even))
		{
			if (even.type == SDL_QUIT) quit = false;
			if (even.type == SDL_MOUSEBUTTONDOWN)
			{
				start = exit.getclick() == -1 ? start : false;
			}
		}
		fon->render(render);
		b.render(render);
		rec.render(render);
		exit.render(render);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(40);
	}
	return quit;
}

bool showsprav(staticgrafelem* fon)
{
	bool quit = true;
	bool start = true;
	int vibor = -1;
	SDL_Event event;
	SDL_PollEvent(&event);

	staticgrafelem b({ HWW - 766 / 2, 100, 766, 492 }, "textures/r.png", render);
	button** knopka;
	knopka = new button*[3];
	knopka[0] = new button("Назад", 50, 0, 490, render, screenSurface);
	knopka[1] = new button("Правила", 50, 0, 200, render, screenSurface);
	knopka[2] = new button("Бестиарий", 50, 0, 254, render, screenSurface);
	int i;
	for (i = 0; i < 3; i++)
	{
		knopka[i]->setcenter(0);
		knopka[i]->setcolor({ 0,0,0 });
		knopka[i]->setindex(i);
	}
	wwt** texts = new wwt*[3];
	texts[0] = new wwt("Рыбалка", 50, 0, 143, render, screenSurface);
	texts[1] = new wwt("", 16, HWW - 766 / 2 + 86, 192, render, screenSurface);
	texts[2] = new button("", 10, HWW - 766 / 2 + 86, 192, render, screenSurface);
	
	dynamic_cast<button*>(texts[2])->settexture("textures/bestiary.png", 595, 300);
	dynamic_cast<button*>(texts[2])->makeparted(595, 300);

	texts[0]->setcenter(0);
	stringstream ss;
	ss << "Цель игры: набрать наибольшее количество очков;\n     Очки даются за поимку рыбы,\n     За поимку мусора очки снимаются;\n\n" <<
		"Одновременно на крючке может быть только\nодин объект. После того как рыба была снята\nс крючка (для этого её надо поднять на сушу),\n"<<
		"крючок надо поднять ещё выше, чтобы заменить червя.\nКрючок управляется клавишами w/s, клавиша d\nактивирует динамит, если он был пойман прежде.\nВремя игры 2 минуты.\n"<<
		"Удачной ловли!                               -Fis" ;
	texts[1]->settext(ss.str());
	texts[1]->setmaxW(10);
	
	int y = 0;
	while (quit && start)
	{
		fon->render(render);
		b.render(render);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = false;
			if (event.type == SDL_MOUSEBUTTONDOWN)
				if (vibor == -1)
					for (i = 0; i < 3; i++)
						vibor = knopka[i]->getclick() == -1 ? vibor : knopka[i]->getindex();
				else
				{
					vibor = knopka[0]->getclick() == -1 ? vibor : -1;
					y = 0;
				}
			if (vibor == 2)
			{
				if (event.type == SDL_MOUSEWHEEL)
				{
					if (event.wheel.y < 0)
						y = y - 15 * event.wheel.y >= 447 ? 447 : y - 15 * event.wheel.y;
					else if (event.wheel.y > 0)
						y = y - 15 * event.wheel.y <= 0 ? 0 : y - 15 * event.wheel.y;
					cout << y << endl;
					dynamic_cast<button*>(texts[2])->setpart(0, y);
				}
			}
		}
		knopka[0]->render(render);
		texts[0]->render(render);
		
		switch (vibor)
		{
		case 0:
			start = false;
		case 1:
			texts[1]->render(render);
			break;
		case 2:
			dynamic_cast<button*>(texts[2])->render(render);
			break;
		default:
			for (i = 0; i < 3; i++)
				knopka[i]->render(render);
		}
		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(20);
	}
	for (i = 0; i < 3; i++)
	{
		delete knopka[i];
		delete texts[i];
	}
	delete[] knopka;
	delete[] texts;
	return quit;
}

info entername(staticgrafelem* fon)
{
	button enter("введите имя", 30, 0, 200, render, screenSurface);
	button truenter("готово", 30, 0, 361, render, screenSurface);
	enter.settexture("textures/g.png", 328, 207);
	enter.setcenter(0);
	enter.setY(208);
	truenter.setcenter(0);
	chatext nametest(36, 0, 280, render, screenSurface);

	nametest.setcolor({ 0,0,0 });
	nametest.setcentred();
	SDL_Event even;
	
	char vibor = -1;
	bool quit = true,
		 zalip = true, //Чтобы бэк спейс вызывался только один раз
		 start = true;
	while (quit && start)
	{
		while (SDL_PollEvent(&even))
		{
			if (even.type == SDL_QUIT) { quit = false; nametest.settext("def"); }
			if (even.type == SDL_MOUSEBUTTONDOWN)
			{
				vibor = enter.getclick();
				start = truenter.getclick() == -1 || !nametest.getlength() ? start : false;
			}
			if (vibor != -1)
			{
				if (even.type == SDL_TEXTINPUT && !keys[SDL_SCANCODE_SPACE]) nametest.write(&even);
				if (even.key.keysym.sym == SDLK_BACKSPACE) { if (zalip) nametest.backspace(); zalip = !zalip; }
			}
		}
		fon->render(render);
		enter.render(render);
		nametest.render(render);
		truenter.render(render);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(30);
	}
	return { 0, nametest.gettext(), quit };
}

info startm()
{
	staticgrafelem zast({ 0,0,WW,WH }, "textures/zast.png", render); //Для заставки
	
	wwt name("О727Б Залесский А.Д.", 35, 4, WH - 50, render, screenSurface);
	wwt title("РЫБАЛКА 0.1", 60, 0, 20, render, screenSurface);
	title.setcenter(0);
	title.setcolor({ 0,0,0 });
	
	info dat;
	dat.quit = true;
	SDL_Event even;
	char start = 2;

	while (dat.quit && start > 0)
	{
		while (SDL_PollEvent(&even))
		{
			if (even.type == SDL_QUIT)
				dat.quit = false;
			if (even.type == SDL_MOUSEBUTTONDOWN)
			if (--start)
			{
				dat = entername(&zast);
				return dat;
			}
		}
		zast.render(render);
		title.render(render);
		name.render(render);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);

		SDL_Delay(30);
	}

	return dat;
}

void menu(string nname, sdata** tlist)
{
	//Фоны
	staticgrafelem menufon({ 0,0,WW,WH }, "textures/zast.png", render);
	//кнопки
	button** knopka;
	knopka = new button*[7];
	// | 0. Назад
	// | 1. Начало
	// | 2. Справка
	// | 3. Рекорды
	// | 4. Настройки
	// | 5. Смена игрока
	// | 6. Выход

	const char* buttontext[] = { "Назад", "Начало", "Справка", "Рекорды", "Смена игрока", "Выход"};
	for (int i = 0; i < 6; i++)
	{
		knopka[i] = new button(buttontext[i], 40, 20, 230 + 45 * i, render, screenSurface);
		knopka[i]->setindex(i);
	}

	sdata tmp = { 0, nname };
	info f = {0, nname, true};
	bool quit = true;
	bool start = true;
	SDL_Event event;
	SDL_PollEvent(&event);

	int i;
	int vibor = -1;
	while (quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = false;
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (vibor != -1)
				{
					vibor = knopka[0]->getclick() == -1 ? vibor : knopka[0]->getindex();
				}
				for ( i = 1; i < 7 && vibor == -1; i++ )
				{
					vibor = knopka[i]->getclick() == -1 ? vibor : knopka[i]->getindex();
				}
				cout << vibor << endl;
			}
		}
		switch (vibor)
		{
		case 5:
			for (i = 0; i < 6; i++)
				delete knopka[i];
			delete[] knopka;
			return;
		case 2:
			quit = showsprav(&menufon);
			vibor = -1;
			break;
		case 3:
			quit = showrecords(tlist, &menufon);
			vibor = -1;
			break;
		case 4:
			f = entername(&menufon);
			vibor = -1;
			tmp.username = f.usrername;
			if (!f.quit) return;
			break;
		case 1:
			f = game();
			quit = f.quit;
			tmp.points = f.scores;
			sortnadd(tlist, tmp);
			if (!f.quit) return;
		}
		menufon.render(render);
		for (int i = 1; i < 6; i++)
			knopka[i]->render(render);
		vibor = -1;
		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(40);
	}
	for (i = 0; i < 6; i++)
		delete knopka[i];
	delete[] knopka;
}

//Создание нового объекта в реке, NULL - не создался
fish* makenew(const char* types[], const int scores[], SDL_Texture* image)
{
	char i = rand() % 100; //Для выбора создаваемого объекта
	char l = rand() % 6; //Для выбора уровня воды
	bool naprav = rand() % 2; //Лево или право
	char speed = 2 + rand() % 7;
	int x = naprav ? -180 : WW;
	fish* q = NULL;
	char vid = 0;
	if (i < 30) vid = 0;
	if (i >= 30 && i < 50) vid = 1;
	if (i >= 50 && i < 65) { vid = 2; l = 3 + rand() % 3; }
	if (i >= 65 && i < 80) vid = 3;
	if (i >= 80 && i < 92) vid = 4;
	if (i >= 92)
	{
		if (!(rand() % 3)) vid = 5;
		else
		{
			vid = 6;
			image = IMG_LoadTexture(render, types[vid]);
			q = new dynamite({ -70, WL + LH * l, 67, 73 }, image, speed, &flag_d_catch);
			return q;
		}
	}
	SDL_Rect dest = { x, WL + LH * l, 180, 90 };
	image = IMG_LoadTexture(render, types[vid]);
	if (naprav) q = new Rfish(dest, image, speed, scores[vid]);
	else q = new Lfish(dest, image, speed, scores[vid]);
	return q;
}

int dynamitee(fish* g[])
{
	int score = 0;
	flag_d_used = true;
	flag_d_catch = false;
	for (int i = 0; i < 15; i++)
	{
		if (g[i])
		{
			g[i]->makehooked();
			score += g[i]->getpoint() > 0 ? g[i]->getpoint() : 0;
			delete g[i];
			g[i] = NULL;
		}
	}
	return score;
}

info game()
{
	const char* files[] = { "textures/f1.png", "textures/f2.png", "textures/f3.png", "textures/tr2.png", "textures/tr.png", "textures/tr3.png", "textures/d.png" };
	const int points[] = { 100, 200, 300, -150, -250, -666, 0 };

	//Для выбора концовки
	const char* efiles[] = { "textures/dend.png", "textures/gend.png", "textures/cend.png", "textures / gend1.png"};
	string elogos[] = {"Вас поймали", "Отличный улов", "В хорошей рыбалке главное не улов", "Язь! Ребята, язь здоровенный!"};

	staticgrafelem fon({ 0,0,WW,WH }, "textures/fon.png", render); //Для главной игры
	staticgrafelem pausetex({ HWW - 125, 200, 250, 300 }, "textures/pause.png", render);
	staticgrafelem peredfon({ 0, WH - 314, WW, 314 }, "textures/peredfon.png", render);

	SDL_Event event;
	SDL_PollEvent(&event);

	//Крюк игрока
	SDL_Texture* test = NULL;
	test = IMG_LoadTexture(render, "textures/hook.png");
	hook player(test, 5);

	//Флаг на динамит
	flag_d_used = false;	//Что динамит был использован (если да, то проигрывается плохая концовка)
	flag_d_catch = false;	//Что динамит был пойман (доставлен на сушу и готов к использования)

	bool quit = true;	//Флаг выхода
	bool eflag = true;	//Флаг завершения игрового цикла
	short int score = 0;
	int fflag = 0;

	int i;
	fish* g[15];
	for (int i = 0; i < 15; i++)
		g[i] = NULL;

	wwt pauset("ПАУЗА", 40, 0, 210, render, screenSurface);
	Timer timer(120, 40, 60, 64, render, screenSurface);		//Таймер
	wwt scores("0", 40, 60, 20, render, screenSurface);			//Счёт 

	pauset.setcenter(0);
	timer.setcolor({ 0, 0, 0 });
	scores.setcolor({ 0, 0, 0 });

	//Кнопки
	// 0. Пауза
	// 1. Динамит
	// 2. Продолжить
	// 3. Справка
	// 4. Выход
	button** knopka = new button*[5];
	const char* c[] = { "Продолжить", "Справка", "Выход" };
	knopka[0] = new button("", 2, HWW + 220, 4, render, screenSurface);		//Кнопка паузы
	knopka[1] = new button("", 2, HWW + 304, 4, render, screenSurface);		//Кнопка для исользования динамита
	for (i = 0; i < 3; i++)
	{
		knopka[i + 2] = new button(c[i], 40, 0, 254 + 44 * i, render, screenSurface);
		knopka[i + 2]->setcenter(0);
	}

	knopka[0]->settexture("textures/pb.png", 50, 50);
	knopka[1]->settexture("textures/db.png", 80, 80);

	//Сама игра
	timer.start();
	SDL_SetRenderDrawColor(render, 255, 128, 128, 255);
	while (quit && eflag) {
		fon.render(render);
		player.render(render);
		knopka[0]->render(render);
		if (flag_d_catch)  knopka[1]->render(render);
		scores.render(render);
		eflag = eflag ? timer.trender(render) : eflag;
		SDL_RenderDrawLine(render, WW / 2, 17, WW / 2, player.getY() + 7);
		//обработка событий (нажатий клавиш и т.п.)
		while (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT)
				quit = false;
			if (event.type == SDL_KEYDOWN)
			{
				if (keys[SDL_SCANCODE_SPACE]) { timer.pause(); }
				if (!timer.ispaused())
				{
					if (keys[SDL_SCANCODE_W]) player.moveU();
					if (keys[SDL_SCANCODE_S]) player.moveD();
					if (flag_d_catch && keys[SDL_SCANCODE_D]) //Использование динамита через клавишу D
					{
						score += dynamitee(g);
						scores.settext(to_string(score));
					}
				}
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (!timer.ispaused()) //Использование динамита через специальную кнопку на экране
				{
					if (knopka[0]->getclick() != -1) timer.pause();
					if (flag_d_catch && knopka[1]->getclick() != -1)
					{
						score += dynamitee(g);
						scores.settext(to_string(score));
					}
				}
				else
				{
					eflag = knopka[4]->getclick() == -1 ? eflag : false;
					if (knopka[2]->getclick() != -1) timer.pause();
					if (knopka[3]->getclick() != -1) {showsprav(&fon); fon.render(render);}
				}
			}
		}
		//Движение рыб, их создание, поимка и отрисовка
		if (!timer.ispaused())
		{
			for (i = 0; i < 15; i++)
			{
				if (g[i] == NULL)
				{
					if (rand() % 900 == 1) g[i] = makenew(files, points, test); //Создание новой рыбы
				}
				else
				{
					g[i]->render(render); //отрисовка
					fflag = g[i]->move(player.getY() + 29, player.getzac()); //Движение
					if (fflag == -1) //проверка на поимку
					{
						player.setzacep();
					}
					if (!fflag) //проверка на уход за край/в улов
					{
						score += g[i]->getpoint();
						scores.settext(to_string(score));
						delete g[i];
						g[i] = NULL;
					}
				}
			}
		}
		else
		{
			pausetex.render(render);
			pauset.render(render);
			for (i = 2; i < 5; i++)
				knopka[i]->render(render);
		}
		peredfon.render(render);
		SDL_RenderPresent(render);
		SDL_RenderClear(render);

		SDL_Delay(20);
	}
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	//Концовка
	if (flag_d_used) //Плохая концовка
		i = 0;
	else if (score > 1500) //Хорошая концовка
		i = rand() % 10 ? 1 : 4;
	else //Обычная концовка
		i=2;
	staticgrafelem efon({ HWW - 766 / 2, 100, 766, 492 }, efiles[i], render);
	wwt escore(to_string(score), 40, 0, 492, render, screenSurface);	//Счёт
	wwt logo(elogos[i], 40, 0, 600, render, screenSurface);				//Подбадривающая надпись
	escore.setcenter(0);
	logo.setcenter(0);
	eflag = true;
	while (quit && eflag)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = false;
			if (event.type == SDL_MOUSEBUTTONDOWN)
				eflag = false;
		}
		fon.render(render);
		peredfon.render(render);
		efon.render(render);
		escore.render(render);
		logo.render(render);
		SDL_RenderPresent(render);
		SDL_RenderClear(render);
	}
	return { score, "", quit};
}