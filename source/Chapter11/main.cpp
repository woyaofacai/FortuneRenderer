#include "Renderer.h"

int main() 
{
	Renderer renderer(1920, 1080, 3, 15, 100, "../scenes/scene11_1.xml");
	renderer.Run();

	return 0;
}
