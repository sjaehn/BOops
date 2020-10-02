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

* **Jack transport is required**


## Usage

TODO


## What's new

* First release


## TODO

* Find a name
* Optional join consecutive pads
* MIDI support
  * MIDI triggering of playback
  * MIDI triggering of effects
* Implement effects
  * Surprise box
  * Pitch (?)
  * Jumble
  * Tape stop
  * Tape start
  * Tape speed
  * Scratch
  * Wobble
  * Crush
  * Decimate
  * Distortion
  * Filter
  * Noise
  * Crackles
* Messaging system
* Enable GUI scaling
* Show waveform
* Optionally load a sample (?)
* Make some presets
