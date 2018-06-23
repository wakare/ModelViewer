#include <iostream>
#include "MainManager.h"

int main(int argc, char* argv[])
{
	auto mainManager = MainManager::GetInstance();
	if (!mainManager->Init())
		return EXIT_FAILURE;

	mainManager->AfterInit();
	mainManager->StartRun();

	return EXIT_SUCCESS;
}