
/*
   ----------------------------------------------------------------

   Notice that the following BSD-style license applies to this one
   file (flayer.h) only.  The rest of Valgrind is licensed under the
   terms of the GNU General Public License, version 2, unless
   otherwise indicated.  See the COPYING file in the source
   distribution for details.

   ----------------------------------------------------------------

   This file is part of Flayer, a heavyweight Valgrind tool for
   tracking marked/tainted data through memory.

   Copyright (C) 2006,2007 Will Drewry <redpig@dataspill.org>
   Some portions copyright (C) 2007 Google Inc.

   Based heavily on MemCheck by jseward@acm.org
   MemCheck: Copyright (C) 2000-2007 Julian Seward
   jseward@acm.org

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. The origin of this software must not be misrepresented; you must 
      not claim that you wrote the original software.  If you use this 
      software in a product, an acknowledgment in the product 
      documentation would be appreciated but is not required.

   3. Altered source versions must be plainly marked as such, and must
      not be misrepresented as being the original software.

   4. The name of the author may not be used to endorse or promote 
      products derived from this software without specific prior written 
      permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   ----------------------------------------------------------------

   Notice that the above BSD-style license applies to this one file
   (flayer.h) only.  The entire rest of Valgrind is licensed under
   the terms of the GNU General Public License, version 2.  See the
   COPYING file in the source distribution for details.

   ---------------------------------------------------------------- 
*/


#ifndef __FLAYER_H
#define __FLAYER_H


/* This file is for inclusion into client (your!) code.

   You can use these macros to manipulate and query memory permissions
   inside your own programs.

   See comment near the top of valgrind.h on how to use them.
*/

#ifndef __VALGRIND_H
#include "valgrind.h"
#endif

/* !! ABIWARNING !! ABIWARNING !! ABIWARNING !! ABIWARNING !! 
   This enum comprises an ABI exported by Valgrind to programs
   which use client requests.  DO NOT CHANGE THE ORDER OF THESE
   ENTRIES, NOR DELETE ANY -- add new ones at the end. */
typedef
   enum { 
      VG_USERREQ__MAKE_MEM_NOACCESS = VG_USERREQ_TOOL_BASE('F','L'),
      VG_USERREQ__MAKE_MEM_TAINTED,
      VG_USERREQ__MAKE_MEM_UNTAINTED,
      VG_USERREQ__DISCARD,
      VG_USERREQ__CHECK_MEM_IS_ADDRESSABLE,
      VG_USERREQ__CHECK_MEM_IS_UNTAINTED,
      VG_USERREQ__DO_LEAK_CHECK,
      VG_USERREQ__COUNT_LEAKS,

      VG_USERREQ__GET_VBITS,
      VG_USERREQ__SET_VBITS,

      VG_USERREQ__CREATE_BLOCK,

      VG_USERREQ__MAKE_MEM_UNTAINTED_IF_ADDRESSABLE,

      /* This is just for flayer's internal use - don't use it */
      _VG_USERREQ__FLAYER_RECORD_OVERLAP_ERROR 
         = VG_USERREQ_TOOL_BASE('M','C') + 256
   } Vg_MemCheckClientRequest;



/* Client-code macros to manipulate the state of memory. */

/* Mark memory at _qzz_addr as unaddressable for _qzz_len bytes. */
#define VALGRIND_MAKE_MEM_NOACCESS(_qzz_addr,_qzz_len)           \
   (__extension__({unsigned int _qzz_res;                        \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0 /* default return */, \
                            VG_USERREQ__MAKE_MEM_NOACCESS,       \
                            _qzz_addr, _qzz_len, 0, 0, 0);       \
    _qzz_res;                                                    \
   }))
      
/* Similarly, mark memory at _qzz_addr as addressable but undefined
   for _qzz_len bytes. */
#define VALGRIND_MAKE_MEM_TAINTED(_qzz_addr,_qzz_len)          \
   (__extension__({unsigned int _qzz_res;                        \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0 /* default return */, \
                            VG_USERREQ__MAKE_MEM_TAINTED,      \
                            _qzz_addr, _qzz_len, 0, 0, 0);       \
    _qzz_res;                                                    \
   }))

/* Similarly, mark memory at _qzz_addr as addressable and defined
   for _qzz_len bytes. */
#define VALGRIND_MAKE_MEM_UNTAINTED(_qzz_addr,_qzz_len)            \
   (__extension__({unsigned int _qzz_res;                        \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0 /* default return */, \
                            VG_USERREQ__MAKE_MEM_UNTAINTED,        \
                            _qzz_addr, _qzz_len, 0, 0, 0);       \
    _qzz_res;                                                    \
   }))

/* This is the old name for VALGRIND_MAKE_MEM_NOACCESS.  Deprecated. */
#define VALGRIND_MAKE_NOACCESS(_qzz_addr,_qzz_len)               \
   VALGRIND_MAKE_MEM_NOACCESS(_qzz_addr,_qzz_len)

/* This is the old name for VALGRIND_MAKE_MEM_TAINTED.  Deprecated. */
#define VALGRIND_MAKE_WRITABLE(_qzz_addr,_qzz_len)               \
   VALGRIND_MAKE_MEM_TAINTED(_qzz_addr,_qzz_len)

/* This is the old name for VALGRIND_MAKE_MEM_UNTAINTED.  Deprecated. */
#define VALGRIND_MAKE_READABLE(_qzz_addr,_qzz_len)               \
   VALGRIND_MAKE_MEM_UNTAINTED(_qzz_addr,_qzz_len)

/* Similar to VALGRIND_MAKE_MEM_UNTAINTED except that addressability is
   not altered: bytes which are addressable are marked as defined,
   but those which are not addressable are left unchanged. */
#define VALGRIND_MAKE_MEM_UNTAINTED_IF_ADDRESSABLE(_qzz_addr,_qzz_len) \
   (__extension__({unsigned int _qzz_res;                        \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0 /* default return */, \
                            VG_USERREQ__MAKE_MEM_UNTAINTED_IF_ADDRESSABLE, \
                            _qzz_addr, _qzz_len, 0, 0, 0);       \
    _qzz_res;                                                    \
   }))

/* Create a block-description handle.  The description is an ascii
   string which is included in any messages pertaining to addresses
   within the specified memory range.  Has no other effect on the
   properties of the memory range. */
#define VALGRIND_CREATE_BLOCK(_qzz_addr,_qzz_len, _qzz_desc)	 \
	(__extension__({unsigned int _qzz_res;			 \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0 /* default return */, \
                            VG_USERREQ__CREATE_BLOCK,            \
                            _qzz_addr, _qzz_len, _qzz_desc,      \
                            0, 0);                               \
    _qzz_res;							 \
   }))

/* Discard a block-description-handle. Returns 1 for an
   invalid handle, 0 for a valid handle. */
#define VALGRIND_DISCARD(_qzz_blkindex)                          \
   (__extension__ ({unsigned int _qzz_res;                       \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0 /* default return */, \
                            VG_USERREQ__DISCARD,                 \
                            0, _qzz_blkindex, 0, 0, 0);          \
    _qzz_res;                                                    \
   }))


/* Client-code macros to check the state of memory. */

/* Check that memory at _qzz_addr is addressable for _qzz_len bytes.
   If suitable addressibility is not established, Valgrind prints an
   error message and returns the address of the first offending byte.
   Otherwise it returns zero. */
#define VALGRIND_CHECK_MEM_IS_ADDRESSABLE(_qzz_addr,_qzz_len)    \
   (__extension__({unsigned int _qzz_res;                        \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0,                      \
                            VG_USERREQ__CHECK_MEM_IS_ADDRESSABLE,\
                            _qzz_addr, _qzz_len, 0, 0, 0);       \
    _qzz_res;                                                    \
   }))

/* Check that memory at _qzz_addr is addressable and defined for
   _qzz_len bytes.  If suitable addressibility and definedness are not
   established, Valgrind prints an error message and returns the
   address of the first offending byte.  Otherwise it returns zero. */
#define VALGRIND_CHECK_MEM_IS_UNTAINTED(_qzz_addr,_qzz_len)        \
   (__extension__({unsigned int _qzz_res;                        \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0,                      \
                            VG_USERREQ__CHECK_MEM_IS_UNTAINTED,    \
                            _qzz_addr, _qzz_len, 0, 0, 0);       \
    _qzz_res;                                                    \
   }))

/* Use this macro to force the definedness and addressibility of an
   lvalue to be checked.  If suitable addressibility and definedness
   are not established, Valgrind prints an error message and returns
   the address of the first offending byte.  Otherwise it returns
   zero. */
#define VALGRIND_CHECK_VALUE_IS_UNTAINTED(__lvalue)                \
   VALGRIND_CHECK_MEM_IS_UNTAINTED(                                \
      (volatile unsigned char *)&(__lvalue),                     \
                      (unsigned int)(sizeof (__lvalue)))

/* This is the old name for VALGRIND_CHECK_MEM_IS_ADDRESSABLE.  Deprecated. */
#define VALGRIND_CHECK_WRITABLE(_qzz_addr,_qzz_len)              \
   VALGRIND_CHECK_MEM_IS_ADDRESSABLE(_qzz_addr,_qzz_len)

/* This is the old name for VALGRIND_CHECK_MEM_IS_UNTAINTED.  Deprecated. */
#define VALGRIND_CHECK_READABLE(_qzz_addr,_qzz_len)              \
   VALGRIND_CHECK_MEM_IS_UNTAINTED(_qzz_addr,_qzz_len)

/* This is the old name for VALGRIND_CHECK_VALUE_IS_UNTAINTED.  Deprecated. */
#define VALGRIND_CHECK_UNTAINTED(__lvalue)                         \
   VALGRIND_CHECK_VALUE_IS_UNTAINTED(__lvalue)


/* Do a memory leak check mid-execution.  */
#define VALGRIND_DO_LEAK_CHECK                                   \
   {unsigned int _qzz_res;                                       \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0,                      \
                            VG_USERREQ__DO_LEAK_CHECK,           \
                            0, 0, 0, 0, 0);                      \
   }

/* Just display summaries of leaked memory, rather than all the
   details */
#define VALGRIND_DO_QUICK_LEAK_CHECK				 \
   {unsigned int _qzz_res;                                       \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0,                      \
                            VG_USERREQ__DO_LEAK_CHECK,           \
                            1, 0, 0, 0, 0);                      \
   }

/* Return number of leaked, dubious, reachable and suppressed bytes found by
   all previous leak checks.  They must be lvalues.  */
#define VALGRIND_COUNT_LEAKS(leaked, dubious, reachable, suppressed)     \
   /* For safety on 64-bit platforms we assign the results to private
      unsigned long variables, then assign these to the lvalues the user
      specified, which works no matter what type 'leaked', 'dubious', etc
      are.  We also initialise '_qzz_leaked', etc because
      VG_USERREQ__COUNT_LEAKS doesn't mark the values returned as
      initialised. */                                                    \
   {unsigned int  _qzz_res;                                              \
    unsigned long _qzz_leaked    = 0, _qzz_dubious    = 0;               \
    unsigned long _qzz_reachable = 0, _qzz_suppressed = 0;               \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0,                              \
                               VG_USERREQ__COUNT_LEAKS,                  \
                               &_qzz_leaked, &_qzz_dubious,              \
                               &_qzz_reachable, &_qzz_suppressed, 0);    \
    leaked     = _qzz_leaked;                                            \
    dubious    = _qzz_dubious;                                           \
    reachable  = _qzz_reachable;                                         \
    suppressed = _qzz_suppressed;                                        \
   }


/* Get the validity data for addresses [zza..zza+zznbytes-1] and copy it
   into the provided zzvbits array.  Return values:
      0   if not running on valgrind
      1   success
      2   [previously indicated unaligned arrays;  these are now allowed]
      3   if any parts of zzsrc/zzvbits are not addressable.
   The metadata is not copied in cases 0, 2 or 3 so it should be
   impossible to segfault your system by using this call.
*/
#define VALGRIND_GET_VBITS(zza,zzvbits,zznbytes)                 \
   (__extension__({unsigned int _qzz_res;                        \
    char* czza     = (char*)zza;                                 \
    char* czzvbits = (char*)zzvbits;                             \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0,                      \
                            VG_USERREQ__GET_VBITS,               \
                            czza, czzvbits, zznbytes, 0, 0 );    \
    _qzz_res;                                                    \
   }))

/* Set the validity data for addresses [zza..zza+zznbytes-1], copying it
   from the provided zzvbits array.  Return values:
      0   if not running on valgrind
      1   success
      2   [previously indicated unaligned arrays;  these are now allowed]
      3   if any parts of zza/zzvbits are not addressable.
   The metadata is not copied in cases 0, 2 or 3 so it should be
   impossible to segfault your system by using this call.
*/
#define VALGRIND_SET_VBITS(zza,zzvbits,zznbytes)                 \
   (__extension__({unsigned int _qzz_res;                        \
    char* czza     = (char*)zza;                                 \
    char* czzvbits = (char*)zzvbits;                             \
    VALGRIND_DO_CLIENT_REQUEST(_qzz_res, 0,                      \
                            VG_USERREQ__SET_VBITS,               \
                            czza, czzvbits, zznbytes, 0, 0 );    \
    _qzz_res;                                                    \
   }))

#endif

