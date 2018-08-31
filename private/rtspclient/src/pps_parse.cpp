#include "pps_parse.h"
#include <stdlib.h>
#include <stdio.h>
#include "exp_golomb.h"
#include <string.h>

pps::pps(U8 * s,U32 l)
{
	memset(&pps_p1,0,sizeof(pps_rbsp_p1));
    	prbsp =s;
    	len =l;
    	bitp = 0;
}

int pps::pps_parse()
{

	U8 * curpos = prbsp;
	U32 curlen = len;
	ExpGolomb b(curpos,0,curlen*8);
	pps_p1.pic_parameter_set_id = b.parse_ue();
	pps_p1.seq_parameter_set_id = b.parse_ue();
	pps_p1.entropy_coding_mode_flag = b.read_1bit();
	pps_p1.pic_order_present_flag = b.read_1bit();
	pps_p1.num_slice_groups_minus1 = b.parse_ue();
	/*
	//changed by simon 2013-1-18
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	printf("     pic_parameter_set_id=%d       \n",pps_p1.pic_parameter_set_id);
	printf("     seq_parameter_set_id=%d       \n",pps_p1.seq_parameter_set_id);
	printf("     entropy_coding_mode_flag=%d       \n",pps_p1.entropy_coding_mode_flag);
	printf("     pic_order_present_flag=%d       \n",pps_p1.pic_order_present_flag);
	printf("     num_slice_groups_minus1=%d       \n",pps_p1.num_slice_groups_minus1);
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	//end
	*/
	
	return 0;
}

U8 pps::get_pic_order_present_flag()
{
    return pps_p1.pic_order_present_flag;
}

