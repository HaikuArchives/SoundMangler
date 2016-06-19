// LowPassFilter
// Be Users Group @ UIUC - SoundMangler Project
// 3/4/1998

#pragma once

#include "SMFilter.h"
#include "MVolume.h"

#pragma export on

extern "C" {
	SMFilter* MakeNewFilter(const char* name);
	BView* AboutView();
}

class LPFView;

class LowPassFilter : public SMFilter {
	public:
		LowPassFilter(const char* name);
		LowPassFilter(BMessage* data);
		static LowPassFilter* Instantiate(BMessage *data);
		virtual status_t Archive(BMessage* msg, bool deep = true) const;
		virtual void MessageReceived(BMessage* msg);
		virtual void Filter( int16* left, int16* right, int32 count);
		virtual BView* PrefsView();
	protected:
		float level;
		BCheckBox* activate_cb;
		MVolume* level_knob;
};

#pragma export off
