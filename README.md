# PowerBasedRefreshRate
A simple windows program that will run in background and change display refresh rate based on power status. So if you disconnect your laptop's power it will automatically change display refresh rate to what you have set in settings file and vice versa when you connect power. It helps increase battery life.

# Known Issue:
Games (maybe apps too) fighting for refresh rate: Sometimes Windows sends power change event (WM_POWERBROADCAST) occasionally without actuall change in power status, which causes this application to check current refresh rate and update it depending on settings in ``PBRR.set``. And that causes screen to change refresh rate multiple times as other game is fighting to keep display at different refresh rate. (Quick fix for now is to just ``End task`` this application ``PBRR.exe`` if you notice your screen flicker twice often when using Fullscreen applications)

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

## Making it run on startup:
* We will be adding shortcut of ``PBRR.exe`` in **Startup** folder.
* First copy the ``PBRR.exe`` by selecting the file and pressing <kbd> Ctrl C </kbd>.
* Now goto ``%AppData%\Microsoft\Windows\Start Menu\Programs\Startup`` (copy-paste into File Exploror's addressbar).
* And <kbd> Right Click </kbd> and select ``Paste shortcut`` to paste shortcut in **Startup** folder.
* Now feel free to restart your computer and make sure that **PBRR.exe** in running in Task Manager after startup.
