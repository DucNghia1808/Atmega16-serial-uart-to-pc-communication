using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZedGraph;
using System.IO.Ports;

namespace bt2
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void zedGraphControl1_Load(object sender, EventArgs e)
        {


        }
        string[] baud = {"1200", "2400", "4800", "9600", "14400", "19200", "38400", "56000", "57600", "115200"};
        string[] databit = { "5", "6", "7", "8" };
        private void Form1_Load(object sender, EventArgs e)
        {
            GraphPane mypannel = zedGraphControl1.GraphPane;
            mypannel.Title.Text = "Temperature, Humidity";
            mypannel.YAxis.Title.Text = "Values";
            mypannel.XAxis.Title.Text = "Time(s)";

            RollingPointPairList list1 = new RollingPointPairList(60000);
            RollingPointPairList list2 = new RollingPointPairList(60000);
            LineItem line1 = mypannel.AddCurve("Temperature", list1, Color.Red, SymbolType.Diamond);
            LineItem line2 = mypannel.AddCurve("Humidity", list2, Color.Blue, SymbolType.Circle);

            mypannel.XAxis.Scale.Min = 0;
            mypannel.XAxis.Scale.Max = 100;
            mypannel.XAxis.Scale.MinorStep = 1;
            mypannel.XAxis.Scale.MajorStep = 5;

            mypannel.YAxis.Scale.Min = 0;
            mypannel.YAxis.Scale.Max = 100;
            mypannel.YAxis.Scale.MinorStep = 1;
            mypannel.YAxis.Scale.MajorStep = 5;

            zedGraphControl1.AxisChange();
            // Serial port
            string[] myport = SerialPort.GetPortNames();
            cBoxPortName.Items.AddRange(myport);
            cBoxBaudRate.Items.AddRange(baud);
            cBoxDataBit.Items.AddRange(databit);
            cBoxParity.Items.AddRange(Enum.GetNames(typeof(Parity)));

        }
        int tong = 0;
        public void draw(double line1, double line2)
        {
            LineItem duongline1 = zedGraphControl1.GraphPane.CurveList[0] as LineItem;
            LineItem duongline2 = zedGraphControl1.GraphPane.CurveList[1] as LineItem;
            if (duongline1 == null)
                return;
            if (duongline2 == null)
                return;
            IPointListEdit list1 = duongline1.Points as IPointListEdit;
            IPointListEdit list2 = duongline2.Points as IPointListEdit;

            list1.Add(tong, line1);// ve
            list2.Add(tong, line2);
            zedGraphControl1.AxisChange(); // thay doi gia tri
            zedGraphControl1.Invalidate(); // mo khoa ve lai
            tong += 1;


        }
        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void btConnect_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort1.PortName = cBoxPortName.Text;
                serialPort1.BaudRate = int.Parse(cBoxBaudRate.Text);
                serialPort1.DataBits = int.Parse(cBoxDataBit.Text);
                serialPort1.Parity = (Parity)Enum.Parse(typeof(Parity), cBoxParity.Text);
                serialPort1.StopBits = (StopBits)Enum.Parse(typeof(StopBits), cBoxStopBit.Text);
                serialPort1.Open();
                btConnect.Enabled = false;
                btDisconnect.Enabled = true;
                progressBar1.Value = 100;

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void btDisconnect_Click(object sender, EventArgs e)
        {
            try
            {
                
                serialPort1.Close();
                btConnect.Enabled = true;
                btDisconnect.Enabled = false;
                progressBar1.Value = 0;

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        // dữ liệu được truyền lên nhảy vào hàm
        string data = "";
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {// du lieu nhiet do tu atmega16 gui len
            data += serialPort1.ReadExisting();
            if (data.Length > 3)
            {
                //int data1 = int.Parse(data);
                Invoke(new MethodInvoker(() => listBox1.Items.Add(data))); //hiển thị giá trị nhiệt độ độ ẩm textbox
                Invoke(new MethodInvoker(() => draw(int.Parse(data)/100, int.Parse(data)%100)));// xư ly du lieu tu vdk
                data = "";
                //data1 = 0;
            }   
        }
        // ham send du lieu xuong
        private void btSend_Click(object sender, EventArgs e)
        {
            serialPort1.Write(tBoxSend.Text);
            tBoxSend.Text = "";
        }

        private void button3_Click(object sender, EventArgs e) // clear listbox
        {
            listBox1.Items.Clear();
        }

        private void cBoxStopBit_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void groupBox2_Enter(object sender, EventArgs e)
        {

        }

        private void progressBar1_Click(object sender, EventArgs e)
        {

        }
    }
}
