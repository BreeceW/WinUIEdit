using System;

namespace WpfIslandsDemo
{
    public class Program
    {
        [STAThread]
        public static void Main()
        {
            using (new IslandsUwpDemoApp.App())
            {
                var app = new App();
                app.InitializeComponent();
                app.Run();
            }
        }
    }
}
