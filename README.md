# btn-touch-mod module

This module is supposed to run a helper script `unblank-screen` whenever a
`BTN_TOUCH` key event appears. The helper script runs a dbus command to turn
off the GNOME screensaver.

## Installation

The module is provided as an out-of-tree kernel module installable as Debian
dkms package. Just run

```
apt-get install ./btn-touch-dkms_0.0.1_amd64.deb
```

## Testing

This is how one can test (requires the `evemu-tools` package):

```
$ gdbus call --session --dest=org.gnome.ScreenSaver --object-path=/org/gnome/ScreenSaver --method=org.gnome.ScreenSaver.GetActive
(true,)
$ sudo evemu-event /dev/input/by-id/usb-Weida_Hi-Tech_CoolTouch_System-event-if00 --type EV_KEY --code 330 --value 1 --sync
$ sudo evemu-event /dev/input/by-id/usb-Weida_Hi-Tech_CoolTouch_System-event-if00 --type EV_KEY --code 330 --value 0 --sync
$ gdbus call --session --dest=org.gnome.ScreenSaver --object-path=/org/gnome/ScreenSaver --method=org.gnome.ScreenSaver.GetActive
(false,)
```

And this should appear in the `syslog` then:

```
<date> panel1 kernel: [  740.395478] btn_touch_mod: BTN_TOUCH requesting unblanking screen.
<date> panel1 kernel: [  740.395486] btn_touch_mod: BTN_TOUCH unblanking screen successful.
```

## Limitations

The program is hardcoded to set the UID to 1000 and one might need to adjust
`btn_touch_ids` to register for the correct device.

If anybody has some suggestions how to improve it, please feel free to send me
your comments.
