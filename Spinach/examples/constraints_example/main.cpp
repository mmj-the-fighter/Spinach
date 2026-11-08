#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <spn_canvas.h>
#include <spn_image.h>
#include <spn_core.h>
#include <spn_geom.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <rmgui/spn_rmgui_ui_manager.h>
#include <rmgui/spn_rmgui_button.h>
#include <rmgui/spn_rmgui_toggle_button.h>
#include <rmgui/spn_rmgui_label.h>
#include <rmgui/spn_rmgui_dropdown.h>
#include <constraints_common.h>
#include <snap_solver.h>
#include <algebraic_solver.h>
#include <numerical_solver.h>

//TODO: Finish all combinations of solvers
//Add documentation
//Add derivation latex files

spn::rmgui::Dropdown* algorithmDropdown;
spn::rmgui::Label* algorithmLabel;
spn::rmgui::Label* cnstLabel;
spn::rmgui::UiManager* uim;
int uiAreaY=150;

spn::rmgui::ToggleButton* parallelCstButton;
spn::rmgui::ToggleButton* perpendicularCstButton;
spn::rmgui::ToggleButton* noCstButton;

Constraint curConstraint = NIL;
Algorithm curAlgorithm = SNAP;

Line2d lines[3];
int lineIndex=-1;
int numLines = 0;


void SolveCsp() {
    switch (curAlgorithm) 
    {
    case SNAP:
        switch (curConstraint) 
        {
        case NIL:
            lines[1] = lines[2];
            break;
        case PARALLEL:
            lines[1] = lines[2];//copy mouse line
            MakeParallelSnap(lines[0], lines[1]);
            break;
        case PERPENDICULAR:
            lines[1] = lines[2];//copy mouse line
            MakePerpendicularSnap(lines[0], lines[1]);
            break;
        }
        break;
    case ALGEBRAIC:
        switch (curConstraint) {
        case NIL:
            lines[1] = lines[2];
            break;
        case PARALLEL:
            lines[1] = lines[2];
            MakeParallelAlgebraically(lines[0], lines[1], lines[2]);
            break;
        case PERPENDICULAR:
            lines[1] = lines[2];
            MakePerpendicularAlgebraically(lines[0], lines[1], lines[2]);
            break;
        }
        break;
    case NUMERICAL:
        switch (curConstraint) {
        case NIL:
            lines[1] = lines[2];
            break;
        case PARALLEL:
            lines[1] = lines[2];
            MakeParallelNumerically(lines[0], lines[1], lines[2]);
            break;
        case PERPENDICULAR:
            //TODO: fix
            lines[1] = lines[2];
            MakePerpendicularNumerically(lines[0], lines[1], lines[2]);
            break;
        }
        break;
    }
}

void UpdateAndRender(spn::Canvas* canvas) {
    //Draw something with the canvas
    canvas->Clear();
    canvas->SetPrimaryColorUint(0xffffff);
    canvas->EnableAlphaBlending(true);
    canvas->EnableAlphaBlending(false);
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
    if (numLines == 2 && curConstraint!= NIL) {
        canvas->SetPrimaryColorUint(0xffff00);
        canvas->DrawDashedLine(
            lines[2].p0.x, lines[2].p0.y,
            lines[2].p1.x, lines[2].p1.y);
        canvas->DrawDot(lines[2].p0.x, lines[2].p0.y);
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
                if (lineIndex == 1) {
                    lines[2].p0 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
                    lines[2].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
                }
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
                if (lineIndex == 1) {
                    lines[2].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
                    SolveCsp();
                }
            }
        }
        break;
    case spn::ui::UiEventType::MouseDrag:
        if (uie.mouseY > uiAreaY) {
            lines[lineIndex].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
            if (lineIndex == 1) {
                lines[2].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
                SolveCsp();
            }
        }
        break;
    }
    uim->HandleUiEvent(uie);
}

void InitUi() {

    using namespace spn::rmgui;
    uim = &UiManager::GetInstance();
    parallelCstButton = uim->CreateWidget<ToggleButton>();
    parallelCstButton->SetPosition(130, 10);
    parallelCstButton->SetSize(128, 32);
    parallelCstButton->SetString("Parallel");
    parallelCstButton->SetCallback([=](int id, bool action) {
        if (perpendicularCstButton->IsPressed()) {
            perpendicularCstButton->Toggle();
        }
        if (noCstButton->IsPressed()) {
            noCstButton->Toggle();
        }
        curConstraint = PARALLEL;
        });

    perpendicularCstButton = uim->CreateWidget<ToggleButton>();
    perpendicularCstButton->SetPosition(130, 10+34);
    perpendicularCstButton->SetSize(128, 32);
    perpendicularCstButton->SetString("Perpendicular");
    perpendicularCstButton->SetCallback([=](int id, bool action) {
        if (parallelCstButton->IsPressed()) {
            parallelCstButton->Toggle();
        }
        if (noCstButton->IsPressed()) {
            noCstButton->Toggle();
        }
        curConstraint = PERPENDICULAR;
        });

    noCstButton = uim->CreateWidget<ToggleButton>();
    noCstButton->SetPosition(130, 10+34 + 34);
    noCstButton->SetSize(128, 32);
    noCstButton->SetString("No Cnst");
    noCstButton->SetCallback([=](int id, bool action) {
        if (parallelCstButton->IsPressed()) {
            parallelCstButton->Toggle();
        }
        if (perpendicularCstButton->IsPressed()) {
            perpendicularCstButton->Toggle();
        }
        curConstraint = NIL;
        });
    noCstButton->Toggle();
    using namespace spn::rmgui;
    uim = &UiManager::GetInstance();
    
    cnstLabel = uim->CreateWidget<Label>();
    cnstLabel->SetCString("Constraint:");
    cnstLabel->SetPosition(20, 10);


    algorithmLabel = uim->CreateWidget<Label>();
    algorithmLabel->SetCString("Method: ");
    algorithmLabel->SetPosition(640 - 225 - 2-60, 10);
    algorithmDropdown = uim->CreateWidget<Dropdown>();
    algorithmDropdown->SetPosition(640 - 225 - 2, 10);
    algorithmDropdown->AddOption("Snap");
    algorithmDropdown->AddOption("Algebraic Csp solver");
    algorithmDropdown->AddOption("Numerical Csp solver");
    algorithmDropdown->SetSize(225, 32);
    algorithmDropdown->SetCallback([=](int id, int selected) {
        switch (selected) {
        case 0:
            curAlgorithm = SNAP;
            break;
        case 1:
            curAlgorithm = ALGEBRAIC;
            break;
        case 2:
            curAlgorithm = NUMERICAL;
            break;
        }
     });
}


int main(int argc, char* argv[])
{
    spn::SpinachCore sc;
    if (!sc.Init(800, 600, "../res/")) {
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