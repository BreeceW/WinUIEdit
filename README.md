# WinUIEdit
This is a very early work-in-progress code editing control for both UWP and WinUI 3. It is a port of the [Scintilla editor](https://www.scintilla.org/). The goal is to be a performant code editor that feels like it belongs on Windows and has a familiar API for XAML developers. It is written in C++, but is just as easily used from .NET in addition to C++ (it should work with any WinRT language).

![Control showing line numbers and short text on a white background but is otherwise empty](https://github.com/BreeceW/WinUIEdit/blob/main/Screenshots/Early.png)

## How to build
Currently, to switch the build between UWP and WinUI 3, you will need to modify [MicaEditor.vcxproj](https://github.com/BreeceW/WinUIEdit/blob/main/MicaEditor/MicaEditor.vcxproj) to change `MicaEditorUseWinUI3` to `true` or `false`, accordingly.
Open WinUIEditor.sln in Visual Studio 2022. Then, set DemoUWP or CppDemoUWP as the startup project for UWP or DemoWinUI3 or CppDemoWinUI3 for WinUI 3. Now you can build and run the project.

## Project structure
The project structure is very much not set in stone at this time.

|Folder|Description|
|-:|:-|
|scintilla|The unmodified source Scintilla code, along with a winui folder containing part of the UWP/WinUI 3 port|
|MicaEditor|The XAML control that hosts the Scintilla port (Note: The C++ project in this folder is also responsible for compiling the scintilla folder code)|
|DemoUWP|Test project for C# UWP|
|CppDemoUWP|Test project for C++/WinRT UWP|
|DemoWinUI3|Test project for C# WinUI 3|
|CppDemoWinUI3|Test project for C++/WinRT WinUI 3|
|Tools|A couple tools to keep the scintilla code up-to-date (see README in folder)|

The most relevant code is in the MicaEditor and scintilla\winui folders. These folders may merge in the future. The control name is also not yet decided (MicaEditor is a placeholder).

### What are UWP and WinUI 3?
[UWP](https://docs.microsoft.com/en-us/windows/uwp/) and [WinUI 3](https://docs.microsoft.com/en-us/windows/apps/winui/winui3/) are both modern ways of writing native apps for Windows 11 and 10. UWP includes a very performant and enjoyable XAML framework that was later developed into a new, though mostly source compatible, UI framework that ships with the [Windows App SDK](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/). As WinUI 3 is very new, it is not entirely equipped to replace UWP yet nor does it support older versions of Windows 10, so this control is being developed for both. This is fairly easy to do, as it mostly entails conditionally redefining namespaces.

#### WinUI 2?
WinUI 2 is a library for the UWP XAML variant. This control does not depend on WinUI 2, so it is not relevant to this project. You could use this control with or without WinUI 2 installed.

#### WPF?
Please note that WinUI 3 is conceptually similar to WPF but is completely different in operation. As such, this control will not work in WPF unless you embed a [XAML island](https://docs.microsoft.com/en-us/windows/apps/desktop/modernize/xaml-islands) in your WPF app. Scintilla also already works in WPF via another embedding strategy.
