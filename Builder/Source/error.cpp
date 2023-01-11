#include "error.h"

void exitFailure(std::string message)
{
	std::cout << "Error: " << message << std::endl;
	exit(1);
}