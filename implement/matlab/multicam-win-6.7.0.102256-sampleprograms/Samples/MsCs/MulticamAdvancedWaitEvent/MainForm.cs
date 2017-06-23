/*
+-------------------------------- DISCLAIMER ---------------------------------+
|                                                                             |
| This application program is provided to you free of charge as an example.   |
| Despite the considerable efforts of Euresys personnel to create a usable    |
| example, you should not assume that this program is error-free or suitable  |
| for any purpose whatsoever.                                                 |
|                                                                             |
| EURESYS does not give any representation, warranty or undertaking that this |
| program is free of any defect or error or suitable for any purpose. EURESYS |
| shall not be liable, in contract, in torts or otherwise, for any damages,   |
| loss, costs, expenses or other claims for compensation, including those     |
| asserted by third parties, arising out of or in connection with the use of  |
| this program.                                                               |
|                                                                             |
+-----------------------------------------------------------------------------+
*/

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.Threading;
using Euresys.MultiCam;

namespace MulticamAdvancedWaitEvent
{
    /// <summary>
    /// Summary description for Form1.
    /// </summary>
    /// 

    public class MainForm : System.Windows.Forms.Form
    {
        // Creation of an event for asynchronous call to paint function
        public delegate void PaintDelegate(Graphics g);
        public delegate void UpdateStatusBarDelegate(String text);

        // The object that will contain the acquired image
        private Bitmap image = null;
        
        // The object that will contain the palette information for the bitmap
        private ColorPalette imgpal = null;

        // The Mutex object that will protect image objects during processing
        private static Mutex imageMutex = new Mutex();
        private static Mutex imageMutex2 = new Mutex();
        private Boolean m_RunLoop = false; 
        // The MultiCam object that controls the acquisition
        UInt32 channel;

        // The MultiCam object that contains the acquired buffer
        private UInt32 currentSurface;

        //MC.CALLBACK multiCamCallback;

        private System.Windows.Forms.MainMenu mainMenu;
        private System.Windows.Forms.StatusBar statusBar;
        private System.Windows.Forms.StatusBarPanel statusBarPanel1;
        private System.Windows.Forms.MenuItem Go;
        private System.Windows.Forms.MenuItem Stop;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.Container components = null;

        public MainForm()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            // Show scope of the sample program
            MessageBox.Show(@"
                This program demonstrates the Advanced Acquisition Mode using Windows Event on an Euresys Board.
                
                The Go! menu starts an acquisition sequence by activating the channel.
                By default, this program requires an area-scan camera connected on connector A.", "Sample program description", MessageBoxButtons.OK, MessageBoxIcon.Information);

        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mainMenu = new System.Windows.Forms.MainMenu();
            this.Go = new System.Windows.Forms.MenuItem();
            this.Stop = new System.Windows.Forms.MenuItem();
            this.statusBar = new System.Windows.Forms.StatusBar();
            this.statusBarPanel1 = new System.Windows.Forms.StatusBarPanel();
            ((System.ComponentModel.ISupportInitialize)(this.statusBarPanel1)).BeginInit();
            this.SuspendLayout();
            // 
            // mainMenu
            // 
            this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] { this.Go, this.Stop });
            // 
            // Go
            // 
            this.Go.Index = 0;
            this.Go.Text = "Go!";
            this.Go.Click += new System.EventHandler(this.Go_Click);
            // 
            // Stop
            // 
            this.Stop.Index = 1;
            this.Stop.Text = "Stop!";
            this.Stop.Click += new System.EventHandler(this.Stop_Click);
            // 
            // statusBar
            // 
            this.statusBar.Location = new System.Drawing.Point(0, 568);
            this.statusBar.Name = "statusBar";
            this.statusBar.Panels.AddRange(new System.Windows.Forms.StatusBarPanel[] { this.statusBarPanel1 });
            this.statusBar.ShowPanels = true;
            this.statusBar.Size = new System.Drawing.Size(768, 22);
            this.statusBar.TabIndex = 0;
            this.statusBar.Text = "Ready. Click on the \'GO!\' button to start.";
            // 
            // statusBarPanel1
            // 
            this.statusBarPanel1.Width = 300;
            // 
            // MainForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(768, 590);
            this.Controls.Add(this.statusBar);
            this.Menu = this.mainMenu;
            this.Name = "MainForm";
            this.Text = "MulticamAdvancedWaitEvent";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.MainForm_Paint);
            this.Closed += new System.EventHandler(this.MainForm_Closed);
            this.Closing += new System.ComponentModel.CancelEventHandler(this.MainForm_Closing);
            ((System.ComponentModel.ISupportInitialize)(this.statusBarPanel1)).EndInit();
            this.ResumeLayout(false);

        }
        #endregion

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.Run(new MainForm());
        }

        private void MainForm_Load(object sender, System.EventArgs e)
        {

            // + MulticamAdvancedWaitEvent Sample Program

            try
            {
                // Open MultiCam driver
                MC.OpenDriver();

                // Enable error logging
                MC.SetParam(MC.CONFIGURATION, "ErrorLog", "error.log");

                // In order to support a 10-tap camera on Grablink Full
                // BoardTopology need to be set to MONO_DECA
                // In order to support 2 cameras on Grablink Expert 2
                // BoardTopology need to be set to DUO
                // In all other case the default value will work properly 
                // and the parameter don't need to be set

                // Set the board topology to support 2 cameras (only with a Grablink Expert 2)
                // MC.SetParam(MC.BOARD + 0, "BoardTopology", "DUO");

                // Set the board topology to support 10 taps mode (only with a Grablink Full)
                // MC.SetParam(MC.BOARD + 0, "BoardTopology", "MONO_DECA");

                // Create a channel and associate it with the first connector on the first board
                MC.Create("CHANNEL", out channel);
                MC.SetParam(channel, "DriverIndex", 0);

                // In order to use single camera on connector A
                // MC_Connector need to be set to A for Grablink Expert 2 and Grablink DualBase
                // For all the other Grablink boards the parameter has to be set to M  

                // For all GrabLink boards exect Grablink Expert 2 and Dualbase
                MC.SetParam(channel, "Connector", "M");
                // For Grablink Expert 2 and Dualbase
                //MC.SetParam(channel, "Connector", "A");

                // Choose the video standard
                MC.SetParam(channel, "CamFile", "1000m_P50RG");
                // Choose the camera expose duration
                MC.SetParam(channel,"Expose_us", 20000);
                // Choose the pixel color format
                MC.SetParam(channel, "ColorFormat", "Y8");

                //Set the acquisition mode to Snapshot
                MC.SetParam(channel, "AcquisitionMode", "SNAPSHOT");
                // Choose the way the first acquisition is triggered
                MC.SetParam(channel, "TrigMode", "IMMEDIATE");
                // Choose the triggering mode for subsequent acquisitions
                MC.SetParam(channel, "NextTrigMode", "REPEAT");
                // Choose the number of images to acquire
                MC.SetParam(channel, "SeqLength_Fr", MC.INDETERMINATE);

                // Enable MultiCam signals
                MC.SetParam(channel, MC.SignalEnable + MC.SIG_SURFACE_FILLED, "ON");
                MC.SetParam(channel, MC.SignalEnable + MC.SIG_ACQUISITION_FAILURE, "ON");
                MC.SetParam(channel, MC.SignalEnable + MC.SIG_END_CHANNEL_ACTIVITY , "ON");

                // Enable Multicam Signal Using Windows Event
                MC.SetParam(channel, MC.SignalHandling + MC.SIG_SURFACE_FILLED, MC.SIGNALHANDLING_OS_EVENT_SIGNALING);
                MC.SetParam(channel, MC.SignalHandling + MC.SIG_ACQUISITION_FAILURE, MC.SIGNALHANDLING_OS_EVENT_SIGNALING);
                MC.SetParam(channel, MC.SignalHandling + MC.SIG_END_CHANNEL_ACTIVITY, MC.SIGNALHANDLING_OS_EVENT_SIGNALING);

                // Prepare the channel in order to minimize the acquisition sequence startup latency
                MC.SetParam(channel, "ChannelState", "READY");
            }
            catch (Euresys.MultiCamException exc)
            {
                // An exception has occurred in the try {...} block. 
                // Retrieve its description and display it in a message box.
                MessageBox.Show(exc.Message, "MultiCam Exception");
                Close();
            }

            // - MulticamAdvancedWaitEvent Sample Program
        }

        public void LiveThreadProc()
        {

            MC.SIGNALINFO sigInfo  = new MC.SIGNALINFO();
            Microsoft.Win32.SafeHandles.SafeWaitHandle pHandle;
            int signalledHandle; 

           // Define an array with 3 ManualResetEvent WaitHandles.
            WaitHandle[] waitHandles = new WaitHandle[]
            {
                new ManualResetEvent(false),
                new ManualResetEvent(false),
                new ManualResetEvent(false)
            };

            // Assignment of WaitHandles
            MC.GetParam(channel, MC.SignalEvent + MC.SIG_SURFACE_FILLED, out pHandle);
            waitHandles[0].SafeWaitHandle = pHandle;
            
            MC.GetParam(channel, MC.SignalEvent + MC.SIG_ACQUISITION_FAILURE, out pHandle);
            waitHandles[1].SafeWaitHandle = pHandle;
            
            MC.GetParam(channel, MC.SignalEvent + MC.SIG_END_CHANNEL_ACTIVITY, out pHandle);
            waitHandles[2].SafeWaitHandle = pHandle;
    
            while (m_RunLoop)
            {

                try
                {
                    // Wait for any of the 3 signal
                    signalledHandle = WaitHandle.WaitAny(waitHandles, 5000);

                    // Get the signal information
                    switch (signalledHandle)
                    {
                        case 0:
                            MC.GetSignalInfo(channel, MC.SIG_SURFACE_FILLED, out sigInfo);
                            break;
                        case 1:
                            MC.GetSignalInfo(channel, MC.SIG_ACQUISITION_FAILURE, out sigInfo);
                            break;

                        case 2:
                            MC.GetSignalInfo(channel, MC.SIG_END_CHANNEL_ACTIVITY, out sigInfo);
                            break;

                        case  WaitHandle.WaitTimeout:
                            throw new Euresys.MultiCamException("Timeout");


                        default:
                            throw new Euresys.MultiCamException("Unknown signal");
                     }

                    HandleSignal(ref sigInfo);
                }
                catch (Euresys.MultiCamException exc)
                {
                    // An exception has occurred in the try {...} block. 
                    // Retrieve its description and display it in a message box.
                    MessageBox.Show(exc.Message, "MultiCam Exception");
                    Close();
                }
            }

        }

        private void HandleSignal(ref MC.SIGNALINFO signalInfo)
        {
            UInt32 hSurface ;

            switch(signalInfo.Signal)
            {
                // Handles captured image 
                case MC.SIG_SURFACE_FILLED:
                    ProcessingCallback(signalInfo);
                    hSurface = signalInfo.SignalInfo;
                    MC.SetParam(hSurface, MC.SurfaceState, MC.SURFACESTATE_FREE );
                    break;
                 
                // Handle Acquisition errors
                case MC.SIG_ACQUISITION_FAILURE:
                    AcqFailureCallback(signalInfo);
                    break;

                // Terminate live thread gracefully 
                case MC.SIG_END_CHANNEL_ACTIVITY :
                    m_RunLoop = false;
                    break;

                default:
                    throw new Euresys.MultiCamException("Unknown signal");
            }
        }

        private void ProcessingCallback(MC.SIGNALINFO signalInfo)
        {
            UInt32 currentChannel = (UInt32)signalInfo.Instance;

            statusBar.Text = "Processing";
            currentSurface = signalInfo.SignalInfo;

            // + MulticamAdvancedWaitEvent Sample Program

            try
            {
                // Update the image with the acquired image buffer data 
                Int32 width, height, bufferPitch;
                IntPtr bufferAddress;
                MC.GetParam(currentChannel, "ImageSizeX", out width);
                MC.GetParam(currentChannel, "ImageSizeY", out height);
                MC.GetParam(currentChannel, "BufferPitch", out bufferPitch);
                MC.GetParam(currentSurface, "SurfaceAddr", out bufferAddress);

                try
                {
                    imageMutex.WaitOne();

                    image = new Bitmap(width, height, bufferPitch, PixelFormat.Format8bppIndexed, bufferAddress);
                    
                    imgpal = image.Palette;

                    // Build bitmap palette Y8
                    for (uint i = 0; i < 256; i++)
                    {
                        imgpal.Entries[i] = Color.FromArgb(
                        (byte)0xFF,
                        (byte)i,
                        (byte)i,
                        (byte)i);
                    }

                    image.Palette = imgpal;

                    /* Insert image analysis and processing code here */
                }
                finally
                {
                    imageMutex.ReleaseMutex();
                }

                // Retrieve the frame rate
                Double frameRate_Hz;
                MC.GetParam(channel, "PerSecond_Fr", out frameRate_Hz);

                // Retrieve the channel state
                String channelState;
                MC.GetParam(channel, "ChannelState", out channelState);

                // Display frame rate and channel state
                statusBar.Text = String.Format("Frame Rate: {0:f2}, Channel State: {1}", frameRate_Hz, channelState);

                // Display the new image
                this.BeginInvoke(new PaintDelegate(Redraw), new object[1] { CreateGraphics() });
            }
            catch (Euresys.MultiCamException exc)
            {
                MessageBox.Show(exc.Message, "MultiCam Exception");
            }
            catch (System.Exception exc)
            {
                MessageBox.Show(exc.Message, "System Exception");
            }
            // - MulticamAdvancedWaitEvent Sample Program
        }

        private void AcqFailureCallback(MC.SIGNALINFO signalInfo)
        {
            UInt32 currentChannel = (UInt32)signalInfo.Context;

            // + MulticamAdvancedWaitEvent Sample Program

            try
            {
                // Display frame rate and channel state
                statusBar.Text = String.Format("Acquisition Failure, Channel State: IDLE");
                this.BeginInvoke(new PaintDelegate(Redraw), new object[1] { CreateGraphics() });
            }
            catch (System.Exception exc)
            {
                MessageBox.Show(exc.Message, "System Exception");
            }

            // - MulticamAdvancedWaitEvent Sample Program
        }

        private void UpdateStatusBar(String text)
        {
            statusBarPanel1.Text = text;
        }

        void Redraw(Graphics g)
        {
            // + MulticamAdvancedWaitEvent Sample Program

            try
            {
                imageMutex.WaitOne();

                if (image != null)
                    g.DrawImage(image, 0, 0);
                UpdateStatusBar(statusBar.Text);
            }
            catch (System.Exception exc)
            {
                MessageBox.Show(exc.Message, "System Exception");
            }
            finally
            {
                imageMutex.ReleaseMutex();
            }

            // - MulticamAdvancedWaitEvent Sample Program
        }

        private void MainForm_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            Redraw(e.Graphics);
        }

        private void MainForm_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                Stop_Click(sender, e);
                // Delete the channel
                if (channel != 0)
                {
                    MC.Delete(channel);
                    channel = 0;
                }
            }
            catch (Euresys.MultiCamException exc)
            {
                MessageBox.Show(exc.Message, "MultiCam Exception");
            }
        }

        private void Go_Click(object sender, System.EventArgs e)
        {
            // + MulticamAdvancedWaitEvent Sample Program


            // Start an acquisition sequence by activating the channel
            String channelState;
            MC.GetParam(channel, "ChannelState", out channelState);
            m_RunLoop = true;

            // Starts acquisition thread
            Thread liveThread = new Thread(LiveThreadProc);
            liveThread.Start();


            if (channelState != "ACTIVE")
                MC.SetParam(channel, "ChannelState", "ACTIVE");
            Refresh();

            // - MulticamAdvancedWaitEvent Sample Program
        }

        private void Stop_Click(object sender, System.EventArgs e)
        {
            // + MulticamAdvancedWaitEvent Sample Program
            // Stop an acquisition sequence by deactivating the channel
            if (channel != 0)
                MC.SetParam(channel, "ChannelState", "IDLE");
            UpdateStatusBar(String.Format("Frame Rate: {0:f2}, Channel State: IDLE", 0));

            // - MulticamAdvancedWaitEvent Sample Program
        }

        private void MainForm_Closed(object sender, System.EventArgs e)
        {
            try
            {
                // Close MultiCam driver
                MC.CloseDriver();
            }
            catch (Euresys.MultiCamException exc)
            {
                MessageBox.Show(exc.Message, "MultiCam Exception");
            }
        }
    }
}
