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
    JackRingBuffer* jrb = (JackRingBuffer *) arg;
    // TODO: Magic Number
    // Ensures that process() is called one last time
    Real blankBuf[1024];
    jrb->add(blankBuf, 1024);
    jrb->_shouldStop = true;
    cout << "JACK SHUTDOWN" << endl;
}

/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle. If this function
 * were to return non-zero, the jack client would end its connection
 * to the server.
 *
 * This client gets the data from the input, adds it to the
 * JackRingBuffer argument's internal RingBufferImpl, and also
 * copies it to the output port.
 */
int jack_process_cb(jack_nframes_t nframes, void *arg) {
    jack_default_audio_sample_t *in, *out;
    
    JackRingBuffer* jrb = (JackRingBuffer *) arg;
    
    in = (jack_default_audio_sample_t *)
        jack_port_get_buffer(jrb->_input_port, nframes);
    out = (jack_default_audio_sample_t *)
        jack_port_get_buffer(jrb->_output_port, nframes);
        
    jrb->add(in, nframes);
    memcpy(out, in,
        sizeof (jack_default_audio_sample_t) * nframes);

    return 0;   
}

JackRingBuffer::JackRingBuffer():_impl(0), _shouldStop(false) {
    declareOutput(_output, 1024, "signal", "data source of what's coming from the ringbuffer");
    declareOutput(_jackTime, "time", "the current time of the JACK transport"); 
    _output.setBufferType(BufferUsage::forAudioStream);
}

JackRingBuffer::~JackRingBuffer() {
    delete _impl;
}

void JackRingBuffer::configure() {
    delete _impl;
    _impl = new RingBufferImpl(RingBufferImpl::kAvailable, parameter("bufferSize").toInt());
    _client_name = parameter("client_name").toString();
    _autoconnect = parameter("autoconnect").toBool();
    cout << "Creating a JACK client with name " << _client_name << endl;
    
    /* open a client connection to the JACK server */
    _client = jack_client_open(_client_name.c_str(), _options, &_status, _server_name);
    if (_client == NULL) {
        cerr << "jack_client_open() failed, status = " << _status << endl;
        if (_status & JackServerFailed) {
            cerr << "Unable to connect to JACK server" << endl;
        }
        return;
    }
    if (_status & JackServerStarted) {
        cerr << "JACK server started" << endl;
    }
    if (_status & JackNameNotUnique) {
        _client_name = jack_get_client_name(_client);
        cerr << "uniqe name `" << _client_name << "' assigned" << endl;
    }

    /* tell the JACK server to call `jack_process_cb(this)' whenever
       there is work to be done.
    */
    jack_set_process_callback(_client, jack_process_cb, this);

    /* tell the JACK server to call `jack_shutdown_cb(this)' if
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
        cerr << "no more JACK ports available" << endl;
    }

    /* Tell the JACK server that we are ready to roll.  Our
     * jack_process_cb callback will start running now. */
    if (jack_activate(_client)) {
        cerr << "cannot activate client" << endl;
        return;
    }

    if (_autoconnect) {
        /* Connect the ports.  You can't do this before the client is
         * activated, because we can't make connections to clients
         * that aren't running.  Note the confusing (but necessary)
         * orientation of the driver backend ports: playback ports are
         * "input" to the backend, and capture ports are "output" from
         * it.
         */     
        const char **_ports = jack_get_ports(_client, NULL, NULL,
                    JackPortIsPhysical|JackPortIsOutput);
        if (_ports == NULL) {
            cerr << "no physical capture ports" << endl;
        }
    
        if (jack_connect(_client, _ports[0], jack_port_name(_input_port))) {
            cerr << "cannot connect input ports" << endl;
        }
        free (_ports);
    
        _ports = jack_get_ports (_client, NULL, NULL,
                    JackPortIsPhysical|JackPortIsInput);
        if (_ports == NULL) {
            cerr << "no physical playback ports" << endl;
        }
        
        if (jack_connect (_client, jack_port_name(_output_port), _ports[0])) {
            cerr << "cannot connect output ports" << endl;
        }
    }
}

void JackRingBuffer::add(Real* inputData, int size) {
    //std::cerr << "adding " << size << " to ringbuffer with space " << _impl->_space << std::endl;
    int added = _impl->add(inputData, size);
    if (added < size) throw EssentiaException("Not enough space in ringbuffer at input");
}


AlgorithmStatus JackRingBuffer::process() {
  //cerr << "ringbufferinput waiting" << endl;
  _impl->waitAvailable();
  if (_shouldStop) {
      cout << "ESSENTIA SHUTDOWN" << endl;
      shouldStop(true);
      return NO_OUTPUT;
  }
  //cerr << "ringbufferinput waiting done" << endl;
  AlgorithmStatus status = acquireData();
  //printf("acquired data\n");

  if (status != OK) {
    cerr << "leaving the ringbufferinput while loop" << endl;
    if (status == NO_OUTPUT) throw EssentiaException("internal error: output buffer full");
    return status;
  }

  vector<AudioSample>& outputSignal = _output.tokens();
  AudioSample* outputData = &(outputSignal[0]);
  int outputSize = outputSignal.size();

  //cerr << "ringbufferinput getting" << outputSize << endl;
  int size = _impl->get(outputData, outputSize);
  //std::cerr << "got " << size << " from ringbuffer with space " << _impl->_space << std::endl;

  _output.setReleaseSize(size);
  //jack_get_time() is in usecs, we want mins
  _jackTime.push((Real) (jack_get_time()/60000000.0));  
    
  releaseData();
  
  assert(size);

  return OK;
}

void JackRingBuffer::reset() {
  Algorithm::reset();
  _impl->reset();
}

} //streaming
} //essentia


