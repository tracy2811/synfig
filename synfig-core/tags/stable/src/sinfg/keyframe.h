/* === S I N F G =========================================================== */
/*!	\file keyframe.h
**	\brief Template Header
**
**	$Id: keyframe.h,v 1.1.1.1 2005/01/04 01:23:14 darco Exp $
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

#ifndef __SINFG_KEYFRAME_H
#define __SINFG_KEYFRAME_H

/* === H E A D E R S ======================================================= */

#include <ETL/handle>
#include <vector>
#include "string.h"
#include "time.h"
#include "uniqueid.h"
#include "guid.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace sinfg {

//! \writeme
class Keyframe :  public UniqueID
{
public:

	Time time_;
	String desc_;
	GUID guid_;
	
public:

	Keyframe();

	Keyframe(const Time &time);

	~Keyframe();

	void set_time(Time x) { time_=x; }
	
	Time get_time()const { return time_; }

	void set_description(String x) { desc_=x; }
	
	String get_description()const { return desc_; }

	const GUID& get_guid()const { return guid_; }
	void set_guid(const GUID& x) { guid_=x; }

	using UniqueID::operator<;
	using UniqueID::operator==;
	using UniqueID::operator!=;
	using UniqueID::operator=;
	
	bool operator<(const Keyframe &rhs)const { return time_<rhs.time_; }
	bool operator<(const Time &rhs)const { return time_<rhs; }

//	bool operator==(const Keyframe &rhs)const { return id_==rhs.id_; }
	bool operator==(const Time &rhs)const { return time_==rhs; }

//	bool operator!=(const Keyframe &rhs)const { return id_!=rhs.id_; }
	bool operator!=(const Time &rhs)const { return time_!=rhs; }
}; // END of class Keyframe

class KeyframeList : public std::vector<Keyframe>
{

public:

	iterator add(const Keyframe &x);

	void erase(const UniqueID &x);

	iterator find(const UniqueID &x);

	const_iterator find(const UniqueID &x)const;

	//! Finds the keyframe at an exact point in time
	iterator find(const Time &x);

	//! Finds the keyframe after that point in time
	iterator find_next(const Time &x);

	//! Finds the keyframe before that point in time
	iterator find_prev(const Time &x);

	const_iterator find(const Time &x)const;
	const_iterator find_next(const Time &x)const;
	const_iterator find_prev(const Time &x)const;
	
	void find_prev_next(const Time& time, Time &prev, Time &next)const;

	void insert_time(const Time& location, const Time& delta);

	void dump()const;
	void sync();	
};

//typedef std::list<Keyframe> KeyframeList;

}; // END of namespace sinfg

/* === E N D =============================================================== */

#endif
