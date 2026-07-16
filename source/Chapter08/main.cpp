#include "Renderer.h"

int main() 
{
	Renderer renderer(1920, 1080, 100, "../scenes/scene08.xml");
	renderer.Run();

	return 0;
}
