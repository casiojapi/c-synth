# C Synth
a cool synthesizer written in C. 

This is a command line program that receives a midi file and gives you back the wave file with the synthesis on it. It will necessarily need a midi file (example.mid), a patch file (a preset to get different kinds of sound: example.txt) and the desired wave file name to get as output. You can create your patch files by changing the parameters on the original patch files, it's pretty straight forward. 
## Requirements

In order to compile and run the program, you will need a linux machine with ```make``` and ```gcc``` installed.

## Install

Open a terminal in the directory and type ```make```. 
This will create an executable called "synth".

## Use
Once the program is installed, go to the directory where you have the executable (called "synth") and call it...

```$ ./synth -s </patches/patch.txt> -i <midiscore.mid> -o <out.wav> [OPTIONAL] [-c <channel>] [-f <sample_rate>] [-r <pulses_per_second>]```

example:
``./synth -s ./patches/uno.txt -i ./midi/libertango.mid -o libertango_uno.wav``



## Arguments

* patch: [-s] preset - It will synthesize with different envelopes and harmonics depending on what patch you choose. 
* midi score: [-i] Midi file with the score you want the synth to play. 
* out: [-o] The name you want for the wave file.
* _optional arguments_
* _[channel: [-c] One stream of MIDI data has a total of 16 independent channels for messages and events. Here you select which one you want to synthesize.]_
* _[sample rate: [-f] The number of samples of audio carried per second. (leave it as it is)]_
* _[pulses per second: [-r] How fast the midi is processed.]_


