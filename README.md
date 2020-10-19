# B.Noname01
Description: Glitch effect sequencer LV2 plugin (** experimental **)

**Warning: B.Noname01 is in an early stage of development.
Not for production use! No guarantees! Some essential features are not (fully) implemented yet.
Major changes in the plugin definition need to be expected. Therefore, future versions of this
plugin may be completely incompatible to this version.**


## Installation

a) Install the bnoname01 package for your system (once established)

b) Build your own binaries in the following three steps.

Step 1: [Download the latest published version](https://github.com/sjaehn/BNoname01/releases) of B.Noname01. Or clone or
[download the master](https://github.com/sjaehn/BNoname01/archive/master.zip) of this repository.

Step 2: Install pkg-config and the development packages for x11, cairo, and lv2 if not done yet. On
Debian-based systems you may run:
```
sudo apt-get install pkg-config libx11-dev libcairo2-dev lv2-dev
```

Step 3: Building and installing into the default lv2 directory (/usr/lib/lv2/) is easy. Simply call:
```
make
sudo make install
```
from the directory where you downloaded the repository files. For installation into an
alternative directory (e.g., /usr/local/lib/lv2/), change the variable `PREFIX` while installing:

```
sudo make install PREFIX=/usr/local
```


## Running

After the installation Ardour, Carla, and any other LV2 host should automatically detect B.Noname01.

If jalv is installed, you can also call it using one of the graphical jalv executables (like
jalv.gtk, or jalv.gtk3, or jalv.qt4, or jalv.qt5, depending on what is installed), like

```
jalv.gtk https://www.jahnichen.de/plugins/lv2/BNoname01
```

to run it stand-alone and connect it to the JACK system.

Notes:

**Jack transport is required to get information about beat and bar position in the host controlled mode and in the MIDI controlled mode**

**The host must provide information about beat and bar position to use B.Noname01 in these modes.**
**Pure audio tracks (Ardour) may lack these information. Try to add a MIDI input!**


## Usage

### Quickstart

1) Add an effect by clicking on the [+] symbol.
2) Click on the menu symbol left to the effect name to change the effect
3) Set a pattern right to the effect name to define the timepoint(s) to apply the effect on the incoming audio signal.
4) Continue with point 1 to add another effects. Change the order of the effects by clicking on the respective symbol.


### User interface

#### Transport buttons

There are three transport buttons on the top left of the user interface. Play, bypass, and stop.


#### Global settings

The global settings on the top of the user interface contain options to control the transport and the size of the pattern.


##### Mode

Choose between autoplay and host controlled playback.

You can manually change the beats per minute (bpm) and beats per bar (bpBar) in the autoplay mode. These values as well as
the time or beat/bar position data are automatically used in the host controlled mode.  


##### Pattern size

Temporal size of a full pattern loop.


##### Steps

Pattern size in steps. Set up to 32 steps.


#### Effect slots

There are up to 12 slots on the left of the user interface. Each slot is linked to a pattern row right to the respective
slot. The effect slots will be applied in the order from the top to the bottom.

You can add (or insert) an effect by pressing the [+] button or remove an effect by pressing the [-] button. If you have
created more than one slot, you can select a slot by clicking on its name. You can
change an effect and select one out of more than 20 effects by pressing the menu button. You can move the effect slot
(including its respective pattern row) by pressing the up or the down button. To mute or un-mute a slot, press the play
button.


#### Pattern

The pattern matrix is located in the center of user interface. Each line of the pattern is linked to an effect slot in the
left of the user interface. In the default drawing mode, the pattern is also connected with the pad properties in the right
of the user interface.


##### Drawing mode

You are in the drawing mode if none of the cut, copy, x-flip, y-flip, or paste symbols is selected. Left click on a pad to
set the pad and apply properties from the right of the user interface. If you click again, you will un-set this pad.
Dragging results in linked pads. And right-clicking results in copying the properties of the respective pad to the pad
properties in the right of the user interface. Directly change the pad mix property by mouse wheel scrolling.


##### Cut, copy or flip mode

Select the mode by clicking on the respective symbol below the pattern. Drag over an area of pads to cut, copy or flip. The
selected pad area is stored in the clipboard.


##### Paste mode

Once you selected the paste, you can paste the clipboard pads by clicking to any pad in the pattern.


##### Reset, undo and redo

No description required.


#### Pad properties

The pad properties (relevant for the pattern drawing mode) are shown on the left of the user interface.


##### Gate

Likelihood of an effect pad.


##### Mix

Dry / wet mixing value of an effect pad.


#### Slot properties

The slot properties for the selected slot are shown on the bottom of the user interface. The slot properties are applied
for all active pads of a slot. The slot properties consist of ADSR envelope, pan, mix and effect-dependent parameters.


##### ADSR envelope

The ADSR envelope parameters ((A)ttack, (D)ecay, (S)ustain, (R)elease) are used for each active pad. The values are
relative to one step.

##### Pan

Panning the effect to the left (-1.00), to the right (1.00) or to both sides (0.00). If you pan to the full left, the
right side will get the dry signal.


##### Mix

Dry / wet mixing value of an effect.


##### Effect-dependent parameters

See effects.


### Effects

TODO


## What's new

* Optional join consecutive pads
* Show 'Getting started' information
* Background images
* Group colors
* GUI re-design
* Show slot index and effect name for selected slot
* Enable effect panning
* Added feedback dial to delay effect
* Implement effects
  * Surprise box
  * Jumble
  * Tape start
  * Tape speed
  * Scratch
  * Wow & Flutter
  * Crush
  * Decimate
  * Distortion
  * Filter
  * Noise
  * Crackles
  * Stutter
  * Flanger
  * Phaser
* Bugfixes
  * Fix update pads in DSP if slot (and thus pads) in GUI changed (insert, delete, up, down)
  * Enable dragging of sliders
  * Enable reset


## TODO

* Find a name
* MIDI support
  * MIDI triggering of playback
  * MIDI triggering of effects
* Implement effects
  * Pitch (?)
  * Tape start (?)
* Messaging system
* Enable GUI scaling
* Show waveform
* Optionally load a sample (?)
* Latency ? (Pitch, Tape start)
* Make some presets
