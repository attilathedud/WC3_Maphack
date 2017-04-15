/*!
*	The front-end responsible for injecting the WC3Maphack.dll into the target process. Uses a dib-frontend
*	for display. Assumes the target process is open before injecting and the dll resides in the same directory
*	as the injector.
*
*	The dib tunnel code is modified code originally written by atom0s.
*
*	Originally written 2010/02/16 by attilathedud.
*/

/*!
*	The main file is primarily responsible for rendering the window and controls.
*/

#include <windows.h>
#include <cmath>
#include "inject.h"
#include "resource.h"
#include "ufmod.h"
#include "common.h"

// Device context used to blt pixels on screen.
HDC dc = 0;

// The path of our dll
char path[256] = {0};

// Flag that will be switched to hide once we inject our dll.
char injected = SW_SHOW;

// Represents whether we will display the info or not.
bool nfo = false;

// Our button handles.
HWND bStart = 0, bAbout = 0, bBack = 0, bExit = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
		/*!
		*	On window creation, place our buttons, start our music, and then start our dib effects.
		*
		*	The end effect is a tunnel in which the start point rotates around in a circle.
		*/
        case WM_CREATE:
            bStart = CreateWindow("button", "Inject", WS_VISIBLE | WS_CHILD, 25, 281, 100, 20, hWnd, (HMENU)1, NULL, NULL);
            bAbout = CreateWindow("button", "About", WS_VISIBLE | WS_CHILD, 150, 281, 100, 20, hWnd, (HMENU)3, NULL, NULL);
            bExit = CreateWindow("button", "Exit", WS_VISIBLE | WS_CHILD, 275, 281, 100, 20, hWnd, (HMENU)2, NULL, NULL);
			bBack = CreateWindow("button", "Back", WS_CHILD, 150, 281, 100, 20, hWnd, (HMENU)4, NULL, NULL);
			uFMOD_PlaySong("#1337", 0, XM_RESOURCE);
			SetTimer(hWnd, 4, 10, 0);
			for(int i = 0; i < 360; i++)
			{
				if(i < 64)
					tunnel[i].z = 512-i*8;
				if(i <300)
				{
					background[i].x = rand()%400;
					background[i].y = rand()%200+1;
				}
				sintable[i] = (float)sin(PI/180*i);
				costable[i] = (float)cos(PI/180*i);
			}
            break;
		case WM_PAINT:
			BitBlt(BeginPaint(hWnd, &ps), 0, 69, 400, 205, dc, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
			break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
				// On inject, get our current path and inject the dll
                case 1:
					GetModuleFileName(NULL, path, sizeof(path));
                    for(int i = strlen(path); path[i] != '\\'; i--)
						path[i] = 0;
                    strcat(path, "irWC3Maphack.dll");
					if(!inject(path))
						MessageBox(hWnd, "Game is not running.", "irWC3Maphack", MB_OK);
					else{
						MessageBox(hWnd, "Injected Successfully!", "irWC3Maphack", MB_OK);
						ShowWindow(bStart, SW_HIDE);
						injected = SW_HIDE;
					}	
                    break;
                case 2:
                    PostQuitMessage(0);
                    break;
				// On About, toggle the info to display
                case 3:
					ShowWindow(bStart, SW_HIDE);
					ShowWindow(bAbout, SW_HIDE);
					ShowWindow(bExit, SW_HIDE);
					ShowWindow(bBack, SW_SHOW);
					nfo = true;
					break;
				// On Back from About, toggle the info off
				case 4:
					ShowWindow(bStart, injected);
					ShowWindow(bAbout, SW_SHOW);
					ShowWindow(bExit, SW_SHOW);
					ShowWindow(bBack, SW_HIDE);
					nfo = false;
			}
            break;
		// On each tick of our timer, calculate the new location of all the pixels for the tunnel effect.
		case WM_TIMER:
			memset(buffer, 0, 328000);
			
			for(int i = 0; i < 400; i++)
			{
				if(i < 300)
					placePixel(background[i].x, background[i].y, 100, 100, 100);

				placePixel(i, 0, 250, 250, 250);
			}

			for(int i = 0; i < 64; i++)
			{
				tunnel[i].x = short int((sintable[(tunnel[i].z + scroll)%360] * sintable[(tunnel[i].z/2+scroll)%360]) * 50);
				tunnel[i].y = short int(costable[(tunnel[i].z + scroll)%360] * 35 - costable[scroll%360]);
				tunnel[i].z -= 2;
			
				if(tunnel[i].z <= 0)
					tunnel[i].z = 512;
				for( int j = 0; j < 360; j += 10 )
					placePixel( (int)(128 * (tunnel[i].x + sintable[j%360] * 100) / tunnel[i].z + 200 - tunnel[i].x * 3),  
								(int)(128 * (tunnel[i].y + costable[j%360] * 100) / tunnel[i].z + 150 - tunnel[i].y * 3 ), 
								75, 125, 256 - tunnel[i].z /2);
			}
			if(scroll++ >= 360)
				scroll = 0;
			
			if(nfo == false)
				DrawText(dc, "= irWC3Maphack :: WarCraft III 1.24d =\n         Code..................attilathedud\n         Graphics............................L.\n         Tune.......................ghidorah", 175, &rect, DT_NOCLIP);
			else
				DrawText(dc, "                            DoxCoding.com presents:\n\n                         irWC3Maphack by attilathedud\n----------------------------------------------------------------------------------------------------\n Thanks to:                                     Keys:\n|         atom0s - for his   |              |        F5 - Toggle Hack        |\n|         tunnel code and   |\n|         his constant help.|\n----------------------------------------------------------------------------------------------------\n\n Shout-Outs: \n                 -------�pandas, STN, yourdecay, Couch�-------", 579, &rect2, DT_NOCLIP);

			RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE);
			break;
		case WM_LBUTTONDOWN:
			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			break;
		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE)
				PostQuitMessage(0);
			break;
        case WM_DESTROY:
			KillTimer(hWnd, 4);
            PostQuitMessage(0);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// WinMain's only responsibility is to create our window and device context and then listen for messages.
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    HWND hwnd;
    WNDCLASS wc = {0};
    MSG msg = {0};
	BITMAPINFO plane = {0};

	plane.bmiHeader.biSize = 40;
	plane.bmiHeader.biWidth = 400;
	plane.bmiHeader.biHeight = -205;
	plane.bmiHeader.biPlanes = 1;
	plane.bmiHeader.biBitCount = 32;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "TrainerEngine";
    wc.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, (char*)ID_BACK));

    RegisterClass(&wc);
    
    hwnd = CreateWindow(wc.lpszClassName, "irWC3Maphack", WS_POPUP,  (GetSystemMetrics(SM_CXSCREEN)-400)/2, (GetSystemMetrics(SM_CYSCREEN)-303)/2, 400, 303, NULL, NULL, hInstance, NULL);

	dc = CreateCompatibleDC(GetDC(hwnd));
	SelectObject(dc, CreateDIBSection(dc, &plane, 0, (void**)&buffer, 0, 0)); 
	SetBkMode(dc, TRANSPARENT);
	SetTextColor(dc, 0xffffff);

	ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    UnregisterClass("TrainerEngine", hInstance);

    while(1)
    {
        GetMessage(&msg, NULL, 0, 0);

        if(msg.message == WM_QUIT)
            break;
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}