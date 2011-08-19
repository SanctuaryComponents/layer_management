/***************************************************************************
 *
 * Copyright 2011 Valeo
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#include "xevents.h"


/******************************************************************************/
/*                             DEFINES & MACROS                               */
/******************************************************************************/
#define ZOOM_FACTOR 		1.2f
#define SURFACE_MOVE_STEP 	10

/******************************************************************************/
/*                             TYPES DEFINITIONS                              */
/******************************************************************************/

enum e_surfMove
{
	move_right,
	move_down,
	move_left,
	move_up
};


/******************************************************************************/
/*                         STATIC FUNCTIONS DEFINITIONS                       */
/******************************************************************************/

static void moveSurface();
static void zoomSurface(t_ilm_int x, t_ilm_int y);
static void unzoomSurface();



/******************************************************************************/
/*                         STATIC GLOBAL VARIABLES                            */
/******************************************************************************/

static t_ilm_int g_surfPosX;
static t_ilm_int g_surfPosY;
static t_ilm_int g_surfWidthOrig;
static t_ilm_int g_surfHeightOrig;

static Display* g_x11Display;
static t_ilm_surface g_surfaceid;
static enum e_surfMove g_surfMoveDirection;




/**
 * @Description Initialize the xevent management module
 *
 * @param dpy X11 Display
 * @param Surface ID of the surface to control
 * @param x Initial destination x position of the surface
 * @param y Initial destination y position of the surface
 * @param width Initial width of the surface destination
 * @param height Initial height of the surface destination
 *
 */
void xeventInitialiaze(Display* dpy, t_ilm_surface surfaceid, t_ilm_int x, t_ilm_int y, t_ilm_int width, t_ilm_int height)
{
	g_surfPosX = x;
	g_surfPosY = y;
	g_surfWidthOrig = width;
	g_surfHeightOrig = height;
	
	g_x11Display = dpy;
	g_surfaceid = surfaceid;
	g_surfMoveDirection = move_right;
}


void parseXEventsNonBlocking()
{
	XEvent e;
	XButtonEvent *peB;

	while (XPending(g_x11Display))
	{
		XNextEvent(g_x11Display, &e);
		switch (e.type)
		{
			case ButtonPress:
				peB = (XButtonEvent *) &e;
				// Make something visible to the end user
				if (peB->button == 2)  // Middle Click
				{
					moveSurface();
				}
				else if (peB->button == 1)  // Left Click
				{
					zoomSurface(peB->x, peB->y);
				}
				else if (peB->button == 3)  // Right Click
				{
					unzoomSurface();
				}
				break;

			default:
				break;
		}
	}
}



static void moveSurface()
{
	static t_ilm_uint screenWidth = 0;
	static t_ilm_uint screenHeight = 0;
	static t_ilm_int surfWidth = 0;
	static t_ilm_int surfHeight = 0;
	struct ilmSurfaceProperties surfaceProperties;

	// Init
	if (!screenWidth || !screenHeight)
	{
		ilm_getScreenResolution(0, &screenWidth, &screenHeight);
	}
	if (!surfWidth || !surfHeight)
	{
		ilm_getPropertiesOfSurface(g_surfaceid, &surfaceProperties);
		surfWidth = (t_ilm_int) surfaceProperties.destWidth;
		surfHeight = (t_ilm_int) surfaceProperties.destHeight;
	}
	
	// Do the real job
	switch (g_surfMoveDirection)
	{
		case move_right:
			g_surfPosX += SURFACE_MOVE_STEP;
			if (g_surfPosX >= (t_ilm_int) (screenWidth - surfWidth))
			{
				g_surfPosX = screenWidth - surfWidth;
				g_surfMoveDirection = move_down;
			}
			break;
			
		case move_down:
			g_surfPosY += SURFACE_MOVE_STEP;
			if (g_surfPosY >= (t_ilm_int) (screenHeight - surfHeight))
			{
				g_surfPosY = screenHeight - surfHeight;
				g_surfMoveDirection = move_left;
			}
			break;
			
		case move_left:
			g_surfPosX -= SURFACE_MOVE_STEP;
			if (g_surfPosX <= 0)
			{
				g_surfPosX = 0;
				g_surfMoveDirection = move_up;
			}
			break;
			
		case move_up:
			g_surfPosY -= SURFACE_MOVE_STEP;
			if (g_surfPosY <= 0)
			{
				g_surfPosY = 0;
				g_surfMoveDirection = move_right;
			}
			break;
		
		default:
			break;
	}

	ilm_surfaceSetDestinationRectangle(g_surfaceid, g_surfPosX, g_surfPosY, surfWidth, surfHeight);
	ilm_commitChanges();
}


static void zoomSurface(t_ilm_int x, t_ilm_int y)
{
	t_ilm_int width;
	t_ilm_int height;
	struct ilmSurfaceProperties surfaceProperties;

	ilm_getPropertiesOfSurface(g_surfaceid, &surfaceProperties);

	width =  (t_ilm_int) ((float) surfaceProperties.sourceWidth)  / ZOOM_FACTOR;
	height = (t_ilm_int) ((float) surfaceProperties.sourceHeight) / ZOOM_FACTOR;

	// Center the zoom on the x,y position
	x -=  width / 2;
	x = x < 0 ? 0 : x;
	x = x + width > g_surfWidthOrig ? g_surfWidthOrig - width - 1: x;

	y -= height / 2;
	y = y < 0 ? 0 : y;
	y = y + height > g_surfHeightOrig ? g_surfHeightOrig - height - 1: y;

	ilm_surfaceSetSourceRectangle(g_surfaceid, x, y, width, height);
	ilm_commitChanges();
}

static void unzoomSurface()
{
	ilm_surfaceSetSourceRectangle(g_surfaceid, 0, 0, g_surfWidthOrig, g_surfHeightOrig);
	ilm_commitChanges();
}


