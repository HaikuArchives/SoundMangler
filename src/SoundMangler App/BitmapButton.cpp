/*
	BitmapButton.cpp
	Chip Paul (cpaul@resgen.com)
	
	A one or two button accepting bitmaps.
	Unlike BPictureButton, allows selected bitmaps when in both modes.
	Behavior:
		B_ONE_STATE_BUTTON
			disabled: DisabledOff
			enabled:  EnabledOff
			pressed:  SelectedOff
		B_TWO_STATE_BUTTON
			Default is off, click to toggle
			disabled: DisabledOff / DisabledOn
			enabled:  EnabledOff / EnabledOn
			pressed:  SelectedOff / SelectedOn
	Messaging:
		Currently copies the model message
*/

#include "BitmapButton.h"

// mainly just a chained constructor
BitmapButton::BitmapButton(BRect frame, const char *name, BMessage *message, uint32 behavior,  uint32 resizeMask, uint32 flags)
			  : BControl(frame, name, "", message,  resizeMask, flags) {
	fBehavior = behavior;
	fDisabled = false;
	msg = message;
}

// Destructor 
BitmapButton::~BitmapButton() {
}
// The good stuff, handle the mouse click
void BitmapButton::MouseDown(BPoint where) {
	// Don't really need to do anything.
	if (fDisabled)
		return;
	BRect rc = Bounds();	
	ulong	buttons;
	// The old value, that is
	int32 value = Value();
	// So it displays right the first time
	bool inside=false;
	if (fBehavior==B_TWO_STATE_BUTTON) {
		// Wait for them to make up their mind
		do {
			// Grab the mouse position
			GetMouse(&where, &buttons);
			if (rc.Contains(where)) {
					if (!inside) {
					if (value==B_CONTROL_ON)
						DrawBitmap(fSelectedOn);
					else
						DrawBitmap(fSelectedOff);	
				}
				inside = true;		
			} else {
				if (inside) {
					if (value==B_CONTROL_ON)
						DrawBitmap(fEnabledOn);
					else
						DrawBitmap(fEnabledOff);
				}
				inside = false;			
			}
			// Save some CPU time
			snooze(1000);
		} while (buttons);
		if (inside) {
			SetValue(!value);		// Switch the value
			Invoke(msg);			// Send the message
		}
	} else {
		// Wait for them to make up their mind
		do {
			// Grab the mouse position
			GetMouse(&where, &buttons);
			if (rc.Contains(where)) {
				if (!inside) {
					SetValue(B_CONTROL_ON);
				}
				inside = true;		
			} else {
				if (inside) {
					SetValue(B_CONTROL_OFF);
				}
				inside = false;			
			}
			// Save some CPU time
			snooze(20000);
		} while (buttons);
		if (inside) {
			SetValue(B_CONTROL_OFF);
			Invoke(msg);			// Send the message
		}
	}

}

// Draws the control
void BitmapButton::Draw(BRect updateRect) {

	// See if on or off
	int32 value = Value();
	if (fBehavior==B_ONE_STATE_BUTTON) {
		if (!fDisabled) 
			if (value==B_CONTROL_ON)
				DrawBitmap(fSelectedOff,updateRect,updateRect);
			else
				DrawBitmap(fEnabledOff,updateRect,updateRect);
		else			
			DrawBitmap(fDisabledOff,updateRect,updateRect);			
	} else {
		if (!fDisabled) {
			if (value==B_CONTROL_ON)
				DrawBitmap(fEnabledOn,updateRect,updateRect);
			else
				DrawBitmap(fEnabledOff,updateRect,updateRect);			
		} else {
			if (value==B_CONTROL_ON)
				DrawBitmap(fDisabledOn,updateRect,updateRect);
			else
				DrawBitmap(fDisabledOff,updateRect,updateRect);			
		}
	}
	// Draw the focus rect
	if (IsFocus()) {
		SetHighColor(keyboard_navigation_color());
		StrokeRect(Bounds());
	}
	
}

// Called to enable/disable the button
void BitmapButton::SetEnabled(bool enabled) {
	if (enabled==IsEnabled())
		return;
	BControl::SetEnabled(enabled);
	fDisabled = !enabled;
	Draw(Bounds());
}

// Return and set the button behavior
void BitmapButton::SetBehavior(uint32 behavior) {
	fBehavior = behavior;
}
uint32 BitmapButton::Behavior() const {
	return fBehavior;
}

// Functions setting the various state bitmaps
void BitmapButton::SetEnabledOn(BBitmap *bmp) {
	fEnabledOn = bmp;
}
void BitmapButton::SetEnabledOff(BBitmap *bmp) {
	fEnabledOff = bmp;
}
void BitmapButton::SetSelectedOn(BBitmap *bmp) {
	fSelectedOn = bmp;
}
void BitmapButton::SetSelectedOff(BBitmap *bmp) {
	fSelectedOff = bmp;
}
void BitmapButton::SetDisabledOn(BBitmap *bmp) {
	fDisabledOn = bmp;
}
void BitmapButton::SetDisabledOff(BBitmap *bmp) {
	fDisabledOff = bmp;
}
// Functions returning the various state bitmaps
BBitmap	*BitmapButton::EnabledOn() const {
	return fEnabledOn;
}
BBitmap	*BitmapButton::EnabledOff() const {
	return fEnabledOff;
}
BBitmap	*BitmapButton::SelectedOn() const {
	return fSelectedOn;
}
BBitmap	*BitmapButton::SelectedOff() const {
	return fSelectedOff;
}
BBitmap	*BitmapButton::DisabledOn() const {
	return fDisabledOn;
}
BBitmap	*BitmapButton::DisabledOff() const {
	return fDisabledOff;
}
