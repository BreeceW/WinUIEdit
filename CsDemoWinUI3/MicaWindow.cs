using Microsoft.UI.Xaml;
using System.Runtime.InteropServices;
using WinRT;

namespace CsDemoWinUI3
{
    public class MicaWindow : Window
    {
        private class WindowsSystemDispatcherQueueHelper
        {
            [StructLayout(LayoutKind.Sequential)]
            struct DispatcherQueueOptions
            {
                internal int dwSize;
                internal int threadType;
                internal int apartmentType;
            }

            [DllImport("CoreMessaging.dll")]
            private static extern int CreateDispatcherQueueController([In] DispatcherQueueOptions options, [In, Out, MarshalAs(UnmanagedType.IUnknown)] ref object dispatcherQueueController);

            object m_dispatcherQueueController = null;
            public void EnsureWindowsSystemDispatcherQueueController()
            {
                if (Windows.System.DispatcherQueue.GetForCurrentThread() != null)
                {
                    // one already exists, so we'll just use it.
                    return;
                }

                if (m_dispatcherQueueController == null)
                {
                    DispatcherQueueOptions options;
                    options.dwSize = Marshal.SizeOf(typeof(DispatcherQueueOptions));
                    options.threadType = 2;    // DQTYPE_THREAD_CURRENT
                    options.apartmentType = 2; // DQTAT_COM_STA

                    CreateDispatcherQueueController(options, ref m_dispatcherQueueController);
                }
            }
        }

        WindowsSystemDispatcherQueueHelper _wsdqHelper; // See separate sample below for implementation
        Microsoft.UI.Composition.SystemBackdrops.MicaController _micaController;
        Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration _configurationSource;

        protected bool TrySetMicaBackdrop()
        {
            if (Microsoft.UI.Composition.SystemBackdrops.MicaController.IsSupported())
            {
                _wsdqHelper = new WindowsSystemDispatcherQueueHelper();
                _wsdqHelper.EnsureWindowsSystemDispatcherQueueController();

                // Hooking up the policy object
                _configurationSource = new Microsoft.UI.Composition.SystemBackdrops.SystemBackdropConfiguration();
                Activated += Window_Activated;
                Closed += Window_Closed;

                // Initial configuration state.
                _configurationSource.IsInputActive = true;
                switch (((FrameworkElement)Content).ActualTheme)
                {
                    case ElementTheme.Dark: _configurationSource.Theme = Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme.Dark; break;
                    case ElementTheme.Light: _configurationSource.Theme = Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme.Light; break;
                    case ElementTheme.Default: _configurationSource.Theme = Microsoft.UI.Composition.SystemBackdrops.SystemBackdropTheme.Default; break;
                }

                _micaController = new Microsoft.UI.Composition.SystemBackdrops.MicaController();

                // Enable the system backdrop.
                // Note: Be sure to have "using WinRT;" to support the Window.As<...>() call.
                _micaController.AddSystemBackdropTarget(this.As<Microsoft.UI.Composition.ICompositionSupportsSystemBackdrop>());
                _micaController.SetSystemBackdropConfiguration(_configurationSource);
                return true; // succeeded
            }

            return false; // Mica is not supported on this system
        }

        private void Window_Activated(object sender, WindowActivatedEventArgs args)
        {
            _configurationSource.IsInputActive = args.WindowActivationState != WindowActivationState.Deactivated;
        }

        private void Window_Closed(object sender, WindowEventArgs args)
        {
            // Make sure any Mica/Acrylic controller is disposed so it doesn't try to
            // use this closed window.
            if (_micaController != null)
            {
                _micaController.Dispose();
                _micaController = null;
            }
            Activated -= Window_Activated;
            _configurationSource = null;
        }
    }
}
