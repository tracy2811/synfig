/*! ========================================================================
** Sinfg
** Template Header File
** $Id: mptr_openexr.h,v 1.1.1.1 2005/01/04 01:23:14 darco Exp $
**
** Copyright (c) 2002 Robert B. Quattlebaum Jr.
**
** This software and associated documentation
** are CONFIDENTIAL and PROPRIETARY property of
** the above-mentioned copyright holder.
**
** You may not copy, print, publish, or in any
** other way distribute this software without
** a prior written agreement with
** the copyright holder.
**
** === N O T E S ===========================================================
**
** ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SINFG_MPTR_OPENEXR_H
#define __SINFG_MPTR_OPENEXR_H

/* === H E A D E R S ======================================================= */

#include <sinfg/importer.h>
#include <stdio.h>
#include <sinfg/string.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

class exr_mptr : public sinfg::Importer
{
	SINFG_IMPORTER_MODULE_EXT

private:
	sinfg::String filename;
public:
	exr_mptr(const char *filename);
	~exr_mptr();



	virtual bool get_frame(sinfg::Surface &,sinfg::Time, sinfg::ProgressCallback *);

};

/* === E N D =============================================================== */

#endif
