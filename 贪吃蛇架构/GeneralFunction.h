#pragma once 
#include <conio.h>
#include <functional>
#include <graphics.h>
#include <Windows.h>
#include <mmsystem.h>  // Windows 音频头文件
#pragma comment(lib, "Winmm.lib") // 链接音频库
bool CheckButtonClick(int x, int y, const RECT& rect);
void DrawButton(const RECT& rect, COLORREF color, const TCHAR* text, bool isHover);
void DrawButton(const RECT& rect, COLORREF color, const TCHAR* text);
struct ToggleSwitch
{
    bool isOn;             // 开关状态（true=开启）
    int posX;              // 滑轨中心X坐标
    int posY;              // 滑轨中心Y坐标
    int trackWidth;        // 滑轨总宽度
    COLORREF onColor;      // 开启状态颜色
    COLORREF offColor;     // 关闭状态颜色
};
void drawToggleSwitch(const ToggleSwitch& sw, bool isPressed);
void drawTextLabel(const TCHAR* text, int x, int y);
bool checkToggleClick(const ToggleSwitch& sw, int mouseX, int mouseY); 
void button(int x, int y, int w, int h, char *text);
bool checkIsHover(int x, int y, const RECT& rect);
void playBackMusic(bool musicOfAll);
void playSoundDie(bool musicOfDie, bool soundeEfect);
void playSoundEat(bool musicOfEat, bool soundEffect);


