using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using Windows.Web.Http;

namespace Tool
{
    public class ThemeTool : ITool
    {
        private readonly HttpClient _http = new();

        private record Style(string Foreground);

        public async Task RunAsync(string path, string[] args)
        {
            var dict = new Dictionary<string, Style>();
            await ProcessAsync(dict, new Uri(args[0]));
            var sb = new StringBuilder();
            foreach (var kvp in dict)
            {
                sb.Append("case Scope::").Append(KeyToEnum(kvp.Key)).Append(": return IntRGBA(0x")
                    .Append(kvp.Value.Foreground[0..2]).Append(", 0x").Append(kvp.Value.Foreground[2..4]).Append(", 0x").Append(kvp.Value.Foreground[4..6])
                    .Append(");\r\n");
            }
            Console.WriteLine(sb.ToString());

            var sb2 = new StringBuilder("enum class Scope\r\n{");
            foreach (var kvp in dict)
            {
                sb2.Append('\t').Append(KeyToEnum(kvp.Key)).Append(", // ").Append(kvp.Key).Append("\r\n");
            }
            sb2.Append("};");
            Console.WriteLine(sb2.ToString());
        }

        private static string KeyToEnum(string key)
        {
            var newKey = string.Empty;
            foreach (var i in key.Replace('-', '_').Replace(" ", "__").Split('.'))
            {
                newKey += i[0..1].ToUpperInvariant() + i[1..];
            }
            var newKey2 = string.Empty;
            foreach (var i in newKey.Split('_'))
            {
                newKey2 += i.Length == 0 ? "_" : i[0..1].ToUpperInvariant() + i[1..] + "_";
            }
            return newKey2[..^1];
        }

        private async Task ProcessAsync(Dictionary<string, Style> dict, Uri uri)
        {
            var json = await _http.GetBufferAsync(uri);
            var doc = JsonDocument.Parse(json.AsStream(), new JsonDocumentOptions
            {
                CommentHandling = JsonCommentHandling.Skip,
                AllowTrailingCommas = true,
            });
            if (doc.RootElement.TryGetProperty("include"u8, out var include))
            {
                await ProcessAsync(dict, new Uri(uri, include.GetString()));
            }
            if (doc.RootElement.TryGetProperty("tokenColors"u8, out var tokenColors))
            {
                foreach (var item in tokenColors.EnumerateArray())
                {
                    if (item.GetProperty("settings"u8).TryGetProperty("foreground"u8, out var foreValue))
                    {
                        var foreString = foreValue.GetString()[1..].ToUpperInvariant();
                        if (foreString.Length == 3)
                        {
                            foreString += foreString;
                        }
                        var style = new Style(foreString);

                        var scope = item.GetProperty("scope"u8);
                        var scopes = scope.ValueKind == JsonValueKind.String
                            ? new List<string>(1)
                            {
                            scope.GetString(),
                            }
                            : scope.EnumerateArray().Select(s => s.GetString());
                        foreach (var s in scopes)
                        {
                            dict[s] = style;
                        }
                    }
                }
            }
        }
    }
}
