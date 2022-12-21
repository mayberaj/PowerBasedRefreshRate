# PowerBasedRefreshRate
A simple windows program that will run in background and change display refresh rate based on power status. So if you disconnect your laptop's power it will automatically change display refresh rate to what you have set in settings file and vice versa when you connect power. It helps increase battery life.


# Installation
## Change settings in `PBRR.set` file:
* First we will change some settings in ``PBRR.set`` file.
* You can open the file with notepad or any other text editor of your choice.
```
144 #Refresh rate when power is connected.
60  #Refresh rate when power is not connected.
#Leave this extra line at the end. Program needs it.
```
* First line is for refresh rate when your device is **PLUGGED** in. Change it to (preferably) highest refresh rate that your monitor supports.
* Second line is for refresh rate when your device is **NOT PLUGGED** in. Change it to lower refresh rate that your monitor supports.
* Note that you should not remove the last line, program needs a ``newwline`` at the end of second line to detect refresh rate settings.

* At this point you should test it out by running PBRR.exe (It will not open any window and just run in background) and try to disconnect and reconnect your power. If both the time it changes the refresh rate to what you have set in ``PBRR.set`` file then you are good to go with next step of making it run on startup.

## Make it run on startup:
* We will be adding shortcut of ``PBRR.exe`` in **Startup** folder.
* First copy the ``PBRR.exe`` by selecting the file and pressing <kbd> Ctrl C </kbd>.
* Now goto ``%AppData%\Microsoft\Windows\Start Menu\Programs\Startup`` (copy-paste into File Exploror's addressbar).
* And <kbd> Right Click </kbd> and select ``Paste shortcut`` to paste shortcut in **Startup** folder.
* Now feel free to restart your computer and make sure that **PBRR.exe** in running in Task Manager after startup.


# Build
* The only compiler I have used to build and know works is ``gcc (MinGW.org GCC Build-2) 9.2.0``.
* To build, open terminal in source folder and type:
```
gcc -ofast PBRR.c -o PBRR -mwindows
```
* You should have a new ``PBRR.exe`` in source folder.
* Now you can follow the [Make it run on startup](#make-it-run-on-startup) steps.
* And don't forget to create the Settings (``PBRR.set``) file. Syntax of that file can be found [Here](#change-settings-in-pbrrset-file).
