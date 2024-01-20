using System;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Web.Http;

namespace Tool
{
    public class PackageTool : ITool
    {
        public async Task RunAsync(string path, string[] args)
        {
            var root = await (await StorageFolder.GetFolderFromPathAsync(path)).GetParentAsync();
            var solution = await root.GetFileAsync("WinUIEditor.sln");
            var uwpNuspec = await root.GetFileAsync("MicaEditorCsWinRT\\nuget\\MicaEditorUWP.nuspec");
            var winui3Nuspec = await root.GetFileAsync("MicaEditorCsWinRT\\nuget\\MicaEditorWinUI3.nuspec");

            if (!ReadArgs(args, out var uwp, out var uwpVersion, out var winui3, out var winui3Version))
            {
                Console.WriteLine("Specify version for WinUI 3 and UWP explicitly");
                Console.WriteLine("Example: Package winui3:1.2.3 uwp:5.6.7");
                return;
            }

            await RestoreAsync(solution.Path, path);

            if (uwp)
            {
                if (uwpVersion != null)
                {
                    await SetVersionAsync(uwpNuspec, uwpVersion);
                }
                await BuildAsync(solution.Path, "MicaEditor", false, "x64");
                await BuildAsync(solution.Path, "MicaEditor", false, "x86");
                await BuildAsync(solution.Path, "MicaEditor", false, "arm64");
                await BuildAsync(solution.Path, "MicaEditor", false, "ARM");
                await BuildAsync(solution.Path, "MicaEditorCsWinRT", false, "Any CPU", "Projection");
            }

            if (winui3)
            {
                if (winui3Version != null)
                {
                    await SetVersionAsync(winui3Nuspec, winui3Version);
                }
                await BuildAsync(solution.Path, "MicaEditor", true, "x64");
                await BuildAsync(solution.Path, "MicaEditor", true, "x86");
                await BuildAsync(solution.Path, "MicaEditor", true, "arm64");
                await BuildAsync(solution.Path, "MicaEditorCsWinRT", true, "Any CPU", "Projection");
            }
        }

        private static async Task SetVersionAsync(IStorageFile file, string version)
        {
            var xml = await FileIO.ReadTextAsync(file);
            var start = xml.IndexOf("<version>");
            var end = xml.IndexOf("</version>");
            await FileIO.WriteTextAsync(file, xml.Replace(xml[start..end], $"<version>{version}"));
        }

        private static async Task BuildAsync(string solution, string project, bool useWinUI3, string architecture, string label = null)
        {
            Console.WriteLine($"Building {label ?? $"{(useWinUI3 ? "WinUI 3" : "UWP")} {architecture}"}");
            var process = new Process();
            process.StartInfo.FileName = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe";
            process.StartInfo.Arguments = $"\"{solution}\" -r -t:\"{project}\" -p:MicaEditorUseWinUI3={useWinUI3};Configuration=Release;Platform=\"{architecture}\"";
            process.Start();
            await process.WaitForExitAsync();
        }

        private static async Task RestoreAsync(string solution, string toolsPath)
        {
            Console.WriteLine($"Restoring solution");

            var tools = await StorageFolder.GetFolderFromPathAsync(toolsPath);
            var nuget = await tools.TryGetItemAsync("nuget.exe");
            if (nuget == null)
            {
                nuget = await tools.CreateFileAsync("nuget.exe", CreationCollisionOption.ReplaceExisting);
                var client = new HttpClient();
                var nugetExe = await client.GetBufferAsync(new Uri("https://dist.nuget.org/win-x86-commandline/latest/nuget.exe"));
                await FileIO.WriteBufferAsync((IStorageFile)nuget, nugetExe);
            }

            var process = new Process();
            process.StartInfo.FileName = nuget.Path;
            process.StartInfo.Arguments = $"restore \"{solution}\"";
            process.Start();
            await process.WaitForExitAsync();
        }


        private static bool ReadArgs(string[] args, out bool uwp, out string uwpVersion, out bool winui3, out string winui3Version)
        {
            uwp = false;
            uwpVersion = null;
            winui3 = false;
            winui3Version = null;

            foreach (var arg in args)
            {
                if (arg.StartsWith("uwp"))
                {
                    uwp = true;
                    if (arg.IndexOf(':') is var colon && colon != -1)
                    {
                        uwpVersion = arg[(colon + 1)..];
                    }
                }
                else if (arg.StartsWith("winui3"))
                {
                    winui3 = true;
                    if (arg.IndexOf(':') is var colon && colon != -1)
                    {
                        winui3Version = arg[(colon + 1)..];
                    }
                }
                else
                {
                    if (uwpVersion == null && winui3Version == null)
                    {
                        uwpVersion = arg;
                        winui3Version = arg;
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            if (!uwp && !winui3)
            {
                uwp = true;
                winui3 = true;
            }
            
            if ((!uwp && uwpVersion != null) || (!winui3 && winui3Version != null))
            {
                return false;
            }

            return true;
        }
    }
}
