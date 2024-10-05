#include <iostream>
#include <memory>

#include <NoEngineCore/Application.hpp>

class MyApp : public NoEngine::Application
{
	virtual void on_update() override
	{
		//std::cout << "Update frame: " << frame++ << std::endl;
	}

	int frame = 0;
};

int main()
{
	auto myApp = std::make_unique<MyApp>();

	int returnCode = myApp->start(1024, 768, "My first App");

	return returnCode;
}