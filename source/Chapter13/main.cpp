#include "Renderer.h"

int main() 
{
	Renderer renderer(1920, 1080, 3, 15, 500, "../scenes/scene13.xml");
	renderer.Run();

	return 0;
}
