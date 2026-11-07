#include <iostream>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_rng.h>
#include <spn_geom.h>
#include <spn_ui_event.h>
#include <spn_ui_event_translator.h>
#include <rmgui/spn_rmgui_ui_manager.h>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <spn_geom.h>
#include <interpolator.h>
#include <rmgui/spn_rmgui_button.h>

//TODO: Add a module for Optimized lift-scheduling, 
// where each person's source and destination are known


struct  UserRequest {
	int requestedFloor;
	int requestedIndex;
};

struct LiftState {
	int currentFloor;
	bool isLiftLoadingAndUnloading;
};

struct LiftScanRange {
	int start;
	int end;
};

struct UserRequestWithProps {
	UserRequest req;
	bool enqueued;
	int floorDelta;
	int absoluteFloorDelta;
};

enum IndicatorStatus {
	PENDING, SERVING, SERVED
};
struct Indicator {
	spn::Vec2d<float> point;
	IndicatorStatus status;
};

std::unordered_map<int, spn::Vec2d<float>> floorToLiftPointMapping;
std::unordered_map<int, Indicator> floorToIndicatorMapping;
std::vector<int> requestedFloors;
utils::Vec2dInterpolator<float> interpolator;
std::unordered_map<int, UserRequestWithProps> indexToRequestMapping;
std::vector<UserRequestWithProps> requestsVec;
std::queue<UserRequest> requests;
LiftState liftState;

spn::rmgui::UiManager* uiManager;
spn::rmgui::Button* nextButton;

bool hasEntitiesInQueue = false;
int floorStartx = 100;
int floorStarty = 50;
int floorsRangeStart = -2;
int floorsRangeEnd = 4;
int floorHeight = 0;
int floorWidth = 200;
int liftStartx;
int liftStarty;
int liftHeight = 0;
int liftWidth = floorWidth * 2 / 3;
int liftx;
int lifty;
int numberOfFloors = 0;
int buildingOffsetFromTop;


int ConvertFloorToIndex(int floor) {
	return floor - floorsRangeStart;
}

int ConvertIndexToFloor(int index) {
	return index + floorsRangeStart;
}

int CalculateTravelCost(int liftfloor, std::queue<UserRequest> r) {
	int i = liftfloor;
	int cost = 0;
	while (!r.empty()) {
		int d = std::abs(
			r.front().requestedIndex -
			ConvertFloorToIndex(liftfloor));
		cost += d;
		liftfloor = r.front().requestedFloor;
		r.pop();
	}
	return cost;
}

void InitLiftPosition(int floor) {
	spn::Vec2d<float> dst = floorToLiftPointMapping[floor];
	spn::Vec2d<float> src = floorToLiftPointMapping[floor];
	interpolator.Set(src, dst, 0.0f);
}

void SetLiftFloor(int floor) {
	liftState.currentFloor = floor;
	liftStarty = buildingOffsetFromTop - (ConvertFloorToIndex(floor) + 1) * floorHeight;
}

bool CompareUserRequestsWithProps(UserRequestWithProps a, UserRequestWithProps b) {
	return a.floorDelta < b.floorDelta;
}

void GenerateRandomRequestsWithPropsAndSort() {
	indexToRequestMapping = {};
	requestsVec = {};
	
	requests = {};
	requestedFloors = {};

	spn::RandomGen& rng = spn::RandomGen::GetInstance();
	//int n = rng.GenerateInRange(0, 10);

	//int arr[] = { -2,2,-1,3, 0 };
	//int lfr = rng.GenerateInRange(floorsRangeStart, floorsRangeEnd);
	//int lfr = 0;
	//SetLiftFloor(lfr);
	//InitLiftPosition(lfr);
	std::cout << "Lift at " << liftState.currentFloor << '\n';

	//create random requests with properties wrt currentFloor of the lift
	for (int i = 0; i < 5; i++) {
		int rfloor = rng.GenerateInRange(floorsRangeStart, floorsRangeEnd);
		//int rfloor = arr[i];
		int rfloorIndex = ConvertFloorToIndex(rfloor);
		if (indexToRequestMapping.find(rfloorIndex) == indexToRequestMapping.end()) {
			UserRequest r = { rfloor, rfloorIndex };
			int floorDelta = rfloor - liftState.currentFloor;
			UserRequestWithProps urwp = { r,false,floorDelta, std::abs(floorDelta) };
			requestsVec.push_back(urwp);
			indexToRequestMapping[rfloorIndex] = urwp;
			std::cout << "Floor requested: " << rfloor << '\n';
		}
	}

	//sort according to floor delta 
	std::sort(requestsVec.begin(), requestsVec.end(), CompareUserRequestsWithProps);
}

void ScheduleAndEnqueueRequests(){
	if (requestsVec.empty()) {
		return;
	}
	LiftScanRange scanRange[2];
	//do nearest extreme scheduling and enqueue requests
	UserRequestWithProps rs = requestsVec.front();
	UserRequestWithProps re = requestsVec.back();
	if (rs.absoluteFloorDelta < re.absoluteFloorDelta) {
		scanRange[0].start = ConvertFloorToIndex(liftState.currentFloor);
		scanRange[0].end = rs.req.requestedIndex;
		scanRange[1].start = rs.req.requestedIndex;
		scanRange[1].end = re.req.requestedIndex;
	}
	else {
		scanRange[0].start = ConvertFloorToIndex(liftState.currentFloor);
		scanRange[0].end = re.req.requestedIndex;
		scanRange[1].start = re.req.requestedIndex;
		scanRange[1].end = rs.req.requestedIndex;
	}

	for (int k = 0; k < 2; k++) {
		int delta = scanRange[k].end - scanRange[k].start;
		if (delta > 0) {
			for (int i = scanRange[k].start; i <= scanRange[k].end; i++) {
				//check if i has a request: then queue it
				if (indexToRequestMapping.find(i) != indexToRequestMapping.end()) {
					UserRequestWithProps& urwp = indexToRequestMapping[i];
					if (!urwp.enqueued) {
						urwp.enqueued = true;
						requests.push(urwp.req);
						std::cout << urwp.req.requestedFloor << '\t';
						requestedFloors.push_back(urwp.req.requestedFloor);
					}
				}
			}
		}
		else if (delta < 0) {
			for (int i = scanRange[k].start; i >= scanRange[k].end; i--) {
				//check if i has a request: then queue it
				if (indexToRequestMapping.find(i) != indexToRequestMapping.end()) {
					UserRequestWithProps& urwp = indexToRequestMapping[i];
					if (!urwp.enqueued) {
						urwp.enqueued = true;
						requests.push(urwp.req);
						std::cout << urwp.req.requestedFloor << '\t';
						requestedFloors.push_back(urwp.req.requestedFloor);

					}
				}
			}
		}
		else if (delta == 0) {
			//check if scanRange[k].start has a request: then queue it
			int i = scanRange[k].start;
			if (indexToRequestMapping.find(i) != indexToRequestMapping.end()) {
				UserRequestWithProps& urwp = indexToRequestMapping[i];
				if (!urwp.enqueued) {
					urwp.enqueued = true;
					requests.push(urwp.req);
					std::cout << urwp.req.requestedFloor << '\t';
					requestedFloors.push_back(urwp.req.requestedFloor);

				}
			}
		}
	}
	std::cout << "Cost "<< CalculateTravelCost(liftState.currentFloor, requests) << '\n';
}

void UpdateAndRender(spn::Canvas* canvas) {
	canvas->Clear();
	canvas->SetPrimaryColorUint(0xffff00);
	int x=floorStartx, y = 50;
	char buffer[64];
	for (int i = 0; i < numberOfFloors; i++) {
		y = buildingOffsetFromTop - (i+1) * floorHeight;
		canvas->DrawRectangle(x, y, x + floorWidth, y + floorHeight);
		if (i + floorsRangeStart == 0) {
			sprintf(buffer, "Ground Floor");
		}
		else {
			sprintf(buffer, "Floor %d", i + floorsRangeStart);
		}
		
		canvas->DrawCString(buffer, x + 4, y + 4);
	}
	spn::Vec2d<float> lp = interpolator.Update(canvas->GetLastFrameTime());
	if (liftState.isLiftLoadingAndUnloading) {
		canvas->DrawFilledRectangle(lp.x, lp.y, lp.x + liftWidth, lp.y + liftHeight);
	}
	canvas->DrawRectangle(lp.x, lp.y, lp.x + liftWidth, lp.y + liftHeight);

	for (const auto& rf: requestedFloors) {
		Indicator indicator = floorToIndicatorMapping[rf];
		switch (indicator.status) {
		case PENDING:
			canvas->SetPrimaryColorUint(0xc0c000);
			break;
		case SERVING:
			canvas->SetPrimaryColorUint(0x0000c0);
			break;
		case SERVED:
			canvas->SetPrimaryColorUint(0xc0c0c0);
			break;
		}
		canvas->DrawFilledRectangle(
			indicator.point.x, 
			indicator.point.y, 
			indicator.point.x + 20, 
			indicator.point.y + 20
		);
	}

	uiManager->Display(canvas);
}

void HandleInput(const SDL_Event* sdlEvent) {
	using namespace spn::rmgui;
	using namespace spn::ui;
	UiEvent uie;
	TranslateSdlEvent(sdlEvent, uie);
	
	//switch (uie.eventType) {
	//case spn::ui::UiEventType::MouseDown:
	//	break;
	//case spn::ui::UiEventType::MouseUp:
	//	break;
	//case spn::ui::UiEventType::MouseDrag:
	//	break;
	//}
	uiManager->HandleUiEvent(uie);
}

void Init(spn::SpinachCore *core) {
	using namespace spn::rmgui;
	uiManager = &UiManager::GetInstance();
	nextButton = uiManager->CreateWidget<Button>();
	nextButton->SetPosition(800 - 130, 600 - 34);
	nextButton->SetSize(128, 32);
	nextButton->SetString("Next");
	nextButton->SetCallback([=](int id) {
		if (!hasEntitiesInQueue) {
			GenerateRandomRequestsWithPropsAndSort();
			ScheduleAndEnqueueRequests();
			hasEntitiesInQueue = true;
			for (auto& [i, indicator] : floorToIndicatorMapping) {
				if (
					indexToRequestMapping.find(ConvertFloorToIndex(i)) 
					== indexToRequestMapping.end()
					)
				{
					indicator.status = SERVED;
				}
				else {
					indicator.status = PENDING;
				}
			}
		}
		});
	numberOfFloors = (floorsRangeEnd - floorsRangeStart) + 1;
	int h = core->GetCanvas()->GetHeight();
	h -= 100;
	floorHeight = h / numberOfFloors;
	buildingOffsetFromTop = 50 + floorHeight * numberOfFloors;
	liftHeight = floorHeight;
	//liftState.currentFloor = floorsRangeStart;
	
	//liftState.currentFloor = (floorsRangeStart +floorsRangeEnd)/2;
	
	liftStartx = floorStartx + floorWidth + 2;
	SetLiftFloor((floorsRangeStart + floorsRangeEnd) / 2);
	int x = liftStartx, y = 50;
	for (int i = 0; i < numberOfFloors; i++) {
		y = buildingOffsetFromTop - (i + 1) * floorHeight;
		floorToLiftPointMapping[ConvertIndexToFloor(i)] =
			spn::Vec2d<float>{
				static_cast<float>(x),
				static_cast<float>(y),
		};
		Indicator indicator;
		indicator.point = spn::Vec2d<float>{
				static_cast<float>(floorStartx + floorWidth - 30),
				static_cast<float>(y + 20),
		};
		indicator.status = SERVED;
		floorToIndicatorMapping[ConvertIndexToFloor(i)] = indicator;
	}
	InitLiftPosition(liftState.currentFloor);
	liftState.isLiftLoadingAndUnloading = false;
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
	Init(&sc);
	std::atomic<bool> userQuit = false;
	std::thread liftMotionThread([&]() {
		while (!userQuit) {
			while (requests.empty()) {
				if (userQuit) {
					break;
				}
				hasEntitiesInQueue = false;
				liftState.isLiftLoadingAndUnloading = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
			if(requests.empty()){
				break;
			}
			auto f = requests.front();
			requests.pop();
			spn::Vec2d<float> dst = floorToLiftPointMapping[f.requestedFloor];
			spn::Vec2d<float> src = floorToLiftPointMapping[liftState.currentFloor];
			float animDelay = 0;
			if (f.requestedFloor < liftState.currentFloor) {
				animDelay = (liftState.currentFloor - f.requestedFloor);
			}
			else {
				animDelay = (f.requestedFloor - liftState.currentFloor);
			}
			std::cout << liftState.currentFloor << " ---> " << f.requestedFloor << '\n';
			interpolator.Set(src, dst, animDelay);
			
			liftState.isLiftLoadingAndUnloading = false;
			while (!interpolator.IsEndValueReached()) {
				if (userQuit) {
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
			liftState.isLiftLoadingAndUnloading = true;
			liftState.currentFloor = f.requestedFloor;
			floorToIndicatorMapping[f.requestedFloor].status = SERVING;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			floorToIndicatorMapping[f.requestedFloor].status = SERVED;
		}
		});
	sc.SetUpdateAndRenderHandler(UpdateAndRender);
	sc.SetInputHandler(HandleInput);	
	sc.SetWindowTitle("Lift Sim");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.LockFps(true);
	sc.MainLoop();
	userQuit = true;
	liftMotionThread.join();
	return 0;
}