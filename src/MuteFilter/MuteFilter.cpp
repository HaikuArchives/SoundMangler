// MuteFilter.cpp
// VERY Stupid Demo Filter that Mutes everything...
// © 1997 Vikram Kulkarni
// © 1997 Be Users Group at UIUC

#include "Colors.h"
#include "MuteFilter.h"

#define MUTE 'mute'
#define HELP_REQUESTED 'help'

class MuteFilterView : public BView {
	public:
		MuteFilterView( BRect rect, BHandler* filter );
};

MuteFilter::MuteFilter( const char * name )
	: SMFilter( name ) {
	mute = false;
}

void MuteFilter::Filter( int16 * left, int16 * right, int32 count) {
	if (mute)
		for (int i = 0; i <= count; i++)
			left[i] = right[i] = 0;
}

BView* MuteFilter::PrefsView( BRect rect ) {
	return new MuteFilterView( rect, this );
}

void MuteFilter::MessageReceived( BMessage *msg ) {
	switch(msg->what)
	{
		case MUTE: {
			mute = !mute;
			break;
		}
		default: {
			BHandler::MessageReceived(msg);
			break;
		}
	}
}

MuteFilterView::MuteFilterView( BRect rect, BHandler* filter )
	:BView(rect, "background", B_FOLLOW_NONE, B_WILL_DRAW | B_NAVIGABLE) {
	BRect my_rect = rect;
	SetViewColor(DarkMetallicBlue);
	
	my_rect.bottom = my_rect.top + 40;
	rect = my_rect;
	rect.OffsetTo(0,0);
	BTextView *text_view = new BTextView(my_rect, "name_view", rect, B_FOLLOW_TOP, B_WILL_DRAW);
	text_view->Insert("Mute Filter\n\n");
	text_view->Insert("	This is a demo filter for Sound Mangler.  It will cut out anything before it in the active plugin list.");
	text_view->MakeSelectable(false);
	text_view->MakeEditable(false);
	text_view->SetViewColor(DarkMetallicBlue);
	text_view->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	AddChild(text_view);
	
	my_rect.top = my_rect.bottom + 5;
	my_rect.bottom = my_rect.top + 21;
	BButton *mute_button = new BButton(rect, "mute_button", "Mute", new BMessage(MUTE));
	AddChild(mute_button);
	mute_button->SetTarget(filter);
	
	BMessage *help_msg = new BMessage(HELP_REQUESTED);
	help_msg->AddString("addon_name", filter->Name());
	my_rect = Bounds();
	BButton *help_button = new BButton(my_rect, "help_button", "Help", help_msg, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	float width = 0.0, height = 0.0;
	help_button->GetPreferredSize(&width, &height);
	help_button->MoveTo(my_rect.Width() - width - 5, my_rect.Height() - height - 5);
	help_button->ResizeToPreferred();
	AddChild(help_button);
	help_button->SetTarget(be_app);
}

SMFilter* MakeNewFilter( const char * name ) {
	return new MuteFilter(name);
}
