// Class SMSoundEngine
// Be Users Group @ UIUC - SoundMangler Project
// 2/16/1998

#pragma once

#include <SupportKit.h>
#include <MediaKit.h>

#define STREAM_BUF_COUNT 2
#define STREAM_BUF_SIZE 2048*2
#define SAMPLE_SIZE 2
#define CHANNEL_COUNT 2
#define SAMPLE_COUNT (STREAM_BUF_SIZE/(SAMPLE_SIZE*CHANNEL_COUNT))
#define SAMPLE_BUF_COUNT (STREAM_BUF_COUNT)
#define SAMPLE_BUF_SIZE (SAMPLE_COUNT*sizeof(float))

enum SoundSource {adc, dac};

// The filter master filters the sound in the audio stream.
class SMSoundEngine : public BLooper {
	public:
		SMSoundEngine(SoundSource src, BList* newList, BLocker* lock);
		~SMSoundEngine();
		virtual void MessageReceived(BMessage* msg);
		
	protected:
		// Initialization Functions
		void InitThreads();
		void InitAudio(SoundSource src);
		void ExitAudio();
		
		// Control Functions
		void Go();
		void Stop();
		
		// Thread Functions
		static bool _listen(void* data, char* buf, size_t count, void* header);
		bool Listen(int16* data, int32 count);
		static bool _talk(void* data, char *buf, size_t count, void* header);
		bool Talk(int16* data, int32 count);
		static status_t proc_invoc(void *arg);
		void Proc(void);
		
		// Member Data
		bool feedthrough_enabled;
		thread_id proc_thread;
		bool continue_processing;
		bool filterAudio;
		// List of filter objects and appropriate locker
		BList* filterList;
		BLocker* filterLock;
		// Subscription information
		BSubscriber* audioIn;
		BSubscriber* audioOut;
		BAbstractBufferStream* inStream;
		BDACStream* outStream;
		// Samples
		int16 samples[STREAM_BUF_COUNT][SAMPLE_COUNT*4];
		// Semaphores for locking read and write channels
		sem_id write_sem[STREAM_BUF_COUNT];
		sem_id read_sem[STREAM_BUF_COUNT];
		// Current indices into buffers
		int read_index, write_index;
};
