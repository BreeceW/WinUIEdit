# Tools

This directory contains a couple of tools to help keep this editor up-to-date with Scintilla.
These tools only run on Windows and require the .NET 6 SDK to be installed.

## Update
This tool updates the version of Scintilla and Lexilla to the version specified.
For Scintilla, it will delete all the subfolders of the scintilla folder except the winui folder, which must
be updated manually, using the following Compare tool. For Lexilla, all subfolders of the lexilla folder
will be deleted. Lexilla should be updatable with limited hassle.

Run the Update tool like this:

`Update scintilla x.y.z`

`Update lexilla x.y.z`

where x.y.z is the version number. Find the latest version number here: https://www.scintilla.org/ScintillaDownload.html

## Compare
This tool automatically generates easy to view diffs of what has changed in the Windows (win32) version
of Scintilla on which WinUIEdit is based. Use the diffs to make appropriate changes in the winui folder. The viewer is based on [Monaco](https://microsoft.github.io/monaco-editor/), which will be downloaded (about 14 MB) to the Diff folder the first time the tool is run. The diffs appear in the Tools folder, which are HTML files that you can view in your web browser.

Run the Compare tool like this:

`Compare a.b.c x.y.z`

where a.b.c is the previous version (see scintilla\version.txt), and x.y.z is the new version (see https://www.scintilla.org/ScintillaDownload.html).

## Uwp
This tool switches the configuration of the editor project to build the UWP
version of the control. If you have Visual Studio open and the configuration is
changed, Visual Studio will ask you to reload the project, which you should.

Run the Uwp tool like this:

`Uwp`

## WinUI3
This tool switches the configuration of the editor project to build the WinUI 3
version of the control. If you have Visual Studio open and the configuration is
changed, Visual Studio will ask you to reload the project, which you should.

Run the WinUI3 tool like this:

`WinUI3`

----
**Note:** At this time, the tools do not have any error checking. If something goes wrong, you might have to run the tools in Visual Studio to debug them.
