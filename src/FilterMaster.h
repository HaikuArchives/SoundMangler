/*
	File:	FilterMaster.h
	Author:	Chip Paul (cpaul@resgen.com)
	Created: 11/22/97 15:35:31
	Function:
		Handles management of the audio streams and threads,
		calling each filter function for teh various filters.
*/
#include <AudioStream.h>
#include <Subscriber.h>

#ifndef FILTERMASTER_H
#define FILTERMASTER_H

/* Audio stream settings. */
#define SRATE 44100
#define STREAM_BUF_COUNT 2

/* Don't change the bufsize until the other fix
	tables are in place.
*/

#define STREAM_BUF_SIZE 2048
#define SAMPLE_SIZE 2
#define CHANNEL_COUNT 2

/*	Because of the overlap, we need twice as many sample buffers as
	stream buffers. */
#define SAMPLE_COUNT (STREAM_BUF_SIZE/(SAMPLE_SIZE*CHANNEL_COUNT))
#define SAMPLE_BUF_COUNT (STREAM_BUF_COUNT*2)
#define SAMPLE_BUF_SIZE (SAMPLE_COUNT*sizeof(float))
enum SoundSource {adc, dac};

class FilterMaster {
private:
	thread_id proc_thread;
	bool continue_processing;
	bool filterAudio;
	
	// List of filter objects and appropriate locker
	BList *filterList;
	BLocker *filterLock;
	
	// Subscription information
	BSubscriber *audioIn;
	BSubscriber *audioOut;
	BAbstractBufferStream *inStream;
	BDACStream *outStream;
	
	// Samples
	int16 samples[STREAM_BUF_COUNT][SAMPLE_COUNT];
	
	// Semaphores for locking read and write channels
	sem_id write_sem[STREAM_BUF_COUNT];
	sem_id read_sem[STREAM_BUF_COUNT];
	// Current indices into buffers
	int read_index, write_index;
	
	// Thread functions
	static bool _listen(void *data, char *buf, size_t count, void *header);
	bool Listen(int16 *data, int32 count);
	static bool _talk(void *data, char *buf, size_t count, void *header);
	bool Talk(int16 *data, int32 count);
	static status_t proc_invoc(void *arg);
	void Proc(void);
	
	void InitThreads();
	// Start and stop the audio
	void InitAudio(SoundSource src);
	void ExitAudio();
	
public:
	FilterMaster();
	~FilterMaster();
	void Go();
	void Stop();
	void SetFilterList(BList *newList, BLocker *lock);
	void SetSoundSource(SoundSource src);
};

#endif
