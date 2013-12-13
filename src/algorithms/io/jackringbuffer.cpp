/** @file simple_client.cpp
 *
 * @brief This simple client demonstrates the most basic features of JACK
 * as they would be used by many applications.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "jackringbuffer.h"
#include "ringbufferimpl.h"
#include "sourcebase.h"
#include "atomic.h"

using namespace std;
namespace essentia {
namespace streaming {
	
const char* JackRingBuffer::name = "JackRingBuffer";
const char* JackRingBuffer::description = DOC(
"This algorithm creates a jack client and outputs its data via a ringbuffer."
);

/**
 * JACK calls this shutdown_callback if the server ever shuts down or
 * decides to disconnect the client.
 */
void jack_shutdown_cb(void *arg) {	
	printf("shutdown callback\n");	
	JackRingBuffer* jrb = (JackRingBuffer *) arg;
	jrb->_shouldStop = true;
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
 * the user (e.g. using Ctrl-C on a unix-ish operating system)
 */
int jack_process_cb(jack_nframes_t nframes, void *arg) {
	printf("process callback\n");
	jack_default_audio_sample_t *in, *out;
	
	JackRingBuffer* jrb = (JackRingBuffer *) arg;
	
	in = (jack_default_audio_sample_t *)
		jack_port_get_buffer (jrb->_input_port, nframes);
	out = (jack_default_audio_sample_t *)
		jack_port_get_buffer (jrb->_output_port, nframes);
	memcpy (out, in,
		sizeof (jack_default_audio_sample_t) * nframes);
		
	jrb->add(in, nframes);

	return 0;   
}

JackRingBuffer::JackRingBuffer():_impl(0), _shouldStop(false)
{
  declareOutput(_output, 1024, "signal", "data source of what's coming from the ringbuffer");
  _output.setBufferType(BufferUsage::forAudioStream);
}

JackRingBuffer::~JackRingBuffer()
{
	delete _impl;
}

void JackRingBuffer::configure()
{
	delete _impl;
	_impl = new RingBufferImpl(RingBufferImpl::kAvailable,parameter("bufferSize").toInt());
	_client_name = parameter("client_name").toString().c_str();
	printf("%s\n", _client_name);
	
	/* open a client connection to the JACK server */
	_client = jack_client_open(_client_name, _options, &_status, _server_name);
	if (_client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", _status);
		if (_status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		return;
	}
	if (_status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (_status & JackNameNotUnique) {
		_client_name = jack_get_client_name(_client);
		fprintf (stderr, "unique name `%s' assigned\n", _client_name);
	}

	/* tell the JACK server to call `process()' whenever
	   there is work to be done.
	*/
	jack_set_process_callback(_client, jack_process_cb, this);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/
	jack_on_shutdown (_client, jack_shutdown_cb, this);

	/* create two ports */
	_input_port = jack_port_register(_client, "input",
					 JACK_DEFAULT_AUDIO_TYPE,
					 JackPortIsInput, 0);
					 
	_output_port = jack_port_register(_client, "output",
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsOutput, 0);

	if ((_input_port == NULL) || (_output_port == NULL)) {
		fprintf(stderr, "no more JACK ports available\n");
		return;
	}

	/* Tell the JACK server that we are ready to roll.  Our
	 * process() callback will start running now. */
	if (jack_activate(_client)) {
		fprintf (stderr, "cannot activate client");
		return;
	}

	/* Connect the ports.  You can't do this before the client is
	 * activated, because we can't make connections to clients
	 * that aren't running.  Note the confusing (but necessary)
	 * orientation of the driver backend ports: playback ports are
	 * "input" to the backend, and capture ports are "output" from
	 * it.
	 */
	 
	/* enable this if you don't want to be able to manually connect
	* the client to ports after it has started up
	*/
	_ports = jack_get_ports(_client, NULL, NULL,
				JackPortIsPhysical|JackPortIsOutput);
	if (_ports == NULL) {
		fprintf(stderr, "no physical capture ports\n");
		//return false;
	}
	
	if (jack_connect(_client, _ports[0], jack_port_name(_input_port))) {
		fprintf (stderr, "cannot connect input ports\n");
	}
	
	free (_ports);
	
	_ports = jack_get_ports (_client, NULL, NULL,
				JackPortIsPhysical|JackPortIsInput);
	if (_ports == NULL) {
		fprintf(stderr, "no physical playback ports\n");
		//return false;
	}
	
	if (jack_connect (_client, jack_port_name(_output_port), _ports[0])) {
		fprintf (stderr, "cannot connect output ports\n");
	}
	
	//free (_ports);
	printf("finished configuring\n");
	
}

void JackRingBuffer::add(Real* inputData, int size)
{
	//std::cerr << "adding " << size << " to ringbuffer with space " << _impl->_space << std::endl;
	int added = _impl->add(inputData,size);
	if (added < size) throw EssentiaException("Not enough space in ringbuffer at input");
}


AlgorithmStatus JackRingBuffer::process() {
  if (_shouldStop) {
	  printf("finished\n");
	  return NO_OUTPUT;
  }
  std::cerr << "ringbufferinput waiting" << std::endl;
  _impl->waitAvailable();
  std::cerr << "ringbufferinput waiting done" << std::endl;
  AlgorithmStatus status = acquireData();
  printf("acquired data\n");

  if (status != OK) {
    std::cerr << "leaving the ringbufferinput while loop" << std::endl;
    if (status == NO_OUTPUT) throw EssentiaException("internal error: output buffer full");
    return status;
  }

  vector<AudioSample>& outputSignal = _output.tokens();
  AudioSample* outputData = &(outputSignal[0]);
  int outputSize = outputSignal.size();

  std::cerr << "ringbufferinput getting" << outputSize << endl;
  int size = _impl->get(outputData, outputSize);
  std::cerr << "got " << size << " from ringbuffer with space " << _impl->_space << std::endl;

  _output.setReleaseSize(size);
  releaseData();

  assert(size);

  return OK;
}

void JackRingBuffer::reset() {
  Algorithm::reset();
  _impl->reset();
}

}
}


