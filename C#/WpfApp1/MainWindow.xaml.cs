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
using System.Windows.Threading;
using ExtendedSerialPort_NS;
using System.Windows.Threading;





namespace WpfApp1
{
    
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window

    {
        
        ExtendedSerialPort serialPort1;
        private string receivedText = string.Empty;
        private bool colorbutton = false;
        public MainWindow()
        {

            DispatcherTimer timerAffichage;

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            serialPort1 = new ExtendedSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            InitializeComponent();
        }
        void TimerAffichage_Tick(object? sender, EventArgs e)
        {
            RichTextBox.Text += receivedText;
            receivedText = string.Empty;
        }
        private void SerialPort1_DataReceived(object? sender, DataReceivedArgs e)
        {
            receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
        }

        private void SendMessage()
        {
            if (textBoxEmission.Text == "\r\n")
            {
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

        private void clear_Click(object sender, RoutedEventArgs e)
        {
            RichTextBox.Clear();
        }
    }
}