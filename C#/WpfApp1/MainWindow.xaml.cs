using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfApp1
{   
    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window

    {
        private bool colorbutton = false;
        public MainWindow()
        {
            InitializeComponent();
        }
        private void SendMessage()
        {
            if (textBoxEmission.Text == "\r\n")
            {
                textBoxEmission.Clear();
                return;
            }
            RichTextBox.Text += "Recu : " + textBoxEmission.Text + "\n";
            textBoxEmission.Clear();
        }
        private void boutonenvoy_Click(object sender, RoutedEventArgs e)
        {
            colorbutton = !colorbutton;
            if (colorbutton)
                boutonenvoy.Background = Brushes.Beige;
            else
                boutonenvoy.Background = Brushes.RoyalBlue;
            SendMessage();

        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }

        }
    }
}