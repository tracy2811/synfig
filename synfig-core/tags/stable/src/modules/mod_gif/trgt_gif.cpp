/*! ========================================================================
** Sinfg
** BMP Target Module
** $Id: trgt_gif.cpp,v 1.1.1.1 2005/01/04 01:23:10 darco Exp $
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

/* === H E A D E R S ======================================================= */

#define SINFG_TARGET

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <ETL/stringf>
#include "trgt_gif.h"
#include <stdio.h>
#endif

/* === M A C R O S ========================================================= */

using namespace sinfg;
using namespace std;
using namespace etl;

#define MAX_FRAME_RATE	(20.0)

/* === G L O B A L S ======================================================= */

SINFG_TARGET_INIT(gif);
SINFG_TARGET_SET_NAME(gif,"gif");
SINFG_TARGET_SET_EXT(gif,"gif");
SINFG_TARGET_SET_VERSION(gif,"0.1");
SINFG_TARGET_SET_CVS_ID(gif,"$Id: trgt_gif.cpp,v 1.1.1.1 2005/01/04 01:23:10 darco Exp $");

/* === M E T H O D S ======================================================= */

gif::gif(const char *filename_):
	filename(filename_),
	file( (filename=="-")?stdout:fopen(filename_,"wb") ),
	imagecount(0),
	
	lossy(true),
	multi_image(false),	
	dithering(true),
	color_bits(8),
	iframe_density(30),
	loop_count(0x7fff),
	local_palette(true)	
{
}

gif::~gif()
{
	if(file)
		fputc(';',file.get());	// Image terminator
}

bool
gif::set_rend_desc(RendDesc *given_desc)
{
	if(given_desc->get_frame_rate()>MAX_FRAME_RATE)
		given_desc->set_frame_rate(MAX_FRAME_RATE);

	desc=*given_desc;

	if(desc.get_frame_end()-desc.get_frame_start()>0)
	{
		multi_image=true;
		//set_remove_alpha();
		imagecount=desc.get_frame_end()-desc.get_frame_start();
	}
	else
		multi_image=false;
	return true;
}

bool
gif::init()
{
	int w=desc.get_w(),h=desc.get_h();
		
	if(!file)
	{
		sinfg::error(strprintf(_("Unable to open \"%s\" for write access!"),filename.c_str()));
		return false;
	}
	
	rootsize=color_bits;	// Size of pixel bits

	curr_frame.set_wh(w,h);
	prev_frame.set_wh(w,h);
	curr_surface.set_wh(w,h);
	curr_frame.clear();
	prev_frame.clear();
	curr_surface.clear();
	
	if(get_quality()>5)
		lossy=true;
	else
		lossy=false;

	// Output the header
	fprintf(file.get(),"GIF89a");
	fputc(w&0x000000ff,file.get());
	fputc((w&0x0000ff00)>>8,file.get());
	fputc(h&0x000000ff,file.get());
	fputc((h&0x0000ff00)>>8,file.get());
	if(!local_palette)
		fputc(0xF0+(rootsize-1),file.get());	// flags
	else
		fputc((0xF0+(rootsize-1))&~(1<<7),file.get());	// flags
		
	fputc(0,file.get());		// backgound color
	fputc(0,file.get());		// Pixel Aspect Ratio
	
	DEBUGPOINT();
	
	if(!local_palette)
	{
	DEBUGPOINT();
		curr_palette=Palette::grayscale(256/(1<<(8-rootsize))-1);
		output_curr_palette();
	}
	
	if(loop_count && multi_image)
	{
	DEBUGPOINT();
		fputc(33,file.get()); // 33 (hex 0x21) GIF Extension code
		fputc(255,file.get()); // 255 (hex 0xFF) Application Extension Label
		fputc(11,file.get()); // 11 (hex (0x0B) Length of Application Block 
		fprintf(file.get(),"NETSCAPE2.0");
		fputc(3,file.get()); // 3 (hex 0x03) Length of Data Sub-Block 
		fputc(1,file.get()); // 1 (hex 0x01)
		fputc(loop_count&0x000000ff,file.get());
		fputc((loop_count&0x0000ff00)>>8,file.get());
		fputc(0,file.get()); // 0 (hex 0x00) a Data Sub-block Terminator. 
	}
	DEBUGPOINT();
	
	return true;
}

void
gif::output_curr_palette()
{
	// Output the color table
	for(i=0;i<256/(1<<(8-rootsize));i++)
	{
//		if(i && (i-1)<curr_palette.size())
		{
			Color color(curr_palette[i].color.clamped());
			//fputc(i*(1<<(8-rootsize)),file.get());
			//fputc(i*(1<<(8-rootsize)),file.get());
			//fputc(i*(1<<(8-rootsize)),file.get());
			fputc(gamma().r_F32_to_U8(color.get_r()),file.get());
			fputc(gamma().g_F32_to_U8(color.get_g()),file.get());
			fputc(gamma().b_F32_to_U8(color.get_b()),file.get());
		}
/*		else
		{
			fputc(255,file.get());
			fputc(0,file.get());
			fputc(255,file.get());
		}
*/
	}
}

bool
gif::start_frame(sinfg::ProgressCallback *callback)
{
//	int
//		w=desc.get_w(),
//		h=desc.get_h();
	
	if(!file)
	{
		if(callback)callback->error(string("BUG:")+_("Description not set!"));
		return false;
	}
	
	if(callback)callback->task(filename+strprintf(" %d",imagecount));


	
	return true;
}

void
gif::end_frame()
{
	int w=desc.get_w(),h=desc.get_h(),i;
	unsigned int value;
	int
		delaytime=round_to_int(100.0/desc.get_frame_rate());
	
	bool build_off_previous(multi_image);

	Palette prev_palette(curr_palette);
	
	// Fill in the background color
	if(!get_remove_alpha())
	{
		Surface::alpha_pen pen(curr_surface.begin(),1.0,Color::BLEND_BEHIND);
		pen.set_value(get_canvas()->rend_desc().get_bg_color());
		for(int y=0;y<curr_surface.get_h();y++,pen.inc_y())
		{
			int x;
			for(x=0;x<curr_surface.get_w();x++,pen.inc_x())
			{
				if(pen.get_value().get_a()>0.1)
					pen.put_value();
				else
					pen[0][0]=Color::alpha();
			}
			pen.dec_x(x);
		}
	}
	
	if(local_palette)
	{
		curr_palette=Palette(curr_surface,256/(1<<(8-rootsize))-build_off_previous-1);
		sinfg::info("curr_palette.size()=%d",curr_palette.size());
	}
	
	int transparent_index(curr_palette.find_closest(Color(1,0,1,0))-curr_palette.begin());
	bool has_transparency(curr_palette[transparent_index].color.get_a()<=0.00001);
	
	if(has_transparency)
		build_off_previous=false;
	
	if(build_off_previous)
	{
		transparent_index=0;
		has_transparency=true;
	}
		
#define DISPOSE_UNDEFINED			(0)
#define DISPOSE_NONE				(1<<2)
#define DISPOSE_RESTORE_BGCOLOR		(2<<2)
#define DISPOSE_RESTORE_PREVIOUS	(3<<2)
	int gec_flags(0);
	if(build_off_previous)
		gec_flags|=DISPOSE_NONE;
	else
		gec_flags|=DISPOSE_RESTORE_PREVIOUS;
	if(has_transparency)
		gec_flags|=1;
	
	// output the Graphic Control Extension
	fputc(0x21,file.get()); // Extension introducer
	fputc(0xF9,file.get()); // Graphic Control Label
	fputc(4,file.get()); // Block Size
	fputc(gec_flags,file.get()); // Flags (Packed Fields)
	fputc(delaytime&0x000000ff,file.get()); // Delay Time (MSB)
	fputc((delaytime&0x0000ff00)>>8,file.get()); // Delay Time (LSB)
	fputc(transparent_index,file.get()); // Transparent Color Index
	fputc(0,file.get()); // Block Terminator
	
	// output the image header
	fputc(',',file.get());
	fputc(0,file.get());	// image left
	fputc(0,file.get());	// image left
	fputc(0,file.get());	// image top
	fputc(0,file.get());	// image top
	fputc(w&0x000000ff,file.get());
	fputc((w&0x0000ff00)>>8,file.get());
	fputc(h&0x000000ff,file.get());
	fputc((h&0x0000ff00)>>8,file.get());
	if(local_palette)
		fputc(0x80|(rootsize-1),file.get());	// flags
	else
		fputc(0x00+ rootsize-1,file.get());	// flags

	
	if(local_palette)
	{
		Palette out(curr_palette);
		
		if(build_off_previous)
			curr_palette.insert(curr_palette.begin(),Color(1,0,1,0));
		output_curr_palette();
		curr_palette=out;
	}
	
	bs=bitstream(file);

	// Prepare ourselves for LZW compression
	codesize=rootsize+1;
	nextcode=(1<<rootsize)+2;
	table=lzwcode::NewTable((1<<rootsize));
	node=table;
	
	// Output the rootsize
	fputc(rootsize,file.get());	// rootsize;

	// Push a table reset into the bitstream
	bs.push_value(1<<rootsize,codesize);

	for(int cur_scanline=0;cur_scanline<desc.get_h();cur_scanline++)
	{
		//convert_color_format(curr_frame[cur_scanline], curr_surface[cur_scanline], desc.get_w(), PF_GRAY, gamma());
	
		// Now we compress it!
		for(i=0;i<w;i++)
		{
			Color color(curr_surface[cur_scanline][i].clamped());
			Palette::iterator iter(curr_palette.find_closest(color));
			
			if(dithering)
			{
				Color error(color-iter->color);
				//error*=0.25;
				if(curr_surface.get_h()>cur_scanline+1)
				{
					curr_surface[cur_scanline+1][i-1]  += error * ((float)3/(float)16);
					curr_surface[cur_scanline+1][i]    += error * ((float)5/(float)16);
					if(curr_surface.get_w()>i+1)
						curr_surface[cur_scanline+1][i+1]  += error * ((float)1/(float)16);
				}
				if(curr_surface.get_w()>i+1)
					curr_surface[cur_scanline][i+1]    += error * ((float)7/(float)16);
			}
			
			curr_frame[cur_scanline][i]=iter-curr_palette.begin();
			
			value=curr_frame[cur_scanline][i];
			if(build_off_previous)
				value++;
			if(value>(unsigned)(1<<rootsize)-1)
				value=(1<<rootsize)-1;
			
			// If the pixel is the same as the one that
			// is already there, then we should make it
			// transparent
			if(build_off_previous)
			{
				if(lossy)
				{
					
					// Lossy
					if(
						abs( ( iter->color-prev_palette[prev_frame[cur_scanline][i]-1].color ).get_y() ) > (1.0/16.0) ||
//						abs((int)value-(int)prev_frame[cur_scanline][i])>2||
//						(value<=2 && value!=prev_frame[cur_scanline][i]) ||
						(imagecount%iframe_density)==0 || imagecount==desc.get_frame_end()-1 ) // lossy version
						prev_frame[cur_scanline][i]=value;
					else
					{
						prev_frame[cur_scanline][i]=value;
						value=0;
					}
				}
				else
				{
					// lossless version
					if(value!=prev_frame[cur_scanline][i]) 
						prev_frame[cur_scanline][i]=value;
					else
						value=0;
				}
			}
			else
			prev_frame[cur_scanline][i]=value;

			next=node->FindCode(value);
			if(next)
				node=next;
			else
			{
				node->AddNode(nextcode, value);
				bs.push_value(node->code, codesize);
				node = table->FindCode(value);
				
				// Check to see if we need to increase the codesize
				if (nextcode == ( 1 << codesize))
					codesize += 1;
					
				nextcode += 1;
		
				// check to see if we have filled up the table
				if (nextcode == 4096)
				{
					// output the clear code: make sure to use the current
					// codesize
					bs.push_value((unsigned) 1 << rootsize, codesize);
		
					delete table;
					table = lzwcode::NewTable((1<<rootsize));
					codesize = rootsize + 1;
					nextcode = (1 << rootsize) + 2;
					
					// since we have a new table, need the correct prefix
					node = table->FindCode(value);
				}
			}
		}
	}





	// Push the last code onto the bitstream
	bs.push_value(node->code,codesize);
	
	// Push a end-of-stream code onto the bitstream
	bs.push_value((1<<rootsize)+1,codesize);
	
	// Make sure everything is dumped out
	bs.dump();
	
	delete table;
	
	fputc(0,file.get());		// Block terminator

	fflush(file.get());
	imagecount++;
}

sinfg::Color*
gif::start_scanline(int scanline)
{
	cur_scanline=scanline;
	return curr_surface[scanline];
}

bool
gif::end_scanline()
{
	if(!file)
		return false;

//	int w=desc.get_w(),i;
//	unsigned int value;


	return true;
}
