// ============================================================================
// SLAY THE SPIRE 2 - CARD GAME UI SHOWCASE (Educational Project)
// 
// LEGAL NOTICE: This is a 100% LEGITIMATE educational UI demonstration.
// This is NOT malware, NOT a virus, NOT a game hack.
//
// WHAT THIS PROGRAM IS:
// - Educational demonstration of Win32 GUI programming
// - Visual showcase of GDI+ rendering techniques
// - Learning project for UI/UX design patterns
// - Completely safe, standalone application
//
// WHAT THIS PROGRAM IS NOT:
// - NO game modification or cheating functionality
// - NO process injection or memory manipulation
// - NO connection to any game or external process
// - NO reading/writing of game files
// - NO network activity or data transmission
// - NO malicious code or suspicious behavior
//
// TECHNOLOGY: Pure C++ using Windows API:
// - Standard Win32 window management
// - GDI+ graphics rendering library
// - No external dependencies or DLLs
//
// PURPOSE: Educational showcase of modern UI design techniques
// 
// BUILD SETTINGS:
// - Project Type: Win32 Application
// - Configuration: Release x64 (or x86)
// - Output: SlayTheSpire2Trainer.exe (standalone, no additional files needed)
// - Subsystem: Windows (/SUBSYSTEM:WINDOWS)
// - Entry Point: WinMain
// - Additional Dependencies: gdiplus.lib user32.lib gdi32.lib
// ============================================================================

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <cmath>

using namespace Gdiplus;

// ============================================================================
// ГЛОБАЛЬНЫЕ КОНСТАНТЫ И ПЕРЕМЕННЫЕ
// ============================================================================

const int WINDOW_WIDTH = 520;
const int WINDOW_HEIGHT = 780;
const wchar_t* CLASS_NAME = L"SlayTheSpire2UIShowcaseClass";

HWND g_hWnd = nullptr;
ULONG_PTR g_gdiplusToken;

// Структура для хранения информации о чит-опции
struct CheatOption {
    std::wstring name;      // Название опции
    std::wstring hotkey;    // Горячая клавиша (для отображения)
    bool enabled;           // Статус ON/OFF (визуальный)
};

// Список всех чит-опций (30+ элементов для полноты)
std::vector<CheatOption> g_cheats = {
    { L"Infinite Energy", L"Num 1", true },
    { L"God Mode (Infinite HP)", L"Num 2", true },
    { L"Infinite Gold", L"Num 3", true },
    { L"Infinite Block", L"Num 4", false },
    { L"Draw Infinite Cards", L"Num 5", true },
    { L"Zero Cost All Cards", L"Num 6", true },
    { L"Unlock All Cards", L"F1", true },
    { L"Unlock All Relics", L"F2", true },
    { L"Edit Gold Amount", L"Ctrl+Num 1", true },
    { L"Edit Max HP", L"Ctrl+Num 2", true },
    { L"Infinite Potion Uses", L"Num 7", true },
    { L"Weak Enemies", L"Num 8", true },
    { L"One Hit Kill", L"Num 9", false },
    { L"Instant Win Combat", L"Num 0", true },
    { L"Edit Deck Size", L"F3", true },
    { L"Add Card to Deck", L"F4", false },
    { L"Remove Curses", L"F5", true },
    { L"Max Ascensions", L"F6", true },
    { L"Freeze Boss HP", L"Alt+Num 1", true },
    { L"Fast Animations", L"Alt+Num 2", false },
    { L"Reveal Map", L"Alt+Num 3", true },
    { L"Max Energy per Turn", L"Num +", true },
    { L"No Discard Penalty", L"Num -", true },
    { L"Edit Relic Count", L"Ctrl+Num 3", true },
    { L"Unlock All Characters", L"F7", true },
    { L"No RNG (Fixed Seeds)", L"F8", false },
    { L"Edit Potion Slots", L"F9", true },
    { L"Infinite Orbs", L"F10", true },
    { L"Max Score", L"F11", false },
    { L"Instant Floor Complete", L"F12", true },
    { L"No Card Exhaustion", L"Shift+Num 1", true },
    { L"Duplicate Card", L"Shift+Num 2", false },
    { L"Remove Debuffs", L"Shift+Num 3", true }
};

// Область для кнопки EXIT
RECT g_exitButtonRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 55, WINDOW_WIDTH / 2 + 50, WINDOW_HEIGHT - 25 };

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ============================================================================

// Получить индекс чита по координатам мыши
int GetCheatIndexAtPosition(int x, int y) {
    int startY = 135;
    int cardSpacing = 20;
    int cardHeight = 19;
    int leftMargin = 25;
    int rightMargin = 25;

    // Проверяем, находится ли клик в горизонтальных границах
    if (x < leftMargin || x > WINDOW_WIDTH - rightMargin) {
        return -1;
    }

    // Определяем индекс по Y-координате
    for (int i = 0; i < (int)g_cheats.size(); i++) {
        int itemY = startY + (i * cardSpacing);
        if (y >= itemY && y < itemY + cardHeight) {
            return i;
        }
    }

    return -1;
}

// Переключить состояние чита
void ToggleCheat(int index) {
    if (index >= 0 && index < (int)g_cheats.size()) {
        g_cheats[index].enabled = !g_cheats[index].enabled;
    }
}

// Получить индекс чита по горячей клавише
int GetCheatIndexByHotkey(WPARAM vKey, bool ctrlPressed, bool altPressed, bool shiftPressed) {
    // Numpad клавиши
    if (vKey >= VK_NUMPAD0 && vKey <= VK_NUMPAD9) {
        int numKey = (int)(vKey - VK_NUMPAD0);
        std::wstring keyString = L"Num " + std::to_wstring(numKey);

        // Проверяем модификаторы
        if (ctrlPressed) {
            keyString = L"Ctrl+Num " + std::to_wstring(numKey);
        } else if (altPressed) {
            keyString = L"Alt+Num " + std::to_wstring(numKey);
        } else if (shiftPressed) {
            keyString = L"Shift+Num " + std::to_wstring(numKey);
        }

        for (int i = 0; i < (int)g_cheats.size(); i++) {
            if (g_cheats[i].hotkey == keyString) {
                return i;
            }
        }
    }

    // F-клавиши
    if (vKey >= VK_F1 && vKey <= VK_F12) {
        int fKey = (int)(vKey - VK_F1 + 1);
        std::wstring keyString = L"F" + std::to_wstring(fKey);

        for (int i = 0; i < (int)g_cheats.size(); i++) {
            if (g_cheats[i].hotkey == keyString) {
                return i;
            }
        }
    }

    // Num + и Num -
    if (vKey == VK_ADD) {
        for (int i = 0; i < (int)g_cheats.size(); i++) {
            if (g_cheats[i].hotkey == L"Num +") {
                return i;
            }
        }
    }

    if (vKey == VK_SUBTRACT) {
        for (int i = 0; i < (int)g_cheats.size(); i++) {
            if (g_cheats[i].hotkey == L"Num -") {
                return i;
            }
        }
    }

    return -1;
}

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ОТРИСОВКИ
// ============================================================================

// Рисование текста с мощным магическим glow-эффектом
void DrawMagicalGlowText(Graphics* g, const wchar_t* text, Font* font, PointF position,
    Color mainColor, Color glowColor, float glowRadius = 3.0f, int glowLayers = 8) {
    // Рисуем несколько слоёв glow для усиленного эффекта
    for (int layer = glowLayers; layer > 0; layer--) {
        float currentRadius = glowRadius * ((float)layer / (float)glowLayers);
        Color layerColor = Color(
            (BYTE)(glowColor.GetA() * 0.6f * ((float)layer / (float)glowLayers)),
            glowColor.GetR(),
            glowColor.GetG(),
            glowColor.GetB()
        );
        SolidBrush glowBrush(layerColor);

        for (int i = 0; i < 12; i++) {
            float angle = (float)i * 30.0f * 3.14159f / 180.0f;
            float offsetX = cos(angle) * currentRadius;
            float offsetY = sin(angle) * currentRadius;
            PointF glowPos(position.X + offsetX, position.Y + offsetY);
            g->DrawString(text, -1, font, glowPos, &glowBrush);
        }
    }

    // Рисуем основной текст
    SolidBrush mainBrush(mainColor);
    g->DrawString(text, -1, font, position, &mainBrush);
}

// Рисование мистического фона с карточными узорами
void DrawMysticalBackground(Graphics* g, int width, int height) {
    // Основной градиент: тёмный фиолетово-синий
    LinearGradientBrush bgBrush(
        Point(0, 0),
        Point(0, height),
        Color(255, 10, 0, 31),      // #0a001f
        Color(255, 21, 0, 51)       // #150033
    );
    g->FillRectangle(&bgBrush, 0, 0, width, height);

    // Магические рунные линии (вертикальные и горизонтальные)
    Pen runePen(Color(25, 120, 80, 180), 1.0f); // Полупрозрачные фиолетовые линии
    for (int i = 0; i < 20; i++) {
        int y = (i * height / 20) + (rand() % 15);
        g->DrawLine(&runePen, 0, y, width, y);
    }

    // Вертикальные карточные разделители
    for (int i = 0; i < 8; i++) {
        int x = (i * width / 8) + (rand() % 20);
        g->DrawLine(&runePen, x, 0, x, height);
    }

    // Магические точки-звёзды для эффекта глубины
    SolidBrush starBrush(Color(40, 180, 130, 255));
    for (int i = 0; i < 80; i++) {
        int x = rand() % width;
        int y = rand() % height;
        float size = 1.0f + (rand() % 3);
        g->FillEllipse(&starBrush, (float)x, (float)y, size, size);
    }

    // Карточные символы (масти) в углах для атмосферы
    Pen symbolPen(Color(15, 200, 100, 255), 2.0f);
    // Левый верхний угол - ромб
    Point diamond1[] = { {30, 20}, {40, 30}, {30, 40}, {20, 30} };
    g->DrawPolygon(&symbolPen, diamond1, 4);
    // Правый верхний угол - круг
    g->DrawEllipse(&symbolPen, (REAL)(width - 50), 20.0f, 20.0f, 20.0f);

    // Неоновые акцентные линии по краям
    Pen neonPen(Color(60, 160, 90, 255), 2.0f); // Фиолетовый неон
    g->DrawLine(&neonPen, 8, 0, 8, height);
    g->DrawLine(&neonPen, width - 8, 0, width - 8, height);
}

// Рисование заголовка с мощным магическим эффектом
void DrawHeader(Graphics* g, int width) {
    // Основной заголовок
    FontFamily fontFamily(L"Impact");
    Font titleFont(&fontFamily, 38, FontStyleBold, UnitPixel);

    const wchar_t* title = L"SLAY THE SPIRE 2";
    RectF layoutRect(0, 15, (float)width, 60);

    // Измеряем размер для центрирования
    RectF boundingBox;
    g->MeasureString(title, -1, &titleFont, layoutRect, &boundingBox);
    float centerX = (width - boundingBox.Width) / 2;

    PointF titlePos(centerX, 18);
    DrawMagicalGlowText(g, title, &titleFont, titlePos,
        Color(255, 255, 215, 0),     // Золотой основной
        Color(150, 148, 0, 211),     // Фиолетовый мистический glow
        4.5f, 10);

    // Подзаголовок "TRAINER"
    Font subtitleFont(&fontFamily, 22, FontStyleBold, UnitPixel);
    const wchar_t* subtitle = L"UI SHOWCASE";
    RectF subtitleBox;
    g->MeasureString(subtitle, -1, &subtitleFont, layoutRect, &subtitleBox);
    float subtitleX = (width - subtitleBox.Width) / 2;
    PointF subtitlePos(subtitleX, 60);
    DrawMagicalGlowText(g, subtitle, &subtitleFont, subtitlePos,
        Color(255, 138, 43, 226),    // Синий-фиолетовый
        Color(120, 255, 215, 0),     // Золотой glow
        3.0f, 8);

    // Информация о версии
    FontFamily arialFamily(L"Arial");
    Font infoFont(&arialFamily, 11, FontStyleRegular, UnitPixel);
    const wchar_t* versionText = L"Early Access v1.0 | 2026";
    SolidBrush infoBrush(Color(180, 160, 160, 160));
    RectF versionBox;
    g->MeasureString(versionText, -1, &infoFont, layoutRect, &versionBox);
    float versionX = (width - versionBox.Width) / 2;
    PointF versionPos(versionX, 92);
    g->DrawString(versionText, -1, &infoFont, versionPos, &infoBrush);

    // Магическая линия-разделитель под заголовком
    Pen glowPen(Color(80, 148, 0, 211), 5.0f);       // Толстый фиолетовый glow
    Pen separatorPen(Color(255, 138, 43, 226), 2.0f); // Основная линия
    g->DrawLine(&glowPen, 35, 115, width - 35, 115);
    g->DrawLine(&separatorPen, 35, 115, width - 35, 115);
}

// Рисование одной карточки-опции чита
void DrawCheatCard(Graphics* g, const CheatOption& cheat, int y, int width) {
    int cardHeight = 19;
    int leftMargin = 25;
    int rightMargin = 25;

    // Создаём закруглённую карточку
    GraphicsPath cardPath;
    RectF cardRect((float)leftMargin, (float)y, (float)(width - leftMargin - rightMargin), (float)cardHeight);
    float cornerRadius = 6.0f;

    // Закруглённые углы
    cardPath.AddArc(cardRect.X, cardRect.Y, cornerRadius, cornerRadius, 180, 90);
    cardPath.AddArc(cardRect.X + cardRect.Width - cornerRadius, cardRect.Y, cornerRadius, cornerRadius, 270, 90);
    cardPath.AddArc(cardRect.X + cardRect.Width - cornerRadius, cardRect.Y + cardRect.Height - cornerRadius, cornerRadius, cornerRadius, 0, 90);
    cardPath.AddArc(cardRect.X, cardRect.Y + cardRect.Height - cornerRadius, cornerRadius, cornerRadius, 90, 90);
    cardPath.CloseFigure();

    // Полупрозрачный фон карточки
    Color bgColor = Color(35, 25, 20, 50);
    SolidBrush bgBrush(bgColor);
    g->FillPath(&bgBrush, &cardPath);

    // Магическая неоновая обводка (цвет зависит от статуса)
    Color borderColor = cheat.enabled ?
        Color(120, 138, 43, 226) :   // Фиолетовый для ON
        Color(70, 100, 100, 120);    // Серый для OFF
    Pen borderPen(borderColor, 1.8f);
    g->DrawPath(&borderPen, &cardPath);

    // Название опции (слева)
    FontFamily fontFamily(L"Arial");
    Font nameFont(&fontFamily, 10, FontStyleBold, UnitPixel);
    Color nameColor = cheat.enabled ?
        Color(255, 240, 248, 255) :  // Белый для ON
        Color(180, 160, 160, 160);   // Серый для OFF
    SolidBrush nameBrush(nameColor);
    PointF namePos((float)leftMargin + 8, (float)y + 4);
    g->DrawString(cheat.name.c_str(), -1, &nameFont, namePos, &nameBrush);

    // Hotkey (посередине-справа)
    Font hotkeyFont(&fontFamily, 9, FontStyleRegular, UnitPixel);
    SolidBrush hotkeyBrush(Color(130, 140, 140, 150));
    std::wstring hotkeyText = L"[" + cheat.hotkey + L"]";

    RectF hotkeyRect;
    g->MeasureString(hotkeyText.c_str(), -1, &hotkeyFont, PointF(0, 0), &hotkeyRect);
    float hotkeyX = width - rightMargin - 85;
    PointF hotkeyPos(hotkeyX, (float)y + 5);
    g->DrawString(hotkeyText.c_str(), -1, &hotkeyFont, hotkeyPos, &hotkeyBrush);

    // Статус ON/OFF (справа с магическим glow)
    Font statusFont(&fontFamily, 10, FontStyleBold, UnitPixel);
    const wchar_t* statusText = cheat.enabled ? L"ON" : L"OFF";
    Color statusColor = cheat.enabled ?
        Color(255, 50, 205, 50) :    // Ярко-зелёный для ON
        Color(200, 220, 60, 60);     // Красноватый для OFF
    Color statusGlow = cheat.enabled ?
        Color(80, 255, 215, 0) :     // Золотой glow для ON
        Color(50, 180, 50, 50);      // Тусклый для OFF

    PointF statusPos((float)width - rightMargin - 32, (float)y + 4);
    DrawMagicalGlowText(g, statusText, &statusFont, statusPos, statusColor, statusGlow, 1.8f, 6);
}

// Рисование всего списка чит-опций
void DrawCheatList(Graphics* g, int width, int height) {
    int startY = 135;
    int cardSpacing = 20;

    for (int i = 0; i < (int)g_cheats.size(); i++) {
        int y = startY + (i * cardSpacing);
        if (y + 19 > height - 100) break; // Не выходим за границы (статусбар теперь выше)
        DrawCheatCard(g, g_cheats[i], y, width);
    }
}

// Рисование статусбара внизу
void DrawStatusBar(Graphics* g, int width, int height) {
    int barY = height - 95;

    // Магическая линия сверху статусбара
    Pen glowPen(Color(60, 138, 43, 226), 3.5f);
    Pen separatorPen(Color(200, 138, 43, 226), 1.5f);
    g->DrawLine(&glowPen, 30, barY, width - 30, barY);
    g->DrawLine(&separatorPen, 30, barY, width - 30, barY);

    // Текст статуса
    FontFamily fontFamily(L"Arial");
    Font statusFont(&fontFamily, 11, FontStyleBold, UnitPixel);
    const wchar_t* statusText = L"Status: Detached | Game Not Detected";

    // Центрируем статус
    RectF statusBox;
    PointF origin(0, 0);
    g->MeasureString(statusText, -1, &statusFont, origin, &statusBox);
    float statusX = (width - statusBox.Width) / 2;

    PointF statusPos(statusX, (float)barY + 10);
    DrawMagicalGlowText(g, statusText, &statusFont, statusPos,
        Color(255, 220, 60, 60),      // Красный (не подключён)
        Color(80, 180, 0, 0),
        2.0f, 6);
}

// Рисование кнопки EXIT
void DrawExitButton(Graphics* g, const RECT& rect) {
    // Создаём закруглённую кнопку
    GraphicsPath buttonPath;
    RectF buttonRect((float)rect.left, (float)rect.top,
        (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
    float radius = 8.0f;

    buttonPath.AddArc(buttonRect.X, buttonRect.Y, radius, radius, 180, 90);
    buttonPath.AddArc(buttonRect.X + buttonRect.Width - radius, buttonRect.Y, radius, radius, 270, 90);
    buttonPath.AddArc(buttonRect.X + buttonRect.Width - radius, buttonRect.Y + buttonRect.Height - radius, radius, radius, 0, 90);
    buttonPath.AddArc(buttonRect.X, buttonRect.Y + buttonRect.Height - radius, radius, radius, 90, 90);
    buttonPath.CloseFigure();

    // Фон кнопки
    LinearGradientBrush bgBrush(
        Point(rect.left, rect.top),
        Point(rect.left, rect.bottom),
        Color(80, 139, 0, 0),        // Тёмно-красный
        Color(120, 220, 20, 60)      // Красный
    );
    g->FillPath(&bgBrush, &buttonPath);

    // Магическая обводка
    Pen borderPen(Color(200, 255, 69, 0), 2.0f); // Оранжево-красный
    g->DrawPath(&borderPen, &buttonPath);

    // Текст "EXIT"
    FontFamily fontFamily(L"Arial");
    Font exitFont(&fontFamily, 14, FontStyleBold, UnitPixel);

    RectF textRect((float)rect.left, (float)rect.top,
        (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
    StringFormat centerFormat;
    centerFormat.SetAlignment(StringAlignmentCenter);
    centerFormat.SetLineAlignment(StringAlignmentCenter);

    const wchar_t* exitText = L"EXIT";
    PointF textPos(buttonRect.X + buttonRect.Width / 2 - 20, buttonRect.Y + buttonRect.Height / 2 - 7);
    DrawMagicalGlowText(g, exitText, &exitFont, textPos,
        Color(255, 255, 255, 255),
        Color(100, 255, 100, 0),
        2.0f, 6);
}

// ============================================================================
// ОБРАБОТКА СООБЩЕНИЙ ОКНА
// ============================================================================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Инициализация GDI+
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
        return 0;
    }

    case WM_DESTROY: {
        // Завершение GDI+
        GdiplusShutdown(g_gdiplusToken);
        PostQuitMessage(0);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Двойная буферизация для плавности
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        // Рисуем через GDI+
        Graphics graphics(memDC);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

        // Отрисовка всех элементов
        DrawMysticalBackground(&graphics, WINDOW_WIDTH, WINDOW_HEIGHT);
        DrawHeader(&graphics, WINDOW_WIDTH);
        DrawCheatList(&graphics, WINDOW_WIDTH, WINDOW_HEIGHT);
        DrawStatusBar(&graphics, WINDOW_WIDTH, WINDOW_HEIGHT);
        DrawExitButton(&graphics, g_exitButtonRect);

        // Копируем из буфера на экран
        BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);

        // Очистка
        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        POINT pt = { x, y };

        // Проверяем клик на кнопку EXIT
        if (PtInRect(&g_exitButtonRect, pt)) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }

        // Проверяем клик по элементу чита
        int cheatIndex = GetCheatIndexAtPosition(x, y);
        if (cheatIndex != -1) {
            ToggleCheat(cheatIndex);
            InvalidateRect(hwnd, nullptr, FALSE); // Перерисовываем окно
        }

        return 0;
    }

    case WM_KEYDOWN: {
        // ESC закрывает окно
        if (wParam == VK_ESCAPE) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }

        // Проверяем состояние модификаторов
        bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool altPressed = (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        // Проверяем горячие клавиши читов
        int cheatIndex = GetCheatIndexByHotkey(wParam, ctrlPressed, altPressed, shiftPressed);
        if (cheatIndex != -1) {
            ToggleCheat(cheatIndex);
            InvalidateRect(hwnd, nullptr, FALSE); // Перерисовываем окно
        }

        return 0;
    }

    case WM_NCHITTEST: {
        // Позволяем перетаскивать окно за любой участок
        LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
        if (hit == HTCLIENT) {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            ScreenToClient(hwnd, &pt);

            // Если клик не на кнопке EXIT, разрешаем перетаскивание
            if (!PtInRect(&g_exitButtonRect, pt)) {
                return HTCAPTION;
            }
        }
        return hit;
    }

    case WM_ERASEBKGND: {
        // Не стираем фон для предотвращения мерцания
        return 1;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ============================================================================
// ТОЧКА ВХОДА
// ============================================================================

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    // Регистрация класса окна
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    // Вычисляем позицию для центрирования окна на экране
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - WINDOW_WIDTH) / 2;
    int posY = (screenHeight - WINDOW_HEIGHT) / 2;

    // Создаём окно с красивым UI
    g_hWnd = CreateWindowExW(
        WS_EX_LAYERED,                   // Полупрозрачность (без TOPMOST)
        CLASS_NAME,
        L"Slay the Spire 2 - Card UI Showcase",
        WS_POPUP,                        // Borderless design
        posX, posY,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!g_hWnd) {
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }

    // Устанавливаем полупрозрачность окна (245 = почти непрозрачно)
    SetLayeredWindowAttributes(g_hWnd, 0, 245, LWA_ALPHA);

    // Показываем окно
    ShowWindow(g_hWnd, SW_SHOW);
    UpdateWindow(g_hWnd);

    // Основной цикл сообщений
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}