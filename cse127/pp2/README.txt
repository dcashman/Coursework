The code here is a heapspray attack on a specially vulnerable "Iceweasel" browser.


[Overview]
Our spray works very much like the spray mentioned in the paper. We construct a
large nopsled and a concatenate with a copy of the shellcode that has been
converted to the %uxxxx format. This combined blob of stuff is then written to
many random positions in the heap through the use of a JavaScript Array object.
By doing it this way, we ensure that we retain references to all of our sprayed
objects and the javaScript GC shouldn't try and collect them.

[Details]
We calculate the length of our nop-sled so that the total length of our sprayed
object is 65536 bytes, which we empirically determined was a good balance
between creating a large enough "landing pad" for pwnMe and being small enough
that it can fit into lots of places. Unlike the example in the NOZZLE paper, we
do not do all of our spraying at once, as we found this to result in a lot of
noticable slow-down of the browser. Instead, we use a function that gets called
by a javascript timer to do our spray incrementally. In this way, there is still
some slow down, but it is intermittent and hopefully less noticeable to the
user. Currently we wait 500ms between each spray and spray 250 objects at a
time. These values were through experimentation and there may very well be
better settings for these, but as is they seem to work fairly well. In total, we
spray a total of 3250 objects on to the heap. Again, this value was determined
experimentally (it seemed to be a good balance between segfaulting because we
had not sprayed enough and getting killed by the linux OOM-killer because we had
sprayed too much). Once all the strings have been sprayed onto the heap, we call
pwnMe with an address of 0x9a0001050, which we found by spraying the iceweasel
heap and then breaking in gdb once our spray had completed. From here, we were
able to discover the pid of the iceweasel process and examine the memory map in
/proc/{id}/smaps. Browsing through this file, we looked for any region of memory
without an associated library/resource that looked like it could be large enough
to hold the iceweasel heap. After repeating this several times, we discovered
that the area in memory from about 0x9xxxxxxx to 0xaxxxxxxx usually looked like
it could be big enough to contain our heap. This exact address is somewhat
random, but we were able to examine several times in GDB and determined that it
often pointed to a NOP-sled. 

Finally, we display some fun pictures to keep the user distracted while we are
pwning his browser/system.
