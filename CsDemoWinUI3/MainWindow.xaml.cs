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

        private void VersionSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (((ToggleSwitch)sender).IsOn)
            {
                // Editor1 -> Editor2
                // var text = GetTextOf_(Editor1);
                // Editor2.Editor.SetText(string.IsNullOrEmpty(text) ? string.Empty : text);
                Editor2.Editor.SetText(Editor1.Editor.GetText(Editor1.Editor.Length));

                ScrollView.Visibility = Visibility.Visible;
                Editor1.Visibility = Visibility.Collapsed;
                Editor2.Focus(FocusState.Programmatic);
            }
            else
            {
                // Editor2 -> Editor1
                // var text = GetTextOf_(Editor2);
                // Editor1.Editor.SetText(string.IsNullOrEmpty(text) ? string.Empty : text);
                Editor1.Editor.SetText(Editor2.Editor.GetText(Editor2.Editor.Length));

                ScrollView.Visibility = Visibility.Collapsed;
                Editor1.Visibility = Visibility.Visible;
                Editor1.Focus(FocusState.Programmatic);
            }
        }

        private void VerticalResizingCheckBox_Clicked(object sender, RoutedEventArgs e)
        {
            Editor2.IsVerticalResizingEnabled = ((bool)((CheckBox)sender).IsChecked);
        }
    }
}
