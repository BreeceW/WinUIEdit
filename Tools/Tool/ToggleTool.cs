using System;
using System.Threading.Tasks;
using Windows.Storage;

namespace Tool
{
    public class ToggleTool : ITool
    {
        private readonly bool _useWinUI3;

        public ToggleTool(bool useWinUI3)
        {
            _useWinUI3 = useWinUI3;
        }

        public async Task RunAsync(string path, string[] args)
        {
            await ToggleTxtAsync(path, "UseWinUI3.txt", _useWinUI3);
        }

        private static async Task ToggleTxtAsync(string path, string project, bool value)
        {
            var file = await (await (await StorageFolder.GetFolderFromPathAsync(path)).GetParentAsync()).GetFileAsync(project);
            await FileIO.WriteTextAsync(file, BoolToString(value));
        }

        private static async Task ToggleXmlAsync(string path, string project, bool value)
        {
            var file = await (await (await StorageFolder.GetFolderFromPathAsync(path)).GetParentAsync()).GetFileAsync(project);
            var text = await FileIO.ReadTextAsync(file);
            if (text.Contains($"<WinUIEditorUseWinUI3>{BoolToString(value)}</WinUIEditorUseWinUI3>"))
            {
                return;
            }
            text = text.Replace($"<WinUIEditorUseWinUI3>{BoolToString(!value)}</WinUIEditorUseWinUI3>", $"<WinUIEditorUseWinUI3>{BoolToString(value)}</WinUIEditorUseWinUI3>");
            await FileIO.WriteTextAsync(file, text);
        }

        private static string BoolToString(bool value) => value ? "true" : "false";
    }
}
