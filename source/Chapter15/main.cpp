#include "Renderer.h"

int main() 
{
	Renderer renderer(1920, 1080, 3, 8, 500, "../scenes/scene15_1.xml");
	renderer.Run();

	return 0;
}
