#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_geom.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <rmgui/spn_rmgui_ui_manager.h>
#include <rmgui/spn_rmgui_button.h>
#include <rmgui/spn_rmgui_label.h>
#include <rmgui/spn_rmgui_dropdown.h>
//TODO: Add dynamic constraints, 
// i.e. make the 2nd line parallel or perpendicular to 
//the first line while the user is drawing the 2nd line
spn::rmgui::Dropdown* actionTypeDropdown;
spn::rmgui::Label* actionLabel;
spn::rmgui::UiManager* uim;
int uiAreaY=150;

struct Line2d {
    spn::Vec2d p0;
    spn::Vec2d p1;
};

Line2d lines[2];
Line2d linesOriginal[2];
int lineIndex=-1;
int numLines = 0;

void MakeParallel(Line2d& first, Line2d& second) {
    float distanceSecond = spn::Distance(second.p1, second.p0);
    float distanceFirst = spn::Distance(first.p1, first.p0);
    spn::Vec2d firstVec = spn::Sub(first.p1, first.p0);
    spn::Vec2d secondVec = spn::Sub(second.p1, second.p0);
    float sign = spn::Dot(firstVec, secondVec) < 0.0f ? -1.0f : 1.0f;
    float ndx = firstVec.x / distanceFirst;
    float ndy = firstVec.y / distanceFirst;
    second.p1.x = second.p0.x + sign * ndx * distanceSecond;
    second.p1.y = second.p0.y + sign * ndy * distanceSecond;   
    float distanceSecond2 = spn::Distance(second.p1, second.p0);
    float delta = distanceSecond2 - distanceSecond;
    std::cout << delta << " parallel error \n";
}


void MakePerpendicular(Line2d& first, Line2d& second) {
    float distanceSecond = spn::Distance(second.p1, second.p0);
    float distanceFirst = spn::Distance(first.p1, first.p0);
    spn::Vec2d firstVec = spn::Sub(first.p1, first.p0);
    spn::Vec2d firstVecPerp = spn::Perp(firstVec);
    spn::Vec2d secondVec = spn::Sub(second.p1, second.p0);
    float sign = spn::Dot(firstVecPerp, secondVec) < 0.0f ? -1.0f : 1.0f;
    float ndx = firstVecPerp.x / distanceFirst;
    float ndy = firstVecPerp.y / distanceFirst;
    second.p1.x = second.p0.x + sign * ndx * distanceSecond;
    second.p1.y = second.p0.y + sign * ndy * distanceSecond;
    float distanceSecond2 = spn::Distance(second.p1, second.p0);
    float delta = distanceSecond2 - distanceSecond;
    std::cout << delta << " perpendicular error \n";
}

void UpdateAndRender(spn::Canvas* canvas) {
    //Draw something with the canvas
    canvas->Clear();
    canvas->SetPrimaryColorUint(0x0000ff);
    canvas->DrawHLine(uiAreaY, 0, 798);
    for (int i = 0; i < numLines; i++) {
        switch (i) {
        case 0:
            canvas->SetPrimaryColorUint(0xffffff);
            break;
        case 1:
            canvas->SetPrimaryColorUint(0xffff00);
            break;
        }
        canvas->SetStrokeRadius(2);
        canvas->DrawStroke(
            lines[i].p0.x, lines[i].p0.y,
            lines[i].p1.x, lines[i].p1.y);
        canvas->SetStrokeRadius(6);
        canvas->DrawDot(lines[i].p0.x, lines[i].p0.y);
    }
    uim->Display(canvas);
}

int ClipY(int y) {
    if (y < uiAreaY) {
        return uiAreaY + 2;
    }
    else {
        return y;
    }
}


void HandleInput(const SDL_Event* sdlEvent) {
    using namespace spn::rmgui;
    using namespace spn::ui;
    UiEvent uie;
    TranslateSdlEvent(sdlEvent, uie);

    
    switch (uie.eventType) {
    case spn::ui::UiEventType::MouseDown:
        if (uie.mouseButton == spn::ui::MouseButton::Left) {
            if (uie.mouseY > uiAreaY) {
                lineIndex = (lineIndex+1)%2;
                numLines = lineIndex + 1;
                lines[lineIndex].p0 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
                lines[lineIndex].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
            }
        }
        break;
    case spn::ui::UiEventType::MouseUp:
        if (uie.mouseButton == spn::ui::MouseButton::Left) {
            if (uie.mouseY > uiAreaY) {
                lines[lineIndex].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
                if (DistanceSquared(lines[lineIndex].p0, lines[lineIndex].p1) < 1) {
                    lineIndex = lineIndex - 1;
                    if (lineIndex < -1) {
                        lineIndex = -1;
                    }
                    numLines = lineIndex + 1;
                }
                else {
                    if (numLines == 2) {
                        linesOriginal[0] = lines[0];
                        linesOriginal[1] = lines[1];
                    }
                }
            }
        }
        break;
    case spn::ui::UiEventType::MouseDrag:
        if (uie.mouseY > uiAreaY) {
            lines[lineIndex].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
        }
        break;
    }
    uim->HandleUiEvent(uie);
}

void InitUi() {
    using namespace spn::rmgui;
    uim = &UiManager::GetInstance();
    actionLabel = uim->CreateWidget<Label>();
    actionLabel->SetCString("Action: ");
    actionLabel->SetPosition(640 - 225 - 2-60, 10);
    actionTypeDropdown = uim->CreateWidget<Dropdown>();
    actionTypeDropdown->SetPosition(640 - 225 - 2, 10);
    actionTypeDropdown->AddOption("Original");
    actionTypeDropdown->AddOption("Make Parallel");
    actionTypeDropdown->AddOption("Make Perpendicular");
    actionTypeDropdown->SetSize(225, 32);
    actionTypeDropdown->SetCallback([=](int id, int selected) {
        if (numLines < 2) return;
        switch (selected) {
        case 0:
            lines[0] = linesOriginal[0];
            lines[1] = linesOriginal[1];
            break;
        case 1:
            lines[0] = linesOriginal[0];
            lines[1] = linesOriginal[1];
            MakeParallel(lines[0], lines[1]);
            break;
        case 2:
            lines[0] = linesOriginal[0];
            lines[1] = linesOriginal[1];
            MakePerpendicular(lines[0], lines[1]);
            break;
        }
     });
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
    InitUi();
    sc.SetUpdateAndRenderHandler(UpdateAndRender);
    sc.SetInputHandler(HandleInput);
    sc.SetWindowTitle("Line Constraints");
    sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
    sc.SetTargetFramesPerSecond(30);
    sc.LockFps(true);
    sc.MainLoop();
    return 0;
}