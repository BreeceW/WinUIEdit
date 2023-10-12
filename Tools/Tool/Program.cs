using System;
using System.Reflection;
using Tool;

var toolName = args[0];
ITool tool = toolName switch
{
    "Compare" => new CompareTool(),
    "Update" => new UpdateTool(),
    "Uwp" => new ToggleTool(false),
    "WinUI3" => new ToggleTool(true),
    "Interface" => new InterfaceGeneratorTool(),
    "Vcxproj" => new VcxprojTool(),
    _ => throw new InvalidOperationException("Not a valid tool"),
};

var dllPath = Assembly.GetEntryAssembly().Location;
var path = dllPath[..(dllPath.LastIndexOf("\\Tools") + 6)];

Console.WriteLine($"Running {toolName} tool...");
await tool.RunAsync(path, args[1..]);
Console.WriteLine($"{toolName} tool finished");
