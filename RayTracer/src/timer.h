#pragma once
#include <chrono>
#include <iostream>

class Timer {
public:
	Timer(std::string str) : msg(str) {
		t_start = std::chrono::high_resolution_clock::now();
	}
	Timer() : msg() {
		t_start = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		t_end = std::chrono::high_resolution_clock::now();
		passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
		std::cerr << msg << passedTime << " ms\n";
	}

public:
	std::string msg;
	std::chrono::steady_clock::time_point t_start;
	std::chrono::steady_clock::time_point t_end;
	double passedTime;
};
