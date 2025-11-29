/*
	WinMain() is the entry point of a Windows program
	(instead of the familiar main() in a console program)
*/

// Includes
//=========

#include "cAlienBreak.h"

// Entry Point
//============

int WINAPI WinMain( HINSTANCE i_thisInstanceOfTheApplication, HINSTANCE, char* i_commandLineArguments, int i_initialWindowDisplayState )
{
	unsigned seed = static_cast<unsigned>(time(nullptr)) ^
		static_cast<unsigned>(clock()) ^
		GetTickCount();
	srand(seed);
	// Warm up
	for (int i = 0; i < 5; ++i) ::rand();
	return eae6320::Application::Run<eae6320::cAlienBreak>( i_thisInstanceOfTheApplication, i_commandLineArguments, i_initialWindowDisplayState );
}
