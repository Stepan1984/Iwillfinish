
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


#include"constants.h"

using namespace std;

SDL_Window* window = NULL; // окно
SDL_Surface* screenSurface = NULL; // рендер
SDL_Renderer* render = NULL; // поврехность
const Uint8* keys = SDL_GetKeyboardState(NULL);

typedef struct // ыданные списка результатов
{
	short int points;
	string username;
} scoreData;

typedef struct // результат игровой сессии
{ 
	short int scores;
	string username;
	bool quit;
} gameResults;

bool Init();
ship* MakeNewShip(const char*[], const int[], SDL_Texture*);
bool MakeRecordsFile(scoreData**, int);
scoreData** ReadRecordsFile(int);
bool ShowRecords(scoreData**, staticGraficalElement*);

gameResults startScreen();
void MainMenu(string, scoreData**);
gameResults Game();

int main(int argc, char* argv[]) 
{    
	scoreData** scoresList = ReadRecordsFile(10);
	if (Init())
	{
		printf("Failed to initialize!\n");
		return 1;
	}
	gameResults ufo;
	ufo = startScreen(); 
	
	if (ufo.quit) MainMenu(ufo.username, scoresList);
	MakeRecordsFile(scoresList, 10);
	return 0;
}

bool Init()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return true;
	if (TTF_Init() == -1) return true;

	window = SDL_CreateWindow("Ships destroyer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) return true;
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (render == NULL) return true;
	screenSurface = SDL_GetWindowSurface( window );
	if (screenSurface == nullptr) return true;
	return false;
}

//Работа со списком рекордов
bool MakeRecordsFile(scoreData** dlist, int nop)
{
	ofstream file("f.dat");
	if (!file.is_open())
	{
		cout << "Ошибка открытия файла" << endl;
		return false;
	}
	for (int i = 0; i < nop && dlist[i]; i++)
	{
		file << dlist[i]->points;
		file << dlist[i]->username;
		file << " ";

	}
	file << "\n";
	file.close();
	return true;

}

scoreData** ReadRecordsFile(int nop = 0)
{
	ifstream file("f.dat");
	if (!file.is_open())
	{
		cout << "Ошибка чтения файла" << endl;
		return NULL;
	}
	scoreData** dlist = new scoreData * [10];
	for (int i = 0; i < 10; i++)
	{
		dlist[i] = NULL;
	}

	char tmp;
	int i;
	file >> i;
	for (i = 0; i < nop && !file.eof(); i++)
	{
		tmp = '0';
		dlist[i] = new scoreData;
		file >> dlist[i]->points;
		for (int g = 0; g < 10 && tmp != ' ' && !file.eof(); g++)
		{
			file >> tmp;
			if (tmp != ' ') dlist[i]->username.push_back(tmp);
		}
	}
	file.close();
	return dlist;
}

void AddRecord(scoreData** scoresList, scoreData newData)
{
	scoreData* tmp1 = new scoreData;
	tmp1->points = newData.points;
	tmp1->username = newData.username;
	int i = -1;
	while (++i < 10 && scoresList[i] && scoresList[i]->points > newData.points);
	if (i == 10) return;
	scoreData* temp = scoresList[i];
	scoresList[i] = tmp1;
	while (++i < 10 && scoresList[i - 1])
	{
		tmp1 = scoresList[i];
		scoresList[i] = temp;
		temp = tmp1;
	}
}

bool ShowRecords(scoreData** scoresList, staticGraficalElement* background)
{
	staticGraficalElement b({ HALF_WINDOW_WIDTH - 766 / 2, 100, 766, 492 }, "images/message.png", render);
	button exit("Назад", 50, 0, 490, render, screenSurface);
	exit.SetColor({ 0,0,0 });
	exit.SetCenterOffset(0);

	SDL_Event even;
	string records;
	int i = 0;
	int j, r;
	stringstream ss;
	while (i < 10 && scoresList[i])
	{
		for (r = j = 0; j < scoresList[i]->username.length(); j++)
			if (scoresList[i]->username[j] == -47 || scoresList[i]->username[j] == -48) r++;
		ss << setw(2) << i << ". " << setw(10 + r) << scoresList[i]->username << " | " << scoresList[i]->points << endl;
		i++;
	}
	records = ss.str();
	textWindow rec(records, 17, HALF_WINDOW_WIDTH - 766 / 2 + 90, 155, render, screenSurface);
	rec.SetColor({ 0,0,0 });
	bool quit = true,
		start = true;
	while (quit && start)
	{
		while (SDL_PollEvent(&even))
		{
			if (even.type == SDL_QUIT) quit = false;
			if (even.type == SDL_MOUSEBUTTONDOWN)
			{
				start = exit.GetClick() == -1 ? start : false;
			}
		}
		background->render(render);
		b.render(render);
		rec.render(render);
		exit.render(render);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(40);
	}
	return quit;
}

bool ShowGuide(staticGraficalElement* background)
{
	bool quit = true;
	bool start = true;
	int choose = -1;
	SDL_Event event;
	SDL_PollEvent(&event);

	staticGraficalElement b({ HALF_WINDOW_WIDTH - 766 / 2, 100, 766, 492 }, "images/message.png", render);
	button** myButton;
	myButton = new button*[3];
	myButton[0] = new button("Назад", 50, 0, 490, render, screenSurface);
	myButton[1] = new button("Правила", 50, 0, 200, render, screenSurface);
	myButton[2] = new button("Справочник", 50, 0, 254, render, screenSurface);
	int i;
	for (i = 0; i < 3; i++)
	{
		myButton[i]->SetCenterOffset(0);
		myButton[i]->SetColor({ 0,0,0 });
		myButton[i]->SetIndex(i);
	}
	textWindow** texts = new textWindow*[3];
	texts[0] = new textWindow("Шип дестроер", 50, 0, 143, render, screenSurface);
	texts[1] = new textWindow("", 16, HALF_WINDOW_WIDTH - 766 / 2 + 86, 192, render, screenSurface);
	texts[2] = new button("", 10, HALF_WINDOW_WIDTH - 766 / 2 + 86, 192, render, screenSurface);
	
	dynamic_cast<button*>(texts[2])->SetTexture("images/ships.png", 595, 300);
	dynamic_cast<button*>(texts[2])->SetRenderingPart(595, 300);

	texts[0]->SetCenterOffset(0);
	stringstream ss;
	ss << "Цель игры: набрать наибольшее количество очков;\nОчки даются за уничтожение кораблей торпедами\n с подводной лодки,\n\n" <<
		"Одновременно может быть запущена только одна торпеда.\n Корабли имеют разную стоимость от 1 до 3 очков,\n а также различаются по скорости.\n\n"<<
		"Торпеда запускается на SPACE.\nПодводная лодка перемещается влево-вправо \nс помощью клавиш A и D\n\nНа стрельбу отведено 10 торпед.\n"<<
		"Удачи" ;
	texts[1]->SetText(ss.str());
	texts[1]->SetMaxWidth(10);
	
	int y = 0;
	while (quit && start)
	{
		background->render(render);
		b.render(render);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = false;
			if (event.type == SDL_MOUSEBUTTONDOWN)
				if (choose == -1)
					for (i = 0; i < 3; i++)
						choose = myButton[i]->GetClick() == -1 ? choose : myButton[i]->GetIndex();
				else
				{
					choose = myButton[0]->GetClick() == -1 ? choose : -1;
					y = 0;
				}
			if (choose == 2)
			{
				if (event.type == SDL_MOUSEWHEEL)
				{
					if (event.wheel.y < 0)
						y = y - 15 * event.wheel.y >= 447 ? 447 : y - 15 * event.wheel.y;
					else if (event.wheel.y > 0)
						y = y - 15 * event.wheel.y <= 0 ? 0 : y - 15 * event.wheel.y;
					dynamic_cast<button*>(texts[2])->SetPart(0, y);
				}
			}
		}
		myButton[0]->render(render);
		texts[0]->render(render);
		
		switch (choose)
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
				myButton[i]->render(render);
		}
		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(20);
	}
	for (i = 0; i < 3; i++)
	{
		delete myButton[i];
		delete texts[i];
	}
	delete[] myButton;
	delete[] texts;
	return quit;
}

gameResults NameInput(staticGraficalElement* background)
{
	button enter("введите имя", 30, 0, 200, render, screenSurface);
	button truenter("готово", 30, 0, 361, render, screenSurface);
	enter.SetTexture("images/message.png", 328, 207);
	enter.SetCenterOffset(0);
	enter.SetY(208);
	truenter.SetCenterOffset(0);
	dynamicTextWindow nametest(36, 0, 280, render, screenSurface);

	nametest.SetColor({ 0,0,0 });
	nametest.SetCentered();
	SDL_Event even;
	
	char vibor = -1;
	bool quit = true,
		 zalip = true, 
		 start = true;
	while (quit && start)
	{
		while (SDL_PollEvent(&even))
		{
			if (even.type == SDL_QUIT) { quit = false; nametest.SetText("def"); }
			if (even.type == SDL_MOUSEBUTTONDOWN)
			{
				vibor = enter.GetClick();
				start = truenter.GetClick() == -1 || !nametest.GetLength() ? start : false;
			}
			if (vibor != -1)
			{
				if (even.type == SDL_TEXTINPUT && !keys[SDL_SCANCODE_SPACE]) nametest.Write(&even);
				if (even.key.keysym.sym == SDLK_BACKSPACE) { if (zalip) nametest.DeleteSymbol(); zalip = !zalip; }
			}
		}
		background->render(render);
		enter.render(render);
		nametest.render(render);
		truenter.render(render);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(30);
	}
	return { 0, nametest.GetText(), quit };
}

gameResults startScreen()
{
	staticGraficalElement background({ 0,0,WINDOW_WIDTH,WINDOW_HEIGHT }, "images/screenSaver.png", render); //фон
	
	textWindow name("О727Б Виноградов С.Д.", 35, 4, WINDOW_HEIGHT - 50, render, screenSurface);
	textWindow title("Шипс дестроер", 60, 0, 300, render, screenSurface);
	title.SetCenterOffset(0);
	title.SetColor({ 0,0,0 });
	name.SetColor({ 0,0,0 });

	
	gameResults data;
	data.quit = true;
	SDL_Event even;
	char start = 2;

	while (data.quit && start > 0)
	{
		while (SDL_PollEvent(&even))
		{
			if (even.type == SDL_QUIT)
				data.quit = false;
			if (even.type == SDL_MOUSEBUTTONDOWN)
			if (--start)
			{
				data = NameInput(&background);
				return data;
			}
		}
		background.render(render);
		title.render(render);
		name.render(render);

		SDL_RenderPresent(render);
		SDL_RenderClear(render);

		SDL_Delay(30);
	}

	return data;
}

void MainMenu(string nname, scoreData** scoresList)
{
	
	staticGraficalElement menufon({ 0,0,WINDOW_WIDTH,WINDOW_HEIGHT }, "images/screenSaver.png", render);
	//Кнопки
	button** myButton;
	myButton = new button*[7];

	const char* buttonsText[] = { "Назад", "Начало", "Справка", "Рекорды", "Смена игрока", "Выход"};
	for (int i = 0; i < 6; i++)
	{
		myButton[i] = new button(buttonsText[i], 40, 20, 230 + 45 * i, render, screenSurface);
		myButton[i]->SetIndex(i);
		myButton[i]->SetColor({ 0,0,0 });
	}

	scoreData tmp = { 0, nname };
	gameResults f = {0, nname, true};
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
					vibor = myButton[0]->GetClick() == -1 ? vibor : myButton[0]->GetIndex();
				}
				for ( i = 1; i < 6 && vibor == -1; i++ )
				{
					vibor = myButton[i]->GetClick() == -1 ? vibor : myButton[i]->GetIndex();
				}
			}
		}
		switch (vibor)
		{
		case 5:
			for (i = 0; i < 6; i++)
				delete myButton[i];
			delete[] myButton;
			return;
		case 2:
			quit = ShowGuide(&menufon);
			vibor = -1;
			break;
		case 3:
			quit = ShowRecords(scoresList, &menufon);
			vibor = -1;
			break;
		case 4:
			f = NameInput(&menufon);
			vibor = -1;
			tmp.username = f.username;
			if (!f.quit) return;
			break;
		case 1:
			f = Game();
			quit = f.quit;
			tmp.points = f.scores;
			AddRecord(scoresList, tmp);
			if (!f.quit) return;
		}
		menufon.render(render);
		for (int i = 1; i < 6; i++)
			myButton[i]->render(render);
		vibor = -1;
		SDL_RenderPresent(render);
		SDL_RenderClear(render);
		SDL_Delay(40);
	}
	for (i = 0; i < 6; i++)
		delete myButton[i];
	delete[] myButton;
}

ship* MakeNewShip(const char* types[], const int scores[], SDL_Texture* image)
{
	char i = rand() % 100; //Для выбора создаваемого объекта
	
	bool naprav = rand() % 2; //Лево или право
	char speed = 2 + rand() % 7; //скорость
	int x = naprav ? -180 : WINDOW_WIDTH;
	ship* q = NULL;
	char vid = 0;
	if (i < 30) vid = 0;
	if (i >= 30 && i < 50) vid = 1;
	if (i >= 50 && i < 65) { vid = 2;}
	if (i >= 65 && i < 80) vid = 3;
	if (i >= 80 && i < 92) vid = 4;
	if (i >= 92)
	if (!(rand() % 3)) vid = 5;
	SDL_Rect dest = { x, WATER_LINE - 90, 180, 90 };
	image = IMG_LoadTexture(render, types[vid]);
	if (naprav) q = new Rship(dest, image, speed, scores[vid]);
	else q = new Lship(dest, image, speed, scores[vid]);
	return q;
}

gameResults Game()
{
	const char* files[] = { "images/ships/boat_s.png", "images/ships/boat_s.png", "images/ships/small_ship_s.png", "images/ships/small_ship_s.png", "images/ships/small_ship_s.png", "images/ships/ship_s.png" };
	const int points[] = { 1,1,1,2,2,3};

	staticGraficalElement background({ 0,0,WINDOW_WIDTH,WINDOW_HEIGHT }, "images/background.png", render);
	staticGraficalElement pausetex({ HALF_WINDOW_WIDTH - 125, 200, 250, 300 }, "textures/pause.png", render);

	SDL_Event event;
	SDL_PollEvent(&event);

	//Крюк игрока
	SDL_Texture* test = NULL;
	rocket* torpeda = new rocket(IMG_LoadTexture(render, "images/torpedo.png"), 30);
	submarine* player = new submarine({HALF_WINDOW_WIDTH - 30, WATER_LINE + 200, 90, 180}, IMG_LoadTexture(render, "images/oceanGateR.png"), 10); // создаём субмарину

	bool quit = true;	//Флаг выхода
	bool eflag = true;	//Флаг завершения игрового цикла
	bool paused = false; // флаг паузы
	int torpedos = 10; // количество торпед
	short int score = 0; // очки
	int fflag = 0;

	int i, tmp;
	ship* g[15];
	for (int i = 0; i < 15; i++)
		g[i] = NULL;

	textWindow pauset("ПАУЗА", 40, 0, 210, render, screenSurface);
	textWindow scores("0", 40, 10, 20, render, screenSurface);			//Счёт 
	textWindow torpsText("Торпеды:", 40, 80, 20, render, screenSurface);
	textWindow torps("10", 40, 270, 20, render, screenSurface);			//торпеды
	pauset.SetCenterOffset(0);
	torpsText.SetColor({ 0,0,0 });
	scores.SetColor({ 0, 0, 0 });
	torps.SetColor({ 0,0,0 });

	button** myButton = new button*[5];
	const char* c[] = { "Продолжить", "Справка", "Выход" };
	myButton[0] = new button("", 2, HALF_WINDOW_WIDTH + 400, 4, render, screenSurface);		
	for (i = 0; i < 3; i++)
	{
		myButton[i + 2] = new button(c[i], 40, 0, 254 + 44 * i, render, screenSurface);
		myButton[i + 2]->SetCenterOffset(0);
	}

	myButton[0]->SetTexture("textures/pb.png", 50, 50);

	//игра
	while (quit && eflag) 
	{
		background.render(render);
		player->render(render);
		if (torpeda->GetLaunched()) // если торпеда запущена
		{
			torpeda->render(render); 
			if(torpeda->MoveU()) 
			{
				torpedos--;
				torps.SetText(to_string(torpedos));

				if (torpedos > 0) // если ещё есть торпеды
				{
					torpeda->SetY(WINDOW_HEIGHT - 225);
					torpeda->SetLaunched(false);
					torpeda->SetCollision(false);
				}
			}
		}
		else
			torpeda->SetX(player->GetX());

		
		myButton[0]->render(render);
		torpsText.render(render);
		scores.render(render);
		torps.render(render);
		eflag = eflag ? torpedos>0 : eflag;
	
		while (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT)
				quit = false;
			if (event.type == SDL_KEYDOWN)
			{
				if (keys[SDL_SCANCODE_RETURN]) {  paused = true; }
				if (!paused)
				{
					if (keys[SDL_SCANCODE_SPACE]) // если нажали SPACE
					{
						if(!torpeda->GetLaunched()) // если торпеда не летит
							torpeda->MoveU(); // запускаем
					}
					if(keys[SDL_SCANCODE_D])
					{
						player->MoveR();
					}
					if (keys[SDL_SCANCODE_A])
					{
						player->MoveL();
					}
				}
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (!paused) 
				{
					if (myButton[0]->GetClick() != -1)  paused = true;
				}
				else
				{
					eflag = myButton[4]->GetClick() == -1 ? eflag : false;
					if (myButton[2]->GetClick() != -1)  paused = false;
					if (myButton[3]->GetClick() != -1) {ShowGuide(&background); background.render(render);}
				}
			}
		}
		//Движение кораблей, их создание, поимка и отрисовка
		if (!paused)
		{
			for (i = 0; i < 10; i++)
			{
				if (g[i] == NULL) // если корабля нет
				{
					if (rand() % 900 == 1) g[i] = MakeNewShip(files, points, test); //Создание нового корабля
				}
				else //если корабль есть
				{
					g[i]->render(render); //отрисовка
					fflag = g[i]->Move( torpeda->GetX(), torpeda->GetY(),torpeda->GetW(), torpeda->GetH(), torpeda->GetSpeed(), torpeda->GetCollision()); //Движение корабля // +29
					if (fflag == -1) //проверка на уничтожение
					{
						torpeda->SetCollision(true);
					}
					if (fflag == 0 || fflag == -1) //проверка на уход за край/ подсчёт очков
					{
						
						tmp = score;
						score += g[i]->GetPoint(); // получаем очки, если есть корабль touched
						scores.SetText(to_string(score));
						if (score != tmp)
						{
							torpedos--; // уменьшаем счётчик
							torps.SetText(to_string(torpedos));
							
							if (torpedos > 0) // если ещё есть торпеды
							{
								torpeda->SetY(WINDOW_HEIGHT - 225);
								torpeda->SetLaunched(false);
								torpeda->SetCollision(false);
							}
						}
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
				myButton[i]->render(render);
		}
		SDL_RenderPresent(render);
		SDL_RenderClear(render);

		SDL_Delay(20);
	}
	SDL_DestroyTexture(test);
	SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
	textWindow escore(to_string(score), 40, 0, 492, render, screenSurface);	//Счёт
	textWindow logo("Отличная игра", 40, 0, 600, render, screenSurface);		
	escore.SetCenterOffset(0);
	logo.SetCenterOffset(0);
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
		background.render(render);

		escore.render(render);
		logo.render(render);
		SDL_RenderPresent(render);
		SDL_RenderClear(render);
	}
	return { score, "", quit};
}
