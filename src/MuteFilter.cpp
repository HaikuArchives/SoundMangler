// MuteFilter.cpp
// VERY Stupid Demo Filter that Mutes everything...
// © 1997 Vikram Kulkarni
// © 1997 Be Users Group at UIUC

#include "MuteFilter.h"

#define MUTE 'mute'

MuteFilter::MuteFilter( const char * name, image_id id )
	: SMFilter( name, id )
{
	mute = false;
//	(new BAlert("", "A MuteFilter has been created!", "Whee!"))->Go();
}

MuteFilter::~MuteFilter()
{
}

void MuteFilter::Filter( int16 * left, int16 * right, int32 count)
{
	if (mute)
	{
		for (int i = 0; i <= count; i++)
			left[i] = right[i] = 0;
	}
}

BView * MuteFilter::ReturnView( BRect rect )
{
	return new MuteFilterView( rect );
}

MuteFilterView::MuteFilterView( BRect rect )
	:BView(rect, "background", B_FOLLOW_NONE, B_WILL_DRAW | B_NAVIGABLE)
{
	BRect my_rect = rect;
	SetViewColor(219,219,219);
	
	my_rect.bottom = my_rect.top + 40;
	rect = my_rect;
	rect.OffsetTo(0,0);
	BTextView *text_view = new BTextView(my_rect, "name_view", rect, B_FOLLOW_TOP, B_WILL_DRAW);
	text_view->Insert("Mute Filter\n\n");
	text_view->Insert("	This is a demo filter for Sound Mangler.  It will cut out anything before it in the active plugin list.");
	text_view->MakeSelectable(false);
	text_view->MakeEditable(false);
	AddChild(text_view);
	
	my_rect.top = my_rect.bottom + 5;
	my_rect.bottom = my_rect.top + 21;
	BButton *mute_button = new BButton(rect, "mute_button", "Mute", new BMessage(MUTE));
	AddChild(mute_button);
	mute_button->SetTarget(this);
	
		
	BMessage *help_msg = new BMessage(HELP_REQUESTED);
	help_msg->AddString("ADD-ON_NAME", filter->ReturnName());
	my_rect = Bounds();
	BButton *help_button = new BButton(my_rect, "help_button", "Help", help_msg, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	float width = 0.0, height = 0.0;
	help_button->GetPreferredSize(&width, &height);
	help_button->MoveTo(my_rect.Width() - width - 5, my_rect.Height() - height - 5);
	help_button->ResizeToPreferred();
	AddChild(help_button);
	help_button->SetTarget(be_app);
}

MuteFilterView::~MuteFilterView()
{
}

void MuteFilterView::MessageReceived( BMessage *in_msg )
{
	switch(in_msg->what)
	{
		case MUTE:
		{
			filter->mute = !(filter->mute);
			break;
		}
		
		default:
		{
			BView::MessageReceived(in_msg);
		}
	}
}

SMFilter * MakeNewFilter( const char * name, image_id id )
{
	return new MuteFilter(name, id);
}