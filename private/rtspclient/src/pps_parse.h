/*
*file:	pps_parse.h
*description:
*		parse the pps struct
*		reserve NAL struct, not used now.
*in: 	src=h264.data
*out:	buf=rbsp
*author:pxh
*creat on:
*		2012-10-24
*t.b.d.:
*/

#ifndef _PPS_PARSE_H_
#define _PPS_PARSE_H_

#include "ptype.h"
/*
pic_parameter_set_rbsp( ) {  C Descri
 pic_parameter_set_id  1 ue(v) 
 seq_parameter_set_id  1 ue(v) 
 entropy_coding_mode_flag  1 u(1) 
 pic_order_present_flag  1 u(1) 
 num_slice_groups_minus1  1 ue(v) 
*/
struct pps_rbsp_p1
{
	U32 pic_parameter_set_id;	//  1 ue(v) 
	U32 seq_parameter_set_id;	//  1 ue(v) 
	U8 entropy_coding_mode_flag;//  1 u(1) 
	U8 pic_order_present_flag;	//  1 u(1) 
	U32 num_slice_groups_minus1;//  1 ue(v) 
};


class pps
{
private:
	U8 * prbsp;
	U32 len;
	U32 bitp;
	pps_rbsp_p1 pps_p1;
public:
	pps(U8*,U32);
	int pps_parse();
	U8 get_pic_order_present_flag();

};



#endif
