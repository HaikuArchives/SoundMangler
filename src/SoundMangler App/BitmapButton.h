/*
	BitmapButton.h
	Chip Paul 
	
	A one or two button accepting bitmaps
*/
#ifndef BITMAPBUTTON_H
#define BITMAPBUTTON_H

#include <Control.h>	
#pragma export on
class BitmapButton : public BControl {
protected:
	uint32	fBehavior;			// i.e. ONE or TWO state button
	
	// The actual bitmaps - BitmapButton will delete them for you
	BBitmap		*fEnabledOn;
	BBitmap		*fEnabledOff;
	BBitmap		*fSelectedOn;
	BBitmap		*fSelectedOff;
	BBitmap		*fDisabledOn;
	BBitmap		*fDisabledOff;

	bool 	fDisabled;			// Is the control disabled (through SetEnabled(false) )
	BMessage *msg;				// Model message
public:
	BitmapButton(BRect frame, const char *name, BMessage *message, uint32 behavior = B_ONE_STATE_BUTTON,
									   uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | B_NAVIGABLE); 
	~BitmapButton();
	virtual	void Draw(BRect updateRect);
	virtual	void MouseDown(BPoint where);
//	virtual	void KeyDown(const char *bytes, int32 numBytes);

 	virtual void SetEnabled(bool enabled);

	virtual	void SetBehavior(uint32 behavior);
	uint32 Behavior() const;

	void SetEnabledOn(BBitmap *bmp);
	void SetEnabledOff(BBitmap *bmp);
	void SetSelectedOn(BBitmap *bmp);
	void SetSelectedOff(BBitmap *bmp);
	void SetDisabledOn(BBitmap *bmp);
	void SetDisabledOff(BBitmap *bmp);
		
	BBitmap	*EnabledOn() const;
	BBitmap	*EnabledOff() const;
	BBitmap	*SelectedOn() const;
	BBitmap	*SelectedOff() const;
	BBitmap	*DisabledOn() const;
	BBitmap	*DisabledOff() const;

};

#endif
