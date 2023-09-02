using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Controls;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace CsDemoWinUI3
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow
    {
        public MainWindow()
        {
            InitializeComponent();

            Title = "Demo WinUI3 (C#)";

            SystemBackdrop = new MicaBackdrop();
        }

        private void CodeEditorControl_Loaded(object sender, RoutedEventArgs e)
        {
            // Needs to set focus explicitly due to WinUI 3 regression https://github.com/microsoft/microsoft-ui-xaml/issues/8816 
            ((Control)sender).Focus(FocusState.Programmatic);
        }
    }
}
