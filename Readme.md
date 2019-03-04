# jpcmd - Japanese Command Prompt ![AppIcon](https://i.imgur.com/r31XqH5.png)

A Command Prompt launcher that sets the codepage to [Shift JIS (932)](https://en.wikipedia.org/wiki/Shift_JIS) for proper Japanese program output.

All command line arguments passed to `jpcmd.exe` are passed directly to `cmd.exe`.

Setting the path to [Locale Emulator's](https://github.com/xupefei/Locale-Emulator) `LEProc.exe` in the `config.ini` file will allow launching the program through `jpcmdrun.exe` to make sure all programs launched from `jpcmd.exe` keep their Japanese locale. If `jpcmd.exe` is running properly in the Japanese locale then the console title will be *Japanese Command Prompt (LE)*.

The reason for the above feature is that even when running `cmd.exe` through [Locale Emulator](https://github.com/xupefei/Locale-Emulator), the codepage will not be set to [Shift JIS (932)](https://en.wikipedia.org/wiki/Shift_JIS), making the output practically useless.

![Preview](https://i.imgur.com/bos9XOe.png)
