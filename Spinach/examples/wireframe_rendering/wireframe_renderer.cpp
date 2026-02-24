#include <imgui/imgui.h>

#include <ui_event_translator.h>
#include "wireframe_renderer.h"



void WireframeRenderer::UpdateAndRender(spn::Canvas* canvas) {
	if (requestedForChangeOfCamera) {
		requestedForChangeOfCamera = false;
		if (camera.IsPerspective()) {
			camera.SetOrthoCameraParams(
				-halfHorizontalExtend,
				halfHorizontalExtend,
				-halfVerticalExtend,
				halfVerticalExtend,
				-20.0,
				20.0);
		}
		else {
			camera.SetPerspectiveCameraParams(fovy, aspectRatio, 1, 2000);
		}
	}
	if (requestedForChangeOfModel) {
		requestedForChangeOfModel = false;
		LoadModel();
	}
	canvas->Clear();
	if (spn::imgui::Checkbox(canvas, uie, "BackFaceCulling", 100, 100, backFaceCullingCheckboxStatus)) {
		rasterizer.EnableBackFaceCulling(backFaceCullingCheckboxStatus);
	}
	
	n3d::mat4x4_set_roation_y(mMat, angle);
	angle += angleIncr;
	if (angle > 359.9) {
		angle = 0.0f;
	}
	rasterizer.SetModelMatrix(mMat);
	rasterizer.RenderMesh(mesh);
	char buffer[256];
	sprintf(buffer, "%0.2f ms", canvas->GetLastFrameTime() * 1000);
	//canvas->DrawText(buffer, 245, 40);
	spinachCore->SetWindowTitle(buffer);
}

void WireframeRenderer::HandleInput(const SDL_Event* sdlEvent) {
	if (sdlEvent->type == SDL_EVENT_KEY_DOWN) {
		switch (sdlEvent->key.key)
		{
		case SDLK_J:
			camera.MoveCamera(n3d::DOWN);
			return;
		case SDLK_U:
			camera.MoveCamera(n3d::UP);
			return;

		case SDLK_A:
			camera.TurnLeft();
			return;

		case SDLK_D:
			camera.TurnRight();
			return;

		case SDLK_W:
			camera.LookDown();
			return;

		case SDLK_S:
			camera.LookUp();
			return;

		case SDLK_LEFT:
			camera.MoveCamera(n3d::LEFT);
			return;
		case SDLK_RIGHT:
			camera.MoveCamera(n3d::RIGHT);
			return;
		case SDLK_UP:
			camera.MoveCamera(n3d::FORWARD);
			return;
		case SDLK_DOWN:
			camera.MoveCamera(n3d::BACKWARD);
			return;
		case SDLK_O:
			if (camera.IsPerspective()) {
				requestedForChangeOfCamera = true;
			}
			return;
		case SDLK_P:
			if (!camera.IsPerspective()) {
				requestedForChangeOfCamera = true;
			}
			break;
		case SDLK_PAGEUP:
			modelIndex--;
			requestedForChangeOfModel = true;
			return;
		case SDLK_PAGEDOWN:
			modelIndex++;
			requestedForChangeOfModel = true;
			return;
		}
	}
	spn::rmgui::TranslateSdlEvent(sdlEvent, uie);
}

void WireframeRenderer::LoadModel() {
	const char* modelpaths[2] = {
		"../examples/res_for_examples/cube.obj",
		"../examples/res_for_examples/suzanne.obj"
	};
	int n = (sizeof(modelpaths) / sizeof(char*));
	if (modelIndex < 0) {
		modelIndex = n - 1;
	}
	else {
		modelIndex = modelIndex % n;
	}
	mesh.LoadObjectFile(modelpaths[modelIndex]);
}

void WireframeRenderer::Init(spn::SpinachCore* sc) {
	spinachCore = sc;
	spn::Canvas* canvas = sc->GetCanvas();
	angle = 0;
	angleIncr = 0.5;
	aspectRatio = canvas->GetAspectRatio();
	canvasWidth = canvas->GetWidth();
	canvasHeight = canvas->GetHeight();


	modelIndex = 0;
	LoadModel();
	requestedForChangeOfModel = false;


	sc->SetWindowTitle("Spinach Demo");
	sc->SetTargetFramesPerSecond(30);

	canvas->SetClearColor(0, 0, 0);
	canvas->SetPrimaryColor(192, 192, 0);
	rasterizer.SetCanvas(canvas);

	eye.x = 0;
	eye.y = 3;
	eye.z = 10;

	at.x = 0;
	at.y = 0;
	at.z = 0;

	up.x = 0;
	up.y = 1;
	up.z = 0;

	camera.SetViewParams(eye, at, up);
	fovy = 45;
	camera.SetPerspectiveCameraParams(fovy, aspectRatio, 1, 2000);
	requestedForChangeOfCamera = false;

	halfHorizontalExtend = 2 * aspectRatio;
	halfVerticalExtend = 2;



	rasterizer.SetCamera(&camera);

	n3d::mat4x4_set_viewport(viewportMat, 0, 0, canvasWidth, canvasHeight, 1);
	rasterizer.SetViewportMatrix(viewportMat);
}
