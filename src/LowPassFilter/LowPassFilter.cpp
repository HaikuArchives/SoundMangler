// LowPassFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#include "Colors.h"
#include "LowPassFilter.h"

LowPassFilter::LowPassFilter(const char* name)
	:SMFilter(name) {
	level = .5;
	level_knob = NULL;
	activate_cb = NULL;
	Run();
}

LowPassFilter::LowPassFilter(BMessage* data)
	:SMFilter(data) {
	data->FindFloat("level", &level);
	level_knob = NULL;
	activate_cb = NULL;
	Run();
}

status_t LowPassFilter::Archive(BMessage* msg, bool deep) const { 
	BHandler::Archive(msg, deep);
	msg->AddString("class", "LowPassFilter");
	msg->AddFloat("level", level);
	return B_OK; 
}

LowPassFilter* LowPassFilter::Instantiate(BMessage* data) {
	if(validate_instantiation(data, "LowPassFilter"))
		return new LowPassFilter(data);
	else
		return NULL;
}

void LowPassFilter::Filter(int16* left, int16* right, int32 count) {
	
	if (activate_cb == NULL || activate_cb->Value() == B_CONTROL_ON) {
		if (level_knob != NULL)
			level = level_knob->Volume() / 2.0;
		if (level > 1)
			level = 1;
		if (level < 0)
			level = 0;
		int32 current_sample = 0;
		int16 delay_left = 0;
		int16 delay_right = 0;
		int16 old_left = 0;
		int16 old_right = 0;
		float out_balance = 1.0 + level;
		while (current_sample < count) {
			old_left = delay_left;
			old_right = delay_right;
			delay_left = left[current_sample];
			delay_right = right[current_sample];
			left[current_sample] -= level * old_left;
			right[current_sample] -= level * old_right;
			left[current_sample] *= out_balance;
			right[current_sample] *= out_balance;
			current_sample++;
		}
	}
}

BView* LowPassFilter::PrefsView() {
	BView* prefs_view = new BView(BRect(2,2,242,247),"prefs_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	prefs_view->SetViewColor(M_FILL_COLOR);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Low Pass Filter");
	title->SetFont(be_bold_font);
	prefs_view->AddChild(title);
	level_knob = new MVolume(NULL);
	level_knob->layout(BRect(10,50,40,80));
	level_knob->round = true;
	prefs_view->AddChild(level_knob);
	prefs_view->AddChild(new BStringView(BRect(50,50,120,70),"level_label","Filter Level"));
	activate_cb = new BCheckBox(BRect(10,90,160,110), "activate_cb", "Activate", NULL);
	activate_cb->SetValue(B_CONTROL_ON);
	prefs_view->AddChild(activate_cb);
	return prefs_view;
}

void LowPassFilter::MessageReceived( BMessage *msg ) {
	if (msg->what != B_PULSE) {
		printf("LowPassFilter:\n");
		msg->PrintToStream();
		printf("\n");
	}
	switch(msg->what) {
		default: {
			BHandler::MessageReceived(msg);
			break;
		}
	}
}

SMFilter* MakeNewFilter(const char* name) {
	return new LowPassFilter(name);
}

BView* AboutView() {
	BView* about_view = new BView(BRect(2,2,242,247),"about_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	about_view->SetViewColor(DarkMetallicBlue);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Low Pass Filter");
	title->SetHighColor(BeInactiveControlGrey);
	title->SetFont(be_bold_font);
	about_view->AddChild(title);
	BTextView* credits1 = new BTextView(BRect(10,40,220,110), "credits1", BRect(0,0,210,70), B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	credits1->SetText("This filter reduces the levels of low frequencies in the sound stream.\n\nCreated by the Be Users Group at the University of Illinois at Urbana-Champaign.");
	credits1->MakeEditable(false);
	credits1->MakeSelectable(false);
	credits1->SetViewColor(DarkMetallicBlue);
	credits1->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	about_view->AddChild(credits1);
	return about_view;
}
