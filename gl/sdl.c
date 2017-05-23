#include "sdl.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define S static
#define SAGC(x,y) SDL_GL_SetAttribute(SDL_GL_CONTEXT_##x, y);
#define SASG(x,y) SDL_GL_SetAttribute(SDL_GL_##x, y);
#define GC(x) SDL_GL_CONTEXT_##x

S SDL_Window *sdl_displayWindow;


S void sdl_init_window(void)
{
    if(SDL_WasInit(SDL_INIT_VIDEO))return;
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE);
    SAGC(MAJOR_VERSION,3);SAGC(MINOR_VERSION,2);
    SDL_GL_SetSwapInterval(0);
    SASG(DOUBLEBUFFER,1); SAGC(PROFILE_MASK,GC(PROFILE_CORE));
    SASG(ACCELERATED_VISUAL,1);
    sdl_displayWindow=SDL_CreateWindow("gl",SDL_WINDOWPOS_UNDEFINED_DISPLAY(1),SDL_WINDOWPOS_UNDEFINED,2*400,2*224,SDL_WINDOW_OPENGL);
    int w,h;
    SDL_GL_GetDrawableSize(sdl_displayWindow,&w,&h);
    printf("windows size: %u x %u\n", w, h);
    SDL_SetRelativeMouseMode(1);
}


S void sdl_init_context(void)
{
    SDL_Window *w = SDL_GL_GetCurrentWindow();
    SDL_GLContext *c = SDL_GL_GetCurrentContext();
    if (c) SDL_GL_DeleteContext(c);
    if (w) sdl_displayWindow = w;
    SDL_GL_CreateContext(sdl_displayWindow);
    printf("GL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
}


S uint32_t sdl_fps_last_report;
S uint32_t sdl_fps_last_frame;
S void sdl_init_fps(void) {
	sdl_fps_last_report = SDL_GetTicks();
	sdl_fps_last_frame = SDL_GetTicks();
}
S uint32_t sdl_fps_average_ms;
S uint32_t sdl_fps_reported = 0;
S void sdl_report_fps(void) {
	uint32_t now = SDL_GetTicks();
	uint32_t dt = now - sdl_fps_last_report;
	if(dt > 1000) {
		if(sdl_fps_average_ms == 0) sdl_fps_average_ms = 1;
		sdl_fps_reported = 1000/sdl_fps_average_ms;
		printf("FPS: %u\n", sdl_fps_reported);
		sdl_fps_last_report = now;
	}
	uint32_t ms = now - sdl_fps_last_frame;
	sdl_fps_last_frame = now;
	float diff = (float)ms - sdl_fps_average_ms;
	float ratio = 1000.0 / ms;
	float add = diff / ratio;
 	printf("FPSd: [%u] ms %u diff %f ratio %f add %f\n", sdl_fps_reported, ms, diff, ratio, add);
	if(add > 0 && add < 1) add = 1;
	if(add < 0 && add > -1) add = -1;
	sdl_fps_average_ms += add;
}

void sdl_init(void) { sdl_init_window(); sdl_init_context(); sdl_init_fps(); }
void sdl_end_frame(void) { SDL_GL_SwapWindow(sdl_displayWindow); sdl_report_fps(); }

