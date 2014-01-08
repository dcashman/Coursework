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

/*! @file
 *  Utilities for SMC tests. 
 */
#ifndef SMC_UTIL_H
#define SMC_UTIL_H

#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "runnable.h"
using namespace std;

#if !defined(TARGET_IA32) && !defined(TARGET_IA32E)
#error Unknown target architecture. Must be IA-32 or Intel64.
#endif

/*!
 * Convert a pointer to <SRC> type into a pointer to <DST> type.
 * Allows any combination of data/function types.
 */
template <typename DST, typename SRC> DST * CastPtr(SRC * src)
{
    union CAST
    {
        DST * dstPtr;
        SRC * srcPtr;
    } cast;
    cast.srcPtr = src;
    return cast.dstPtr;
}

/*!
 * Type of a position-independent function that copy its name into the specified buffer.
 */
typedef void FOO_BAR_T(char *); 
typedef FOO_BAR_T *FOO_BAR_PTR; 

/*!
 * Implementation of the PI_FUNC for FOO_BAR_T functions.
 */
class FOO_BAR_FUNC : public PI_FUNC
{
public:

    FOO_BAR_FUNC(FOO_BAR_PTR func, size_t size) : m_func(func), 
                                                  m_size(size),
                                                  m_throwException(false),
                                                  m_status(STATUS_OK)
    {
        m_func(m_name);
    }

    // Get/set the exception mode for the Execute() function:
    // TRUE  - execute the function with arguments that cause an exception in the function
    // FALSE - execute the function without exceptions
    bool GetExceptionMode() const {return m_throwException;}
    void SetExceptionMode(bool throwException) {m_throwException = throwException;}

    FUNC_OBJ & Execute()
    {
        if (m_throwException)
        {
            ExecuteThrow();
        }
        else
        {
            ExecuteNoThrow();
        }
        return *this;
    }

    PI_FUNC & Copy(void * buffer)
    {
        memcpy(buffer, CastPtr<void>(m_func) , m_size);
        m_func = CastPtr<FOO_BAR_T>(buffer);
        return *this;
    }

    string ErrorMessage() const
    {
        switch (m_status)
        {
        case STATUS_OK:
            return "Success";
        case STATUS_HANDLED_EXCEPTION:
            return "Exception handled successfully";
        case STATUS_UNEXPECTED_RESULT:
            return "Unexpected result";
        case STATUS_UNEXPECTED_EXCEPTION:
           return "Unexpected exception";
        default:
            return "Unknown status";
        }
    }

    bool Status() const {return ((m_status == STATUS_OK) || (m_status == STATUS_HANDLED_EXCEPTION));}
    string Name() const {return m_name;}
    void * Start() const {return CastPtr<void>(m_func);}
    size_t Size() const {return m_size;}
    FUNC_OBJ * Clone() const {return new FOO_BAR_FUNC(*this);}

protected:

    FUNC_OBJ & HandleException(void * exceptIp)
    {
        char * start = CastPtr<char>(m_func);
        char * ip = CastPtr<char>(exceptIp);
        if (m_throwException && (ip >= start) && (ip < start + m_size))
        {
            ExecuteNoThrow();
            if (m_status == STATUS_OK)
            {
                m_status = STATUS_HANDLED_EXCEPTION;
            }
        }
        else
        {
            m_status =  STATUS_UNEXPECTED_EXCEPTION;
        }
        return *this;
    }

private:

    FOO_BAR_PTR m_func;
    size_t m_size;
    bool m_throwException;
    enum STATUS
    {
        STATUS_OK,
        STATUS_HANDLED_EXCEPTION,
        STATUS_UNEXPECTED_RESULT,
        STATUS_UNEXPECTED_EXCEPTION
    } m_status;
    char m_name[16];

    void ExecuteThrow()
    {
        m_func(0);
        m_status = STATUS_UNEXPECTED_RESULT; // should never get here
    }

    void ExecuteNoThrow()
    {
        char result[16];
        m_func(result);
        m_status = ((strcmp(result, m_name) == 0)? STATUS_OK : STATUS_UNEXPECTED_RESULT);
    }

};

class FOO_FUNC : public FOO_BAR_FUNC
{
public:  FOO_FUNC();
};

class BAR_FUNC : public FOO_BAR_FUNC
{
public:  BAR_FUNC();
};

#endif //SMC_UTIL_H

/* ===================================================================== */
/* eof */
/* ===================================================================== */
