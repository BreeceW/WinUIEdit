using System;
using System.Diagnostics;
using System.IO;
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
                if (file.Name != "winui")
                {
                    await file.DeleteAsync(StorageDeleteOption.PermanentDelete);
                }
            }

            var scintillaZipDownload = await _client.GetAsync(new Uri($"https://www.scintilla.org/{selectedLibrary}{updateVersion}.zip"));
            var scintillaZipFile = await folder.CreateFileAsync($"{selectedLibrary}.zip", CreationCollisionOption.ReplaceExisting);
            await scintillaZipDownload.Content.WriteToStreamAsync(await scintillaZipFile.OpenAsync(FileAccessMode.ReadWrite));
            await Process.Start("tar", new[] { "-xf", scintillaZipFile.Path, "-C", root.Path, }).WaitForExitAsync();
            await scintillaZipFile.DeleteAsync();
        }
    }
}
