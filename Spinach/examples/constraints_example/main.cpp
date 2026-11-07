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
//TODO: Add dynamic constraints, 
// i.e. make the 2nd line parallel or perpendicular to 
//the first line while the user is drawing the 2nd line
spn::rmgui::Dropdown* algorithmDropdown;
spn::rmgui::Label* algorithmLabel;
spn::rmgui::Label* cnstLabel;
spn::rmgui::UiManager* uim;
int uiAreaY=150;

spn::rmgui::ToggleButton* parallelCstButton;
spn::rmgui::ToggleButton* perpendicularCstButton;
spn::rmgui::ToggleButton* noCstButton;

struct Line2d {
    spn::Vec2d<double> p0;
    spn::Vec2d<double> p1;
};

enum Constraint {
    NIL,
    PARALLEL,
    PERPENDICULAR
};

enum Algorithm {
    SNAP,
    ALGEBRAIC,
    NUMERICAL
};

Constraint curConstraint = NIL;
Algorithm curAlgorithm = SNAP;

Line2d lines[3];
int lineIndex=-1;
int numLines = 0;

inline bool AlgebraicParallelConstraint3(
    double x, double y, 
    double mx, double my, 
    double p2x, double p2y) {
    return (x - p2x) * (mx - p2x) + (y - p2y) * (my - p2y) > 0;
};

//TODO: add documentation
void MakeParallelAlgebraically(Line2d& first, Line2d& second, Line2d& mouseLine) {
    spn::Vec2d<double> firstVec = spn::Sub(first.p1, first.p0);
    //Constraint1: firstline and newline should be parrallel
    double A = firstVec.y;
    if (A == 0) {
        //edge case: first line is horizontal
        double y2 = second.p0.y;
        double D = 1.0;
        double E = 1.0;
        double G = -second.p0.y;
        double LenSq = (mouseLine.p1.x - mouseLine.p0.x) * (mouseLine.p1.x - mouseLine.p0.x) +
            (mouseLine.p1.y - mouseLine.p0.y) * (mouseLine.p1.y - mouseLine.p0.y);
        double H = second.p0.x * second.p0.x + second.p0.y * second.p0.y - LenSq;
        double F = -second.p0.x;
        double I = D;
        double K = E * y2 * y2 + 2.0 * G * y2 + H;
        double J = 2.0 * F;
        double dc = J * J - 4.0 * K * I;
        if (dc < 0) {
            std::cout << "error1 calculating solution\n";
            return;
        }
        double xx0 = (-J + sqrt(dc)) / (2.0 * I);
        double yy0 = y2;
        double xx1 = (-J - sqrt(dc)) / (2.0 * I);
        double yy1 = y2;
        if (AlgebraicParallelConstraint3(xx0, yy0,
            mouseLine.p1.x, mouseLine.p1.y,
            second.p0.x, second.p0.y)) {

            second.p1.x = xx0;
            second.p1.y = yy0;
        }
        else if(
            AlgebraicParallelConstraint3(xx1, yy1,
            mouseLine.p1.x, mouseLine.p1.y,
            second.p0.x, second.p0.y)
            ){
            second.p1.x = xx1;
            second.p1.y = yy1;
        }
        return;
    }
    double B = -firstVec.x;
    double C = -(second.p0.x * firstVec.y - second.p0.y * firstVec.x);
    
    //Constraint2: Length of the mouseLine should be equal to length of the newLine
    double D = 1.0;
    double E = 1.0;
    double F = -second.p0.x;
    double G = -second.p0.y;
    double LenSq = (mouseLine.p1.x - mouseLine.p0.x) * (mouseLine.p1.x - mouseLine.p0.x) +
        (mouseLine.p1.y - mouseLine.p0.y) * (mouseLine.p1.y - mouseLine.p0.y);
    double H = second.p0.x * second.p0.x + second.p0.y * second.p0.y - LenSq;
    double squaredA = A * A;
    double squaredB = B * B;
    double squaredC = C * C;
    double reciprocalA = 1.0 / A;
    double reciprocalAsquared = 1.0 / squaredA;
    double I = (D * squaredB * reciprocalAsquared) + E;
    double J = 2.0 * (B * C * D * reciprocalAsquared - B * F * reciprocalA + G);
    double K = (squaredC * D * reciprocalAsquared) - 2.0 * C * F * reciprocalA + H;

    //substituting first constraint in second constraint and solving quadratic equation
    double d1 = J * J - 4.0 * K * I;
    if (d1 < 0) {
        std::cout << "error1 calculating solution\n";
        return;
    }
    double yy0 = (-J + sqrt(d1)) / (2.0 * I);
    double xx0 = (-B * yy0 - C) * reciprocalA;
    double yy1 = (-J - sqrt(d1)) / (2.0 * I);
    double xx1 = (-B * yy1 - C) * reciprocalA;
    
    //accept the solution that satisfies the third condtion
    if (AlgebraicParallelConstraint3(xx0, yy0,
        mouseLine.p1.x, mouseLine.p1.y,
        second.p0.x, second.p0.y)) {

        second.p1.x = xx0;
        second.p1.y = yy0;
    }
    else if(AlgebraicParallelConstraint3(xx1, yy1,
        mouseLine.p1.x, mouseLine.p1.y,
        second.p0.x, second.p0.y)) {
        second.p1.x = xx1;
        second.p1.y = yy1;
    }
    return;
}

void MakeParallel(Line2d& first, Line2d& second) {
    double distanceSecond = spn::Distance(second.p1, second.p0);
    double distanceFirst = spn::Distance(first.p1, first.p0);
    spn::Vec2d<double> firstVec = spn::Sub(first.p1, first.p0);
    spn::Vec2d<double> secondVec = spn::Sub(second.p1, second.p0);
    double sign = spn::Dot(firstVec, secondVec) < 0.0 ? -1.0 : 1.0;
    double ndx = firstVec.x / distanceFirst;
    double ndy = firstVec.y / distanceFirst;
    second.p1.x = second.p0.x + sign * ndx * distanceSecond;
    second.p1.y = second.p0.y + sign * ndy * distanceSecond;   
    double distanceSecond2 = spn::Distance(second.p1, second.p0);
    double delta = distanceSecond2 - distanceSecond;
    //std::cout << delta << " parallel error \n";
}


void MakePerpendicular(Line2d& first, Line2d& second) {
    double distanceSecond = spn::Distance(second.p1, second.p0);
    double distanceFirst = spn::Distance(first.p1, first.p0);
    spn::Vec2d<double> firstVec = spn::Sub(first.p1, first.p0);
    spn::Vec2d<double> firstVecPerp = spn::Perp(firstVec);
    spn::Vec2d<double> secondVec = spn::Sub(second.p1, second.p0);
    double sign = spn::Dot(firstVecPerp, secondVec) < 0.0 ? -1.0 : 1.0;
    double ndx = firstVecPerp.x / distanceFirst;
    double ndy = firstVecPerp.y / distanceFirst;
    second.p1.x = second.p0.x + sign * ndx * distanceSecond;
    second.p1.y = second.p0.y + sign * ndy * distanceSecond;
    double distanceSecond2 = spn::Distance(second.p1, second.p0);
    double delta = distanceSecond2 - distanceSecond;
    secondVec = spn::Sub(second.p1, second.p0);
    double dot = spn::Dot(firstVec, secondVec);
    //std::cout << " length error" << delta << "dot product error" << 0 - dot << '\n';
}

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
            lines[1] = lines[2];
            MakeParallel(lines[0], lines[1]);
            break;
        case PERPENDICULAR:
            lines[1] = lines[2];
            MakePerpendicular(lines[0], lines[1]);
            break;
        }
        break;
    case ALGEBRAIC:
        switch (curConstraint) {
        case NIL:
            lines[1] = lines[2];
            break;
        case PARALLEL:
            MakeParallelAlgebraically(lines[0], lines[1], lines[2]);
            break;
        }
        break;
    case NUMERICAL:
        break;
    }
    
}

void UpdateAndRender(spn::Canvas* canvas) {
    SolveCsp();
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
                }
            }
        }
        break;
    case spn::ui::UiEventType::MouseDrag:
        if (uie.mouseY > uiAreaY) {
            lines[lineIndex].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
            if (lineIndex == 1) {
                lines[2].p1 = { (float)uie.mouseX,(float)ClipY(uie.mouseY) };
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
        std::cout << "par cst " << action << '\n';
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
        std::cout << "perp cst " << action << '\n';
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
        std::cout << "no cst " << action << '\n';
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