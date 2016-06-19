// Class SMSoundEngine
// Chip Paul - cpaul@resgen.com
// Be Users Group @ UIUC - SoundMangler Project
// 2/17/1998

#include "SMFilter.h"
#include "SMMsgDefs.h"
#include "SMSoundEngine.h"

#define SRATE 44100

SMSoundEngine::SMSoundEngine(SoundSource src, BList* newList, BLocker* lock)
	:BLooper("filter master") {
	InitAudio(src);
	feedthrough_enabled = outStream->IsDeviceEnabled(B_LOOPBACK);
	InitThreads();
	for(int i=0;i<STREAM_BUF_COUNT;i++) {
		write_sem[i]=create_sem(1,"buf write sem");
		read_sem[i]=create_sem(0,"buf read sem");
	}
	write_index = 0;
	read_index = 0;
	filterList = newList;
	filterLock = lock;
	Run();
}

SMSoundEngine::~SMSoundEngine() {
	if (feedthrough_enabled)
		outStream->EnableDevice(B_LOOPBACK,true);
	ExitAudio();
	for(int i=0;i<STREAM_BUF_COUNT;i++) {
		delete_sem(write_sem[i]);
		delete_sem(read_sem[i]);
	}
}

// Tells the processing thread that it should process...
void SMSoundEngine::Go() {
	outStream->EnableDevice(B_LOOPBACK,false);
	audioIn->EnterStream(NULL,true, (void *)this,_listen,NULL,true);
	audioOut->EnterStream(NULL,true, (void *)this,_talk,NULL,true);
}

// Stops the processing
void SMSoundEngine::Stop() {
	// Exit the audio streams to save CPU time
	audioIn->ExitStream();
	audioOut->ExitStream();
	//suspend_thread(proc_thread);
	if (feedthrough_enabled)
		outStream->EnableDevice(B_LOOPBACK,true);
}

// Invokes the Listen function
bool SMSoundEngine::_listen(void *data, char *buf, size_t count, void *header) {
	return ((SMSoundEngine*)data)->Listen((int16*)buf, count/SAMPLE_SIZE);
}

// Reads in the buffers.
bool SMSoundEngine::Listen(int16* data, int32 count) {
	// Read into the buffers.
	acquire_sem(write_sem[write_index]);
	for (int i=0;i<count;i++)
		 samples[write_index][i]=data[i];
		 
	/* Send the underlap buffer to the proc thread. */
	if (send_data(proc_thread, write_index, NULL, 0) < B_NO_ERROR)
		return false;
		
	write_index = (write_index+1)%STREAM_BUF_COUNT;
	return true;
}

// Invokes the Talk function
bool SMSoundEngine::_talk(void *data, char *buf, size_t count, void *header) {
	return ((SMSoundEngine *)data)->Talk((int16 *)buf, count/SAMPLE_SIZE);
	
}
// Writes the result into the DAC Stream
bool SMSoundEngine::Talk(int16* data, int32 count) {
	acquire_sem(read_sem[read_index]);
	for (int i=0;i<count;i++)
		data[i] = samples[read_index][i]/4;
	release_sem(write_sem[read_index]);
	read_index = (read_index+1)%STREAM_BUF_COUNT;
	return true;
}

// Invokes the Proc routine that loops around in another thread
status_t SMSoundEngine::proc_invoc(void *arg) {
	((SMSoundEngine*)arg)->Proc();
	return B_OK;
}

// Process loop.  Filters available buffers.
void SMSoundEngine::Proc() {
	thread_id sendingThread;
	int32 index = 0;
	int16 left[STREAM_BUF_SIZE/CHANNEL_COUNT];
	int16 right[STREAM_BUF_SIZE/CHANNEL_COUNT];
	
	while (true) {
		// Recieve index of current input buffer.
		index = receive_data(&sendingThread, NULL, 0);
		if (!continue_processing) break;
		
		// Split stereo channels
		// Recombine the stereo channels
		for (int i=0;i<STREAM_BUF_SIZE/CHANNEL_COUNT;i++) {
			left[i] = samples[index][2*i];
			right[i] = samples[index][2*i+1];
		}
		
		filterLock->Lock();
		SMFilter* currentFilter;
		int32 numFilters = filterList->CountItems();
		for (int i=0;i<numFilters;i++) {
			currentFilter = (SMFilter*)filterList->ItemAt(i);
			currentFilter->Filter((int16*)&left,(int16*)&right, STREAM_BUF_SIZE/CHANNEL_COUNT);	
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
void SMSoundEngine::InitThreads() {
	continue_processing = true;
	proc_thread = spawn_thread(proc_invoc,"sound processor",90,
		(void *)this);
	resume_thread(proc_thread);
}

// Initializes the audio streams and subscribers.
void SMSoundEngine::InitAudio(SoundSource src) {
	outStream = new BDACStream();
	if (src==adc) {
		inStream = new BADCStream();
		outStream->EnableDevice(B_ADC_IN,true);
		((BADCStream *)inStream)->SetSamplingRate(SRATE);
	} else {
		inStream = new BDACStream();
		((BDACStream *)inStream)->SetSamplingRate(SRATE);
	}
	
	outStream->SetSamplingRate(SRATE);
	
	// Make the subscribers
	audioIn = new BSubscriber("audio in");
	audioOut = new BSubscriber("audio out");
	
	// subscribe to the audio streams.
	audioIn->Subscribe(inStream);
	audioOut->Subscribe(outStream);
}

// Called to close streams and sync threads.
void SMSoundEngine::ExitAudio() {
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

void SMSoundEngine::MessageReceived(BMessage *msg) {
	if (msg->what != B_PULSE) {
		printf("SMSoundEngine:\n");
		msg->PrintToStream();
		printf("\n");
	}
	switch(msg->what) {
		case SM_START:
			Go();
			break;
		case SM_STOP:
			Stop();
			break;
		default:
			BLooper::MessageReceived(msg);
			break;
		}
	}
