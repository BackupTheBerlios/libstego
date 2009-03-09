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
 
 
 
#include "libstego/errors.h"

const char *LSTG_ERROR_MSGS[] = {
/*LSTG_E_OPENFAILED  */    "File open failed.",
/*LSTG_E_READFAILED  */    "File read failed.",
/*LSTG_E_WRITEFAILED */    "File write failed.",
/*LSTG_E_FRMTNOTSUPP */    "File format not supported.",
/*LSTG_E_INVALIDPARAM*/    "Invalid parameter.",
/*LSTG_E_INSUFFCAP   */    "Insufficient capacity.",
/*LSTG_E_EXTLIB      */    "External library error.",
/*LSTG_E_SIZEMISMATCH*/    "Size mismatch.",
/*LSTG_E_MSGTOOLONG  */    "Message too long to embed.",
/*LSTG_E_XPATH_CONT  */    "XPath context creation failed.",
/*LSTG_E_XPATH_EXPR  */    "XPath expression evaluation failed.",

/*LSTG_E_MALLOC      */    "Memory allocation failed."
};

enum LSTG_ERROR_CODE lstg_errno;
