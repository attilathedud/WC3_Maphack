/*!
*	Dib-related helper functions.
*
*	Originally written 2010/02/16 by attilathedud.
*/

#define WIN32_LEAN_AND_MEAN
#define PI 3.14159

// Contains a coordinate for each pixel activated on screen. The z member is unused.
struct STAR
{
	short int x;
	short int y;
	short int z;
};

// Represents the current spin of the start point of the tunnel.
int scroll = 0;

// A pointer to our screen buffer.
unsigned char* buffer;

// Tables used for calculating the circle effect of the tunnel.
float sintable[360] = {0}, costable[360] = {0};

// The active pixels for our background and the rotating tunnel.
STAR background[300] = {0, 0, 0}, tunnel[64] = {0, 0, 0};

PAINTSTRUCT ps = {0};

// The display coordinates for our text displays.
RECT rect = {80, 50, 70, 70}, rect2 = {0, 0, 400, 205};

// A helper function to place pixels in the buffer with their coordinating colors. Coordinates 
// are accessed in the buffer by: loc = ( y * 400[ width of window ] ) + x. 
void placePixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	register int loc = 0;
	
	if(x >= 0 && x <= 400 && y >= 0 && y <= 205)
	{
		loc = (y * 400) + x;
		buffer[loc*4] = b;
		buffer[loc*4+1] = g;
		buffer[loc*4+2] = r;
	}
}
