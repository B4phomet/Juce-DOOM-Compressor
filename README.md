# DOOM-Compressor
Original plugin by Mick Gordon, I just reverse-engineered a version of that plugin.
I am no experienced programmer and completely new to C++ and Juce, so don't expect any magic from me; I just bandaided some preexisting code for (multiband) compression together and named it my DOOM Compressor.
When loaded in Cubase, the plugin occasionally crashes the whole DAW, so there's definitely something wrong. Also, when you close the plugin window and reopen it, the settings are reset.

If you look into my spaghetti code and happen to see how this plugin can be improved in both function and stability, please let me know in the discussion tab.

The original function, as intended by Mick Gordon, was that it takes any incoming audio above -200dB and cranking it up to 0dB. It also has a multiband mode which does that to every band individually.

My version doesn't do that directly. I just crank the volume of the incoming audio with "buffer.applyGain();", the value for that being up to 2000. Then I use a stripped down version of Alex Rycroft's (Audio Ordeal) compressor (https://audioordeal.co.uk/how-to-build-a-vst-compressor/) to compress that cranked signal to 0dB. For the multiband part, I used the band separation and merging setup from amoghmatt's multiband compressor (https://github.com/amoghmatt/MultiBandCompressor).
