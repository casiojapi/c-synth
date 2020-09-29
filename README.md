# c-synth
<ul>
  <li>a cool synthesizer written in C</li>
</ul>

## install
open a terminal in the directory and type ```make```
this will create an executable called "synth"

## use
<span>Use: $ ./synth -s </patches/patch.txt> -i <midiscore.mid> -o <out.wav> [OPTIONAL] [-c <channel>] [-f <sample_rate>] [-r <pulses_per_second>]</span>

patch: [-s] preset - It will synthesize with different envelopes and harmonics depending on what patch you choose. 
midi score: [-i] Midi file with the score you want the synth to play. 
out: [-o] The name you want for the wave file.
channel: [-c] One stream of MIDI data has a total of 16 independent channels for messages and events. Here you select which one you want to synthesize. 
sample rate: [-f] The number of samples of audio carried per second. (leave it as it is)
pulses per second: [-r] How fast the midi is processed.


