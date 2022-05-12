# WinUIEdit
This is a very early work-in-progress code editing control for both UWP and WinUI 3. It is a port of the [Scintilla editor](https://www.scintilla.org). The goal is to be a performant code editor that feels like it belongs on Windows and has a familiar API for XAML developers. It is written in C++, but is just as easily used from .NET in addition to C++ (it should work with any WinRT language).

## Demo
You can download a demo from Microsoft Store [here](https://www.microsoft.com/store/apps/9PGZBDP9PSPF). This demo is not necessarily current with this repo and most features do not work at all. It will likely also experience crashes.

### Current state of demos
You should be able to mostly use the mouse and keyboard to interact with the control. IME support is somewhat present, though far from finished. Text input support will vary depending on the kind of device you are using, ranging from fairly well, somewhat glitchy, to not at all. There are no scroll bars. It will crash if you minimize the window and in some other scenarios. With large amounts of text, there will be significant slow downs. This is not a fault of the editor, but rather a complication of IME support that should be easily resolved. The demo is a clone of the Windows 11 Notepad app, though currently none of the menu items do anything.

![Control showing line numbers and short text on a white background but is otherwise empty](https://user-images.githubusercontent.com/18747724/157655089-aab93167-1ee4-4809-8bb1-2b9911e93116.png)

## How to build
Currently, to switch the build between UWP and WinUI 3, you will need to modify [MicaEditor.vcxproj](https://github.com/BreeceW/WinUIEdit/blob/main/MicaEditor/MicaEditor.vcxproj) to change `MicaEditorUseWinUI3` to `true` or `false`, accordingly.
Open WinUIEditor.sln in Visual Studio 2022. Then, set DemoUWP or CppDemoUWP as the startup project for UWP or DemoWinUI3 or CppDemoWinUI3 for WinUI 3. Now you can build and run the project.

## Project structure
The project structure is very much not set in stone at this time.

||Folder|Description|
|-|-:|:-|
|_Control_:||
||**MicaEditor**|XAML control that hosts the Scintilla port (Note: The C++ project in this folder is also responsible for compiling the scintilla folder code)|
||**scintilla\winui**|Port of Scintilla to UWP/WinUI 3|
|_Libraries_:||
||scintilla|Unmodified Scintilla source code with new winui subfolder|
||lexilla|Unmodified Lexilla source code|
|_Demos_:||
||CsDemoUwp|Test project for C# UWP|
||CppDemoUwp|Test project for C++/WinRT UWP|
||CsDemoWinUI3|Test project for C# WinUI 3|
||CppDemoWinUI3|Test project for C++/WinRT WinUI 3|
||WinFormsIslandsDemo|Test project for C# WinForms XAML Islands|
||WpfIslandsDemo|Test project for C# WPF XAML Islands|
||CsIslandsUwpDemoApp|Support project for C# XAML Islands apps|
||CppIslandsDemo|Test project for C++ XAML Islands|
||CppIslandsUwpDemoApp|Support project for C++ XAML Islands app|
|_Misc._:||
||Tools|A couple tools to keep the scintilla code up-to-date (see README in folder)|

The most relevant code is in the MicaEditor and scintilla\winui folders. These folders may merge in the future. The control name is also not yet decided (MicaEditor is a placeholder).

## Planned Windows version support
The UWP version of this control should work on Windows 10, version 1703 and later. It is intended that the control will work equally well on all Windows platforms. It will run on ARM64, ARM32, x64, and x86. The WinUI 3 version of this control should work on Windows 10, version 1809 and later, and support ARM64, x64, and x86.

### What are UWP and WinUI 3?
[UWP](https://docs.microsoft.com/en-us/windows/uwp/) and [WinUI 3](https://docs.microsoft.com/en-us/windows/apps/winui/winui3/) are both modern ways of writing native apps for Windows 11 and 10. UWP includes a very performant and enjoyable XAML framework that was later developed into a new, though somewhat source compatible, UI framework that ships with the [Windows App SDK](https://docs.microsoft.com/en-us/windows/apps/windows-app-sdk/) called WinUI 3. As WinUI 3 is very new, it is not entirely equipped to replace UWP yet nor does it support older versions of Windows 10, so this control is being developed for both. This is fairly easy to do, as it mostly entails conditionally redefining namespaces.

#### WinUI 2?
WinUI 2 is a controls and styles library for the UWP XAML framework, not a UI framework on its own, like WinUI 3.
This control does not depend on WinUI 2, so you can use this control with or without WinUI 2 installed.

#### XAML Islands?
[XAML Islands](https://docs.microsoft.com/en-us/windows/apps/desktop/modernize/xaml-islands) is a way to use the UWP XAML framework
in an old-style win32 app, like a WinForms, WPF (see note below), or classic win32 app written in C++. You can embed this control into a win32 app using XAML Islands. If you are not otherwise using XAML, you may want to use the original Scintilla, which supports this well.
Also note that WinUI 3 is planned to support its own form of XAML Islands in the future, allowing you to partially embed a WinUI 3 UI into an old app. Expect this control to support that scenario when it arrives.

#### WPF?
Please note that WinUI 3 is conceptually similar to WPF but is completely different in operation.
This control will work in WPF using XAML Islands.
If you are using WPF, you may want to use this control, or you can embed the original Windows version of Scintilla.
