# WinUIEdit
[![WinUI 3 NuGet version](https://img.shields.io/nuget/v/WinUIEdit?label=nuget%20(WinUI%203))](https://www.nuget.org/packages/WinUIEdit)
[![UWP NuGet version](https://img.shields.io/nuget/v/WinUIEdit.Uwp?label=nuget%20(UWP))](https://www.nuget.org/packages/WinUIEdit.Uwp)

This is an early work-in-progress code editor control for both UWP and WinUI 3. It is a port of the [Scintilla editor component](https://www.scintilla.org). It is written in C++ and works with C#, C++, and any other WinRT-compatible language.

> [!NOTE]
> This control is currently not production ready. Breaking API changes are very likely at this stage.

## Quick start
Install either the [WinUIEdit](https://www.nuget.org/packages/WinUIEdit) NuGet package if using WinUI 3 or the [WinUIEdit.Uwp](https://www.nuget.org/packages/WinUIEdit.Uwp) NuGet package if using UWP (WinUI 2).

Add the namespace and code editor control to your page as shown below:

```xml
<Page
    xmlns:editor="using:WinUIEditor">
    <editor:CodeEditorControl x:Name="MyEditor" HighlightingLanguage="csharp" />
</Page>
```

Syntax highlighting is available through the `HighlightingLanguage` property. To control the editor, use the `Editor` property. For example, to set the editor text:
```csharp
MyEditor.Editor.SetText("Vintage tee, brand new phone");
```
See the [Scintilla documentation](https://www.scintilla.org/ScintillaDoc.html) for complete remarks on all methods.

If using C++/WinRT, add `#include <winrt/WinUIEditor.h>` to your `pch.h` file.

## Demo
You can download a demo from Microsoft Store [here](https://apps.microsoft.com/detail/9PGZBDP9PSPF?cid=github&launch=true). Expect some occasional glitches and incomplete features.

<img alt="Mica Editor: replica of Windows 11 Notepad with WinUIEdit control using syntax highlighting, line numbers, and mica, demoing C++ sample code" src="https://user-images.githubusercontent.com/18747724/213900470-3c57b252-3488-40d1-b708-f392a30aab2f.png" width="500" />

## Advanced usage
`CodeEditorControl` wraps `EditorBaseControl` and includes additional features and changes default settings and styles. `EditorBaseControl` can be used directly if only the original Scintilla control is desired. The Scintilla API is available through a WinRT wrapper exposed by the `Editor` property, as well as the original window message-based API via the `Scintilla` method.

## Windows version support
The WinUI 3 version of this control should work on Windows 10, version 1809 and later and support ARM64, x64, and x86. The UWP version of this control should work on Windows 10, version 1703 and later. It will run on ARM64, ARM32, x64, and x86.

## Development

### How to build this project
To switch the build between WinUI 3 and UWP, you will need to modify [UseWinUI3.txt](https://github.com/BreeceW/WinUIEdit/blob/main/UseWinUI3.txt) to `true` or `false`. To do this quickly, use the [WinUI3](https://github.com/BreeceW/WinUIEdit/tree/main/Tools#winui3) and [Uwp](https://github.com/BreeceW/WinUIEdit/tree/main/Tools#uwp) tools.
Open WinUIEditor.sln in Visual Studio 2022. Then, set CsDemoUWP or CppDemoUWP as the startup project for UWP or CsDemoWinUI3 or CppDemoWinUI3 for WinUI 3. Now you can build and run the project.

### Project file structure
|Folder|Description|
|-:|:-|
|**WinUIEditor**|XAML control that hosts the Scintilla port (the C++ project in this folder is also responsible for compiling the scintilla folder code)|
|**scintilla\winui**|Scintilla platform code for UWP/WinUI 3|
|scintilla|Unmodified Scintilla source code with new winui subfolder|
|lexilla|Lexilla source code (patches noted in [WinUIModified.txt](https://github.com/BreeceW/WinUIEdit/blob/main/lexilla/WinUIModified.txt))|
|Tools|Tools to keep the Scintilla code up-to-date and aid in development (see Tools\README)|
