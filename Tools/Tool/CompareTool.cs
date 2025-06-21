using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Storage;
using Windows.Web.Http;

namespace Tool
{
    public class CompareTool : ITool
    {
        private readonly HttpClient _client = new();

        public async Task RunAsync(string path, string[] args)
        {
            var a = args[0];
            var b = args[1];
            var aVersion = a.Replace('.', '-');
            var bVersion = b.Replace('.', '-');

            var files = new[]
            {
                "HanjaDic.cxx",
                "HanjaDic.h",
                "PlatWin.cxx",
                "PlatWin.h",
                "ScintillaDLL.cxx",
                "ScintillaWin.cxx",
                "ScintillaWin.h",
                "SurfaceD2D.cxx",
                "SurfaceD2D.h",
                "WinTypes.h",
            };

            var folder = await StorageFolder.GetFolderFromPathAsync(path);

            if (await folder.TryGetItemAsync("Diff\\monaco") is null)
            {
                var monacoZipDownload = await _client.GetAsync(new Uri("https://registry.npmjs.org/monaco-editor/-/monaco-editor-0.52.2.tgz"));
                var monacoZipFile = await folder.CreateFileAsync("Monaco.tar.gz", CreationCollisionOption.ReplaceExisting);
                await monacoZipDownload.Content.WriteToStreamAsync(await monacoZipFile.OpenAsync(FileAccessMode.ReadWrite));
                await Process.Start("tar", new[] { "-xf", monacoZipFile.Path, "-C", Path.Combine(folder.Path, "Diff"), }).WaitForExitAsync();
                await monacoZipFile.DeleteAsync();
                var packageFolder = await folder.GetFolderAsync("Diff\\package");
                await packageFolder.RenameAsync("monaco");
            }

            var template = await FileIO.ReadTextAsync(await folder.GetFileAsync("Diff\\template.html"));

            foreach (var file in files)
            {
                string aCode;
                try
                {
                    aCode = await _client.GetStringAsync(new Uri($"https://sourceforge.net/p/scintilla/code/ci/rel-{aVersion}/tree/win32/{file}?format=raw"));
                }
                catch
                {
                    aCode = string.Empty;
                }
                string bCode;
                try
                {
                    bCode = await _client.GetStringAsync(new Uri($"https://sourceforge.net/p/scintilla/code/ci/rel-{bVersion}/tree/win32/{file}?format=raw"));
                }
                catch
                {
                    bCode = string.Empty;
                }

                if (aCode != bCode)
                {
                    var content = template
                        .Replace("'{original}'", JsonValue.CreateStringValue(aCode).Stringify())
                        .Replace("'{modified}'", JsonValue.CreateStringValue(bCode).Stringify());

                    await FileIO.WriteTextAsync(await folder.CreateFileAsync($"Diff {a} to {b}\\{file}.html", CreationCollisionOption.ReplaceExisting), content);
                }
            }
        }
    }
}
