using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Media;
using Windows.UI.Core;

namespace WpfIslandsDemo
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        // Required or else the drawing code will get cut off
        private void WindowsXamlHost_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateCoreWindowSize((Visual)sender, new Point(e.NewSize.Width, e.NewSize.Height));
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

        private void UpdateCoreWindowSize(Visual visual, Point size)
        {
            if (_coreWindowHWnd == IntPtr.Zero
                && CoreWindow.GetForCurrentThread() is { } coreWindow
                && (ICoreWindowInterop)(object)coreWindow is var interop)
            {
                _coreWindowHWnd = interop.WindowHandle;
            }

            if (_coreWindowHWnd != IntPtr.Zero)
            {
                var rawSize = PresentationSource.FromVisual(visual).CompositionTarget.TransformToDevice.Transform(size);
                SetWindowPos(_coreWindowHWnd, IntPtr.Zero, 0, 0, (int)rawSize.X, (int)rawSize.Y, SWP_NOMOVE | SWP_NOZORDER);
            }
        }
    }
}
