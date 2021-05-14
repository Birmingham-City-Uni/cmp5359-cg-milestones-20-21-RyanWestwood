#pragma once
#include <chrono>
#include <iostream>

class Timer {
public:
	Timer() {
		t_start = std::chrono::high_resolution_clock::now();
		std::cout << "Hello" << std::endl;
	}

	~Timer() {
		passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		std::cout << "World" << std::endl;
	}

public:
	std::chrono::steady_clock::time_point t_start;
	std::chrono::steady_clock::time_point t_end;
	double passedTime;
};
