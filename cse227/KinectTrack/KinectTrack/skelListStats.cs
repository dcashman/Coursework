using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Kinect;

namespace KinectTrack
{
    class skelListStats
    {
        public float maxX, maxY, maxZ, minX, minY, minZ;
        private int numKlusters;  //try starting with 128 clusters?
        private SkeletonPoint[,] klusterz;  //ignoring skeleton position
        private SkeletonPoint[,] oldKlusterz;  //use this to calculate difference between skeleton iterations
        private List<List<int>> discreteFrames;  //a list of each "walking" sequence represented as a sequence of discrete klusters
        private List<List<Skeleton>> closet;
        private String klusterFile;//="klusters.txt";
        private String trainingAsKlusters;//="trainingAsKlusters.txt";
        private String classifyFile;// = "klustersToBeClassified.txt";
        private Random rando = new Random();
        private const double KlusterTrainingValue = .001;  //TODO: alter this?  this determines when clusters stop moving

        public skelListStats(List<List<Skeleton>> closet, Boolean training, int numKlusters, String klusterFile, String trainingAsKlustersFile)
        {
            this.closet = closet;  //should work since it won't be garbage-collected with a reference...  TODO: right?
            this.klusterFile = klusterFile + numKlusters + ".txt";
            this.trainingAsKlusters = trainingAsKlustersFile + numKlusters + ".txt";
            this.classifyFile = "Classified" + numKlusters + ".txt";
            this.numKlusters = numKlusters;
            this.klusterz= new SkeletonPoint[numKlusters,20];  //ignoring skeleton position
            this.oldKlusterz = new SkeletonPoint[numKlusters, 20];  //use this to calculate difference between skeleton iterations
            if (training)
            {           
                calculateExtrema();  //find the bounds for the random init of the clusters
                initKlusters();      //initialize clusters to starting value
                trainKlusters();     //use k-means clustering to get final cluster values
                printKlusters(this.klusterFile);     //print the trained clusters to a file so they can be used later for classifying
                assignAllToKlusters();
                //printDataAsKlusters(this.trainingAsKlusters);  //TODO: print the actual data as represented by clusters
            }
            else
            {   //not training just need to turn data into clusters
                initKlusters(this.klusterFile);
                assignAllToKlusters();
                printDataAsKlusters(this.trainingAsKlusters);
            }
        }

        /*
         * calculateExtrema - calculate the max and min for the three dimensions for use in creating the K clusters
         */
        private void calculateExtrema()
        {
            //assume that 0 will be within the range of values
            maxX = 0;
            minX = 0;
            maxY = 0;
            minY = 0;
            maxZ = 0;
            minZ = 0;
            for (int i = 0; i < closet.Count; i++)
            {
                List<Skeleton> currentList = closet[i];
                for (int k = 0; k < currentList.Count; k++)
                {
                    Skeleton currentFrame = currentList[k];
                    SkeletonPoint currentPos;
                    for (int j = 0; j < 20; j++)
                    {  //iterate through each joint
                        currentPos = currentFrame.Joints[(JointType)j].Position;
                        if (currentPos.X > maxX)
                        {
                            maxX = currentPos.X;
                        }
                        else if (currentPos.X < minX)
                        {
                            minX = currentPos.X;
                        }
                        if (currentPos.Y > maxY)
                        {
                            maxY = currentPos.Y;
                        }
                        else if (currentPos.Y < minY)
                        {
                            minY = currentPos.Y;
                        }
                        if (currentPos.Z > maxZ)
                        {
                            maxZ = currentPos.Z;
                        }
                        else if (currentPos.Z < minZ)
                        {
                            minZ = currentPos.Z;
                        }

                    }
                }
            }
            return;
        }
        /*
         * initialize Klusters - set the desired number of clusters to their randomized values
         */
        public void initKlusters()
        {
            for (int i = 0; i < numKlusters; i++)
            {
                for (int j = 0; j < 20; j++)
                {
                    klusterz[i, j].X = (float)getRand(minX, maxX);
                    klusterz[i, j].Y = (float)getRand(minY, maxY);
                    klusterz[i, j].Z = (float)getRand(minZ, maxZ);
                }
            }
            return;
        }

        /*
         * initKlsuters(String fileName) - get klusters from a file (in the case where we are classifying and are using klusters determined from training
         */
        public void initKlusters(String fileName)
        {
            //grab the clusters from the given file of clusters
            //TODO: assert numClusters from file matches number desired here
            int lineCount = 0;
            List<List<SkeletonPoint>> klustersFromFile = new List<List<SkeletonPoint>>();

            String[] lines = System.IO.File.ReadAllLines(fileName);
            if (numKlusters != lines.Length-1)    //TODO: DEBUG: make sure this -1 thing is right
            {
                return;  //TODO: get rid of this maybe?
            }
            //foreach (String line in lines)
            for(int line = 0; line < (lines.Length - 1); line++)  // length -1 to get rid of ""
            {
               
                klustersFromFile.Add(new List<SkeletonPoint>());
                String[] splitLine = lines[line].Split(new Char[] { '\t' });
                Queue<String> lineStack = new Queue<string>(splitLine);
                // Joints are stored in xyz order in the order they are defined in JointType
                
                for(int i=0; i < 20; i++)
                {
                    SkeletonPoint sp = new SkeletonPoint();
                    String Xstring = lineStack.Dequeue();
                    sp.X = (float)Convert.ToDouble(Xstring);
                    sp.Y = (float)Convert.ToDouble(lineStack.Dequeue());
                    sp.Z = (float)Convert.ToDouble(lineStack.Dequeue());
                    //klustersFromFile[lineCount][i] = sp;
                    klusterz[lineCount,i] = sp;  //TODO: assert that it's within boundary
                }
                lineCount++;
                if (lineCount >= numKlusters)
                {
                    return;  //TODO: remove this and add an assert above
                }
            }

            return;
        }

        /*
         * train clusters - uses k-means clustering to train final set of clusters
         */
        public void trainKlusters(){
            double movement =0;
            
            do{
                //create a bucket for each cluster, it will contain a list of all skeletons mapped to that cluster
                List<Skeleton>[] buckets = new List<Skeleton>[numKlusters]; 
                for(int i = 0; i < numKlusters; i++){
                    buckets[i]=new List<Skeleton>();
                }
                //assign every frame to a cluster (cluster values shown in "discreteFrames"
                assignAllToKlusters();
                //add each frame to the corresponding cluster-bucket
                //for each training sequence
                for(int i = 0; i < discreteFrames.Count; i++){
                    //for each frame in each training sequence
                    for( int j = 0; j < discreteFrames[i].Count; j++){
                         buckets[discreteFrames[i][j]].Add(closet[i][j]);
                    }
                }
                //go through each cluster to assign old cluster and new cluster
                for(int i = 0; i < numKlusters; i++){
                    //add all of the coordinates up from each frame assigned to this cluster
                    int numInKluster=buckets[i].Count;//check this value
                    if (numInKluster == null || numInKluster < 1)
                    {
                        continue;
                    }
                    SkeletonPoint[] tempFrame = new SkeletonPoint[20];
                    for(int j = 0; j < numInKluster; j++){
                        Skeleton currentSkel = buckets[i][j];
                        for(int k = 0; k < 20; k++){
                            tempFrame[k].X+=currentSkel.Joints[(JointType)k].Position.X;
                            tempFrame[k].Y+=currentSkel.Joints[(JointType)k].Position.Y;
                            tempFrame[k].Z+=currentSkel.Joints[(JointType)k].Position.Z;
                        }
                    }
                    for(int k = 0; k < 20; k++){
                        //save old klusterz values
                        oldKlusterz[i,k].X=klusterz[i,k].X;  //TODO: cast to double?
                        oldKlusterz[i,k].Y=klusterz[i,k].Y;
                        oldKlusterz[i,k].Z=klusterz[i,k].Z;
                        //calculate new ones
                        klusterz[i, k].X = tempFrame[k].X / numInKluster;  //TODO: cast to double?
                        klusterz[i, k].Y = tempFrame[k].Y / numInKluster;
                        klusterz[i, k].Z = tempFrame[k].Z / numInKluster;
                    }
                    //get the difference between them
                    movement = Math.Max(movement,(klusterDistFromKluster(i, i)));
                    //TODO: add debug statment here?
                }
            }while(movement < KlusterTrainingValue);
            return;
        }


        private double getRand(double min, double max)
        {
            return rando.NextDouble() * (max - min) + min;
        }

        public void printKlustersWithLabels(String fileName)
        {

            String output = "";  //where all the goodies will go
            for (int i = 0; i < numKlusters; i++)
            {
                output += "Cluster " + i + ":\n";
                for (int j = 0; j < 20; j++)
                {
                    output += "X: " + klusterz[i, j].X + "\t";
                    output += "Y: " + klusterz[i, j].Y + "\t";
                    output += "Z: " + klusterz[i, j].Z + "\t";
                }
                output += "\n";
            }

            using (System.IO.StreamWriter file = new System.IO.StreamWriter(fileName, false))  
            {
                file.WriteLine(output);
            }
            return;
        }


        /*
         * printKlusters - normal, without labels
         */
        public void printKlusters(String fileName)
        {

            String output = "";  //where all the goodies will go
            for (int i = 0; i < numKlusters; i++)
            {
                for (int j = 0; j < 20; j++)
                {
                    output +=klusterz[i, j].X + "\t";
                    output +=klusterz[i, j].Y + "\t";
                    output +=klusterz[i, j].Z + "\t";
                }
                output += "\n";
            }

            using (System.IO.StreamWriter file = new System.IO.StreamWriter(fileName, false))
            {
                file.WriteLine(output);
            }
            return;
        }


        public void printDataAsKlusters(string fileName)
        {

                String output = "";  //where all the goodies will go
                //for each training sequence
                for (int i = 0; i < discreteFrames.Count; i++)
                {
                    //print each state
                    for (int j = 0; j < discreteFrames[i].Count; j++)
                    {
                        output += discreteFrames[i][j] + "\t";
                    }
                    output += "\n";
                }
                //output += "###"+note+"\n";//### will denote the note
                using (System.IO.StreamWriter file = new System.IO.StreamWriter(fileName, false))  //TDO: not sure what the @ does
                {
                    file.WriteLine(output);
                }
                return;
        }




        /*
         * assignAllToKlusters() - assigns the given list of list of skeletons to a list of list of ints representing states
         *      - uses already initialized klusters
         */
        private void assignAllToKlusters()
        {
            discreteFrames = new List<List<int>>();
            for (int i = 0; i < closet.Count; i++)
            {  //for every frame sequence (each training sample)
                discreteFrames.Add(new List<int>());
                for (int j = 0; j < closet[i].Count; j++)
                {
                    discreteFrames[i].Add(frameToKluster(closet[i][j]));   //TODO: check change from discreteFrames[i][j] did not ruin
                }
            }
            return;
        }


        /*
         * frameToKluster - takes a frame and returns the cluster it maps to
         */
        public int frameToKluster(Skeleton skelly)
        {
            int assignedKluster=0;
            double minimum = frameDistFromKluster(0, skelly);
            //iterate over all klusters and return the one with the minimum error
            for (int i = 0; i < numKlusters; i++)
            {
                double temp = frameDistFromKluster(i, skelly);
                if (temp < minimum)
                {
                    minimum = temp;
                    assignedKluster = i;
                }
            }
            //TODO: assert that value is an actual kluster  (not -1) 
            return assignedKluster;
        }

        /*
         * frameDistFromKluster - calculates the difference between the given skeleton and a cluster represented as 
         *      (Xi - Ci)^2 where Xi is a value from the current frame and Ci is the corresponding value for the cluster
         */
        public double frameDistFromKluster(int klusterNum, Skeleton frame)
        {
            //TODO: assert klusterz has been initialized and frame != null
            double sum=0.0;
            for (int i = 0; i < 20; i++)
            {
                sum += Math.Pow((klusterz[klusterNum, i].X - frame.Joints[(JointType)i].Position.X), 2);
                sum += Math.Pow((klusterz[klusterNum, i].Y - frame.Joints[(JointType)i].Position.Y), 2);
                sum += Math.Pow((klusterz[klusterNum, i].Z - frame.Joints[(JointType)i].Position.Z), 2);
            }
            return sum;
        }

        /*
         * klusterDistFromKluster - calculates the difference between the given kluster and a cluster represented as 
         *      (Xi - Ci)^2 where Xi is a value from the current frame and Ci is the corresponding value for the cluster
         */
        public double klusterDistFromKluster(int klusterNum, int oldKlusterNum)
        {
            //TODO: assert klusterz has been initialized and frame != null
            double sum=0.0;
            for (int i = 0; i < 20; i++)
            {
                sum += Math.Pow((klusterz[klusterNum, i].X - oldKlusterz[klusterNum, i].X), 2);
                sum += Math.Pow((klusterz[klusterNum, i].Y - oldKlusterz[klusterNum, i].Y), 2);
                sum += Math.Pow((klusterz[klusterNum, i].Z - oldKlusterz[klusterNum, i].Z), 2);
            }
            return sum;
        }
    }
}
