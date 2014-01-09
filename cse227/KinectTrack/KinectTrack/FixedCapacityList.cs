using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace KinectTrack
{
    class FixedCapacityList<T> where T: IConvertible
    {
        private LinkedList<T> backingList;
        private int capacity;

        public FixedCapacityList(int capacity)
        {
            this.capacity = capacity;
            this.backingList = new LinkedList<T>();
        }

        public void addElement(T el)
        {
            if (backingList.Count == capacity)
            {
                backingList.RemoveLast();
            }
            backingList.AddFirst(el);
        }

        public Double average()
        {
            return backingList.Average<T>(e => Convert.ToDouble(e));
        }
    }
}
