
/* Test program to compare the compile-time version of SDL with the linked
   version of SDL
*/

#include <stdio.h>

#include "SDL.h"
#include "SDL_byteorder.h"

int main(int argc, char *argv[])
{
	SDL_version compiled;

	/* Initialize SDL */
	if ( SDL_Init(0) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
#ifdef DEBUG
	fprintf(stderr, "SDL initialized\n");
#endif
	SDL_VERSION(&compiled);
	printf("Compiled version: %d.%d.%d\n",
			compiled.major, compiled.minor, compiled.patch);
	printf("Linked version: %d.%d.%d\n",
			SDL_Linked_Version()->major,
			SDL_Linked_Version()->minor,
			SDL_Linked_Version()->patch);
	printf("This is a %s endian machine.\n",
		(SDL_BYTEORDER == SDL_LIL_ENDIAN) ? "little" : "big");
	SDL_Quit();
	return(0);
}
