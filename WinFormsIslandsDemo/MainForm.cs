using Microsoft.Toolkit.Forms.UI.XamlHost;
using System.Windows.Forms;

namespace WinFormsIslandsDemo
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();

            var host = new WindowsXamlHost
            {
                Dock = DockStyle.Fill,
            };
            var control = Microsoft.Toolkit.Win32.UI.XamlHost.UWPTypeFactory.CreateXamlContentByType("MicaEditor.MicaEditorControl");
            host.Child = control;
            Controls.Add(host);
        }
    }
}
