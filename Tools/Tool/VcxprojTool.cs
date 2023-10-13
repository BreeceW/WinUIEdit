using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Xml.Dom;
using Windows.Storage;

namespace Tool
{
    public class VcxprojTool : ITool
    {
        private static readonly string s_ns = "http://schemas.microsoft.com/developer/msbuild/2003";
        private static readonly string s_scintilla = "Scintilla";
        private static readonly string s_lexilla = "Lexilla";
        private static readonly string s_lexers = "Lexers";

        public async Task RunAsync(string path, string[] args)
        {
            var root = await (await StorageFolder.GetFolderFromPathAsync(path)).GetParentAsync();
            var micaEditor = await root.GetFolderAsync("MicaEditor");
            var themes = await micaEditor.GetFolderAsync("Themes");
            var winui = await root.GetFolderAsync("scintilla\\winui");
            var scintillaInclude = await root.GetFolderAsync("scintilla\\include");
            var scintillaSrc = await root.GetFolderAsync("scintilla\\src");
            var call = await root.GetFolderAsync("scintilla\\call");
            var lexillaSrc = await root.GetFolderAsync("lexilla\\src");
            var lexlib = await root.GetFolderAsync("lexilla\\lexlib");
            var lexillaInclude = await root.GetFolderAsync("lexilla\\include");
            var lexers = await root.GetFolderAsync("lexilla\\lexers");

            var doc = new XmlDocument();

            var project = doc.CreateElementNS(s_ns, "Project");
            project.SetAttribute("ToolsVersion", "15.0");
            doc.AppendChild(project);
            {
                var filters = doc.CreateElementNS(s_ns, "ItemGroup");
                project.AppendChild(filters);
                {
                    AppendFilter(doc, filters, "Resources", "accd3aa8-1ba0-4223-9bbe-0c431709210b", "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tga;tiff;tif;png;wav;mfcribbon-ms");
                    AppendFilter(doc, filters, "Generated Files", "{926ab91d-31b4-48c3-b9a4-e681349f27f0}");
                    AppendFilter(doc, filters, s_scintilla, "{d2af1cc1-e66c-4161-aa1f-2dce16d662a7}");
                    AppendFilter(doc, filters, s_lexilla, "{dcec4768-41d4-43a2-9eaa-ebecc2ebbcca}");
                    AppendFilter(doc, filters, s_lexers, "{d97ff389-708f-4fce-a1ad-4f6ad3a99854}");
                }

                var cppItems = doc.CreateElementNS(s_ns, "ItemGroup");
                project.AppendChild(cppItems);
                {
                    AppendInclude(doc, cppItems, "ClCompile", "$(GeneratedFilesDir)module.g.cpp", "Generated Files");
                    await AppendIncludesAsync(doc, cppItems, micaEditor, "ClCompile", "cpp|cxx");
                    await AppendIncludesAsync(doc, cppItems, winui, "ClCompile", "cpp|cxx", "..\\scintilla\\winui\\");
                    await AppendIncludesAsync(doc, cppItems, scintillaSrc, "ClCompile", "cpp|cxx", "..\\scintilla\\src\\", s_scintilla);
                    await AppendIncludesAsync(doc, cppItems, call, "ClCompile", "cpp|cxx", "..\\scintilla\\call\\", s_scintilla);
                    await AppendIncludesAsync(doc, cppItems, lexers, "ClCompile", "cpp|cxx", "..\\lexilla\\lexers\\", s_lexers);
                    await AppendIncludesAsync(doc, cppItems, lexlib, "ClCompile", "cpp|cxx", "..\\lexilla\\lexlib\\", s_lexilla);
                    await AppendIncludesAsync(doc, cppItems, lexillaSrc, "ClCompile", "cpp|cxx", "..\\lexilla\\src\\", s_lexilla);
                }

                var hItems = doc.CreateElementNS(s_ns, "ItemGroup");
                project.AppendChild(hItems);
                {
                    await AppendIncludesAsync(doc, hItems, micaEditor, "ClInclude", "h");
                    await AppendIncludesAsync(doc, hItems, winui, "ClInclude", "h", "..\\scintilla\\winui\\");
                    await AppendIncludesAsync(doc, hItems, scintillaInclude, "ClInclude", "h", "..\\scintilla\\include\\", s_scintilla);
                    await AppendIncludesAsync(doc, hItems, scintillaSrc, "ClInclude", "h", "..\\scintilla\\src\\", s_scintilla);
                    await AppendIncludesAsync(doc, hItems, lexlib, "ClInclude", "h", "..\\lexilla\\lexlib\\", s_lexilla);
                    await AppendIncludesAsync(doc, hItems, lexillaInclude, "ClInclude", "h", "..\\lexilla\\include\\", s_lexilla);
                }

                var modItems = doc.CreateElementNS(s_ns, "ItemGroup");
                project.AppendChild(modItems);
                {
                    AppendInclude(doc, modItems, "None", "MicaEditor.def");
                    AppendInclude(doc, modItems, "None", "packages.config");
                    AppendInclude(doc, modItems, "None", "Defines.idl");
                }

                var xamlItems = doc.CreateElementNS(s_ns, "ItemGroup");
                project.AppendChild(xamlItems);
                {
                    await AppendIncludesAsync(doc, xamlItems, themes, "Page", "xaml", "Themes\\");
                    await AppendIncludesAsync(doc, xamlItems, micaEditor, "Page", "xaml");
                }

                var natvisItems = doc.CreateElementNS(s_ns, "ItemGroup");
                project.AppendChild(natvisItems);
                {
                    AppendInclude(doc, natvisItems, "Natvis", "$(MSBuildThisFileDirectory)..\\..\\natvis\\wil.natvis");
                }

                var idlItems = doc.CreateElementNS(s_ns, "ItemGroup");
                project.AppendChild(idlItems);
                {
                    await AppendIncludesAsync(doc, idlItems, micaEditor, "Midl", "idl", exclude: "Defines.idl");
                }
            }

            var xml = PrettyPrint(doc.GetXml());

            var filtersFile = await micaEditor.GetFileAsync("MicaEditor.vcxproj.filters");
            await FileIO.WriteTextAsync(filtersFile, xml);

            var projectFile = await micaEditor.GetFileAsync("MicaEditor.vcxproj");
            var projectText = await FileIO.ReadTextAsync(projectFile);
            var expanded = await WildcardExpandAsync(micaEditor, projectText);
            await FileIO.WriteTextAsync(projectFile, expanded);
        }

        private static async Task<string> WildcardExpandAsync(IStorageFolder root, string text)
        {
            var replacements = new List<(string, string)>();

            var i = 0;
            while (true)
            {
                var initial = "<!--<Wildcard";
                i = text.IndexOf("<!--<Wildcard", i);
                if (i == -1)
                {
                    break;
                }
                i++;

                var space = text.IndexOf(' ', i);

                var type = text[(i + initial.Length - 1)..space];

                var quote = text.IndexOf('"', space) + 1;
                var endQuote = text.IndexOf('"', quote);

                var path = text[quote..endQuote];
                var extension = path[(path.LastIndexOf('.') + 1)..];
                var prefix = path[..path.LastIndexOf('*')];

                var endMessage = "<!--</WildcardClInclude>-->";
                var opening = text[(i - 1)..(text.IndexOf(">-->", i) + 4)];
                var tag = text[(i - 1)..(text.IndexOf(endMessage, i) + endMessage.Length)];
                var noPch = tag.Contains("PrecompiledHeader=\"NotUsing\"");

                var directory = await StorageFolder.GetFolderFromPathAsync(Path.GetDirectoryName(Path.GetFullPath(Path.Combine(root.Path, path))));
                var matches = await MatchFilesAsync(directory, extension);
                var replacement = opening + "\r\n" + string.Join("\r\n", matches.Select(m => !noPch
                    ? $"    <{type} Include=\"{prefix}{m}\" />"
                    : $"    <{type} Include=\"{prefix}{m}\">\r\n      <PrecompiledHeader>NotUsing</PrecompiledHeader>\r\n    </{type}>"))
                    + $"\r\n    {endMessage}";

                replacements.Add((tag, replacement));
            }

            foreach (var replacement in replacements)
            {
                text = text.Replace(replacement.Item1, replacement.Item2);
            }

            return text;
        }

        private static void AppendFilter(XmlDocument doc, XmlElement parent, string name, string id, string extensions = null)
        {
            var filter = doc.CreateElementNS(s_ns, "Filter");
            parent.AppendChild(filter);
            filter.SetAttribute("Include", name);

            var uniqueIdentifier = doc.CreateElementNS(s_ns, "UniqueIdentifier");
            filter.AppendChild(uniqueIdentifier);
            {
                uniqueIdentifier.InnerText = id;
            }

            if (extensions != null)
            {
                var ext = doc.CreateElementNS(s_ns, "Extensions");
                filter.AppendChild(ext);
                {
                    ext.InnerText = extensions;
                }
            }
        }

        private static async Task AppendIncludesAsync(XmlDocument doc, XmlElement parent, IStorageFolder folder, string type, string extension, string path = null, string filter = null, string exclude = null)
        {
            var exludes = exclude?.Split('|') ?? Array.Empty<string>();
            foreach (var file in await MatchFilesAsync(folder, extension))
            {
                if (exludes.Contains(file))
                {
                    continue;
                }
                AppendInclude(doc, parent, type, path + file, filter);
            }
        }

        private static async Task<IList<string>> MatchFilesAsync(IStorageFolder folder, string extension)
        {
            var files = await folder.GetFilesAsync();
            var extensions = extension.Split('|');
            return files.Where(f => extensions.Any(n => f.Name.EndsWith("." + n, StringComparison.OrdinalIgnoreCase))).Select(f => f.Name).ToList();
        }

        private static void AppendInclude(XmlDocument doc, XmlElement parent, string type, string file, string filter = null)
        {
            var element = doc.CreateElementNS(s_ns, type);
            parent.AppendChild(element);
            element.SetAttribute("Include", file);
            if (filter != null)
            {
                var sub = doc.CreateElementNS(s_ns, "Filter");
                element.AppendChild(sub);
                sub.InnerText = filter;
            }
        }

        private static string PrettyPrint(string xml)
        {
            var sb = new StringBuilder("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");

            var xws = new System.Xml.XmlWriterSettings
            {
                Indent = true,
                OmitXmlDeclaration = true,
                NewLineChars = "\r\n",
            };

            using (var xmlWriter = System.Xml.XmlWriter.Create(sb, xws))
            {
                System.Xml.Linq.XDocument.Parse(xml).Save(xmlWriter);
            }

            return sb.ToString();
        }
    }
}
