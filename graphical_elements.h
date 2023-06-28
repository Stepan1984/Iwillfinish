#pragma once
#include <SDL.h>
#include <string>
#include "globals.h"
#include "dynamic_element.h" // ������������ ���� � ������������ ������������ �������� ������ ����������� �������� (�� ����� ���� ������ ��������� � ���� ����) ��-��
class static_graphical_element //����������� ����������� ������� (����������� ������� �����)
{
public:
	virtual void SetX(const int) = 0; // ���������� ������� �� X �� �������
	virtual void SetY(const int) = 0; // ���������� ������� �� Y �� �������
	virtual void SetSize(const int, const int) = 0; // ���������� ������ ������� �� ������� (width, heigh)
	virtual void SetTexture(SDL_Texture *) = 0; // ���������� �������� (SDL_Texture pointer)
	virtual void SetFlip(SDL_RendererFlip newFlip) = 0; // ���������� flip
	virtual void LoadTexture(std::string path, SDL_Renderer* gRenderer) = 0; // �������� �������� �� �����
	virtual int GetLeftX() = 0; // �������� X ����� ������� �������� �� �������
	virtual int GetRightX() = 0; // �������� X ������ ������� �������� �� �������
	virtual int GetUpperY() = 0; // �������� Y ������� ������� �������� �� �������
	virtual int GetLowerY() = 0; // �������� Y ������ ������� �������� �� �������
	virtual int GetWidth() = 0;
	virtual int GetHeight() =0;
	virtual SDL_Texture* GetTexture() = 0; // �������� ��������
	virtual SDL_RendererFlip GetFlip() = 0; // �������� flip 
	virtual void Render(SDL_Renderer * gRenderer) = 0; // ���������� �� �������
	virtual void FreeTexture() = 0; // ����� ��������� �� ��������
	
};

class rectangle : static_graphical_element // ����� "�������������"
{
protected:
	SDL_Rect rect; // ������� � ������� �� �������
	SDL_Texture* sprite; // ��������
	SDL_RendererFlip flip;
public:
	//rectangle();//�����������
	rectangle(SDL_Rect rect, std::string path, SDL_Renderer* render); // ����������� � �����������
	~rectangle(); // ����������
	rectangle(const rectangle& other); // ����������� �����������
	void SetX(const int x) override; // ���������� ������� �� X �� �������
	void SetY(const int y) override; // ���������� ������� �� Y �� �������
	void SetSize(const int w, const int h) override; // ���������� ������ ������� �� ������� (width, heigh)
	void SetTexture(SDL_Texture* newImage) override; // ���������� ��������
	void SetFlip(SDL_RendererFlip newFlip) override; //���������� 
	void LoadTexture(std::string path, SDL_Renderer* gRenderer) override; // �������� �������� �� �����
	int GetLeftX() override; // �������� X ����� ������� �������� �� �������
	int GetRightX() override; // �������� X ������ ������� �������� �� �������
	int GetUpperY() override; // �������� Y ������� ������� �������� �� �������
	int GetLowerY() override; // �������� Y ������ ������� �������� �� �������
	int GetWidth() override;
	int GetHeight() override;
	SDL_Texture* GetTexture() override; // �������� ��������� �� ��������
	SDL_RendererFlip GetFlip() override; // �������� flip 
	void Render(SDL_Renderer* gRenderer) override; // ���������� �� �������
	void FreeTexture() override; // ����� ��������
};

class dynamic_graphical_element : public rectangle, dynamic_element // ������������ ����������� ��������
{
public:
	//dynamic_graphical_element(); // �����������
	dynamic_graphical_element(SDL_Renderer* render, SDL_Rect newRect, std::string newTexturePath, int newXSpeed, int newYspeed); // ����������� � �����������
	~dynamic_graphical_element(); // ����������
	dynamic_graphical_element(dynamic_graphical_element& other); // ����������� �����������
	void Move(const int XStepsAmount, const int YStepsAmount) override; // ��������� �� ���������� ����� �� ���� X � Y
	void SetXSpeed(const int speed) override; // ���������� ������ ���� �� ��� X
	void SetYSpeed(const int speed) override; // ���������� ������ ���� �� ��� Y
	int GetXSpeed();
	int GetYSpeed();
};

class ship :public dynamic_graphical_element // ����� - �������
{
private:
	int points;
	bool shot;
public:
	ship(); // �����������
	ship(SDL_Renderer* render, SDL_Rect newRect, std::string newTexturePath, int newXSpeed, int newYspeed, int newPoints);
	~ship(); // ����������
	void SetPoints(int points); // ���������� ���������� ����� �� �������
	void SetShot(bool flag);
	void Move(const int XStepsAmount, const int YStepsAmount) override; // ��������� �� ���������� ����� �� ���� X � Y
	bool GetShot();
	int GetPoints(); // �������� ���������� ����� �� ����������� �������
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





