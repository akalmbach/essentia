#!/bin/bash

jackd -d alsa &

gst-launch -v souphttpsrc location=http://stream.ckut.ca:8000/903fm-128-stereo ! queue2 ! decodebin2 ! audioconvert ! audioresample ! jackaudiosink &	
jack_disconnect gst-launch-0.10:out_jackaudiosink0_1 system:playback_1
jack_disconnect gst-launch-0.10:out_jackaudiosink0_2 system:playback_2

/home/arnold/Projects/essentia/build/src/rtwords/genVocab &
jack_disconnect essentia:output system:playback_1

jack_connect gst-launch-0.10:out_jackaudiosink0_1 essentia:input
jack_connect gst-launch-0.10:out_jackaudiosink0_2 essentia:input
jack_connect essentia:output system:playback_1
jack_connect essentia:output system:playback_2
