/* === S I N F G =========================================================== */
/*!	\file conicalgradient.cpp
**	\brief Template Header
**
**	$Id: conicalgradient.cpp,v 1.1.1.1 2005/01/04 01:23:10 darco Exp $
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

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <sinfg/string.h>
#include <sinfg/time.h>
#include <sinfg/context.h>
#include <sinfg/paramdesc.h>
#include <sinfg/renddesc.h>
#include <sinfg/surface.h>
#include <sinfg/value.h>
#include <sinfg/valuenode.h>
#include <sinfg/angle.h>

#include "conicalgradient.h"

#endif

/* === U S I N G =========================================================== */

using namespace etl;
using namespace std;
using namespace sinfg;

/* === G L O B A L S ======================================================= */

SINFG_LAYER_INIT(ConicalGradient);
SINFG_LAYER_SET_NAME(ConicalGradient,"conical_gradient");
SINFG_LAYER_SET_LOCAL_NAME(ConicalGradient,_("Conical Gradient"));
SINFG_LAYER_SET_CATEGORY(ConicalGradient,_("Gradients"));
SINFG_LAYER_SET_VERSION(ConicalGradient,"0.1");
SINFG_LAYER_SET_CVS_ID(ConicalGradient,"$Id: conicalgradient.cpp,v 1.1.1.1 2005/01/04 01:23:10 darco Exp $");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */

ConicalGradient::ConicalGradient():
	Layer_Composite(1.0,Color::BLEND_STRAIGHT),
	gradient(Color::black(),Color::white()),
	center(0,0),
	angle(Angle::zero()),
	symmetric(false)
{
}
	
bool
ConicalGradient::set_param(const String & param, const ValueBase &value)
{
	IMPORT(gradient);
	IMPORT(center);
	IMPORT(angle);
	IMPORT(symmetric);
	return Layer_Composite::set_param(param,value);
}

ValueBase
ConicalGradient::get_param(const String &param)const
{
	EXPORT(gradient);
	EXPORT(center);
	EXPORT(angle);
	EXPORT(symmetric);

	EXPORT_NAME();
	EXPORT_VERSION();
		
	return Layer_Composite::get_param(param);	
}

Layer::Vocab
ConicalGradient::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Composite::get_param_vocab());
	
	ret.push_back(ParamDesc("gradient")
		.set_local_name(_("Gradient"))
	);

	ret.push_back(ParamDesc("center")
		.set_local_name(_("Center"))
	);
	
	ret.push_back(ParamDesc("angle")
		.set_local_name(_("Angle"))
		.set_origin("center")
	);

	ret.push_back(ParamDesc("symmetric")
		.set_local_name(_("Symmetric"))
	);
	
	return ret;
}

inline Color
ConicalGradient::color_func(const Point &pos, float supersample)const
{
	const Point centered(pos-center);
	Angle::rot a=Angle::tan(-centered[1],centered[0]).mod();
	a+=angle;
	Real dist(a.mod().get());
	
	dist-=floor(dist);

	if(symmetric)
	{
		dist*=2.0;
		supersample*=2.0;
		if(dist>1)dist=2.0-dist;
	}
/*	if(dist+supersample*0.5>1.0)
	{
		Color pool(gradient(dist,supersample*0.5)*(1.0-(dist-supersample*0.5)));
		pool+=gradient((dist+supersample*0.5)-1.0,supersample*0.5)*((dist+supersample*0.5)-1.0);
		if(pool.get_a() && pool.is_valid())
		{
			pool.set_r(pool.get_r()/pool.get_a());
			pool.set_g(pool.get_g()/pool.get_a());
			pool.set_b(pool.get_b()/pool.get_a());
			pool.set_a(pool.get_a()/supersample);
		}
		return pool;
	}

	if(dist-supersample*0.5<0.0)
	{
		Color pool(gradient(dist,supersample*0.5)*(dist+supersample*0.5));
		pool+=gradient(1.0-(dist-supersample*0.5),supersample*0.5)*(-(dist-supersample*0.5));
		if(pool.get_a() && pool.is_valid())
		{
			pool.set_r(pool.get_r()/pool.get_a());
			pool.set_g(pool.get_g()/pool.get_a());
			pool.set_b(pool.get_b()/pool.get_a());
			pool.set_a(pool.get_a()/supersample);
			return pool;
		}
	}
*/
	if(dist+supersample*0.5>1.0)
	{
		Color pool(gradient(dist,supersample*0.5).premult_alpha()*(1.0-(dist-supersample*0.5)));
		pool+=gradient((dist+supersample*0.5)-1.0,supersample*0.5).premult_alpha()*((dist+supersample*0.5)-1.0);
		return pool.demult_alpha();
	}
	if(dist-supersample*0.5<0.0)
	{
		Color pool(gradient(dist,supersample*0.5).premult_alpha()*(dist+supersample*0.5));
		pool+=gradient(1.0-(dist-supersample*0.5),supersample*0.5).premult_alpha()*(-(dist-supersample*0.5));
		return pool.demult_alpha();
	}

	return gradient(dist,supersample);
}

float
ConicalGradient::calc_supersample(const sinfg::Point &x, float pw,float ph)const
{
	Point adj(x-center);
	if(abs(adj[0])<abs(pw*0.5) && abs(adj[1])<abs(ph*0.5))
		return 0.5;
	return (pw/Point(x-center).mag())/(PI*2);
}

sinfg::Layer::Handle
ConicalGradient::hit_check(sinfg::Context context, const sinfg::Point &point)const
{
	if(get_blend_method()==Color::BLEND_STRAIGHT && get_amount()>=0.5)
		return const_cast<ConicalGradient*>(this);
	if(get_amount()==0.0)
		return context.hit_check(point);
	if((get_blend_method()==Color::BLEND_STRAIGHT || get_blend_method()==Color::BLEND_COMPOSITE) && color_func(point).get_a()>0.5)
		return const_cast<ConicalGradient*>(this);
	return context.hit_check(point);
}

Color
ConicalGradient::get_color(Context context, const Point &pos)const
{
	const Color color(color_func(pos));
	
	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
		return color;
	else
		return Color::blend(color,context.get_color(pos),get_amount(),get_blend_method());
}
	
bool
ConicalGradient::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	SuperCallback supercb(cb,0,9500,10000);

	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
	{
		surface->set_wh(renddesc.get_w(),renddesc.get_h());
	}
	else
	{
		if(!context.accelerated_render(surface,quality,renddesc,&supercb))
			return false;
		if(get_amount()==0)
			return true;
	}

		
	int x,y;

	Surface::pen pen(surface->begin());
	const Real pw(renddesc.get_pw()),ph(renddesc.get_ph());
	Point pos;
	Point tl(renddesc.get_tl());
	const int w(surface->get_w());
	const int h(surface->get_h());
	
	if(get_amount()==1.0 && get_blend_method()==Color::BLEND_STRAIGHT)
	{
		if(quality<9)
		{
			for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
				for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
					pen.put_value(color_func(pos,calc_supersample(pos,pw,ph)));
		}
		else
		{
			for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
				for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
					pen.put_value(color_func(pos,0));
		}
	}
	else
	{
		if(quality<9)
		{
			for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
				for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
					pen.put_value(Color::blend(color_func(pos,calc_supersample(pos,pw,ph)),pen.get_value(),get_amount(),get_blend_method()));
		}
		else
		{
			for(y=0,pos[1]=tl[1];y<h;y++,pen.inc_y(),pen.dec_x(x),pos[1]+=ph)
				for(x=0,pos[0]=tl[0];x<w;x++,pen.inc_x(),pos[0]+=pw)
					pen.put_value(Color::blend(color_func(pos,0),pen.get_value(),get_amount(),get_blend_method()));
		}
	}

	// Mark our progress as finished
	if(cb && !cb->amount_complete(10000,10000))
		return false;

	return true;
}
