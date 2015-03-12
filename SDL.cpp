#include "stdafx.h"

#include <windows.h>


namespace
{

	const char* windowTitle = "NoX: UniMod(NoXWorld.ru) with SDL patch(Zoaedk and Morden)";

#define BUTTON_RELEASED 0
#define BUTTON_PRESSED 1

	//#define ASSIGN(X,Y) *((DWORD*)&(X))=((DWORD)(Y));

	typedef struct {

		int x;
		int y;
		int z;

		int field_C;
		int field_10;
		int button0;
		int field_18;
		int button0_seq;
		int button1;
		int field_24;
		int button1_seq;
		int button2;
		int field_30;
		int button2_seq;
	} MouseData;

	/* must increment in Flip */
	DWORD * pFrameNumber = (DWORD *)0x973C7C;
	/* these must be initialized */
	DWORD * pScreenWidthBytes = (DWORD *)0x974864;
	DWORD * pScreenPitchBytes = (DWORD *)0x974868;
	DWORD * pScreenWidthDwords = (DWORD *)0x974844;
	DWORD * pScreenWidthChunks = (DWORD *)0x97485C; /* 32-byte chunks */

	DWORD * pScreenFlag1 = (DWORD *)0x974848;
	DWORD * pScreenFlag2 = (DWORD *)0x973FDC;

	DWORD * pScreenNumRows = (DWORD *)0x974850;
	DWORD * pArrayRowBuffer = (DWORD *)0x973C94;

	/* should return 1 */
	typedef int(__cdecl * TrueMouseInit) ();
	TrueMouseInit pTrueMouseInit = (TrueMouseInit)0x47D8D0;

	/* unnecessary */
	typedef void(__cdecl * TrueUpdateMouse) (int x, int y, BOOL absolute);
	TrueUpdateMouse pTrueUpdateMouse = (TrueUpdateMouse)0x430A00;

	/* must fill in mouseData as appropriate */
	typedef char(__cdecl * TrueGetMouseData) (MouseData * mouseData);
	TrueGetMouseData pTrueGetMouseData = (TrueGetMouseData)0x47DB20;

	/* handle our messages, then send them appropriate keyboard (WM_CHAR) messages */
	typedef void(__cdecl * TrueMessageLoop) ();
	TrueMessageLoop pTrueMessageLoop = (TrueMessageLoop)0x4453A0;

	typedef void(__cdecl * TrueWaitForFlip) ();
	TrueWaitForFlip pTrueWaitForFlip = (TrueWaitForFlip)0x48A220;

	typedef void(__cdecl * TrueDraw) ();
	TrueDraw pTrueDraw = (TrueDraw)0x4AD170;

	typedef void(__cdecl * TrueFlip) ();
	TrueFlip pTrueFlip = (TrueFlip)0x48A290;

	typedef void(__cdecl * TrueSetupWindow) ();
	TrueSetupWindow pTrueSetupWindow = (TrueSetupWindow)0x48AA40;

	typedef void(__cdecl * TrueSetupWindowAfter) (DWORD uFlags);
	TrueSetupWindowAfter pTrueSetupWindowAfter = (TrueSetupWindowAfter)0x48A820;

	typedef int(__cdecl * TrueDirectDrawInit) ();
	TrueDirectDrawInit pTrueDirectDrawInit = (TrueDirectDrawInit)0x48B000;

	typedef void(__cdecl * TrueSetupFullscreenSurface) (int width, int height, int bpp);
	TrueSetupFullscreenSurface pTrueSetupFullscreenSurface = (TrueSetupFullscreenSurface)0x48AD40;

	typedef void(__cdecl * TrueSetupVideo) (HWND handle, int width, int height, int bpp);
	TrueSetupVideo pTrueSetupVideo = (TrueSetupVideo)0x48A040;

	typedef int(__cdecl * TrueSkipped1) ();
	TrueSkipped1 pTrueSkipped1 = (TrueSkipped1)0x4338D0;

	typedef int(__cdecl * TrueSkipped2) ();
	TrueSkipped2 pTrueSkipped2 = (TrueSkipped2)0x48B1F0;

	typedef int(__cdecl * TrueDirectDrawCreatePalette) ();
	TrueDirectDrawCreatePalette pTrueDirectDrawCreatePalette = (TrueDirectDrawCreatePalette)0x434FB0;

	void(__cdecl * LoadAutomationCfg) (const char * fn) = (void(__cdecl *) (const char * fn)) 0x4A7D70;

	// Cleanly Exiting the NoX Process - Thanks Zoaedk //////
	void(__cdecl * EndGameReason) (int reason) = (void(__cdecl *) (int reason)) 0x43DDD0;
	void(__cdecl * EndGame) () = (void(__cdecl *) ()) 0x43DE60;



	// ALLREADY WORKING FUNCTIONS //////
	int(__cdecl * noxDrawRect) (int xLeft, int yTop, int width, int height) = (int(__cdecl *) (int xLeft, int yTop, int width, int height)) 0x0049CE30;
	int(__cdecl * npxSetRectColorMB) (int) = (int(__cdecl *) (int)) 0x00434460;
	int(__cdecl * noxDrawRectAlpha) (int xLeft, int yTop, int width, int height) = (int(__cdecl *) (int xLeft, int yTop, int width, int height)) 0x0049CF10;
	int(__cdecl *consolePrint) (int Color, wchar_t *Text) = (int(__cdecl *)(int Color, wchar_t *Text)) 0x00450B90;

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* texture = NULL;

	int *noxScreenWidth = (int*)0x0059D684;
	int *noxScreenHeight = (int*)0x0059D688;
	int *cursorScreenX = (int*)0x006990B0;
	int *cursorScreenY = (int*)0x006990B4;

	wchar_t TextBuffer[255];


	SDL_DisplayMode mode;
	SDL_Texture* debugTexture;
	SDL_Rect debugPosition;
	TTF_Font* debugFont;

	float scaleFactor = 0.0f;
	float MouseAccel = 1.0f;
	float customscaleFactor = 0.0f;
	int ShowDebug = 0;
	int UseFullScreen = 0;
	int ScreenWidth = 0;
	int ScreenHeight = 0;
	int UseBorderless = 0;
	int ScaleQuality = 0;
	int UseAcceleration = 1;
	int UseDirectX = 1;

	int noxScreenX_SDL = 0;
	int noxScreenY_SDL = 0;

	SDL_Rect srcRec;
	SDL_Rect destRec;

	static BOOL serverOnly = FALSE;
	static SDL_bool fullScreen = SDL_FALSE;

	int gameWidth, gameHeight;

	static BOOL reportMouse = TRUE;
	int button0;
	unsigned int button0_valid = 0;
	int button1;
	unsigned int button1_valid = 0;
	int button2;
	unsigned int button2_valid = 0;

	/* scroll wheel */
#define Z_QUANTUM 1
	int z_diff;

	bool SDL_enabled = false;

	void ReadConfigFile()
	{

		char Variable[255];
		char Value[255];
		FILE *fil = fopen("noxSDL.cfg", "r");

		if (fil == NULL)
			return;
		// Read stuff and parse damn it
		// Need to do this correctly at some point instead of half assed text parsing
		while (!feof(fil))
		{
			fscanf(fil, "%s", Variable);
			fscanf(fil, "%s", Value);

			if (!strcmp("MOUSEACCEL", Variable))
			{
				MouseAccel = atof(Value);
			}
			else if (!strcmp("SHOWDEBUG", Variable))
			{
				ShowDebug = atoi(Value);
			}
			else if (!strcmp("VIEWSCALE", Variable))
			{
				customscaleFactor = atof(Value);
			}
			else if (!strcmp("FULLSCREEN", Variable))
			{
				UseFullScreen = atoi(Value);
			}
			else if (!strcmp("SCREENWIDTH", Variable))
			{
				ScreenWidth = atoi(Value);
			}
			else if (!strcmp("SCREENHEIGHT", Variable))
			{
				ScreenHeight = atoi(Value);
			}
			else if (!strcmp("BORDERLESS", Variable))
			{
				UseBorderless = atoi(Value);
			}
			else if (!strcmp("ACCELERATION", Variable))
			{
				UseAcceleration = atoi(Value);
			}
			else if (!strcmp("SCALEQUALITY", Variable))
			{
				ScaleQuality = atoi(Value);
			}
			else if (!strcmp("USEDIRECTX", Variable))
			{
				UseDirectX = atoi(Value);
			}
			else if (!strcmp("NOXRESX", Variable))
			{
				int value = atoi(Value);
				if (value != 0)
				{
					*noxScreenWidth = value;
					noxScreenX_SDL = value;
				}
			}
			else if (!strcmp("NOXRESY", Variable))
			{
				int value = atoi(Value);
				if (value != 0)
				{
					*noxScreenHeight = value;
					noxScreenY_SDL = value;
				}
			}
			else if (!strcmp("ENABLED", Variable))
			{
				int value = atoi(Value);
				if (value != 0)
				{
					SDL_enabled = true;
				}
			}
		}

		fclose(fil);
	}

	static void UpdateDebugSurface(char * str)
	{
		SDL_Color color;
		SDL_Surface* surface;
		color.r = 0xFF;
		color.g = 0xFF;
		color.b = 0xFF;
		surface = TTF_RenderText_Solid(debugFont, str, color);
		debugTexture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		TTF_SizeText(debugFont, str, &debugPosition.w, &debugPosition.h);
		debugPosition.x = 0;
		debugPosition.y = 0;
	}

	static void SetDebugText(char * fmt, ...)
	{
		va_list args;
		char buffer[1024];

		va_start(args, fmt);
		vsprintf(buffer, fmt, args);
		UpdateDebugSurface(buffer);
	}
	static void MessageLoop()
	{
		//return pTrueMessageLoop();
		SDL_Event event;
		const char * button_name;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				ExitProcess(0);
				EndGameReason(0);
				EndGame();
				break;
			case SDL_KEYDOWN:
				//SetDebugText("KeyDown is %d", event.key.keysym.sym);
				if ((event.key.keysym.mod & KMOD_ALT) && event.key.keysym.sym == SDLK_F4)
				{
					ExitProcess(0);
					EndGameReason(0);
					EndGame();
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					button_name = "LEFT";
					button0 = event.button.state == SDL_PRESSED ? BUTTON_PRESSED : BUTTON_RELEASED;
					button0_valid = 1;
					break;
				case SDL_BUTTON_RIGHT:
					button_name = "RIGHT";
					button1 = event.button.state == SDL_PRESSED ? BUTTON_PRESSED : BUTTON_RELEASED;
					button1_valid = 1;
					break;
				case SDL_BUTTON_MIDDLE:
					button_name = "MIDDLE";
					button2 = event.button.state == SDL_PRESSED ? BUTTON_PRESSED : BUTTON_RELEASED;
					button2_valid = 1;
					break;
				case SDL_BUTTON_X1:
					button_name = "X1";
					break;
				case SDL_BUTTON_X2:
					button_name = "X2";
					break;
				}
				//SetDebugText("%s is %s", button_name, event.button.state == SDL_PRESSED ? "DOWN" : "UP");
				break;
			case SDL_MOUSEWHEEL:
				//SetDebugText("Wheel moved %d", event.wheel.y);
				z_diff += event.wheel.y;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
				{
					if (UseFullScreen == 1)
						SDL_GetCurrentDisplayMode(0, &mode);
					else if (ScreenWidth == 0 || ScreenHeight == 0)
						SDL_GetCurrentDisplayMode(0, &mode);
					else
					{
						mode.w = ScreenWidth;
						mode.h = ScreenHeight;
					}

					// Allow choosing display to show game on
					SDL_RenderSetViewport(renderer, NULL);
				}
				}
				break;
			default:
				break;
			}
		}
	}

	static void MouseInit()
	{
		if (serverOnly) return;

		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	static char GetMouseData(MouseData* data)
	{
		int x, y;
		int buttons;

		if (reportMouse == FALSE) return 0;

		buttons = SDL_GetRelativeMouseState(&x, &y);
		data->x = x * MouseAccel;
		data->y = y * MouseAccel;
		data->z = z_diff;
		z_diff = 0;

		/* you must only send button event, when they happen, not multiple times */
		data->button0 = button0;
		data->button0_seq = button0_valid;
		data->button1 = button1;
		data->button1_seq = button1_valid;
		data->button2 = button2;
		data->button2_seq = button2_valid;
		button0_valid = 0;
		button1_valid = 0;
		button2_valid = 0;

		reportMouse = FALSE;
		return 1;
	}

	static void SetupWindow()
	{
	}

#if 0
	static int DirectDrawInit()
	{
		return 1;
	}

	static void SetupFullscreenSurface(int width, int height, int bpp)
	{
	}

	static void SetupWindowAfter(DWORD uFlags)
	{
	}
#endif

	static void SetupVideo(HWND handle, int width, int height, int bpp)
	{
		if (noxScreenY_SDL != 0 && noxScreenX_SDL != 0)
		{
			*noxScreenHeight = noxScreenY_SDL;
			*noxScreenWidth = noxScreenX_SDL;
			gameWidth = width;
			gameHeight = height;
		}
		else
		{
			gameWidth = width;
			gameHeight = height;
		}
		/* make it a multiple of 32 */
		width &= ~31;

		*pScreenWidthBytes = width * 2;
		*pScreenWidthDwords = width * 2 / 4;
		*pScreenWidthChunks = width * 2 / 32;
		*pScreenPitchBytes = width * 2;

		*pScreenFlag1 = 1;
		*pScreenFlag2 = 2; // pixel format (0=INDEX8,1=??,2=RGB565)

		if (texture)
		{
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, width, height);
	}

	static void WaitForFlip()
	{
	}

	static void Draw()
	{
		int i;
		void ** arrayPixelData = (void**)*pArrayRowBuffer;
		unsigned char * dstPixelData;
		int dstPitch;

		if (serverOnly) return;

		// This will return the size of the NoX in game screen before stretch
		SDL_QueryTexture(texture, NULL, NULL, &srcRec.w, &srcRec.h);

		// If the user did not set a custom scale, then automatically create one that fits best
		if (customscaleFactor != 0.0f && srcRec.w > 640)
			scaleFactor = customscaleFactor;
		else
			scaleFactor = ((float)mode.h) / ((float)srcRec.h);

		destRec.h = srcRec.h*scaleFactor;
		destRec.w = srcRec.w*scaleFactor;

		if (destRec.w > mode.w)
		{
			// This IF is a hack, if they use 640,480 in game res for some reson it will break
			// I didn't see this as a possibility ever so I ignored it
			// Currently the main menus default to 640x480 so I can ignore custom scaling in menus with this
			if (customscaleFactor != 0.0f && srcRec.w > 640)
				scaleFactor = customscaleFactor;
			else
				scaleFactor = ((float)mode.w) / ((float)srcRec.w);

			destRec.h = srcRec.h*scaleFactor;
			destRec.w = srcRec.w*scaleFactor;
		}

		// Zoaedks texture writing function, not really anything to improve here, about as fast as it is going to get
		SDL_LockTexture(texture, NULL, (void**)&dstPixelData, &dstPitch);

		for (i = 0; i < *pScreenNumRows; i++)
		{
			void * rowPixelData = arrayPixelData[i];
			memcpy(dstPixelData, rowPixelData, *pScreenPitchBytes);
			dstPixelData += dstPitch;
		}

		SDL_UnlockTexture(texture);


		// Center the scaled image on the screen if it doesn't take up X or Y fully

		// Center Width
		if (destRec.w < mode.w)
			destRec.x = mode.w / 2 - destRec.w / 2;
		else
			destRec.x = 0;

		// Center Height
		if (destRec.h < mode.h)
			destRec.y = mode.h / 2 - destRec.h / 2;
		else
			destRec.y = 0;

		// Clear the renderer so that garbage isn't displayed on the edges
		SDL_RenderClear(renderer);

		// Not sure why i'm not scaling on full screen - Check this
		if (UseFullScreen == 1)
			SDL_RenderCopy(renderer, texture, NULL, NULL);
		else
			SDL_RenderCopy(renderer, texture, NULL, &destRec);

		if (ShowDebug != 0)
			SDL_RenderCopy(renderer, debugTexture, NULL, &debugPosition);
	}


	static void Flip()
	{
		reportMouse = TRUE;
		*pFrameNumber++;

		if (serverOnly) return;

		SDL_RenderPresent(renderer);
	}

	/* nullsub for something we just want to ignore for now */
	static int Skipped()
	{
		return 1;
	}
}

extern void InjectJumpTo(DWORD Addr, void *Fn);
void initSDL()
{
	// Get the players settings
	ReadConfigFile();

	if (SDL_enabled)
	{
		InjectJumpTo((DWORD)pTrueMessageLoop, &MessageLoop);
		InjectJumpTo((DWORD)pTrueMouseInit, &MouseInit);
		InjectJumpTo((DWORD)pTrueGetMouseData, &GetMouseData);
		InjectJumpTo((DWORD)pTrueFlip, &Flip);
		InjectJumpTo((DWORD)pTrueWaitForFlip, &WaitForFlip);
		InjectJumpTo((DWORD)pTrueDraw, &Draw);
		InjectJumpTo((DWORD)pTrueSetupVideo, &SetupVideo);
		InjectJumpTo((DWORD)pTrueSkipped2, &Skipped);
		InjectJumpTo((DWORD)pTrueDirectDrawCreatePalette, &Skipped);
		InjectJumpTo((DWORD)pTrueSetupWindow, &SetupWindow);

		/* #TODO:
		Add in some of the unimod hooks like NoX lists, buttons, and element message functions
		Then in game menus would be possible just like it is with them.

		Should also probably wrap up the NoX in game drawing functions into a class
		*/


		SDL_Init(SDL_INIT_VIDEO);

		TTF_Init();

		debugFont = TTF_OpenFont("C:\\Windows\\Fonts\\verdana.ttf", 20);

		SDL_GetCurrentDisplayMode(0, &mode);
		if (ScreenWidth != 0 && ScreenHeight != 0)
		{
			mode.h = ScreenHeight;
			mode.w = ScreenWidth;
		}

		// Create the window, I could just set flags instead, but I was lazy at the time, should probably revise to use flags
		// so that I don't look like an ass later to whoever is reading this ~(o.O)~
		if (UseFullScreen == 1)
		{
			window = SDL_CreateWindow(windowTitle, // title
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // x and y
				mode.w, mode.h, // w and h
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

			if (ScreenWidth == 0 || ScreenHeight == 0)
				SDL_SetWindowSize(window, gameWidth, gameHeight);

			else
				SDL_SetWindowSize(window, ScreenWidth, ScreenHeight);

			SDL_SetWindowFullscreen(window, 1);
		}
		else if (UseBorderless == 1)
		{
			window = SDL_CreateWindow(windowTitle, // title
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // x and y
				mode.w, mode.h, // w and h
				SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
		}
		else
		{
			window = SDL_CreateWindow(windowTitle, // title
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // x and y
				mode.w, mode.h, // w and h
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		}


		// Force the game to use a specific renderer, this is here because it defaults to one automatically and directx is the only one
		// that is listed in the SDL documentation as supporting the best scaling anti-aliasing.
		if (UseDirectX)
			SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
		else
			SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

		switch (ScaleQuality)
		{
			// Case 0 is the only one that works if accelleration is turned off
		case 0:	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); break; // No scaling antialiasing, ugly as shit
		case 1:	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); break; // DirectX / OpenGL scaling with antialiasing
		case 2:	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); break; // According to documentation, DirectX only
		default: break;
		}

		// Anyone that can, needs to leave acceleration on
		if (UseAcceleration)
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		else
		{
			SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "0");
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		}
	}
}
