// Class SMAddOnDirectory
// Be Users Group @ UIUC - SoundMangler Project
// 2/16/1998

#pragma once

// The add-on manager monitors the add-ons directory for new add-ons.
class SMAddOnDirectory : public BLooper {
	public:
		SMAddOnDirectory();
		~SMAddOnDirectory();
		virtual void MessageReceived(BMessage* msg);
		
	protected:
		// Member Data
		node_ref addons_ref;
		BList* ref_list;
};
