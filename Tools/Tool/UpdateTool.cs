using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Web.Http;

namespace Tool
{
    public class UpdateTool : ITool
    {
        private readonly HttpClient _client = new();

        public async Task RunAsync(string path, string[] args)
        {
            var updateLibrary = args[0].Trim().ToUpperInvariant();

            string selectedLibrary;
            if (updateLibrary == "SCINTILLA")
            {
                selectedLibrary = "scintilla";
            }
            else if (updateLibrary == "LEXILLA")
            {
                selectedLibrary = "lexilla";
            }
            else
            {
                Console.WriteLine("Invalid library. Please specifiy scintilla or lexilla, followed by the version.");
                return;
            }

            var update = args[1];
            var updateVersion = update.Replace(".", string.Empty);

            var folder = await StorageFolder.GetFolderFromPathAsync(path);
            var root = await folder.GetParentAsync();
            var sourceFolder = await root.GetFolderAsync(selectedLibrary);

            foreach (var file in await sourceFolder.GetItemsAsync())
            {
                if (file.Name != "winui" && file.Name != "WinUIModified.txt")
                {
                    await file.DeleteAsync(StorageDeleteOption.PermanentDelete);
                }
            }

            var scintillaZipDownload = await _client.GetAsync(new Uri($"https://www.scintilla.org/{selectedLibrary}{updateVersion}.zip"));
            var scintillaZipFile = await folder.CreateFileAsync($"{selectedLibrary}.zip", CreationCollisionOption.ReplaceExisting);
            await scintillaZipDownload.Content.WriteToStreamAsync(await scintillaZipFile.OpenAsync(FileAccessMode.ReadWrite));
            await Process.Start("tar", new[] { "-xf", scintillaZipFile.Path, "-C", root.Path, }).WaitForExitAsync();
            await scintillaZipFile.DeleteAsync();

            if (await sourceFolder.TryGetItemAsync("WinUIModified.txt") is IStorageFile modified)
            {
                var lines = (await FileIO.ReadLinesAsync(modified)).Where(l => !string.IsNullOrWhiteSpace(l)).ToList();
                if (lines.Count == 0)
                {
                    return;
                }
                Console.BackgroundColor = ConsoleColor.Yellow;
                Console.ForegroundColor = ConsoleColor.Black;
                Console.Write("Patch the following files by re-applying the deleted lines marked ");
                Console.ForegroundColor = ConsoleColor.DarkGreen;
                Console.Write("// WinUI");
                Console.ForegroundColor = ConsoleColor.Black;
                Console.WriteLine(" in the diff editor:");
                Console.ResetColor();
                foreach (var line in lines)
                {
                    Console.WriteLine($"* {Path.GetRelativePath(root.Path, Path.Combine(Path.GetDirectoryName(modified.Path), line))}");
                }
            }
        }
    }
}
