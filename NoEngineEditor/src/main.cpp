#include "../includes/Editor.hpp"
#include <memory>

int main(char argc, char** argv)
{
	auto pNoEngineEditor = std::make_unique<NoEngineEditor>();
	int returnCode = pNoEngineEditor->start(1920, 1080/*1024*/, "NoEngine Editor", argv[0]);

	return returnCode;
}