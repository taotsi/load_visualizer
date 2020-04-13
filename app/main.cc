#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "load_visualizer.hh"

using namespace std::chrono_literals;

int main(int, char**){
	dr::LoadVisualizer host;
	std::this_thread::sleep_for(2s);
	host.add_box({1, 1, 1, 5, 10, 3});

	return 0;
}
