# C Synth
<ul>
  <li>A cool synthesizer written in C.</li>
</ul>

## Install
Open a terminal in the directory and type ```make```. 
This will create an executable called "synth"

## Use
```$ ./synth -s </patches/patch.txt> -i <midiscore.mid> -o <out.wav> [OPTIONAL] [-c <channel>] [-f <sample_rate>] [-r <pulses_per_second>]```

## Parameters

* patch: [-s] preset - It will synthesize with different envelopes and harmonics depending on what patch you choose. 
* midi score: [-i] Midi file with the score you want the synth to play. 
* out: [-o] The name you want for the wave file.
* _[channel: [-c] One stream of MIDI data has a total of 16 independent channels for messages and events. Here you select which one you want to synthesize.]_
* _[sample rate: [-f] The number of samples of audio carried per second. (leave it as it is)]_
* _[pulses per second: [-r] How fast the midi is processed.]_


