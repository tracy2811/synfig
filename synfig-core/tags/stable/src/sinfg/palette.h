/* === S I N F G =========================================================== */
/*!	\file Palette.h
**	\brief Template Header
**
**	$Id: palette.h,v 1.1.1.1 2005/01/04 01:23:14 darco Exp $
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

#ifndef __SINFG_PALETTE_H
#define __SINFG_PALETTE_H

/* === H E A D E R S ======================================================= */

#include "color.h"
#include "string.h"
#include <vector>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace sinfg {

class Surface;
	
struct PaletteItem
{
	Color color;
	String name;
	int weight;
	
	PaletteItem():weight(1) { }

	PaletteItem(const Color& color, const String& name, int weight=1):
		color(color),name(name),weight(weight) { }

	PaletteItem(const Color& color, int weight=1):
		color(color),weight(weight) { }
		
	void add(const Color& x, int weight=1);
		
	bool operator<(const PaletteItem& rhs)const { return weight<rhs.weight; }
}; // END of struct PaletteItem

class Palette : public std::vector<PaletteItem>
{
	String name_;
	
public:
	Palette();
	Palette(const String& name_);
	
	/*! Generates a palette for the given
	**	surface
	*/
	Palette(const Surface& surface, int size=256);

	iterator find_closest(const Color& color, float* dist=0);
	const_iterator find_closest(const Color& color, float* dist=0)const;

	iterator find_heavy();
	
	iterator find_light();

	static Palette grayscale(int steps=16);

	void save_to_file(const sinfg::String& filename)const;

	static Palette load_from_file(const sinfg::String& filename);
}; // END of class Palette

}; // END of namespace sinfg

/* === E N D =============================================================== */

#endif
