/*
 *  This file is part of libstego.
 *
 *  libstego is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libstego is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libstego.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  Copyright 2008 2009 by Jan C. Busch, René Frerichs, Lars Herrmann,
 *  Matthias Kloppenborg, Marko Krause, Christian Kuka, Sebastian Schnell,
 *  Ralf Treu.
 *
 *  For more info visit <http://parsys.informatik.uni-oldenburg.de/~stego/>
 */
 
 
 
#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <libstego/types.h>

// Error flags, returned directly by functions
#define LSTG_OK		0
#define LSTG_ERROR	1

#define FAIL(e) lstg_errno = e;  \
  return LSTG_ERROR;


#define LSTG_E_MSG_UNKNOWN "Unknown error."

/// Error codes, set globally in lstg_errno when an error is encountered
enum LSTG_ERROR_CODE {
    LSTG_E_OPENFAILED   = 1,
    LSTG_E_CLOSEFAILED,
    LSTG_E_READFAILED,
    LSTG_E_WRITEFAILED,
    LSTG_E_FRMTNOTSUPP,
    LSTG_E_INVALIDPARAM,
    LSTG_E_INSUFFCAP,
    LSTG_E_EXTLIB,
    LSTG_E_SIZEMISMATCH,
    LSTG_E_MSGTOOLONG,
    LSTG_E_XPATH_CONT,
    LSTG_E_XPATH_EXPR,
    LSTG_E_MALLOC,
    LSTG_E_INVALIDFILE,
    LSTG_E_PQ_LTFAIL,
    LSTG_E_PQ_GAUSSFAIL,
    LSTG_E_MLEN_EM_FAIL,
    
    /// NOT an error code, used to determine whether an error exists or not
    LSTG_E_LAST     // has to be kept last in this list
};


/// Error messages, defined in 'errors.c'
extern const char *LSTG_ERROR_MSGS[];

/// Global error variable, defined in 'errors.c'
extern enum LSTG_ERROR_CODE lstg_errno;

/**
 * Returns a description of the error number;
 *
 */
const char *lstg_strerror(const enum LSTG_ERROR_CODE errno);



#endif //__ERRORS_H__
