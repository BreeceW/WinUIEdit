using System;
using System.IO;
using Tool;

var toolName = args[0];
ITool tool = toolName switch
{
    "Compare" => new CompareTool(),
    "Update" => new UpdateTool(),
    _ => throw new InvalidOperationException("Not a valid tool"),
};

var dllPath = System.Reflection.Assembly.GetEntryAssembly().Location;
var path = dllPath[..(dllPath.LastIndexOf("\\Tools") + 6)];

await tool.RunAsync(path, args[1..]);
