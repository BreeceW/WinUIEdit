using System;

namespace WpfIslandsDemo
{
    public class Program
    {
        [STAThread]
        public static void Main()
        {
            using (new CsIslandsUwpDemoApp.App())
            {
                var app = new App();
                app.InitializeComponent();
                app.Run();
            }
        }
    }
}
