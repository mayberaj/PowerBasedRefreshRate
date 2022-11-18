#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <Windows.h>

#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

#include <stdio.h>
#include <time.h>
#include <string.h>


LRESULT CALLBACK WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void Log (const char*);
void UpdateRefreshRate ();
void LoadSettings ();


const char* PBRR_LOG_FILE_NAME = "PBRR.log";
const char* PBRR_SETTINGS_FILE_NAME = "PBRR.set";

const char* PBRRS_LOG_STARTED = "_____Started_____";
const char* PBRRS_LOG_EXITING = "_____Exiting_____";
const wchar_t PBRRS_WINDOW_CLASS[]  = L"PBRRS Power Status Event Listener";

const char PBRR_EXIT_UNABLE_TO_OPEN_LOG_FILE = -3;
const char PBRR_EXIT_UNABLE_TO_READ_SETTINGS_FILE = -3;

unsigned long REFRESH_RATE_ON_POWER = -1; // -1 should set it equivalent to ULONG_MAX from limits.h.
unsigned long REFRESH_RATE_OFF_POWER = -1;


// This should be set to anything other than 0, 1 and 255 at first.
unsigned char powerStatusOnLastUpdate = 155; 

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    Log (PBRRS_LOG_STARTED);
    // Load refresh rate settings.
    LoadSettings ();

    UpdateRefreshRate ();

    // Registering window class to listen to power status change event.
    WNDCLASS windowClass = { };
    windowClass.lpfnWndProc   = WindowProc;
    windowClass.hInstance     = hInstance;
    windowClass.lpszClassName = PBRRS_WINDOW_CLASS;
    RegisterClass (&windowClass);

    // For some reason we have to create a window (hidden even) to
    // recive events from OS. There is surely a better way I dont know of.
    HWND hwnd = CreateWindowEx(0, PBRRS_WINDOW_CLASS, PBRRS_WINDOW_CLASS,
                               0, 0, 0, 0, 0,
                               NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        Log ("Error: Unable to create event window");
        return 0;
    }

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Log (PBRRS_LOG_EXITING);
    return 0;
}

LRESULT CALLBACK WindowProc (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_POWERBROADCAST:
            UpdateRefreshRate ();
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void UpdateRefreshRate ()
{
    SYSTEM_POWER_STATUS powerStatus;
    ZeroMemory (&powerStatus, sizeof (powerStatus));

    if (GetSystemPowerStatus (&powerStatus) != FALSE)
    {
        DEVMODE dm;
        ZeroMemory (&dm, sizeof(dm));
        dm.dmSize = sizeof (dm);
        if (EnumDisplaySettings (NULL, ENUM_CURRENT_SETTINGS, &dm) != FALSE)
        {
            unsigned long currentRefreshRate = dm.dmDisplayFrequency;
            char shouldRequestChange = FALSE;

            // Only update if power status has changed.
            if (powerStatus.ACLineStatus == powerStatusOnLastUpdate)
            {
                return;
            }

            // 0 = AC Power status: Not connected.
            if (powerStatus.ACLineStatus == 0 && currentRefreshRate != REFRESH_RATE_OFF_POWER)
            {
                dm.dmDisplayFrequency = REFRESH_RATE_OFF_POWER;
                shouldRequestChange = TRUE;
            }
            // 1 = AC Power Status: Connected.
            else if (powerStatus.ACLineStatus == 1 && currentRefreshRate != REFRESH_RATE_ON_POWER)
            {
                dm.dmDisplayFrequency = REFRESH_RATE_ON_POWER;
                shouldRequestChange = TRUE;
            }
            // 255 - Power status - Unknown.
            else if (powerStatus.ACLineStatus == 255)
            {
                Log ("Power status is unknown to OS.");
                return;
            }
            if (shouldRequestChange == FALSE)
            {   
                return;
            }

            // Asking OS to change refresh rate.
            LONG ret = ChangeDisplaySettings (&dm, 0);
            switch (ret)
            {
                case DISP_CHANGE_SUCCESSFUL:
                    Log ("Successfully changed refresh rate.");
                    powerStatusOnLastUpdate = powerStatus.ACLineStatus;
                    break;
                case DISP_CHANGE_BADDUALVIEW:
                    Log ("Error changing refresh rate: The settings change was unsuccessful because the system is DualView capable.");
                    break;
                case DISP_CHANGE_BADFLAGS:
                    Log ("Error changing refresh rate: An invalid set of flags was passed in.");
                    break;
                case DISP_CHANGE_BADMODE:
                    Log ("Error changing refresh rate: The graphics mode is not supported.");
                    break;
                case DISP_CHANGE_BADPARAM:
                    Log ("Error changing refresh rate: An invalid parameter was passed in. This can include an invalid flag or combination of flags.");
                    break;
                case DISP_CHANGE_FAILED:
                    Log ("Error changing refresh rate: The display driver failed the specified graphics mode.");
                    break;
                case DISP_CHANGE_NOTUPDATED:
                    Log ("Error changing refresh rate: Unable to write settings to the registry.");
                    break;
                case DISP_CHANGE_RESTART:
                    Log ("Error changing refresh rate: The computer must be restarted for the graphics mode to work.");
                    break;
            }
        }
        else
        {
            Log ("Unable to get display information from OS.");
            return;
        }
    }
    else
    {
        Log ("Unable to get power status from OS.");
        return;
    }
}

void LoadSettings ()
{
    FILE* settingsFile;
    settingsFile = fopen (PBRR_SETTINGS_FILE_NAME, "r");

    if (settingsFile == NULL)
    {
        Log ("Error: Unable to load settings file (PBRR.set).");
        exit (PBRR_EXIT_UNABLE_TO_READ_SETTINGS_FILE);
    }

    fseek (settingsFile, 0, SEEK_END);
    long fileSize = ftell (settingsFile);
    fseek (settingsFile, 0, SEEK_SET);
    char fileContent[++fileSize];
    fread (fileContent, 1, fileSize, settingsFile);

    char currentLine[fileSize];
    int lineCount = 1;
    int secondLineIndex = -1;

    for (int i = 0; i < fileSize; i++)
    {
        if (fileContent[i] == '\n')
        {
            if (lineCount == 1)
            {
                fileContent[i] = '\0';
                strcpy (currentLine, fileContent);
                REFRESH_RATE_ON_POWER = atoi (currentLine);
                fileContent[i] = '\n';
                lineCount++;
                secondLineIndex = i + 1;
            }
            else if (lineCount == 2)
            {
                fileContent[i] = '\0';
                strcpy (currentLine, &fileContent[secondLineIndex]);
                REFRESH_RATE_OFF_POWER = atoi (currentLine);
                fileContent[i] = '\n';
                lineCount++;
            }
            else
            {
                break;
            }
        }
    }

    if (REFRESH_RATE_ON_POWER == -1 || REFRESH_RATE_OFF_POWER == -1)
    {
        Log ("Unable to read refresh rate settings. Make sure there is a extra line at the end.");
        exit (-4);
    }
}

void Log (const char* message)
{
    // Getting timestanp.
    time_t rawTime = time (NULL);
    struct tm timeInfo = *(localtime (&rawTime));
    timeInfo.tm_mon += 1;
    timeInfo.tm_year += 1900;

    // Adding log timestamp in front of the message.
    size_t size = strlen (message);    
    char logBuffer[size + 23];
    snprintf (logBuffer, 23, "[%02d:%02d:%02d %02d:%02d:%04d] ",
              timeInfo.tm_hour, timeInfo.tm_min,
              timeInfo.tm_sec, timeInfo.tm_mday,
              timeInfo.tm_mon, timeInfo.tm_year);
    strcat (logBuffer, message);

    // Opeaning log file to append to and appending log message.
    FILE* logFile;
    logFile = fopen (PBRR_LOG_FILE_NAME, "a");

    if (logFile == NULL)
    {
        exit (PBRR_EXIT_UNABLE_TO_OPEN_LOG_FILE);
    }

    fprintf (logFile, "\n%s",logBuffer);
    fclose (logFile);
}
