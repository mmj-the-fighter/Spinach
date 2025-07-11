#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "spn_utils.h"


namespace spn
{
	void FindRectToRectIntersection(const Rect& r1, const Rect& r2, Rect& ir)
	{
		/*compute intersection*/
		//1st rectangle
		int p1x = r1.left;
		int p1y = r1.top;
		int p2x = p1x + r1.width;
		int p2y = p1y + r1.height;

		//second rectangle
		int p3x = r2.left;
		int p3y = r2.top;
		int p4x = p3x + r2.width;
		int p4y = p3y + r2.height;

		//intersected rectangle
		int p5x = std::max(p1x, p3x);
		int p5y = std::max(p1y, p3y);
		int p6x = std::min(p2x, p4x);
		int p6y = std::min(p2y, p4y);

		ir.left = p5x;
		ir.top = p5y;
		ir.width = std::abs(p5x - p6x);
		ir.height = std::abs(p5y - p6y);
	}

	void FindRectToRectUnion(const Rect& r1, const Rect& r2, Rect& ur)
	{
		ur.left = std::min(r1.left, r2.left);
		int r1Right = r1.left + r1.width;
		int r2Right = r2.left + r2.width;
		int right = std::max(r1Right, r2Right);
		ur.width = right - ur.left;

		ur.top = std::min(r1.top, r2.top);
		int r1Bottom = r1.top + r1.height;
		int r2Bottom = r2.top + r2.height;
		int bottom = std::max(r1Bottom, r2Bottom);
		ur.height = bottom - ur.top;
	}

	void testRectUnion() {
		Rect r1 = { 10,10,10,300 }; 
		Rect r2 = { 10,20,20,30 };

		Rect r;
		FindRectToRectUnion(r1, r2, r);

		std::cout << r.left << " " << r.top << " " << r.width << " " << r.height << "\n";
	}

	//This function was developed with the help of ChatGPT AI agent
	std::string GetTimeBasedScreenShotFileName()
	{
		auto now = std::chrono::system_clock::now();
		std::time_t time_now = std::chrono::system_clock::to_time_t(now);
		std::tm tm_now = *std::localtime(&time_now);

		std::ostringstream oss;
		oss << std::put_time(&tm_now, "%Y%m%d_%H%M%S.png");

		return oss.str();
	}
}

