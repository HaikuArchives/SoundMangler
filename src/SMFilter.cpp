// SMFilter.cpp
//
// November 22, 1997
// Vikram Kulkarni
// Be Users Group at UIUC

#include "SMFilter.h"


SMFilter::SMFilter( const char * name, image_id id )
	:MyID(id)
{
	MyName = strdup(name);
	(new BAlert(MyName, "Congratulations! You instantiated a filter!", "Whee!"))->Go();
}

SMFilter::~SMFilter()
{
	if (MyName)
		delete [] MyName;
}

// This is for any initialization that you need to do when you app is
// loaded.
status_t SMFilter::Initialize()
{
	return B_NO_ERROR;
}

void SMFilter::LoadPreferences( BMessage *msg )
{
}

BMessage * SMFilter::ReturnPreferences()
{
	return NULL;
}

// Returns your settings BView
// the SMFilter's version imply returns a view with your
// Filter's name.
BView * SMFilter::ReturnView( BRect rect )
{
	BRect my_rect = rect;
	BView *background_view = new BView(my_rect, "Background view", B_FOLLOW_TOP, B_WILL_DRAW);
	background_view->SetViewColor(219,219,219);
	
	my_rect.bottom = my_rect.top + 20;
	BStringView *name_view = new BStringView(my_rect, "Name view", MyName);
	background_view->AddChild(name_view);
	
	BMessage *help_msg = new BMessage(HELP_REQUESTED);
	help_msg->AddString("ADD-ON_NAME", MyName);
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


