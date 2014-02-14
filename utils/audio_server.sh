#!/bin/bash

jackd -d alsa &
sleep 1

gst-launch -v souphttpsrc location=http://stream.ckut.ca:8000/903fm-128-stereo ! queue2 ! decodebin2 ! audioconvert ! audioresample ! jackaudiosink &

sleep 1
jack_disconnect gst-launch-0.10:out_jackaudiosink0_1 system:playback_1
jack_disconnect gst-launch-0.10:out_jackaudiosink0_2 system:playback_2

../build/src/rtwords/genVocab ./vocab.txt &

sleep 1
jack_disconnect essentia:input system:capture_1
jack_disconnect essentia:output system:playback_1

sleep 1
jack_connect gst-launch-0.10:out_jackaudiosink0_1 essentia:input
jack_connect gst-launch-0.10:out_jackaudiosink0_2 essentia:input
jack_connect essentia:output system:playback_1
jack_connect essentia:output system:playback_2
