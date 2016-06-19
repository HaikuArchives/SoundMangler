/*
	File:	SMFilterMaster.cpp
	Author:	Chip Paul (cpaul@resgen.com)
	Created: 11/22/97 17:41:39
	Function:
		Handles management of the audio streams and threads,
		calling each filter function for teh various filters.		
*/
#include "FilterMaster.h"
#include "SMFilter.h"

FilterMaster::FilterMaster() {
	InitAudio(adc);
	InitThreads();
	for(int i=0;i<STREAM_BUF_COUNT;i++) {
		write_sem[i]=create_sem(1,"buf write sem");
		read_sem[i]=create_sem(0,"buf read sem");
	}
	write_index = 0;
	read_index = 0;
		
}

FilterMaster::~FilterMaster() {
	ExitAudio();
	for(int i=0;i<STREAM_BUF_COUNT;i++) {
		delete_sem(write_sem[i]);
		delete_sem(read_sem[i]);
	}
}

// Tells the processing thread that it should process...
void FilterMaster::Go() {
	
	audioIn->EnterStream(NULL,true, (void *)this,_listen,NULL,true);
	audioOut->EnterStream(NULL,true, (void *)this,_talk,NULL,true);
	
	resume_thread(proc_thread);
}

// Stops the processing
void FilterMaster::Stop() {
	
	// Exit the audio streams to save CPU time
	audioIn->ExitStream();
	audioOut->ExitStream();
	
	suspend_thread(proc_thread);	
}

// Invokes the Listen function
bool FilterMaster::_listen(void *data, char *buf, size_t count, void *header) {
	return ((FilterMaster *)data)->Listen((int16 *)buf, count/SAMPLE_SIZE);
}

// Reads in the buffers.
bool FilterMaster::Listen(int16 *data, int32 count) {
	// Read into the buffers.
	acquire_sem(write_sem[write_index]);
	for (int i=0;i<count;i++)
		 samples[write_index][i]=data[i];
		 
	/* Send the underlap buffer to the proc thread. */
	if (send_data(proc_thread, write_index, NULL, 0) < B_NO_ERROR)
		return false;
		
	write_index = (write_index+1)%STREAM_BUF_COUNT;
}

// Invokes the Talk function
bool FilterMaster::_talk(void *data, char *buf, size_t count, void *header) {
	return ((FilterMaster *)data)->Talk((int16 *)buf, count/SAMPLE_SIZE);
	
}
// Writes the result into the DAC Stream
bool FilterMaster::Talk(int16 *data, int32 count) {
	acquire_sem(read_sem[read_index]);
	for (int i=0;i<count;i++)
		data[i] = samples[read_index][i];
	release_sem(write_sem[read_index]);
	read_index = (read_index+1)%STREAM_BUF_COUNT;
	
}
// Invokes the Proc routine that loops around in another thread
status_t FilterMaster::proc_invoc(void *arg) {
	((FilterMaster *)arg)->Proc();
	return B_OK;
}
// Process loop.  Filters available buffers.
void FilterMaster::Proc(void) {
	int numFilters;
	thread_id sendingThread;
	int32 index;
	int16 left[STREAM_BUF_SIZE/CHANNEL_COUNT];
	int16 right[STREAM_BUF_SIZE/CHANNEL_COUNT];
	
	SMFilter *currentFilter;
	
	while(continue_processing) {
		// Recieve index of current input buffer.
		index = receive_data(&sendingThread, NULL, 0);
		if (!continue_processing) return;
		
		// Split stereo channels
		// Recombine the stereo channels
		for (int i=0;i<STREAM_BUF_SIZE/CHANNEL_COUNT;i++) {
			left[i] = samples[index][2*i];
			right[i] = samples[index][2*i+1];
		}
		
		filterLock->Lock();
		numFilters = filterList->CountItems();
		for (int i=0;i<numFilters;i++) {
			currentFilter = (SMFilter *) filterList->ItemAt(i);
			currentFilter->Filter((int16 *)&left,(int16 *)&right, STREAM_BUF_SIZE/CHANNEL_COUNT);	
		}
		filterLock->Unlock();
		
		// Recombine the stereo channels
		for (int i=0;i<STREAM_BUF_SIZE/CHANNEL_COUNT;i++) {
			samples[index][2*i] = left[i];
			samples[index][2*i+1] = right[i];
		}
		// Allow transmission of this buffer
		release_sem(read_sem[index]);
	}
	
}

// Initializes thread stuff
void FilterMaster::InitThreads() {
	continue_processing = true;
	proc_thread = spawn_thread(proc_invoc,"Mangler proc",B_REAL_TIME_PRIORITY,
		(void *)this);
	
}
// Initializes the audio streams and subscribers.
void FilterMaster::InitAudio(SoundSource src) {
	outStream = new BDACStream();
	if (src==adc) {
		inStream = new BADCStream();
		outStream->EnableDevice(B_ADC_IN,true);
		outStream->EnableDevice(B_LOOPBACK,FALSE);
		((BADCStream *)inStream)->SetSamplingRate(SRATE);
	} else {
		inStream = new BDACStream();
		((BDACStream *)inStream)->SetSamplingRate(SRATE);
	}
	
	outStream->SetSamplingRate(SRATE);
	
	// Make the subscribers
	audioIn = new BSubscriber("audioIn");
	audioOut = new BSubscriber("audioOut");
	
	// subscribe to the audio streams.
	audioIn->Subscribe(inStream);
	audioOut->Subscribe(outStream);
}

// Called to close streams and sync threads.
void FilterMaster::ExitAudio() {
	int32 ret;
	
	continue_processing = false;
	
	// Exit the audio streams.
	audioIn->ExitStream();
	audioOut->ExitStream();
	
	// Wait for the proc thread to exit.
	send_data(proc_thread,0,NULL,0);
	wait_for_thread(proc_thread,&ret);
	
	// delete the pointers
	delete audioIn;
	delete audioOut;
	delete inStream;
	delete outStream;
	
}
// Sets teh list and lock
void FilterMaster::SetFilterList(BList *newList, BLocker *lock) {
	filterList = newList;
	filterLock = lock;
}

// Sets where the sound comes from
void FilterMaster::SetSoundSource(SoundSource src) {
	if (continue_processing)
		ExitAudio();
	InitAudio(src);
}






