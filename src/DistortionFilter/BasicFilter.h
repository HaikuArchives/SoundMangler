// BasicFilter
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

class BasicFilter : public SMFilter {
	public:
		BasicFilter(const char* name);
		BasicFilter(BMessage* data);
		~BasicFilter();
		static BasicFilter* Instantiate(BMessage *data);
		virtual status_t Archive(BMessage* msg, bool deep = true) const;
		virtual void MessageReceived(BMessage* msg);
		virtual void Filter(int16* left, int16* right, int32 count);
		virtual BView* PrefsView();
	protected:
		int16* map;
		MVolume* level_knob;
		BCheckBox* activate_cb;
		float level;
};

#pragma export off
