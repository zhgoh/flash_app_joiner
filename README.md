# Flash application joiner

Basic application that combines the flash standalone player file and swf file to create an application that becomes a standalone applications that runs. Note: Only tested to run on Windows.

## Setup

Make sure to download and extract the `flashplayer_32_sa_debug.exe` or `flashplayer_32_sa.exe` file in the current directory and compile with the following.

```bash
# Note: using mingw32/cygwin
gcc -O2 main.c -o CreateProjector.exe
```

