using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Kinect;
using System.Windows;
using System.Windows.Media.Media3D;
using System.Windows.Media;
using System.IO;

namespace KinectTrack
{
    class Stride
    {
        public List<DanSkeleton> capturedFrames; //TODO: make private again
        

        private int firstFrame;  //first frame of stride as determined by alg
        private int lastFrame;  //last frame of "stride" as determiend by alg
        private int midFrame;
        private List<Skeleton> rawSkelList;
        
       
        /*
        private double[] jointStandardDeviations = new double[20];  //20 joints same for below
        private double[] jointAvg = new double[20];
        private double[] jointDistStandardDeviations = new double[20];  //TODO: also not 20
        private double[] jointDistAvg = new double[20];  //TODO: also not 20
        private double[] anglesStandardDeviations = new double[20];  //TODO: it's not 20
        private double[] anglesAvg = new double[20];  //TODO: not 20, stupid
        */
        public Stride(List<Skeleton> rawSkeletonList) : this(rawSkeletonList, true)
        {
        }
        public Stride(List<Skeleton> rawSkeletonList, bool doRotate)
        {

            capturedFrames = convertToDanFromSkel(rawSkeletonList);
            rawSkelList = rawSkeletonList;
            this.numFrames = capturedFrames.Count;

            //rotateSkelList capturedFrames so that it is aligned with x-axis direction of movement
            if(doRotate) rotateSkelList(this.capturedFrames);
            //use foot crossing function to determine actual stride start and end (save as firstFrame and lastFrame)
            this.getStridePositions();
            //TODO: sanitize/add checks
            capturedFrames[firstFrame].isStepSkel = true;
            capturedFrames[lastFrame].isStepSkel = true;
            //now with a "Stride," calculate descriptive values and store as local vars
        }
        public static String listOfStridesToARFF(List<Stride> strideList, String relationName)
        {
            ARFFMaker maker = new ARFFMaker();
            maker.addProp("leftFootAngleAvg");
            maker.addProp("leftFootAngleMax");
            maker.addProp("leftFootAngleMin");

            maker.addProp("rightFootAngleAvg");
            maker.addProp("rightFootAngleMax");
            maker.addProp("rightFootAngleMin");

            maker.addProp("maxHeadHeightMeters");
            maker.addProp("minHeadHeightMeters");

            maker.addProp("strideLengthMeters");

            maker.addProp("widthBetweenFeetAvg");

            maker.addProp("distanceBetweenAllPointsAvg");


            // TODO: figure out which 
            // stride length
            // max foot elevation

            //addAttr(arffStr, "StrideLength", "numeric");
            return maker.getARFF(strideList, relationName);
        }

        internal static string listOfStridesToSVM(List<Stride> list, string prefix)
        {
            var props = new List<String>();
            //props.Add("leftFootAngleAvg");
            props.Add("leftFootAngleMax");
            props.Add("leftFootAngleMin");

            props.Add("maxLFootHeight");
            props.Add("maxRFootHeight");

            props.Add("strideMetersPerSecond");

            //props.Add("rightFootAngleAvg");
            props.Add("rightFootAngleMax");
            props.Add("rightFootAngleMin");

            props.Add("maxHeadHeightMeters");
            props.Add("minHeadHeightMeters");

            props.Add("strideLengthMeters");

            props.Add("widthBetweenFeetAvg");
            props.Add("distanceBetweenAllPointsAvg");

            var sb = new StringBuilder();
            foreach (Stride s in list)
            {
                sb.Append(prefix + " ");
                int i = 1;
                foreach (String pName in props)
                {
                    if (pName.StartsWith("distance"))
                    {
                        double[] dists = s.distanceBetweenAllPointsAvg;
                        for (int j = 0; j < 190; j++)
                        {
                            switch (pName)
                            {
                                case "distanceBetweenAllPointsAvg":
                                    sb.Append(i + ":" + dists[j]);
                                    if (double.IsNaN(dists[j]))
                                    {
                                        Console.WriteLine("bad");
                                        double[] dists2 = s.distanceBetweenAllPointsAvg;
                                    }
                                    sb.Append(" ");
                                    break;
                                default:
                                    break;
                            }
                            i++;
                        }
                    }
                    else
                    {
                        var v = s.GetType().GetProperty(pName).GetValue(s, null);
                        sb.Append(i + ":" + v + "  " );
                        i++;
                    }
                }
                sb.Append("\n");
            }
            return sb.ToString();
        }            
        /*
         * rawSkelFromStride - gets the underlying skelton list 
         */
        public List<Skeleton> rawSkelFromStride()
        {
            return rawSkelList;
        }

        public static void writeListOfStridesToFile(List<Stride> sList, String fileName) {
            //var outFile = File.Create(fileName, 32, FileOptions.None);
            StringBuilder output = new StringBuilder();
            foreach (Stride s in sList)
            {
                //output.AppendLine(
                // append a split sequence [NEW SKELETON] or something like that
                List<DanSkeleton> curSkelList = s.capturedFrames;
                DanSkeleton currentFrame;
                SkeletonPoint currentPos;
                for (int i = 0; i < curSkelList.Count; i++)  //TODO check that this in fact should not be <=
                {
                    //for each frame, print out a tab delimited list of values
                    currentFrame = curSkelList[i];
                    for (int j = 0; j < 20; j++)
                    {  //iterate through each joint
                        currentPos = currentFrame.Joints[(JointType)j].Position;
                        output.Append(currentPos.X + "\t" + currentPos.Y + "\t" + currentPos.Z + "\t");
                    }
                    //add skeleton position too
                    currentPos = currentFrame.Position;
                    output.Append(currentPos.X + "\t" + currentPos.Y + "\t" + currentPos.Z + "\t");
                    output.Append("\n");
                }
                output.Append("[STRIDE] \n");
            }
            System.IO.File.AppendAllText(fileName, output.ToString()); 
        }

        public static List<Stride> loadListOfStridesFromFile(String fileName)
        {

            // read all the strides
            
            var allLines = new List<String>(System.IO.File.ReadAllLines(fileName));

            var curStrideLines = new List<String>();
            List<Stride> output = new List<Stride>();
            foreach (var line in allLines) 
            {
                // Iterate throught the output until we find a stride marker
                if (!line.StartsWith("[STRIDE]"))
                {
                    curStrideLines.Add(line);
                }
                else
                {
                    //build a stride and add it to the output
                    Stride buildMe;
                    List<Skeleton> strideList = new List<Skeleton>();
                    // each line represents one skelelton/frame
                    foreach (String skelLine in curStrideLines)
                    {
                        String strippedSkelLine = skelLine.Replace(" ", "");
                        String[] splitSkelLine = strippedSkelLine.Split(new Char[] { '\t' });
                        double[] posArray = new double[splitSkelLine.Length];
                        int index = 0;
                        // convert to doubles
                        foreach (String s in splitSkelLine)
                        {
                            if(s.Equals("")) break;
                            posArray[index] = Convert.ToDouble(s);
                            index++;
                        }
                        // build joints
                        Joint[] joints4Skel = new Joint[20];
                        int doubleIndex = 0;
                        for(int jointIndex = 0; jointIndex < 20; jointIndex++) {
                            SkeletonPoint p = new SkeletonPoint();
                            p.X = (float)posArray[doubleIndex];
                            doubleIndex++;
                            p.Y = (float)posArray[doubleIndex];
                            doubleIndex++;
                            p.Z = (float)posArray[doubleIndex];
                            doubleIndex++;
                            joints4Skel[jointIndex] = new Joint();
                            joints4Skel[jointIndex].Position = p;
                        }
                        SkeletonPoint skelPos = new SkeletonPoint();
                        skelPos.X = (float)posArray[doubleIndex];
                        doubleIndex++;
                        skelPos.Y = (float)posArray[doubleIndex];
                        doubleIndex++;
                        skelPos.Z = (float)posArray[doubleIndex];
                        doubleIndex++;

                        DanSkeleton frameSkel = new DanSkeleton(joints4Skel, skelPos);
                        strideList.Add(frameSkel);
                    }
                    buildMe = new Stride(strideList, false);
                    output.Add(buildMe);
                    // clear curStrideLines
                    curStrideLines.Clear();
                }
                
            }

            return output;
        }

        public String strideToStats()
        {
            //TODO: impelment me
            return "";
        }

        public static Stride buildStrideFromFile(String fileName)
        {
            List<Skeleton> dList = new List<Skeleton>();

            String[] lines = System.IO.File.ReadAllLines(fileName);
            foreach (String line in lines)
            {
                String[] splitLine = line.Split(new Char[] { '\t' });
                Queue<String> lineStack = new Queue<string>(splitLine);
                // Joints are stored in xyz order in the order they are defined in JointType
                var jointVals = Enum.GetValues(typeof(JointType));
                Joint[] curJoints = new Joint[20];
                if(lineStack.Peek().Equals("###")) break;
                foreach (JointType jt in jointVals)
                {
                    int tester = (int) jt;
                    Joint addJoint = new Joint();
                    SkeletonPoint sp = new SkeletonPoint();
                    String Xstring = lineStack.Dequeue();
                    sp.X = (float)Convert.ToDouble(Xstring);
                    sp.Y = (float)Convert.ToDouble(lineStack.Dequeue());
                    sp.Z = (float)Convert.ToDouble(lineStack.Dequeue());
                    addJoint.Position = sp;
                    curJoints[(int)jt] = addJoint;
                }
                SkeletonPoint pos = new SkeletonPoint();
                pos.X = (float)Convert.ToDouble(lineStack.Dequeue());
                pos.Y = (float)Convert.ToDouble(lineStack.Dequeue());
                pos.Z = (float)Convert.ToDouble(lineStack.Dequeue());
                dList.Add(new DanSkeleton(curJoints, pos));
            }
            return new Stride(dList);
        }

        private List<DanSkeleton> convertToDanFromSkel(List<Skeleton> rawSkeletonList){
            //transform this into DanSkeletons
            //take rawSkeleton list and transform to DanSkeletons - save as CapturedFrames
            var outList = new List<DanSkeleton>(rawSkeletonList.Count);
            foreach (Skeleton s in rawSkeletonList)
            {
                DanSkeleton d = new DanSkeleton(s);
                outList.Add(d);
            }
            return outList;
        }

        public List<DanSkeleton> rotateSkelList(List<DanSkeleton> skelList)
        {
            //TODO: rotate the DanSkel list that we got from initial list
            SkeletonPoint startPos = skelList[0].Position;
            SkeletonPoint endPos = skelList[skelList.Count -1].Position;  //TODO: change so that if no skel, no prob

            // Get a 2d Vector describing the start and end in terms of z and x
            Vector angleVector = new Vector(endPos.X - startPos.X, endPos.Z - startPos.Z);
            // Reference straight vector
            Vector idealVector = new Vector(1, 0);

            double angle = Vector.AngleBetween(angleVector, idealVector);

            //foreach (DanSkeleton ds in skelList)
            for(int i = 0; i < skelList.Count; i++)
            {
                DanSkeleton ds = skelList[i];
                ds.rotateJointsXZ(startPos.X, startPos.Z, angle);
            }
            //use first and last frames to determine movement direction
            //rotate entire skeleton to act as though movign along x-axis
            //return
            return null;
        }

        // Returns the difference in the position between the left foot and the right foot for a given skeleton
        private double footDifference(DanSkeleton d)
        {
            return d.Joints[JointType.FootLeft].Position.X - d.Joints[JointType.FootRight].Position.X;

        }

        private RelDir firstFootDown;
        private RelDir midFootDown;
        private RelDir lastFootDown;

        public void getStridePositions(){
            //use x-axis basis to determine foot-overlap period
            //set firstFrame and lastFrame variables (indexes to determien stride from private capturedFrames
            //iterate through skeleton frames to find first, second and third "crossing points"
            int crossingPointNum=0;
            //base it on left - right (x-axis based after rotation)
            bool truthTest;
            DanSkeleton skeletonZero = capturedFrames[0];
            truthTest=(footDifference(skeletonZero) < 0); // if lfoot behind rfoot then truthTest = true
            //TODO: Do we need to correct for jitter in this function?
            for(int i=0;i<capturedFrames.Count;i++){
                //if truthTest differs, then the relative positions of the points have changed
                if(truthTest!=(footDifference(capturedFrames[i]) < 0)){
                    truthTest=(!truthTest);
                    if(crossingPointNum==0){
                        this.firstFrame=i;
                        crossingPointNum++;
                        firstFootDown = getLowerFoot(capturedFrames[i]);
                    }else if(crossingPointNum==2){  //we are interested in the 3rd point to end a full-stride
                        this.lastFrame=i;
                        crossingPointNum++;
                        lastFootDown = getLowerFoot(capturedFrames[i]);
                        break;  //TODO: use a "break" statment? gasp! // WRONG: break's are awesome. you should take one now!
                    }else{
                        crossingPointNum++;
                        midFootDown = getLowerFoot(capturedFrames[i]);
                        this.midFrame = i;
                    }
                }
            }
            // Handle cases where we don't find a last frame
            if (this.lastFrame <= this.firstFrame) this.lastFrame = this.capturedFrames.Count-1;
            return;
        }

        private RelDir getLowerFoot(DanSkeleton skel)
        {
            if (skel.Joints[JointType.FootLeft].Position.Y < skel.Joints[JointType.FootRight].Position.Y)
                return RelDir.L;
            else
                return RelDir.R;
        }


        private DanSkeleton firstSkelInCycle
        {
            get
            {
                return capturedFrames[firstFrame];
            }
        }

        private DanSkeleton lastSkelInCycle
        {
            get
            {
                return capturedFrames[lastFrame];
            }
        }
        /*
         * TODO: DESCRIPTIVE ATTRIBTUES - write functions to calculate these and record them as class variables
         *       List is below
         */

        //percent of cycle in swing v. stance mode //TODO: need to find the location of all footfalls in cycle to know this

        //strideLength in meters 
        public double strideLengthMeters
        {
            get
            {
                return Utils3D.skelPointDist(firstSkelInCycle.Joints[JointType.FootRight].Position,
                    lastSkelInCycle.Joints[JointType.FootRight].Position);
            }
        }

        //strideLenght in time (number of frames)
        public int strideLengthFrames
        {
            get
            {
                return lastFrame - firstFrame;
            }
        }

        //velocity (combination of distance + frames)  note: learning alg should 
        //          be able to determine this implicitly, but the more info we give it the better  (can use to guess age!)
        public double strideMetersPerSecond
        {
            get
            {
                return (strideLengthMeters / (strideLengthFrames / 30.0)); //TODO: This assumes we never drop frames! 
                //IDEA: Could we record the wall clock time for all captured skeletons and use that? Or do we do well enough
                // that assuming 30 fps is ok?
            }
        }

        //step length (should be strideLength/2 but could calculate for right and left to see if there is a difference
        //TODO: need to know location of all footfalls

        //step length vs leg length (pg 43)
        
        //step width pg 41 (distance between feet between touchdown points) //TODO: Average? // Average over all frames, min, max
        //foot separation (max and min)

        public double widthBetweenFeetAvg
        {
            get
            {
                double widthSum = 0;
                for (int i = firstFrame; i < lastFrame; i++)
                {
                    DanSkeleton curSkel = capturedFrames[i];
                    widthSum += Math.Abs((curSkel.Joints[JointType.FootRight].Position.Z -
                        curSkel.Joints[JointType.FootLeft].Position.Z));
                }

                var outvar = (widthSum / (lastFrame - firstFrame));

                return outvar;
            }
        }
        
        //arm motion/arc
        // same things as with legs, do with arms

        private List<List<double>> getAllDistLists()
        {
            // for every pair of points, calculate the distance between them
            var allDists = new List<List<double>>();
            int numDists = 190; // Dan says this is right // IS 190 right?

            int lFramei = lastFrame < 0 ? capturedFrames.Count : lastFrame;
            for (int i = firstFrame; i < lastFrame + 1; i++)
            {
                DanSkeleton curSkel = capturedFrames[i];

                List<double> curSkelDists = new List<double>();
                for (int j = 0; j < (Enum.GetNames(typeof(JointType))).Length; j++)
                {
                    for (int k = j + 1; k < (Enum.GetNames(typeof(JointType))).Length; k++)
                    {
                        double jointDist = Utils3D.jointDist(curSkel.Joints[(JointType)j], curSkel.Joints[(JointType)k]);
                        curSkelDists.Add(jointDist);

                    }
                }
                allDists.Add(curSkelDists);
                var what = allDists.Count;
                var wha = curSkelDists.Count;
                if (curSkelDists.Count != numDists) throw new Exception("You are wrong, dan!");
            }
            return allDists;
        }
        public double[] distanceBetweenAllPointsMax
        {
            get
            {
                int numDists = 190; // Dan says this is right 
                var allDists = this.getAllDistLists();
                var outList = new List<double>();
                for (int i = 0; i < numDists; i++)
                {
                    double curMax = double.MinValue;
                    foreach (List<double> l in allDists)
                    {
                        curMax = Math.Max(curMax, l[i]);
                   
                    }
                    outList.Add(curMax);
                }
                return outList.ToArray();
            }
        }

        public double[] distanceBetweenAllPointsMin
        {
            get
            {
                int numDists = 190; // Dan says this is right 
                var allDists = this.getAllDistLists();
                var outList = new List<double>();
                for (int i = 0; i < numDists; i++)
                {
                    double curMin = double.MaxValue;
                    foreach (List<double> l in allDists)
                    {
                        curMin = Math.Min(curMin, l[i]);
                   
                    }
                    outList.Add(curMin);
                }
                return outList.ToArray();
            }
        }
        public double[] distanceBetweenAllPointsAvg

        {
            get
            {
                int numDists = 190; // Dan says this is right 
                var allDists = this.getAllDistLists();
                var outList = new List<double>();
                for (int i = 0; i < numDists; i++)
                {
                    double sumVal = 0;
                    foreach (List<double> l in allDists)
                    {
                        if (double.IsNaN(l[i]))
                        {
                            Console.WriteLine("crap");
                        }
                        sumVal += l[i];
                    }
                    outList.Add(sumVal / allDists.Count);
                }
                if (outList.Count < 100) throw new Exception("NOOOOOOOOOOOOOOOOOOOOOO");
                return outList.ToArray();
            }
        }
        //averages and standard deviations of all point positions
        //averages and standard deviations of all distances between points
        //averages and standard deviations of all angles between lines between points


        //max height off of ground (probably just max head height)
        public double maxHeadHeightMeters
        {
            get
            {
                return capturedFrames.Max(x => x.Position.Y);
            }
        }
        //min height from ground (prob just min head height)
        public double minHeadHeightMeters
        {
            get
            {
                return capturedFrames.Min(x => x.Position.Y);
            }
        }

        // Use vectors to get rotation
        // use min, max, average for all
        //pelvic functions
          //pelvic rotation pg 4
          //pelvic list  pg 4p
       
        //knee flexion?? pg 4 (might not be able to measure this)


        //other rotations
          //rotations of thorax and shoulders
          //rotations of thigh and leg
          //rotations in the ankle and foot
         
        //foot angle
        private double[] getAllFootAngles(RelDir foot)
        {
            var startEnum = foot == RelDir.L ? 13 : 17;

            var aList = new List<double>();

            var lframei = lastFrame <= firstFrame ? capturedFrames.Count : lastFrame;
            for (int i = firstFrame; i < lframei; i++)
            {
                DanSkeleton curSkel = capturedFrames[i];
                aList.Add(angleBetweenJointPairs(Tuple.Create(curSkel.Joints[(JointType) startEnum], curSkel.Joints[(JointType) startEnum+1]),
                                       Tuple.Create(curSkel.Joints[(JointType) startEnum + 1], curSkel.Joints[(JointType) startEnum+2])));
            }
            return aList.ToArray();
        }
        public double leftFootAngleAvg
        {
            get
            {
                double[] angles = getAllFootAngles(RelDir.L);
                return angles.Sum() / (lastFrame - firstFrame);
            }
        }

        public double rightFootAngleAvg
        {
            get
            {
                double[] angles = getAllFootAngles(RelDir.R);
                return angles.Sum() / (lastFrame - firstFrame);
            }
        }

        public double leftFootAngleMin
        {
            get
            {
                double[] angles = getAllFootAngles(RelDir.L);
                return angles.Min();
            }
        }

        public double rightFootAngleMin
        {
            get
            {
                double[] angles = getAllFootAngles(RelDir.R);
                return angles.Min();
            }
        }


        public double leftFootAngleMax
        {
            get
            {
                double[] angles = getAllFootAngles(RelDir.L);
                return angles.Max();
            }
        }
        public double rightFootAngleMax
        {
            get
            {
                double[] angles = getAllFootAngles(RelDir.R);
                return angles.Max();
            }
        }
        //TODO: When should we get the angle? min angle, max angle? average? 

        private double angleBetweenJointPairs(Tuple<Joint,Joint> pair1, Tuple<Joint, Joint> pair2) //Joint[] pair1, Joint[] pair2)
        {
            if (!pair1.Item2.Position.Equals(pair2.Item1.Position))
            {
                throw new ArgumentException("Joint Pairs must share a common base!");
            }
            Vector3D v1 = jointPairToVector3D(pair1);
            Vector3D v2 = jointPairToVector3D(pair2);
            return Vector3D.AngleBetween(v1, v2);
        }

        private Vector3D jointPairToVector3D(Tuple<Joint, Joint> pair)
        {
            return new Vector3D(
                pair.Item2.Position.X - pair.Item1.Position.X,
                pair.Item2.Position.Y - pair.Item1.Position.Y, 
                pair.Item2.Position.Z - pair.Item1.Position.Z);
        }
        
        //max foot elevation
        public double maxRFootHeight
        {
            get
            {
                return capturedFrames.Max(x => x.Joints[JointType.FootRight].Position.Y);
            }
        }
        public double maxLFootHeight
        {
            get
            {
                return capturedFrames.Max(x => x.Joints[JointType.FootLeft].Position.Y);
            }
        }
        public double maxFootHeightOverall
        {
            get
            {
                return Math.Max(this.maxLFootHeight, this.maxRFootHeight);
            }
        }
        //lateral displacement of body pg 9
            //increased with feet farther apart, decreased closer together


        // Check for stooped-ness
        //center of mass measurements? see pg 3 
        
        //"straightness"

        //...

        //print to file in format that would be used by machine learning svm/neural net/descriptive statistics, whatever
        public void printToFile(){
            return;
        }


        /// <summary>
        /// Given a framenumber and viewport, render the skeleton that corresponds to that frame in the viewport
        /// </summary>
        /// <param name="frameNumber"></param>
        /// <param name="skelViewport"></param>
        public void drawFrameToViewport(int frameNumber, System.Windows.Controls.Viewport3D skelViewport)
        {
            Model3DGroup skel3dGroup = new Model3DGroup();
            DanSkeleton renderSkel = capturedFrames[frameNumber];
            foreach(Joint j in renderSkel.Joints) {
                // Create a cube for each joint
                ModelVisual3D curJointCube;
                // Color the skeletons based on if they are step skels
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
                skelViewport.Children.Add(curJointCube);
            }
            // Put the camera in a place where it can see the whole stride
            skelViewport.Camera = new PerspectiveCamera(new Point3D(this.getStrideMidX(), 0, -3), new Vector3D(0,0,1), new Vector3D(0, 1, 0), 75);
            // Add a light so that colors are visible
            skelViewport.Children.Add(new ModelVisual3D() { Content = new AmbientLight(Colors.White) });
        }

        // Return the middle coordinate of a recorded stride set
        private double getStrideMidX()
        {
            return (this.capturedFrames[0].Position.X + this.capturedFrames[this.capturedFrames.Count - 1].Position.X) / 2;
        }


        //private void renderA
        public int numFrames { get; set; }

    }
}
