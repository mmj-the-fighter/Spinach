#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <tictactoeengine.h>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_image.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <imgui/spn_imgui_imgui.h>

spn::Image xImage;
spn::Image oImage;
spn::Image baseImage;
int width;
int height;
float maxwidth;
float maxheight;
int startX = 125;
int startY = 125;
TicTacToeEngine ttt;
int turn = HUMAN;
int playedLocation = -1;
GameResult gameResult = ONGOING;
spn::ui::UiEvent uie;
int board[9];
int buttonState = spn::imgui::BTN_RELEASE;
int buttonX;
int buttonY;
int buttonWidth;
int buttonHeight;
const char moveNumChars[9] = {
    '0','1','2',
    '3','4','5',
    '6','7','8'
};
char boardMoveChar[9] = {
    ' ',' ',' ',
    ' ',' ',' ',
    ' ',' ',' '
};

//ai generated class
class Timer
{
public:
    void Start(int delayMs)
    {
        expiry =
            std::chrono::steady_clock::now() +
            std::chrono::milliseconds(delayMs);
        active = true;
    }

    bool Expired()
    {
        if (!active) return false;
        if (std::chrono::steady_clock::now() >= expiry)
        {
            active = false;
            return true;
        }
        return false;
    }

private:
    bool active = false;
    std::chrono::steady_clock::time_point expiry;
};

Timer timer;
bool waitingForTimer; 

void Restart() {
    switch (gameResult) {
    case DRAWN:
        std::cout << "Draw\n";
        break;
    case LOST:
        std::cout << "Computer Won\n";
        break;
    case WON:
        std::cout << "Human Won\n";
        break;
    }
    gameResult = ONGOING;
    ttt.Restart();
    turn = ttt.Toss() ? HUMAN : COMPUTER;
    for (int i = 0; i < 9; i++) {
        board[i] = EMPTY;
    }
    for (int i = 0; i < 9; i++) {
        boardMoveChar[i] = ' ';
    }
    if (turn == COMPUTER) {
        turn = HUMAN;
        waitingForTimer = true;
        timer.Start(500);
    }
}

void InitApp() {
    
    xImage.CreateFromPng("../examples/res_for_examples/x_player.png");
    width = xImage.GetCanvas()->GetWidth();
    height = xImage.GetCanvas()->GetHeight();
    maxwidth = 3.0 * (width + 4);
    maxheight = 3.0 * (height + 4);
    startX = (640 - maxwidth) / 2;
    startY = (480 - maxheight) / 2;
    buttonWidth = 90;
    buttonHeight = 30;
    buttonX = (640 - buttonWidth) / 2;
    buttonY = startY + maxheight + buttonHeight / 2;
    oImage.CreateFromPng("../examples/res_for_examples/o_player.png");
    baseImage.CreateSolidColorBlockImage(width, height, 255, 255, 255, 1.0);

    srand(static_cast<unsigned int>(time(NULL)));
    ttt.Init(rand());
    Restart();
}

void UpdateAndRender(spn::Canvas* canvas) {
    if (waitingForTimer && timer.Expired()) {
        waitingForTimer = false;
        turn = COMPUTER;
    }
    
    gameResult = ttt.GetGameResult();
    //Draw something with the canvas
    canvas->Clear();
    canvas->SetPrimaryColorUint(0x0000ff);
 
    float charw, charh;
    for (int i = 0; i < 9; i++) {
        int r = i / 3;
        int c = i % 3;
        int x = startX + c * (width+4);
        int y = startY + r * (height+4);
        
        canvas->DrawImage(&baseImage, x, y);
        int tile = board[i];
        switch (tile) {
        case HUMAN:
            canvas->DrawImage(&xImage, x, y);
            canvas->DrawChar(boardMoveChar[i], x+5, y+20);
            break;
        case COMPUTER:
            canvas->DrawImage(&oImage, x, y);
            canvas->GetCharDisplaySize(boardMoveChar[i], charw, charh);
            canvas->DrawChar(boardMoveChar[i], x+width/2-charw/2, y+height/2-charh/2+4);
            break;
        }
    }
   
    if (gameResult == ONGOING) {
        if (turn == COMPUTER) {
            int aiMove = ttt.PlayAiMove();
            boardMoveChar[aiMove] = moveNumChars[ttt.GetMoveNo()-1];
            board[aiMove] = COMPUTER;
            turn = HUMAN;
        }
        else {
            if (waitingForTimer) {
                canvas->DrawCString("I am Thinking... ", startX+20, startY - 40);
            }
            else {
                canvas->DrawCString("Your Turn", startX+20, startY - 40);
            }
        }
        
    }

    switch (gameResult) {
    case DRAWN:
        canvas->DrawCString("It's a Draw", startX+20, startY - 40);
        //std::cout << "Draw\n";
        break;
    case LOST:
        canvas->DrawCString("I Won!", startX+20, startY - 40);
        //std::cout << "Computer Won\n";
        break;
    case WON:
        canvas->DrawCString("You Won!", startX+20, startY - 40);
        //std::cout << "Human Won\n";
        break;
    }

    if (spn::imgui::Button(canvas, uie, "Restart", buttonX, buttonY, buttonWidth, buttonHeight, buttonState)) {
        Restart();
    }
}

void HandleInput(const SDL_Event* sdlEvent) {
    if (waitingForTimer) {
        return;
    }
    spn::ui::TranslateSdlEvent(sdlEvent, uie);
    switch (uie.mouseButton) {
        case spn::ui::MouseButton::Left:
        if (uie.eventType == spn::ui::UiEventType::MouseDown) 
        {
            int c = (uie.mouseX - startX) / (width + 4);
            int r = (uie.mouseY - startY) / (height + 4);
            int idx = r * 3 + c;
            // std::cout << idx << '\n';
            if (gameResult == ONGOING && turn == HUMAN && idx >= 0 && idx < 9) 
            {
                boardMoveChar[idx] = moveNumChars[ttt.GetMoveNo()];
                if (ttt.PlayHumanMove(idx)) {
                    board[idx] = HUMAN;
                    timer.Start(500);
                    waitingForTimer = true;
                }
                uie = {};
            }
        }
    }
    if (uie.keyCode == spn::ui::KeyCode::Right) {
        Restart();
        uie = {};
    }
    
}



int main(int argc, char* argv[])
{
    spn::SpinachCore sc;
    if (!sc.Init(640, 480, "../res/")) {
        std::cout << "initialization failed with error "
            << sc.GetInitializationResult()
            << std::endl;
        return 1;
    }
    InitApp();
    sc.SetUpdateAndRenderHandler(UpdateAndRender);
    sc.SetInputHandler(HandleInput);    
    sc.SetWindowTitle("Spinach Demo");
    sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
    sc.GetCanvas()->EnableAlphaBlending(true);
    sc.SetTargetFramesPerSecond(30);
    sc.LockFps(true);
    sc.MainLoop();
    ttt.DeInit();
    return 0;
}