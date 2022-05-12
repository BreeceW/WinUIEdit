using System;
using System.Threading.Tasks;
using Windows.Storage;

namespace Tool
{
    internal class ToggleTool : ITool
    {
        private readonly bool _useWinUI3;

        public ToggleTool(bool useWinUI3)
        {
            _useWinUI3 = useWinUI3;
        }

        public async Task RunAsync(string path, string[] args)
        {
            var file = await (await (await StorageFolder.GetFolderFromPathAsync(path)).GetParentAsync()).GetFileAsync("MicaEditor\\MicaEditor.vcxproj");
            var text = await FileIO.ReadTextAsync(file);
            if (text.Contains($"<MicaEditorUseWinUI3>{BoolToString(_useWinUI3)}</MicaEditorUseWinUI3>"))
            {
                return;
            }
            text = text.Replace($"<MicaEditorUseWinUI3>{BoolToString(!_useWinUI3)}</MicaEditorUseWinUI3>", $"<MicaEditorUseWinUI3>{BoolToString(_useWinUI3)}</MicaEditorUseWinUI3>");
            await FileIO.WriteTextAsync(file, text);
        }

        private static string BoolToString(bool value) => value ? "true" : "false";
    }
}
