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

await tool.RunAsync(args[1..]);
