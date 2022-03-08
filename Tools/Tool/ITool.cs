using System.Threading.Tasks;

namespace Tool
{
    public interface ITool
    {
        Task RunAsync(string path, string[] args);
    }
}
