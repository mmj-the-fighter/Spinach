//Developed with AI assistance for mathematics, general algorithm and debugging
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
#include <imgui/spn_imgui_imgui.h>
#include <spn_ui_scheme.h>
#include <matrix.h>

//TODO: Adaptive damping
//TODO: Avoid inverse
//TODO: Add documentation

spn::Vec2d<double> joints[6];
double angles[6];
double absoluteAngles[6];
double lengths[6];
bool ikEnabled = false;
spn::Vec2d<double> mousePoint = {};
spn::ui::UiEvent uie = {};
double angle = 0;
Mtx::Matrix<double> jacobianMat(2, 5);
Mtx::Matrix<double> deltaPosition(2, 1);
Mtx::Matrix<double> deltaTheta(5, 1);

//eqn
//deltaPosition = jacobianMat * deltaTheta;
//jacobianMat^-1 * deltaPosition = deltaTheta
template<typename T>
constexpr T deg_to_rad(T deg) {
    return deg * (T(3.14159265358979323846) / T(180));
}

template<typename T>
constexpr T rad_to_deg(T rad) {
    return rad * (T(180) / T(3.14159265358979323846));
}
void InitializeInitialJoint(double x, double y) {
    joints[0].x = x;
    joints[0].y = y;
}
void InitLengths() {
    for (int i = 0; i < 5; i++) {
        lengths[i] = 50;
    }
}
void InitAngles() {
    //angles[0] = deg_to_rad(-30.0);
    angles[0] = deg_to_rad(0.0);
    double step=0.0;
    for (int i = 1; i < 5; i++) {
        double a = 0 + step;
        angles[i] = deg_to_rad(a);
        //step += 5.0;
    }
}

void CalculateAbsoluteAngles() {
    absoluteAngles[0] = angles[0];
    for (int i = 1; i < 5; i++) {
        absoluteAngles[i] = absoluteAngles[i-1] + angles[i];
    }
}

void RotateBones() {
    for (int i = 1; i < 6; i++) {
        double alpha = absoluteAngles[i-1];
        joints[i].x = joints[i-1].x + lengths[i-1] * cos(alpha);
        joints[i].y = joints[i-1].y + lengths[i-1] * sin(alpha);
    }
}

void CalculateJacobianMatrix() {
    constexpr int n = 5;
    const spn::Vec2d<double>& effector = joints[n];
    for (int i = 0; i < n; i++) {
        spn::Vec2d v = spn::Sub(effector, joints[i]);
        jacobianMat.at(0, i) = -v.y;
        jacobianMat.at(1, i) = v.x;
    }
}

void Init() {
    InitializeInitialJoint(100, 200);
    InitLengths();
    InitAngles();
    CalculateAbsoluteAngles();
    RotateBones();
    spn::ui::UiScheme::GetInstance().checkboxColor = 0xffffff;
}

void CalculateDeltaTheta(double lambdaSquared) {
    Mtx::Matrix<double> identity(2, 2);
    identity.at(0, 0) = 1;
    identity.at(0, 1) = 0;
    identity.at(1, 0) = 0;
    identity.at(1, 1) = 1;
    
    Mtx::Matrix<double> jjtranspose = jacobianMat * jacobianMat.transpose();
    Mtx::Matrix<double> Ainverse = Mtx::inv(jjtranspose + identity * lambdaSquared);
    deltaTheta = jacobianMat.transpose() * Ainverse * deltaPosition;
}

void CorrectAngles() {
    double rate = 0.2;
    constexpr double maxStep = deg_to_rad(5.0);
    for (int i = 0; i < 5; i++) {
        angles[i] += rate * std::clamp(deltaTheta.at(i, 0), -maxStep, maxStep);
    }
}

double CalculateDeltaPosition() {
    spn::Vec2d delta = spn::Sub(mousePoint, joints[5]);
    deltaPosition.at(0, 0) = delta.x;
    deltaPosition.at(1, 0) = delta.y;
    return spn::MagnitudeSquared(delta);
}

void Solve_Jacobian_DLS_IK() {
    constexpr double tol = 0.1;
    constexpr double tolSquared = tol * tol;
    constexpr int maxIterations = 10;
    int iterations = 0;
    double lambda = 10.0;
 
    while (iterations < maxIterations) {
        CalculateAbsoluteAngles();
        RotateBones();
        double errorSquared = CalculateDeltaPosition();
        if (errorSquared < tolSquared) {
            break;
        }
        CalculateJacobianMatrix();
        CalculateDeltaTheta(lambda*lambda);
        CorrectAngles();
        ++iterations;
    }
}


void RenderBones(spn::Canvas* canvas) {
    unsigned int colors[5] = {
        0xff4444, // Red
        0x44cc44, // Green
        0x00cccc,  // Cyan
        0x0000ff, // Blue
        0xffcc33 // Gold
    };

    int x = joints[0].x;
    int y = joints[0].y;
    canvas->SetStrokeRadius(2);
    for (int i = 1; i < 6; i++) {
        canvas->SetPrimaryColorUint(colors[i-1]);
        canvas->DrawStroke(x, y, joints[i].x, joints[i].y);
        x = joints[i].x;
        y = joints[i].y;
    }

    canvas->SetPrimaryColorUint(0xffffff);
    canvas->SetStrokeRadius(8);
    canvas->DrawDot(joints[0].x, joints[0].y);
    canvas->SetStrokeRadius(4);
    for (int i = 1; i < 6; i++) {
        canvas->DrawDot(joints[i].x, joints[i].y);
    }
}

void UpdateAndRender(spn::Canvas* canvas) {
    canvas->Clear();
    canvas->SetPrimaryColorUint(0x0000ff);
    RenderBones(canvas);
    canvas->SetStrokeRadius(8);
    canvas->SetPrimaryColorUint(0x00ff00);
    canvas->DrawDot(mousePoint.x, mousePoint.y);
    canvas->SetPrimaryColorUint(0xffffff);
    if (spn::imgui::Checkbox(canvas, uie, "Enable IK", 600 - 150, 100, ikEnabled)) {
        if (!ikEnabled) {
            Init();
        }
    }
}


void HandleInput(const SDL_Event* sdlEvent) {
    using namespace spn::ui;
    TranslateSdlEvent(sdlEvent, uie);
    switch (uie.eventType) {
    case spn::ui::UiEventType::MouseDown:
        if (uie.mouseButton == spn::ui::MouseButton::Left) {
        }
        break;
    case spn::ui::UiEventType::MouseUp:
        if (uie.mouseButton == spn::ui::MouseButton::Left) {
        }
        break;
    case spn::ui::UiEventType::MouseDrag:
        mousePoint.x = uie.mouseX;
        mousePoint.y = uie.mouseY;
        if (ikEnabled) {
            Solve_Jacobian_DLS_IK();
        }
        break;
    }
}


int main(int argc, char* argv[])
{
    Init();
    spn::SpinachCore sc;
    if (!sc.Init(800, 600, "../res/")) {
        std::cout << "initialization failed with error "
            << sc.GetInitializationResult()
            << std::endl;
        return 1;
    }
    sc.SetUpdateAndRenderHandler(UpdateAndRender);
    sc.SetInputHandler(HandleInput);
    sc.SetWindowTitle("2D Inverse Kinematics Jacobian DLS");
    sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
    sc.SetTargetFramesPerSecond(30);
    sc.LockFps(true);
    sc.MainLoop();
    return 0;
}