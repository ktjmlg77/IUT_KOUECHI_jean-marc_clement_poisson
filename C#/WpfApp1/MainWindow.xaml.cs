using System.IO.Ports;
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
using ExtendedSerialPort_NS;



namespace WpfApp1
{


    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window

    {
        ExtendedSerialPort serialPort1;
        

        private bool colorbutton = false;
        public MainWindow()
        {
            serialPort1 = new ExtendedSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            InitializeComponent();
        }

        private void SerialPort1_DataReceived(object? sender, DataReceivedArgs e)
        {
            RichTextBox.Text += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
        }

        private void SendMessage()
        {
            if (textBoxEmission.Text == "\r\n")
            {s
                textBoxEmission.Clear();
                return;
            }
            serialPort1.WriteLine(textBoxEmission.Text.Trim()); 
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