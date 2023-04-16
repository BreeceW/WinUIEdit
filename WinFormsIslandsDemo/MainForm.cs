using Microsoft.Toolkit.Forms.UI.XamlHost;
using System.Runtime.InteropServices;
using System;
using System.Windows.Forms;
using Windows.UI.Core;

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

            // Required or else the drawing code will get cut off
            if (CoreWindow.GetForCurrentThread() is { } coreWindow && (ICoreWindowInterop)(object)coreWindow is var interop)
            {
                _coreWindowHWnd = interop.WindowHandle;
            }
            UpdateCoreWindowSize();
            host.SizeChanged += Host_SizeChanged;
        }

        private void Host_SizeChanged(object sender, EventArgs e)
        {
            UpdateCoreWindowSize();
        }

        [ComImport, Guid("45D64A29-A63E-4CB6-B498-5781D298CB4F")]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        interface ICoreWindowInterop
        {
            IntPtr WindowHandle { get; }
            bool MessageHandled { set; }
        }

        [DllImport("user32.dll")]
        private static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);
        private const uint SWP_NOMOVE = 0x0002;
        private const uint SWP_NOZORDER = 0x0004;

        private IntPtr _coreWindowHWnd = IntPtr.Zero;

        private void UpdateCoreWindowSize()
        {
            if (_coreWindowHWnd != IntPtr.Zero)
            {
                SetWindowPos(_coreWindowHWnd, IntPtr.Zero, 0, 0, Controls[0].Width, Controls[0].Height, SWP_NOMOVE | SWP_NOZORDER);
            }
        }
    }
}
