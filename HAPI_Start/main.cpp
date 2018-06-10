#include "Simulation.hpp"

#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
// Every HAPI program has a HAPI_Main as an entry point
// When this function exits the program will close down
void HAPI_Main()
{
	//Pointer to the world, so that all variables of the class and subclases will be in the heap
	SIMULATION::Simulation* s = new SIMULATION::Simulation(1280, 720);
	s->start();
	delete s;

}

