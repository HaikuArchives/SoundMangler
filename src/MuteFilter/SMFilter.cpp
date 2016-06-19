// SMFilter.cpp
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#include "SMFilter.h"

#define HELP_REQUESTED 'help'

SMFilter::SMFilter( const char* name )
	:
	BHandler(name) {
	_name = new char[strlen(name)+1];
	strcpy(_name, name);
}

const char* SMFilter::getName() const {
	return _name;
}

SMFilter::~SMFilter() {
}

// This is for any initialization that you need to do when you app is
// loaded.
status_t SMFilter::Initialize() {
	fprintf(stdout,"Called initialize in filter!\n"); fflush(stdout);
	beep();
	return B_NO_ERROR;
}

/*
// Returns your settings BView
// the SMFilter's version imply returns a view with your
// Filter's name.
BView* SMFilter::PrefsView( BRect rect )
{
	BRect my_rect = rect;
	BView *background_view = new BView(my_rect, "Background view", B_FOLLOW_TOP, B_WILL_DRAW);
	background_view->SetViewColor(219,219,219);
	
	my_rect.bottom = my_rect.top + 20;
	BStringView *name_view = new BStringView(my_rect, "Name view", Name());
	background_view->AddChild(name_view);
	
	BMessage *help_msg = new BMessage(HELP_REQUESTED);
	help_msg->AddString("addon_name", Name());
	my_rect = rect;
	BButton *help_button = new BButton(my_rect, "help_button", "Help", help_msg, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	float width = 0.0, height = 0.0;
	help_button->GetPreferredSize(&width, &height);
	help_button->MoveTo(my_rect.Width() - width - 5, my_rect.Height() - height - 5);
	help_button->ResizeToPreferred();
	background_view->AddChild(help_button);
	help_button->SetTarget(be_app);
	
	return background_view;
}
*/

