#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	// change to kiosk user
	if (setuid(1000) != 0 || seteuid(1000) != 0)
		return(99);

	// set DBUS_SESSION_BUS_ADDRESS environment variable
	if (setenv("DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/1000/bus", 1) != 0)
		return(99);

	// call dbus to unblank the screen
	return system("gdbus call --session --dest=org.gnome.ScreenSaver --object-path=/org/gnome/ScreenSaver --method=org.gnome.ScreenSaver.SetActive false");
}
