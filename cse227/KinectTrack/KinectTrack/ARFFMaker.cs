using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KinectTrack
{
    class ARFFMaker
    {
        List<String> pNames;

        public ARFFMaker()
        {
            pNames = new List<string>();
        }

        public void addProp(String name)
        {
            if(name.StartsWith("distance"))
            {
                for(int i = 0; i < 190; i++) 
                {
                    pNames.Add(name + "|" + i);
                }
            }
            else 
            {
                pNames.Add(name);
            }
        }

        public String getARFF(List<Stride> strideList, String relationName)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("@relation " + relationName);
            sb.Append("\n");
            foreach (String name in pNames)
            {
                sb.Append("@attribute " + name + " numeric");
                sb.Append("\n");
            }

            sb.Append("@data \n");

            foreach (Stride s in strideList)
            {
                foreach (String name in pNames)
                {
                    if (name.StartsWith("distance"))
                    {
                        String[] splitName = name.Split('|');
                        switch (splitName[0])
                        {
                            case "distanceBetweenAllPointsAvg":
                                sb.Append(s.distanceBetweenAllPointsAvg[Convert.ToInt32(splitName[1])]);
                                sb.Append(", ");
                                break;
                            default:
                                break;
                        }


                    }
                    else
                    {
                        var value = s.GetType().GetProperty(name).GetValue(s, null);
                        sb.Append(value);
                        sb.Append(", ");
                    }
                }
                sb.Append("\n");
            }

            return sb.ToString();
        }

    }
}
