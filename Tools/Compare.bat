@echo off

dotnet run --project Tool\Tool.csproj -c Release Compare %*
