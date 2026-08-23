# Running Doom on an Orbic RC400L

After installing EFF's [Rayhunter](https://www.eff.org/deeplinks/2025/03/meet-rayhunter-new-open-source-tool-eff-detect-cellular-spying) on the Orbic hotspot, I wanted to utilize their rootshell to see what else this thing is capable of. The SoC MDM9207 is ARM Cortex-A7 processor with a 1.3GHz clock, definitely plenty to run Doom.

This documents how to cross-compile and run fbDOOM on an Orbic RC400L. It is based on [huawei-doom](https://github.com/FrenchPythonLover/huawei-doom), modified for the Orbic’s 128x128 display and input devices.

## Hardware

```sh
cat /sys/class/graphics/fb0/name              # fb_st7735s
cat /sys/class/graphics/fb0/modes             # U:128x128p-0
cat /sys/class/graphics/fb0/bits_per_pixel    # 16
```

The display is a 128×128 ST7735S LCD exposed as a 16-bit RGB565 framebuffer at `/dev/fb0`.

Confirmed button mappings:

```text
Menu:  /dev/input/event1, keycode 10
Reset: /dev/input/event1, keycode 9
Power: /dev/input/event2, keycode 110
```

## 1. Install Rayhunter and enable ADB

Download the latest release from [EFForg/rayhunter](https://github.com/EFForg/rayhunter/releases).

This guide uses Rayhunter’s USB installation path because it enables ADB and installs `/bin/rootshell`:

```sh
./installer orbic-usb
```

If Windows reports that the interface uses `usbrndis6` instead of WinUSB, use [Zadig](https://zadig.akeo.ie/) to install WinUSB for the Orbic/MDM9207 USB interface.

Verify ADB:

```sh
adb devices
adb shell
```

Enter the root shell:

```sh
/bin/rootshell
```

## 2. Set up the toolchain for cross compilation

An ARMv7 hard-float musl toolchain is required. The compiler must be available as:

```text
armv7l-linux-musleabihf-gcc
```

Add the toolchain to your path:

```sh
mkdir -p "$HOME/toolchains"
cd "$HOME/toolchains"

wget https://more.musl.cc/x86_64-linux-musl/armv7l-linux-musleabihf-cross.tgz
tar -xzf armv7l-linux-musleabihf-cross.tgz

export PATH="/path/to/armv7l-linux-musleabihf-cross/bin:$PATH"
armv7l-linux-musleabihf-gcc --version
```

## 3. Build Doom

```sh
git clone https://github.com/dc336/Orbic_RC400L_DOOM/
cd Orbic_RC400L_DOOM

make clean
make CC=armv7l-linux-musleabihf-gcc
file doom
```

The result should be a 32 bit ARM EABI executable.

## 4. Push Doom to the Orbic

From the computer:

```sh
adb push doom /tmp/doom
adb push doom1.wad /tmp/doom1.wad
adb shell
```

On the Orbic:

```sh
/bin/rootshell

mkdir -p /data/doom
cp /tmp/doom /data/doom/doom
cp /tmp/doom1.wad /data/doom/doom1.wad
chmod 755 /data/doom/doom
```

## 5. Start Doom manually

Stop software that also uses the framebuffer. Run this while the display is on:

```sh
for PID in $(ps | awk '/rayhunter|start_qt_daemon|qt_daemon|qt_process/ && !/awk/ {print $1}'); do kill -9 "$PID" done
```

Start Doom:

```sh
cd /data/doom
./doom -warp 1 1 -skill 2
```

## Controls

```text
Menu held:           Turn left
Power held:          Turn right
Menu + Power:        Move forward
Reset:               Fire
Menu + Reset:        Use/open
Power in menus:      Select
Menu in menus:       Move through options
```

Holding Power for about ten seconds will turn off the Orbic. This is controlled by the Qualcomm drivers, can't easily be prevented.

## 6. Automatic startup

The stock firmware launches:

```text
/etc/init.d/start_qt_daemon
```

The supplied startup script replaces that hook, waits for `/data/doom`, `/dev/fb0`, and the input devices, stops Qt and Rayhunter, then launches Doom.

From the computer:

```sh
adb push start_qt_daemon /tmp/start_qt_daemon
adb shell
```

On the Orbic:

```sh
/bin/rootshell

/bin/busybox setuidgid 1000:1000 cp \
    /tmp/start_qt_daemon \
    /etc/init.d/start_qt_daemon

/bin/busybox setuidgid 1000:1000 chmod 755 \
    /etc/init.d/start_qt_daemon
```

Doom will start automatically on the next reboot.
