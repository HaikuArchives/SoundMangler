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
		MuteFilterView( BRect rect, MuteFilter* filter );
};

MuteFilter::MuteFilter(const char * name)
	:SMFilter(name) {
	mute = false;
	Run();
}

MuteFilter::MuteFilter(BMessage* archive)
	:SMFilter(archive) {
	archive->FindBool("mute", &mute);
}

status_t MuteFilter::Archive(BMessage* archive, bool deep) const { 
	BHandler::Archive(archive, deep);
	archive->AddString("class", "MuteFilter");
	archive->AddBool("mute", mute);
	return B_OK; 
}

MuteFilter* MuteFilter::Instantiate(BMessage* archive) {
	if(validate_instantiation(archive, "MuteFilter"))
		return new MuteFilter(archive);
	else
		return NULL;
}

void MuteFilter::Filter( int16 * left, int16 * right, int32 count) {
	if (mute)
		for (int i = 0; i <= count; i++)
			left[i] = right[i] = 0;
}

BView* MuteFilter::PrefsView() {
	return new MuteFilterView(BRect(2,2,242,247), this);
}

void MuteFilter::MessageReceived( BMessage *msg ) {
	switch(msg->what) {
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

MuteFilterView::MuteFilterView( BRect rect, MuteFilter* filter )
	:BView(rect, "background", B_FOLLOW_NONE, B_WILL_DRAW | B_NAVIGABLE) {
	BRect my_rect = rect;
	SetViewColor(BeBackgroundGrey);
	
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Mute Filter");
	title->SetFont(be_bold_font);
	AddChild(title);
	
	BButton *mute_button = new BButton(BRect(10,50,90,70), "mute_button", "Mute", new BMessage(MUTE));
	mute_button->SetTarget(filter);
	AddChild(mute_button);
	
	BMessage *help_msg = new BMessage(HELP_REQUESTED);
	help_msg->AddString("addon_name", filter->Name());
	BButton *help_button = new BButton(BRect(100,50,180,70), "help_button", "Help", help_msg, B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	AddChild(help_button);
	help_button->SetTarget(be_app);
}

SMFilter* MakeNewFilter( const char * name ) {
	return new MuteFilter(name);
}

BView* AboutView() {
	BView * about_view = new BView(BRect(2,2,242,247), "Blah", B_FOLLOW_NONE, B_WILL_DRAW);
	about_view->SetViewColor(DarkMetallicBlue);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Mute Filter");
	title->SetHighColor(BeInactiveControlGrey);
	title->SetFont(be_bold_font);
	about_view->AddChild(title);
	BTextView* text_view = new BTextView(BRect(10,40,220,110), "name_view", BRect(0,0,210,70), B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	text_view->Insert("This is a demo filter for Sound Mangler.  It will cut out anything before it in the active plugin list.");
	text_view->MakeSelectable(false);
	text_view->MakeEditable(false);
	text_view->SetViewColor(DarkMetallicBlue);
	text_view->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	about_view->AddChild(text_view);
	return about_view;
}
