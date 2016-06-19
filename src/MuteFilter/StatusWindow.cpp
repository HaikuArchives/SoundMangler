#include "Colors.h"
#include "StatusWindow.h"

StatusWindow::StatusWindow(BRect frame, const char* title)
	:BWindow(frame, title, B_TITLED_WINDOW, B_NOT_RESIZABLE, B_CURRENT_WORKSPACE)
	{
	uint32 rmask = B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP;
	BRect rect = Bounds();
	BView* base = new BView(rect, B_EMPTY_STRING, rmask, B_WILL_DRAW);
	base->SetViewColor(BeBackgroundGrey);
	AddChild(base);
	
	rect.top = 8;
	rect.left = 8;
	rect.right -= 8;
	rect.bottom = rect.top + 32;
	status_bar = new BStatusBar(rect, "status_bar", "");
	base->AddChild(status_bar);
	
	rect.top = rect.bottom + 8;
	rect.left = rect.right - 64;
	rect.bottom = Bounds().bottom - 8;
	BButton* cancel_button = new BButton(rect, "cancel_button", "Cancel", NULL);
	base->AddChild(cancel_button);
	}

void StatusWindow::UpdateStatus(float delta, char* label)
	{
	Lock();
	status_bar->Update(delta, label);
	Unlock();
	}
