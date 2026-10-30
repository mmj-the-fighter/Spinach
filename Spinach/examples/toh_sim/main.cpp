#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_geom.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <rmgui/spn_rmgui_ui_manager.h>
#include <rmgui/spn_rmgui_button.h>
#include <rmgui/spn_rmgui_textbox.h>
#include <rmgui/spn_rmgui_textscrollview.h>
#include <interpolator.h>



spn::rmgui::Textbox* disksCountTextBox;
spn::rmgui::Button* startButton;
spn::rmgui::TextScrollView* tsv;
spn::rmgui::UiManager* uim;

struct Disk {
	spn::Vec2d pos;
	int diameter;
	int thickness;
	unsigned int color;
	bool isVisible;
};

enum TowerId{ Src=0, Aux=1, Dst=2 };

struct Command {
	TowerId from;
	TowerId to;
};

std::vector<Command> commandBuffer;

spn::Vec2d inputArray[4];
ThreeSegmentAnimator threeLineAnimator;
bool isAnimating = false;
Disk* animatedDisk = nullptr;
const int diskThickness = 40;
char buffer[256];


//AI Generated function
void toh(int n, TowerId source,TowerId auxiliary, TowerId destination)
{
	// Base case: only one disk remains.
	if (n == 1) {
		std::cout << "move " << source << " to " << destination<<'\n';
		sprintf(buffer,"move %d to %d", source, destination);
		tsv->AddText(buffer);
		commandBuffer.push_back({ source,destination });
		return;
	}

	// Step 1: Move n-1 disks out of the way.
	toh(n - 1, source, destination, auxiliary);

	// Step 2: Move the largest remaining disk.
	std::cout << "move " << source << " to " << destination<<'\n';
	sprintf(buffer, "move %d to %d", source, destination);
	tsv->AddText(buffer);
	commandBuffer.push_back({ source,destination });

	// Step 3: Move n-1 disks onto the largest disk.
	toh(n - 1, auxiliary, source, destination);
}

class Tower {
public:
	Tower(std::string name, spn::Vec2d position, int height, int thickness, int color)
		:tag(name), pos(position), h(height), t(thickness), c(color) {}

	void ClearDisks() {
		disks.clear();
	}

	unsigned int RandomColor() {
		int r = rand() % 256;
		int g = rand() % 256;
		int b = rand() % 256;
		return (r << 16) | (g << 8) | b;
	}

	void EnableTopDiskVisibility(bool flag) {
		std::lock_guard<std::mutex> lock(mtx);
		int n = disks.size();
		if (n > 0) {
			disks[n - 1].isVisible = flag;
		}
	}

	Disk* GetTopDisk() {
		int n = disks.size();
		if (n > 0) {
			return &(disks[n - 1]);
		}
		else {
			return nullptr;
		}
	}

	void TransferTopDisk(Tower &other) {
		std::lock_guard<std::mutex> lock(mtx);
		int n = disks.size();
		if (n > 0) {
			Disk d = disks[n - 1];
			d.isVisible = true;
			other.disks.push_back(d);
			disks.pop_back();
		}
	}

	spn::Vec2d GetTopTowerTopPos() {
		spn::Vec2d p = pos;
		p.y -= (h + 3*diskThickness/2);
		return p;
	}

	spn::Vec2d GetTopDiskPos() {
		int n = disks.size();
		if (n == 0) {
			return pos;
		}
		else {
			return disks[n - 1].pos;
		}
	}

	spn::Vec2d GetTopEmptyPos() {
		int n = disks.size();
		if (n == 0) {
			spn::Vec2d topDiskPos = pos;
			topDiskPos.y -= diskThickness;
			return topDiskPos;
		}
		else {
			spn::Vec2d topDiskPos = disks[n - 1].pos;
			topDiskPos.y -= disks[0].thickness;
			return topDiskPos;
		}
	}

	void GenerateDisks(int n, int startingDiameter) {
		unsigned int colors[5] = {
			0xff4444, // Red
			0x44cc44, // Green
			0x0000ff, // Blue
			0xffcc33, // Gold
			0x00cccc  // Cyan
		};
		disks.clear();
		int d = startingDiameter;
		for (int i = 0; i < n; i++) {
			spn::Vec2d diskPos = { pos.x, pos.y - (i+1) * diskThickness };
			unsigned int color = (i <= 4) ? colors[i] : RandomColor();
			disks.push_back({ diskPos, d, diskThickness, color, true });
			d -= 30;
		}
	}

	void Display(spn::Canvas* canvas) {
		std::lock_guard<std::mutex> lock(mtx);
		int left = pos.x - t / 2;
		int top = pos.y - h;
		int right = left + t;
		int bottom = pos.y;
		canvas->SetPrimaryColorUint(0xffffff);
		canvas->DrawRectangle(left, top, right, bottom);

		int i = 0;
		for (const auto& d : disks) {
			int left = d.pos.x - d.diameter / 2;
			int top = d.pos.y;
			int right = left + d.diameter;
			int bottom = top + d.thickness;
			if (d.isVisible) {
				canvas->SetPrimaryColorUint(d.color);
				canvas->DrawFilledRectangle(left, top, right, bottom);
			}
			++i;
		}
		canvas->SetPrimaryColor(255, 255, 255);
		canvas->DrawCString(tag.c_str(), pos.x-15, pos.y);
	}

private:
	spn::Vec2d pos;
	int h;
	int t;
	int c;
	std::string tag;
	mutable std::mutex mtx;
	std::vector<Disk> disks;
};

Tower towerSrc("0:Src", { 100, 320 }, 200, 8, 0xff0000);
Tower towerAux("1:Aux", { 300, 320 }, 200, 8, 0xff0000);
Tower towerDst("2:Dst", { 500, 320 }, 200, 8, 0xff0000);

Tower& GetTowerById(TowerId id) {
	switch (id) {
	case TowerId::Src:
		return towerSrc;
	case TowerId::Aux:
		return towerAux;
	case TowerId::Dst:
		return towerDst;
	}
}

void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	towerSrc.Display(canvas);
	towerAux.Display(canvas);
	towerDst.Display(canvas);
	
	
	if (isAnimating) {
		spn::Vec2d& a = threeLineAnimator.GetPoint(0);
		spn::Vec2d& b = threeLineAnimator.GetPoint(1);
		spn::Vec2d& c = threeLineAnimator.GetPoint(2);
		spn::Vec2d& d = threeLineAnimator.GetPoint(3);
		canvas->DrawDashedVLine(a.x, a.y, b.y);
		canvas->DrawDashedHLine(b.y, b.x, c.x);
		canvas->DrawDashedVLine(c.x, c.y, d.y);
		spn::Vec2d pos = threeLineAnimator.Update(canvas->GetLastFrameTime());
		if (animatedDisk != nullptr) {
			animatedDisk->pos = pos;
			int left = animatedDisk->pos.x - animatedDisk->diameter / 2;
			int top = animatedDisk->pos.y;
			int right = left + animatedDisk->diameter;
			int bottom = top + animatedDisk->thickness;
			canvas->SetPrimaryColorUint(animatedDisk->color);
			canvas->DrawFilledRectangle(left, top, right, bottom);
		}
	}
	uim->Display(canvas);
}

void ProcessCommandBuffer(std::vector<Command> commandBuffer) {
	std::thread t([=]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		isAnimating = true;
		int i = 0;
		for (const auto& c : commandBuffer) {
			tsv->HighLight(i);
			Tower& fromTower = GetTowerById(c.from);
			Tower& toTower = GetTowerById(c.to);
			inputArray[0] = fromTower.GetTopDiskPos();
			inputArray[1] = fromTower.GetTopTowerTopPos();
			inputArray[2] = toTower.GetTopTowerTopPos();
			inputArray[3] = toTower.GetTopEmptyPos();
			fromTower.EnableTopDiskVisibility(false);
			animatedDisk = fromTower.GetTopDisk();
			threeLineAnimator.Set(1.5, inputArray);
			while (!threeLineAnimator.IsEndValueReached()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
			fromTower.TransferTopDisk(toTower);
			tsv->ScrollDown();
			++i;
			
		}
		isAnimating = false;
		tsv->ClearHighLight();
	});
	t.detach();
}

void HandleInput(const SDL_Event* sdlEvent) {
	spn::ui::UiEvent uie;
	spn::ui::TranslateSdlEvent(sdlEvent, uie);
	uim->HandleUiEvent(uie);
}

void Init(spn::SpinachCore* sc) {
	using namespace spn::rmgui;
	uim = &UiManager::GetInstance();
	
	tsv = uim->CreateWidget<spn::rmgui::TextScrollView>();
	tsv->Init(800 - 128 - 20, 480 / 2 - 100, 128, 200);
	disksCountTextBox = uim->CreateWidget<Textbox>();
	disksCountTextBox->SetPosition(10, 480 - 50);
	disksCountTextBox->SetSize(64, 30);
	std::string str = "3";
	disksCountTextBox->SetText(str);
	startButton = uim->CreateWidget<Button>();
	startButton->SetPosition(84, 480-50);
	startButton->SetSize(128 - 8, 32);
	startButton->SetString("Start");
	startButton->SetCallback([=](int id) {
		if (isAnimating) {
			std::cout << "Please wait to finish the current simulation...\n";
			return;
		}
		int n = std::stoi(disksCountTextBox->GetText());
		if (n > 0 && n < 6) {
			towerSrc.GenerateDisks(n, 140);
		}
		else {
			std::cout << "Valid value: 1-5\n";
			return;
		}
		towerDst.ClearDisks();
		towerAux.ClearDisks();
		commandBuffer.clear();
		tsv->ClearText();
		
		std::cout << "Tower of Hanoi: "<< n << " disks: steps\n";
		toh(n, TowerId::Src, TowerId::Aux, TowerId::Dst);
		std::cout << "*****************\n";
		ProcessCommandBuffer(commandBuffer);
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
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);
	sc.SetWindowTitle("Tower of Hanoi: Simulation");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.LockFps(true);
	Init(&sc);
	sc.MainLoop();
	return 0;
}