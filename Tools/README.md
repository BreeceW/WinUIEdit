# Tools

This directory contains a few tools to help with development and to keep this library up-to-date with Scintilla.
These tools only run on Windows and require the .NET 7 SDK or newer to be installed.

## Package
This tool builds the library and its C#/WinRT projection and creates NuGet packages.

Run the Package tool like this:

`Package` builds WinUI 3 and UWP variants and does not change version

`Package x.y.z` builds WinUI 3 and UWP variants and changes versions of both packages to x.y.z

`Package winui3` builds the WinUI 3 variant and does not change version

`Package uwp:x.y.z` builds the UWP variant and changes version to x.y.z

`Package winui3:a.b.c uwp:x.y.z` builds WinUI 3 and UWP variants and changes version of the WinUI 3 package to a.b.c and the UWP package to x.y.z

The resulting package(s) are located in the WinUIEditorCsWinRT\nuget folder.

## Update
This tool updates the version of Scintilla and Lexilla to the version specified.
For Scintilla, it will delete all the subfolders of the scintilla folder except the winui folder, which must
be updated manually, using the following Compare tool. For Lexilla, all subfolders of the lexilla folder
will be deleted. Lexilla should be updatable with limited hassle.

Run the Update tool like this:

`Update scintilla x.y.z`

`Update lexilla x.y.z`

where x.y.z is the version number. Find the latest version number here: https://www.scintilla.org/ScintillaDownload.html

The Interface and Vcxproj tools should be run after updating.

## Compare
This tool automatically generates easy to view diffs of what has changed in the Windows (win32) version
of Scintilla on which WinUIEdit is based. Use the diffs to make appropriate changes in the winui folder. The viewer is based on [Monaco](https://microsoft.github.io/monaco-editor/), which will be downloaded (about 14 MB) to the Diff folder the first time the tool is run. The diffs appear in the Tools folder, which are HTML files that you can view in your web browser.

Run the Compare tool like this:

`Compare a.b.c x.y.z`

where a.b.c is the previous version (see scintilla\version.txt), and x.y.z is the new version (see https://www.scintilla.org/ScintillaDownload.html).

## Interface
This tool generates a user-friendly WinRT API for Scintilla and Lexilla
called EditorWrapper based on their respective .iface files.

Run the Interface tool like this:

`Interface`

## Theme
This tool extracts the colors and TextMate scopes from a Visual Studio Code theme.
It is used to generate C++ theme files that are temporarily used until a proper theme
system is developed. The theme file must be passed as a web URL.

Run the Theme tool like this:

`Theme https://raw.githubusercontent.com/microsoft/vscode/main/extensions/theme-defaults/themes/dark_plus.json`

## Uwp
This tool switches the configuration of the editor project to build the UWP
version of the control.

Run the Uwp tool like this:

`Uwp`

## WinUI3
This tool switches the configuration of the editor project to build the WinUI 3
version of the control.

Run the WinUI3 tool like this:

`WinUI3`

## Vcxproj
This tool generates the WinUIEditor.vcxproj and WinUIEditor.vcxproj.filters files.
It adds support for wildcards to vcxproj files and filters them according to predefined folders.
Filters in the WinUIEditor project must be modified by changing the Vcxproj tool, not in Visual Studio.

Run the Vcxproj tool like this:

`Vcxproj`

Wildcard includes in vcxproj files are comments that look like this:

```xml
<!--<WildcardClInclude Include="path\*.h">-->
...auto generated includes go here...
<!--</WildcardClInclude>-->
```

or this:

```xml
<!--<WildcardClCompile Include="path\*.cpp" PrecompiledHeader="NotUsing">-->
...auto generated includes go here...
<!--</WildcardClCompile>-->
```

----
**Note:** At this time, the tools do not have any error handling. They also do not have highly maintainable code. If something goes wrong, you might have to run the tools in Visual Studio to debug them.
