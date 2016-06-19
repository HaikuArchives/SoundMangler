// HighPassFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#include "Colors.h"
#include "HighPassFilter.h"

HighPassFilter::HighPassFilter(const char* name)
	:SMFilter(name) {
	level = .5;
	level_knob = NULL;
	activate_cb = NULL;
	queue_length = 3;
	delay_queue = new int16[2 * queue_length];
	queue_tail = 0;
	queue_head = 1;
	Run();
}

HighPassFilter::HighPassFilter(BMessage* data)
	:SMFilter(data) {
	data->FindFloat("level", &level);
	level_knob = NULL;
	activate_cb = NULL;
	queue_length = 3;
	delay_queue = new int16[2 * queue_length];
	queue_tail = 0;
	queue_head = 1;
	Run();
}

HighPassFilter::~HighPassFilter() {
	delete[] delay_queue;
}

status_t HighPassFilter::Archive(BMessage* msg, bool deep) const { 
	BHandler::Archive(msg, deep);
	msg->AddString("class", "HighPassFilter");
	msg->AddFloat("level", level);
	return B_OK; 
}

HighPassFilter* HighPassFilter::Instantiate(BMessage* data) {
	if(validate_instantiation(data, "HighPassFilter"))
		return new HighPassFilter(data);
	else
		return NULL;
}

void HighPassFilter::Filter(int16* left, int16* right, int32 count) {
	if (activate_cb == NULL || activate_cb->Value() == B_CONTROL_ON) {
		if (level_knob != NULL)
			level = level_knob->Volume() / 2.0;
		if (level > 1)
			level = 1;
		if (level < 0)
			level = 0;
		float out_balance = 1.0 + level;
		int32 current_sample = 0;
		while (current_sample < count) {
			delay_queue[2 * queue_tail] = left[current_sample];
			delay_queue[1 + 2 * queue_tail] = right[current_sample];
			left[current_sample] = (left[current_sample] + level * delay_queue[2 * queue_head]) / out_balance;
			right[current_sample] = (right[current_sample] + level * delay_queue[1 + 2 * queue_head]) / out_balance;
			current_sample++;
			queue_tail = (queue_tail + 1) % queue_length;
			queue_head = (queue_head + 1) % queue_length;
		}
	}
}

BView* HighPassFilter::PrefsView() {
	BView* prefs_view = new BView(BRect(2,2,242,247),"prefs_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	prefs_view->SetViewColor(M_FILL_COLOR);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "High Pass Filter");
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

void HighPassFilter::MessageReceived( BMessage *msg ) {
	if (msg->what != B_PULSE) {
		printf("HighPassFilter:\n");
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
	return new HighPassFilter(name);
}

BView* AboutView() {
	BView* about_view = new BView(BRect(2,2,242,247),"about_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	about_view->SetViewColor(DarkMetallicBlue);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "High Pass Filter");
	title->SetHighColor(BeInactiveControlGrey);
	title->SetFont(be_bold_font);
	about_view->AddChild(title);
	BTextView* credits1 = new BTextView(BRect(10,40,220,110), "credits1", BRect(0,0,210,70), B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	credits1->SetText("This filter reduces the levels of high frequencies in the sound stream.\n\nCreated by the Be Users Group at the University of Illinois at Urbana-Champaign.");
	credits1->MakeEditable(false);
	credits1->MakeSelectable(false);
	credits1->SetViewColor(DarkMetallicBlue);
	credits1->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	about_view->AddChild(credits1);
	return about_view;
}
