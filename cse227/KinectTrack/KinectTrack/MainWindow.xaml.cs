using System;
using System.Collections.Generic;
using System.Linq;
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
using Microsoft.Kinect;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.IO;
using System.Drawing;
using System.Windows.Media.Media3D;
using Petzold.Media3D;
//NOTE: Color is aliased here to avoid conflicting with the class of the same name in System.Windows.Media
using DColor = System.Drawing.Color;
using WMColor = System.Windows.Media.Color;


namespace KinectTrack
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        KinectSensor sensor;   //our sensor

        // Should the depth frame be drawn
        private bool drawDepthFrame = false;
        // Should the 3d skeleton frame be drawn
        private bool draw3dFrame = false;
        //Should we want to pring debug statements
        private bool debugging = false;

        //TODO: probably delete this?
        private FixedCapacityList<double> rkneelist;  //???  WHAT IS this for?

        // Bitmaps for the color and depth frames 
        WriteableBitmap colorBmp;
        WriteableBitmap depthBmp;
        //NOTE: this is a bit hacky, as I have hardcoded the sizes of the frames in order to lessen GC pauses
        Int32Rect colorFrameRect = new Int32Rect(0, 0, 640, 480);
        Int32Rect depthFrameRect = new Int32Rect(0, 0, 640, 480);

        //The skeleton frame that gets written to whenever the skeleton is in the frame
        List<Skeleton> skelList = new List<Skeleton>();

        // The skelList for use by the 3d display function
        List<DanSkeleton> copySkelList;

        // The skelList for testing normalized skeletons
        List<Skeleton> normSkelList = new List<Skeleton>();

        public MainWindow()
        {
            InitializeComponent();
            Loaded += MainWindow_Loaded;
        }

        private void MainWindow_Loaded(object sender, EventArgs e)
        {

            if (KinectSensor.KinectSensors.Count < 1)
            {
                MessageBox.Show("You Should probably make sure the kinect is plugged in!\n");
                return;
            }
            try
            {
                sensor = KinectSensor.KinectSensors[0];  //initialize sensor 
                if (sensor == null)
                {
                    MessageBox.Show("Kinect Sensor Returned Null");
                    return;
                }
                //enable sensor streams
                sensor.ColorStream.Enable(ColorImageFormat.RgbResolution640x480Fps30);
                sensor.DepthStream.Enable(DepthImageFormat.Resolution640x480Fps30);
                sensor.SkeletonStream.Enable();  //TODO: Should we add smoothing params?
                //declare new event handler
                sensor.AllFramesReady += new EventHandler<AllFramesReadyEventArgs>(sensor_AllFramesReady);
                sensor.Start();

                rkneelist = new FixedCapacityList<double>(90);  //TODO: ??? 
            }
            catch
            {
                MessageBox.Show("Caught Something\n");
            }
        }

        /*
         * sensor_AllFramesReady() - event handler for when the kinect is ready to send us frames
         */
        private Boolean skelDetected = false;

        void sensor_AllFramesReady(object sender, AllFramesReadyEventArgs e)
        {
            // Handle image frame
            using(ColorImageFrame colorFrame = e.OpenColorImageFrame()) 
            {
                if(colorFrame != null) 
                {
                    //DebugLabel.Content = "Woot, we got frames!";
                    byte[] pixels = new byte[colorFrame.PixelDataLength];
                    colorFrame.CopyPixelDataTo(pixels);

                    int stride = colorFrame.Width * 4; // Bytes per row in the image

                    if(colorBmp == null) {
                       colorBmp = new WriteableBitmap(BitmapSource.Create(colorFrame.Width,colorFrame.Height,96,96, PixelFormats.Bgr32, null, pixels, stride));
                    } else {
                        colorBmp.WritePixels(colorFrameRect, pixels, stride, 0);
                    }
                    visBox.Source = colorBmp;
                }
            }

            DepthImageFrame depthFrame = e.OpenDepthImageFrame();
            SkeletonFrame skelFrame = e.OpenSkeletonFrame();

            //NOTE: Ideally, these would be in using blocks, but that fact that skelToBitmap needs the depthframe complicates things
            //TODO: (to NOTE above - what about nested Using statements?) http://stackoverflow.com/questions/1329739/nested-using-statements-in-c-sharp
            if (depthFrame != null && drawDepthFrame)
            {
                byte[] pixels = generateColorPixels(depthFrame);

                int stride = depthFrame.Width * 4; // Bytes per row in the image

                if (depthBmp == null)
                {
                    depthBmp = new WriteableBitmap(BitmapSource.Create(depthFrame.Width, depthFrame.Height, 96, 96, PixelFormats.Bgr32, null, pixels, stride));
                }
                else
                {
                    depthBmp.WritePixels(depthFrameRect, pixels, stride, 0);
                }
                depthBox.Source = depthBmp;

                
            }
            if (skelFrame != null)
            {
                // Stuff goes here...
                Skeleton firstSkel = getFirstSkeleton(skelFrame);
                if (firstSkel != null)
                {
                    if (!skelDetected)
                    {
                        // Play sound here
                        System.Media.SoundPlayer player = new System.Media.SoundPlayer(@"..\..\Audio\skeleton_detected.wav");
                        player.Play();
                        skelDetected = true;
                    }
                    // Print the fun face on the image frame
                    Skeleton normSkel = normalizeSkel(firstSkel);
                    //testing for point indices  TODO:  not working on display, delete this
                    SkeletonPoint p = new SkeletonPoint();
                    p.X = .25F;
                    p.Y = .25F;
                    p.Z = .25F;
                    //Skeleton sSkel = shiftSkel(normSkel, p);
                    SkelToBitmap(firstSkel, depthFrame);
                    //SkelToBitmap(sSkel, depthFrame);   //seeing if we get the normalized skeleton
                    skelList.Add(firstSkel);
                    normSkelList.Add(normSkel);  //parallel list of normalized skeletons (smaller)

                    // Print some basic stats
                    double ankleToKneeRight = jointDistance(firstSkel.Joints[JointType.AnkleRight], firstSkel.Joints[JointType.KneeRight]);
                    double ankleToKneeLeft = jointDistance(firstSkel.Joints[JointType.AnkleLeft], firstSkel.Joints[JointType.KneeLeft]);
                    // Stupid .NET uses a different syntax for format strings for no discernable reason
                    String s = String.Format("Ankle to right knee dist = {0,5:f} Ankle to left knee dist = {1,5:f} \n", ankleToKneeRight, ankleToKneeLeft);

                    double kneeToHipRight = jointDistance(firstSkel.Joints[JointType.KneeRight], firstSkel.Joints[JointType.HipRight]);
                    double kneeToHipLeft = jointDistance(firstSkel.Joints[JointType.KneeLeft], firstSkel.Joints[JointType.HipLeft]);
                    String s2 = String.Format("Knee to Hip right = {0,5:f} Knee to Hip left = {1,5:f} \n", kneeToHipRight, ankleToKneeLeft);


                    rkneelist.addElement(ankleToKneeRight);
                    string s3 = String.Format("Average of ankle to knee is {0,5:f}", rkneelist.average());
                    //NOTE: Stringbuilder needed?
                    skelInfo.Text = s + s2 + s3;
                }
                else
                {
                    skelDetected = false;
                }
            }

            // Dispose of skelFrame and depthFrame
            if (skelFrame != null) skelFrame.Dispose();
            if (depthFrame != null) depthFrame.Dispose();
        }

        /// <summary>
        /// Returns the Euclidean distance between joints. Only makes sense when called with adjacent points
        /// </summary>
        /// <param name="joint"></param>
        /// <param name="joint2"></param>
        /// <returns></returns>
        private double jointDistance(Joint j, Joint j2)
        {
            double x = j.Position.X - j2.Position.X;
            double y = j.Position.Y - j2.Position.Y;
            double z = j.Position.Z - j2.Position.Z;
            return Math.Sqrt(x * x + y * y + z * z);
        }

        Skeleton getFirstSkeleton(SkeletonFrame frame)
        {
            Skeleton[] closet = new Skeleton[frame.SkeletonArrayLength];
            frame.CopySkeletonDataTo(closet);
            Skeleton first = (from s in closet where s.TrackingState == SkeletonTrackingState.Tracked select s).FirstOrDefault();  //nifty
            return first;
        }

        /// <summary>
        /// This function generates some pixel data from a given DepthImageFrame
        /// </summary>
        /// <param name="depthFrame"></param>
        /// <returns></returns>
        private byte[] generateColorPixels(DepthImageFrame depthFrame)
        {
            short[] rawData = new short[depthFrame.PixelDataLength];
            depthFrame.CopyPixelDataTo(rawData);

            byte[] pixels = new byte[depthFrame.Height * depthFrame.Width * 4];

            for (int depthIndex = 0, colorIndex = 0;
                depthIndex < rawData.Length && colorIndex < pixels.Length;
                depthIndex++, colorIndex += 4)
            {
                // Get player (0 if no player, 1-6 otherwise)
                int player = rawData[depthIndex] & DepthImageFrame.PlayerIndexBitmask;

                // Get depth value
                // This is in mm
                // range for depth is 800-4000mm (2.5 feet - 13.1 feet) in normal mode
                int depth = rawData[depthIndex] >> DepthImageFrame.PlayerIndexBitmaskWidth;

                //Note that we only need to write to the non-zero indices (probably)
                // NOTE: setPixelColor is likely to be slow when called with non-static colors
                if (depth <= 900)
                {
                    pixels.setPixelColor(colorIndex, System.Drawing.Color.Blue);
                    //pixels[colorIndex + Utils.BlueIndex] = 255;
                    //pixels[colorIndex + Utils.GreenIndex] = 0;
                    //pixels[colorIndex + Utils.RedIndex] = 0;
                }
                if (depth > 900 && depth <= 2000)
                {
                    pixels.setPixelColor(colorIndex, System.Drawing.Color.Red);
                   // pixels[colorIndex + Utils.BlueIndex] = 0;
                    //pixels[colorIndex + Utils.GreenIndex] = 255;
                    //pixels[colorIndex + Utils.RedIndex] = 0;
                }
                if (depth > 2000)
                {
                    pixels.setPixelColor(colorIndex, System.Drawing.Color.CadetBlue);
                    //pixels[colorIndex + Utils.BlueIndex] = 0;
                    //pixels[colorIndex + Utils.GreenIndex] = 0;
                    //pixels[colorIndex + Utils.RedIndex] = 255;
                }
            }
            return pixels;
        }

        private void SkelToBitmap(Skeleton skel, DepthImageFrame dFrame)
        {

            if (dFrame != null)
            {
                DepthImagePoint headDepthPoint = dFrame.MapFromSkeletonPoint(skel.Joints[JointType.Head].Position);
                ColorImagePoint headColorPoint = dFrame.MapToColorImagePoint(headDepthPoint.X, headDepthPoint.Y, ColorImageFormat.RgbResolution640x480Fps30);

                moveFaceToPoint(headColorPoint);
            }
        }

        private void moveFaceToPoint(ColorImagePoint headColorPoint)
        {
            var marg = visBox.Margin;
            var visLeft = marg.Left;
            var visTop = marg.Top;
            // NOTE: This is hacked up
            // NOTE: note that we have to divide the values in headColorPoint by 2 since we are displaying the images at 1/2 resolution
            // TODO: determine if there is a way to do this without creating a new object on every call!
            faceImage.Margin = new Thickness(visLeft + (headColorPoint.X/2) - (faceImage.Width/2), visTop + (headColorPoint.Y/2) - (faceImage.Height /2), 0, 0);

        }

        /*
         * normalizeSkel - normalizes a skeleton such that all points are treated as vectors and normalized as such: each point has its x, y, and z
         *                  coordinate divided by the length of the vector for that point.  Returns a new skeleton with normalized components.
         */
        private Skeleton normalizeSkel(Skeleton skelly){
            if (debugging == true)
            {
                System.Console.Write("Value of head.x before normalization\t" + skelly.Joints[JointType.Head].Position.X + "\n");
                System.Console.Write("Value of Skel.Position before normalization\t" + skelly.Position.X + "\n");
            }
            DanSkeleton newSkelly = new DanSkeleton(skelly);
            newSkelly.normalize();
            if (debugging == true)
            {
                System.Console.Write("Value of head.x after normalization\t" + newSkelly.Joints[JointType.Head].Position.X + "\n");
                System.Console.Write("Value of Skel.Position after normalization\t" + newSkelly.Position.X + "\n");
            }
            return newSkelly;   //polymorphism boosh!
        }


        /*
         * shiftSkel-  shifts skeleton from its position to this new position
         * param:
         *   SkeletonPoint newCenter - the new point around which to center the skeleton 
         *   Skeleton skelly - the skeleton to shift
         */
        private Skeleton shiftSkel(Skeleton skelly, SkeletonPoint newCenter)
        {

            if (debugging == true)
            {
                System.Console.Write("Value of head.x before shift\t" + skelly.Joints[JointType.Head].Position.X + "\n");
                System.Console.Write("Value of Skel.Position before shift\t" + skelly.Position.X + "\n");
            }
            //figure out the values by which to shift
            //based on ORIGINAL SKELETON POS - NEW SKELETON POS 
            float xAmount, yAmount, zAmount;
            xAmount = skelly.Position.X - newCenter.X;
            yAmount = skelly.Position.Y - newCenter.Y;
            zAmount = skelly.Position.Z - newCenter.Z;
            //shift it
            DanSkeleton newSkelly = new DanSkeleton(skelly);
            newSkelly.shift(xAmount, yAmount, zAmount);
            if (debugging == true)
            {
                System.Console.Write("Value of head.x after shift\t" + newSkelly.Joints[JointType.Head].Position.X + "\n");
                System.Console.Write("Value of Skel.Position after shift\t" + newSkelly.Position.X + "\n");
            }
            return newSkelly;   //polymorphism boosh!
        }
       


        private void tiltButton_Click(object sender, RoutedEventArgs e)
        {
            sensor.ElevationAngle = (int)tiltSlider.Value.Clamp(sensor.MinElevationAngle, sensor.MaxElevationAngle);
        }

        //What?
        private void visBox_ImageFailed(object sender, ExceptionRoutedEventArgs e)
        {

        }

        private void depthCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            drawDepthFrame = !drawDepthFrame;
        }

        private void depthCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            drawDepthFrame = !drawDepthFrame;
        }

        List<Stride> allStrides = new List<Stride>();

        private void grabSkelList_Click(object sender, RoutedEventArgs e)
        {
            //strideAnalizer = new Stride(skelList);
            allStrides.Add(new Stride(skelList));
            //strideAnalizer = Stride.buildStrideFromFile(@"..\..\..\training.txt");
            //TODO: this might not be the best place for this but it ought to work:
            //SkelListToFile(skelList,0,skelList.Count-1,"\tTest output", @".\training.txt");
            //SkelListToFile(skelList,0,skelList.Count-1,"\tTest output", @"C:\Users\DCashman\Documents\UCSD\Courses\CSE227\KinectTrack\training.txt");
            //TODO: add null checks and stuff here
            /*
            copySkelList = new List<DanSkeleton>();

            foreach(Skeleton s in skelList) 
            {
                DanSkeleton d = new DanSkeleton(s);
                copySkelList.Add(d);
            }
            */
            // Set up the slider
            skelSlider.Minimum = 0;
            skelSlider.Maximum = allStrides[allStrides.Count - 1].numFrames; // strideAnalizer.numFrames;
            skelSlider.IsSnapToTickEnabled = true;

        }

        private void skelSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            // Get the slider frame number
            int frameNumber = (int)e.NewValue;
            //Clear the viewport
            skelViewport.Children.Clear();
            //renderSkeleton(copySkelList[frameNumber.Clamp(0,copySkelList.Count-1)]);

            //strideAnalizer.drawFrameToViewport(frameNumber.Clamp(0, strideAnalizer.numFrames - 1), skelViewport);
            Stride mostRecent = allStrides[allStrides.Count - 1];
            mostRecent.drawFrameToViewport(frameNumber.Clamp(0, mostRecent.numFrames - 1), skelViewport);
        }


        /*
        private void renderSkeleton(DanSkeleton renderSkel)
        {
            Model3DGroup skel3dGroup = new Model3DGroup();
            foreach(Joint j in renderSkel.Joints) {
                // Create a cube for each joint
                ModelVisual3D curJointCube;
                if (renderSkel.isStepSkel)
                {
                    curJointCube = Utils3D.getCube(Colors.Blue);
                }
                else
                {
                    curJointCube = Utils3D.getCube(Colors.Red);
                }
                Transform3DGroup tGroup = new Transform3DGroup();
                // move the joints to the right positions
                tGroup.Children.Add(Utils3D.getJointPosTransform(j, 10));
                // Make the squares smaller
                tGroup.Children.Add(new ScaleTransform3D(.5, .5, .5));
                curJointCube.Transform = tGroup;
                this.skelViewport.Children.Add(curJointCube);
            }
            // Put the camera in the position of the kinect (more or less)
            this.skelViewport.Camera = new PerspectiveCamera(new Point3D(0, 0, -3), new Vector3D(renderSkel.Position.X, renderSkel.Position.Y, renderSkel.Position.Z), new Vector3D(0, 1, 0), 75);
            // Add a light so that colors are visible
            this.skelViewport.Children.Add(new ModelVisual3D() { Content = new AmbientLight(Colors.White) });

        }
        */

        /*
         * SkelListToFile - appends each joint in each frame in a given skeleton List to the given file. If the file does not exist, it creates a new one
         * params:
         *     closet - the skeleton list to print
         *     startFrame - the frame where we should start printing, should not be less than 0
         *     endFrame - the last frame to print to the list, should not exceed the last index of the given list
         *     note - a note to append to the end of the line (for our purposes should at least include the person (classification))
         *     fileName - the path and file to which to write
         */
        public void SkelListToFile(List<Skeleton> closet, int startFrame, int endFrame, String note, String fileName)
        {
            if (startFrame < 0 || endFrame >= closet.Count)
            {  //classic off by one
                //error message
                return;
            }
            Skeleton currentFrame;
            SkeletonPoint currentPos;
            String output = "";  //where all the goodies will go
            //TODO: open file for appending/writing
            for (int i = startFrame; i <= endFrame; i++)
            {
                //for each frame, print out a tab delimited list of values
                currentFrame = closet[i];
                for (int j = 0; j < 20; j++)
                {  //iterate through each joint
                    currentPos = currentFrame.Joints[(JointType)j].Position;
                    output += currentPos.X + "\t" + currentPos.Y + "\t" + currentPos.Z + "\t";
                }
                //add skeleton position too
                currentPos=currentFrame.Position;
                output += currentPos.X + "\t" + currentPos.Y + "\t" + currentPos.Z + "\t";
                output += "\n";
            }
            output += "###"+note+"\n";//### will denote the note
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(fileName, true))  //TDO: not sure what the @ does
            {
                file.WriteLine(output);
            }


        }


        /*
         * FileToSkelLists - 
         */
        public List<List<Skeleton>> FileToSkelLists(String fileName)
        {
            return null;
        }

        private void DEBUG_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void clearButton_Click(object sender, RoutedEventArgs e)
        {
            skelList.Clear();
        }

        private void printButton_Click(object sender, RoutedEventArgs e)
        {
            Stride.writeListOfStridesToFile(allStrides, "classifyPaulCrazy.txt");
           /* List<List<Skeleton>> masterSkelList = new List<List<Skeleton>>();
           for (int i = 0; i < allStrides.Count; i++)
            {
                masterSkelList.Add(allStrides[i].rawSkelFromStride());
            }
            skelListStats s = new skelListStats(masterSkelList, true, "klusters.txt");
            */

        }

        private void inputButton_Click(object sender, RoutedEventArgs e)
        {
            //Stride.writeListOfStridesToFile(allStrides, "rawStrides.txt");
            List<List<Skeleton>> masterSkelList = new List<List<Skeleton>>();
            List<Stride> fileStrides = new List<Stride>();

            fileStrides = Stride.loadListOfStridesFromFile("classifyPaulCrazy.txt");
            //Stride.writeListOfStridesToFile(fileStrides, "combinedDataCopy.txt");
            //var blob = Stride.listOfStridesToARFF(fileStrides);
            for (int i = 0; i < fileStrides.Count; i++)
            {
                masterSkelList.Add(fileStrides[i].rawSkelFromStride());
                //normalize skeletons and shift to center (added)
                SkeletonPoint newCenter = new SkeletonPoint();
                newCenter.X = 0;
                newCenter.Y = 0;
                newCenter.Z = 0;
                for (int j = 0; j < masterSkelList[i].Count; j++)
                {
                    masterSkelList[i][j] = normalizeSkel(masterSkelList[i][j]);
                    masterSkelList[i][j] = shiftSkel(masterSkelList[i][j], newCenter);  //shift values
                }
            }
            int fourPower=1;
            for (int i = 1; i <= 5; i++)
            {
                fourPower *= 4;
                skelListStats s = new skelListStats(masterSkelList, false, fourPower, "klusters", "classifyPaulCrazyAsKlusters");
            }
            

        }

        private void ARFFButton_Click(object sender, RoutedEventArgs e)
        {
            String path = "combinedData.txt";
            List<Stride> slist = Stride.loadListOfStridesFromFile(path);

            String arffstr = Stride.listOfStridesToARFF(slist, "allData");
            arffBox.Text = arffstr;
            System.IO.File.WriteAllText("allData.arff", arffstr);
          


        }

        private void svmButton_Click(object sender, RoutedEventArgs e)
        {
            //String[] paths = { "paulNormal.txt", "danNormal.txt", "paulCrazyLegs.txt", "danCrazyArms.txt" };
            String[] paths = { "ptest.txt" };
            Dictionary<String, List<Stride>> listDict = new Dictionary<string, List<Stride>>();
            // Load the strides
            foreach(String path in paths)
            {
                listDict.Add(path,
                    Stride.loadListOfStridesFromFile(path));
            }

            Console.WriteLine("loaded Strides");
            // Transform the lists of strides into the svm format with and without prefix
            Dictionary<String, String> path2svm = new Dictionary<string, string>();
            foreach (KeyValuePair<String, List<Stride>> kvp in listDict)
            {
                String svmStr1 = Stride.listOfStridesToSVM(kvp.Value, "+1");
                String svmStr2 = Stride.listOfStridesToSVM(kvp.Value, "-1");
                path2svm.Add(kvp.Key + "+", svmStr1);
                path2svm.Add(kvp.Key + "-", svmStr2);
                Console.WriteLine("toSVM!");
            }
            

            // build the 4 output files
            foreach(String path in paths)
            {
                String outPath = path + "out.txt";
                String output = "";

                foreach (String path2 in paths)
                {
                    if (path == path2)
                    {
                        output += path2svm[path2 + "+"];
                    }
                    else
                    {
                        output += path2svm[path2 + "-"];
                    }
                }
                System.IO.File.WriteAllText(outPath, output);
                Console.WriteLine("didfile " + outPath);
            }
        }
    }
}
