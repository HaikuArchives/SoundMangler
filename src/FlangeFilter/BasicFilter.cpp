// BasicFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#include "Colors.h"
#include "BasicFilter.h"

BasicFilter::BasicFilter(const char* name)
	:SMFilter(name) {
	delay_queue = new int16[1000000];
	left_queue_tail = 0;
	left_queue_head = 1;
	right_queue_tail = 0;
	right_queue_head = 1;
	int32 delay = 0;
	int32 sweep_depth = 8*44;
	min_delay = delay;
	max_delay = delay + sweep_depth;
	left_delay = (min_delay + max_delay) / 2;
	right_delay = min_delay;
	ldelta = 2;
	rdelta = 2;
	level_knob = NULL;
	feedback_knob = NULL;
	Run();
}

BasicFilter::BasicFilter(BMessage* data)
	:SMFilter(data) {
	Run();
}

BasicFilter::~BasicFilter() {
}

status_t BasicFilter::Archive(BMessage* msg, bool deep) const { 
	BHandler::Archive(msg, deep);
	msg->AddString("class", "BasicFilter");
	return B_OK; 
}

BasicFilter* BasicFilter::Instantiate(BMessage* data) {
	if(validate_instantiation(data, "BasicFilter"))
		return new BasicFilter(data);
	else
		return NULL;
}

void BasicFilter::Filter(int16* left, int16* right, int32 count) {
	float level = 1.0;
	float wave_delta = 0.002;
	if (activate_cb != NULL) {
		level = level_knob->Volume();
		min_delay = atol(delay_field->Text()) * 44.1;
		max_delay = min_delay + atol(sweep_field->Text()) * 44.1;
		wave_delta = 2 * (max_delay - min_delay) / (44.1 * atof(wavelength_field->Text()));
	}
	float level_balance = 1.0 + level;
	int32 index = 0;
	uint16 avg_delay = (min_delay + max_delay) * 0.2;
	if (activate_cb == NULL || activate_cb->Value() == B_CONTROL_ON)
		while (index < count) {
			if (left_delay > max_delay) {
				ldelta = -wave_delta;
				right_delay = avg_delay;
			} else if (left_delay < min_delay) {
				ldelta = wave_delta;
				right_delay = avg_delay;
			}
			left_delay += ldelta;
			if (avg_delay > left_delay)
				right_delay += wave_delta;
			else
				right_delay -= wave_delta;
			left[index] = (int32(left[index]) + level * delay_queue[2 * left_queue_head]) / level_balance;
			right[index] = (int32(right[index]) + level * delay_queue[1 + 2 * right_queue_head]) / level_balance;
			delay_queue[2 * left_queue_tail] = left[index];
			delay_queue[1 + 2 * right_queue_tail] = right[index];
			left_queue_tail = (left_queue_tail + 1) % int16(left_delay);
			left_queue_head = (left_queue_head + 1) % int16(left_delay);
			right_queue_tail = (right_queue_tail + 1) % int16(right_delay);
			right_queue_head = (right_queue_head + 1) % int16(right_delay);
			index++;
		}
}

BView* BasicFilter::PrefsView() {
	BView* prefs_view = new BView(BRect(2,2,242,247),"prefs_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	prefs_view->SetViewColor(M_FILL_COLOR);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Flange Filter");
	title->SetFont(be_bold_font);
	prefs_view->AddChild(title);
	level_knob = new MVolume(NULL);
	level_knob->layout(BRect(50,50,80,80));
	level_knob->round = true;
	prefs_view->AddChild(level_knob);
	BStringView* label = new BStringView(BRect(30,80,100,100),"level_label","Flange Level");
	label->SetAlignment(B_ALIGN_CENTER);
	prefs_view->AddChild(label);
	feedback_knob = new MVolume(NULL);
	feedback_knob->layout(BRect(140,50,170,80));
	feedback_knob->round = true;
	prefs_view->AddChild(feedback_knob);
	label = new BStringView(BRect(110,80,200,100),"level_label","Feedback Level");
	label->SetAlignment(B_ALIGN_CENTER);
	prefs_view->AddChild(label);
	delay_field = new BTextControl(BRect(10,110,150,130), "delay_field", "Delay:", "0", NULL);
	delay_field->SetDivider(80);
	prefs_view->AddChild(delay_field);
	sweep_field = new BTextControl(BRect(10,140,150,160), "sweep_field", "Sweep Depth:", "10", NULL);
	sweep_field->SetDivider(80);
	prefs_view->AddChild(sweep_field);
	wavelength_field = new BTextControl(BRect(10,170,150,190), "wavelength_field", "Wavelength:", "1000", NULL);
	wavelength_field->SetDivider(80);
	prefs_view->AddChild(wavelength_field);
	activate_cb = new BCheckBox(BRect(10,200,110,220), "activate_cb", "Activate", NULL);
	activate_cb->SetValue(B_CONTROL_ON);
	prefs_view->AddChild(activate_cb);
	return prefs_view;
}

void BasicFilter::MessageReceived( BMessage *msg ) {
	if (msg->what != B_PULSE) {
		printf("BasicFilter:\n");
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
	return new BasicFilter(name);
}

BView* AboutView() {
	BView* about_view = new BView(BRect(2,2,242,247),"about_view", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	about_view->SetViewColor(DarkMetallicBlue);
	BStringView* title = new BStringView(BRect(10,10,220,30), "title", "Flange Filter");
	title->SetHighColor(BeInactiveControlGrey);
	title->SetFont(be_bold_font);
	about_view->AddChild(title);
	BTextView* credits1 = new BTextView(BRect(10,40,220,110), "credits1", BRect(0,0,210,70), B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_NAVIGABLE);
	credits1->SetText("This filter creates a stereo flange effect in the sound stream.\n\nCreated by the Be Users Group at the University of Illinois at Urbana-Champaign.");
	credits1->MakeEditable(false);
	credits1->MakeSelectable(false);
	credits1->SetViewColor(DarkMetallicBlue);
	credits1->SetFontAndColor(be_plain_font, B_FONT_ALL, &BeInactiveControlGrey);
	about_view->AddChild(credits1);
	return about_view;
}
