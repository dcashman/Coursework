using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Media3D;
using Microsoft.Kinect;
using Petzold.Media3D;
using System.Drawing;
using System.Windows.Media;

namespace KinectTrack
{
    // Utility functions for 3D stuff
    class Utils3D
    {
        // Create a TranslateTransform3d corresponding to a joint * a multiplier
        public static TranslateTransform3D getJointPosTransform(Joint j, double multiplier)
        {
            return new TranslateTransform3D(j.Position.X * multiplier, j.Position.Y * multiplier, j.Position.Z * multiplier);
        }
        //Create a TranslateTransform3d corresponding to a joint 
        public static TranslateTransform3D getJointPosTransform(Joint j)
        {
            return getJointPosTransform(j, 1);
        }

        public static double jointDist(Joint j1, Joint j2)
        {
            return skelPointDist(j1.Position, j2.Position);
        }

        // Returns the distance in 3d space between 2 skel points
        public static double skelPointDist(SkeletonPoint s1, SkeletonPoint s2)
        {
            double xterm = s1.X - s2.X;
            double yterm = s1.Y - s2.Y;
            double zterm = s1.Z - s2.Z;
            return Math.Sqrt((xterm * xterm) + (yterm * yterm) + (zterm * zterm));
        }

        // Returns a cube model. Any color you like!
        public static ModelVisual3D getCube(System.Windows.Media.Color cubeColor)
        {
            BoxMesh b = new BoxMesh();
            Material material = new DiffuseMaterial(
                new SolidColorBrush(cubeColor));
            GeometryModel3D boxModel = new GeometryModel3D(
                b.Geometry, material);
            ModelVisual3D model = new ModelVisual3D();
            model.Content = boxModel;

            return model;
        }
        // Returns a cube model. Red by default
        public static ModelVisual3D getCube()
        {
            return getCube(Colors.Red);
        }

    }
}
