/** @file simple_client.cpp
 *
 * @brief This simple client demonstrates the most basic features of JACK
 * as they would be used by many applications.
 */

#include <jack/jack.h>

#ifndef ESSENTIA_STREAMING_JACKRINGBUFFER_H
#define ESSENTIA_STREAMING_JACKRINGBUFFER_H

#include "ringbufferinput.h"
#include "streamingalgorithm.h"

namespace essentia {
namespace streaming {

int jack_process_cb(jack_nframes_t nframes, void *arg);
void jack_shutdown_cb(void * arg);


class JackRingBuffer : public Algorithm {
 protected:
	Source<Real> _output;
	Source<Real> _jackTime;
	class RingBufferImpl* _impl;

	std::string _client_name;
	const char **_ports;
	const char *_server_name;
	jack_options_t _options;
	jack_status_t _status;
	jack_client_t *_client;
	bool _autoconnect;
	
 public:
	JackRingBuffer();
	~JackRingBuffer();
 
 
	AlgorithmStatus process();
		
	void add(Real* inputData, int size);

	void declareParameters() {
		declareParameter("bufferSize", "the size of the ringbuffer", "", 8192);
		declareParameter("client_name", "the name of the jack client", "", "essentia");
		declareParameter("autoconnect", "Connect to system input/output defaults automatically", "", true);
	}

	void configure();
	void reset();

	static const char* name;
	static const char* description;
	bool _shouldStop;
		
	jack_port_t *_input_port;
	jack_port_t *_output_port;
		
};

} // streaming
} // essentia

#endif
