/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2008 Intel Corporation 
All rights reserved. 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
#include <stdio.h>

#if !defined(TARGET_WINDOWS)

#include <unistd.h>
#include <sys/mman.h>
#if !defined(TARGET_MAC)
#include <malloc.h>
#endif

void * const TargetPrefix = (void *)0xb0000000;
void * const SwizzlePrefix = (void *)0xb1000000;
#define EXPORT_SYM

#else //defined(TARGET_WINDOWS)

#include <windows.h>
void * const TargetPrefix =  (void *)0x1000000;
void * const SwizzlePrefix = (void *)0x1100000;
// declare all functions as exported so pin can find them,
// must be all functions since only way to find end of one function is the begining of the next
// Another way is to compile application with debug info (Zi) - pdb file, but that causes probelms
// in the running of the script 
#define EXPORT_SYM __declspec( dllexport ) 

#endif

#if defined(__cplusplus)
extern "C"
#endif
EXPORT_SYM
int Swizzled(void *ptr)
{
    return ((((size_t)ptr) & ((size_t)SwizzlePrefix))==(size_t)SwizzlePrefix);
}

#if defined(__cplusplus)
extern "C"
#endif
EXPORT_SYM
void MyFree(void * p)
{
	if (Swizzled(p)) {
		fprintf (stderr, "Error MyFree got swizzled %x\n", p);
        fflush (stderr);
	}
}

#if defined(__cplusplus)
extern "C"
#endif
EXPORT_SYM
void * MyAlloc(void *ptr)
{
    return ptr; // returned value will be swizzled
}



EXPORT_SYM
void * mmemcpy(void * to, const void * from, size_t n, void ** toout, void **fromout)
{
void * d0, *d1, *d2;
#if defined( __GNUC__)
__asm__ __volatile__(
	"cld; rep ; movsl; testb $2,%b4; je 1f; movsw;"
	"1:testb $1,%b4;je 2f;"
	"movsb;"
	"2:"
	: "=&c" (d0), "=&D" (d1), "=&S" (d2):"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from) : "memory");
  *toout = d1;
  *fromout = d2;
#else
  extern EXPORT_SYM void mmemcpy_ia32(void * to, const void * from, size_t n, void ** toout, void **fromout);
  mmemcpy_ia32(to, from, n,toout, fromout);

#endif
return (to);
}

EXPORT_SYM
void memindex(void * ad)
{
    void * ptr = 0;
#if defined( __GNUC__)    
    __asm__ __volatile__("movl $0,(%0,%1,1)" :: "r"(ptr),"r"(ad));
#else
    extern EXPORT_SYM void memindex_ia32(void * ad, void *ptr);
    memindex_ia32(ad, ptr);

#endif
}

EXPORT_SYM
void GetMem (void **mm)
{

#if defined(TARGET_MAC)
    *mm = mmap(TargetPrefix, getpagesize(), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);
#elif defined(TARGET_WINDOWS)
    // mmap with MAP_FIXED and TargetPrefix==0xb0000000 does not succeed on windows
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    *mm = VirtualAlloc((LPVOID)TargetPrefix, sysInfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE);
#else
    *mm = mmap(TargetPrefix, getpagesize(), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
#endif
}

int n = 8;

EXPORT_SYM
int main()
{
    char buffer2[20];
    char buffer3[20];
    void * al;
    void *toout,*fromout;
    void * mm = NULL;

    GetMem (&mm);
    if (mm != TargetPrefix)
    {
        fprintf (stderr, "failed to get memory at %x\n", TargetPrefix);
        return (-1);
    }
    
    al = MyAlloc(mm); // cause the swizzle
    if (!Swizzled (al)) {
        fprintf (stderr,"Error0 al not swizzled\n");
        fflush (stderr);
    }

    
    ((char*)al)[0] = 1;
    
    
    buffer2[0] = 2;
    buffer3[0] = 3;

    mmemcpy(buffer2, al, n, &toout, &fromout);
    
    fprintf(stderr, "al[0] %d buffer2[0] %d  toout - to %x fromout - from %x\n", 
           ((char*)al)[0], buffer2[0], 
           (char*)toout - (char*)buffer2, (char*)fromout- (char*)al);
    fflush (stderr);
    if (!Swizzled (al)) {
        fprintf (stderr,"Error1 al not swizzled\n");
        fflush (stderr);
    }
    if (!Swizzled (fromout)) {
        fprintf (stderr,"Error2 fromout not swizzled\n");
        fflush (stderr);
    }

    mmemcpy(al, buffer3, n, &toout, &fromout);
    if (!Swizzled (al)) {
        fprintf (stderr,"Error3 al not swizzled\n");
        fflush (stderr);
    }
    if (!Swizzled (toout)) {
        fprintf (stderr,"Error4 toout not swizzled\n");
        fflush (stderr);
    }
    fprintf(stderr, "al[0] %d  buffer3[0] %d  toout - to %x fromout - from %x\n",((char*)al)[0], 
            buffer3[0], (char*)toout-(char *)al, (char*)fromout - (char*)buffer3);
    fflush (stderr);
    
    memindex(al);
    
    MyFree(al);
    
    return 0;
}
    

