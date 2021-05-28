#pragma once
#include <chrono>
#include <iostream>

class Timer {
public:
	Timer(std::string str) : msg(str) {
		t_start = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		
	}

	~Timer() {
		t_end = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		std::cerr << msg << t_end - t_start << " ms\n";
	}

public:
	std::string msg;
	double t_start, t_end;
};
