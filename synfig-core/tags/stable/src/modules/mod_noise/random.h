/* === S I N F G =========================================================== */
/*!	\file noise.h
**	\brief Template Header
**
**	$Id: random.h,v 1.2 2005/01/13 05:14:02 darco Exp $
**
**	\legal
**	Copyright (c) 2002 Robert B. Quattlebaum Jr.
**
**	This software and associated documentation
**	are CONFIDENTIAL and PROPRIETARY property of
**	the above-mentioned copyright holder.
**
**	You may not copy, print, publish, or in any
**	other way distribute this software without
**	a prior written agreement with
**	the copyright holder.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SINFG_RANDOM_H
#define __SINFG_RANDOM_H

/* === H E A D E R S ======================================================= */

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */


#define POOL_SIZE	(256)
class Random
{
	int seed_;
public:
	
	void set_seed(int x);
	int get_seed()const { return seed_; }
	
	float operator()(int subseed,int x,int y=0, int t=0)const;	
	float operator()(int smooth,int subseed,float x,float y=0, float t=0)const;	
};

/* === E N D =============================================================== */

#endif
