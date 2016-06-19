#pragma once

#include "BitmapButton.h"

#define BUTTON_WIDTH		 (38 - 1)
#define BUTTON_HEIGHT		 (22 - 1)

class SMPlayButton : public BitmapButton
	{
	public:
		SMPlayButton(BRect frame, const char *name, BMessage *message, uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP);
	};