#include "Deep.h"
#include "Deep_Containers.h"
#include "Deep_Math.h"
#include "Deep_Engine.h"
#include "Deep_String.h"

#include <time.h>
#include <signal.h>

static volatile int running = DEEP_TRUE;

void closeSignalHandler(int dummy) {
	running = DEEP_FALSE;
}

int main()
{
	signal(SIGINT, closeSignalHandler);

#ifdef DEEP_DEBUG_MEMORY
	Deep_Debug_Memory_Start();
#endif

	struct Deep_Vector3 vec3 = Deep_Vector3_Create(10, 10, 0);
	Deep_Vector3_Scale_InPlace(&vec3, 2);
	printf("%f %f %f\n", vec3.x, vec3.y, vec3.z);

	while(running)
	{

	}

#ifdef DEEP_DEBUG_MEMORY
	Deep_Debug_Memory_End();
#endif
}

