#include "Renderer.h"

int main() 
{
	Renderer renderer(1920, 1080, 3, 15, 1000, "../scenes/scene09.xml");
	renderer.Run();

	return 0;
}
