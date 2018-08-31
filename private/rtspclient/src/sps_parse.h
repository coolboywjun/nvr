/*
*file:	sps_parse.h
*description:
*		parse h264 sps
*		
*in: 	src=sps rbsp data
*out:	struct sps
*author:pxh
*creat on:
*		2012-10-24
*edit_on:
*		2012-10-29
*		edit according to ISO14496 201003 I
*t.b.d.:
*^		not completely sps struct.
*		vui_parameters struct parse to fixed_frame_rate_flag.
*/


#ifndef _SPS_PARSE_H_
#define _SPS_PARSE_H_

#include "ptype.h"


struct sps_rbsp_p1
{
	U8 profile_idc;// 											u(8)
	U8 reserved_zero_2bits:2;		//  (equal to 0) 					u(2)
	U8 constraint_set5_flag:1;	
	U8 constraint_set4_flag:1;	
	U8 constraint_set3_flag:1;	
	U8 constraint_set2_flag:1;	// 									u(1)
	U8 constraint_set1_flag:1;	// 									u(1)
	U8 constraint_set0_flag:1;	// 									u(1)
	U8 level_idc;// 											u(8)
};
/*
 seq_parameter_set_id  0 ue(v) 

if( profile_idc  = =  100  | |  profile_idc  = =  110  | | 
    profile_idc  = =  122  | |  profile_idc  = =  244  | |  profile_idc  = =  44  | | 
    profile_idc  = =  83  | |  profile_idc  = =  86  | |  profile_idc  = =  118  | | 
    profile_idc  = =  128 ) 

chroma_format_idc  0 ue(v) 
   if( chroma_format_idc  = =  3 )     
   separate_colour_plane_flag  0 u(1) 
  bit_depth_luma_minus8  0 ue(v) 
  bit_depth_chroma_minus8  0 ue(v) 
  qpprime_y_zero_transform_bypass_flag  0 u(1) 
  seq_scaling_matrix_present_flag  0 u(1)
*/
/*
scaling_list( scalingList, sizeOfScali ngList, useDefaultScalingMatrixFlag ) {  C  Descriptor 
  lastScale = 8     
  nextScale = 8     
  for( j = 0; j < sizeOfScalingList; j++ ) {     
    if( nextScale != 0 ) {     
    delta_scale   0 | 1  se(v) 
      nextScale = ( lastScale + delta_scale + 256 ) % 256     
      useDefaultScalingMatrixFlag = ( j  = =  0 && nextScale  = =  0 )     
  }     
    scalingList[ j ] = ( nextScale   = =  0 ) ? lastScale : nextScale     
    lastScale = scalingList[ j ]     
 }     
}     
 

*/
/*


scaling_list( scalingList, sizeOfScali ngList, useDefaultScalingMatrixFlag ) {  C  Descriptor 
  lastScale = 8     
  nextScale = 8     
  for( j = 0; j < sizeOfScalingList; j++ ) {     
    if( nextScale != 0 ) {     
    delta_scale   0 | 1  se(v) 
      nextScale = ( lastScale + delta_scale + 256 ) % 256     
      useDefaultScalingMatrixFlag = ( j  = =  0 && nextScale  = =  0 )     
  }     
    scalingList[ j ] = ( nextScale   = =  0 ) ? lastScale : nextScale     
    lastScale = scalingList[ j ]     
 }     
}     
*/

struct sps_rbsp_p2
{
	U32     seq_parameter_set_id;						//ue(v)
	U32		chroma_format_idc;							//ue(v)
	U8		separate_colour_plane_flag;					//u(1)
	U32		bit_depth_luma_minus8;						//0 ue(v) 
	U32		bit_depth_chroma_minus8;					//0 ue(v) 
	U8		qpprime_y_zero_transform_bypass_flag;		//0 u(1) 
	U8		seq_scaling_matrix_present_flag;			//0 u(1)
	U8		seq_scaling_list_present_flag[12];
	U8		UseDefaultScalingMatrix4x4Flag[6];
	U8		UseDefaultScalingMatrix8x8Flag[6];
	S32		ScalingList4x4[6][16];
	S32		ScalingList8x8[6][64];
	U32     log2_max_frame_num_minus4;					//0	ue(v)
    U32     pic_order_cnt_type;						//0 ue(v)
	U32		log2_max_pic_order_cnt_lsb_minus4;				//

};

struct sps_rbsp_sp1
{	
	U32	delta_pic_order_always_zero_flag;// 				u(1)
	S32	offset_for_non_ref_pic;// 							se(v)
	S32	offset_for_top_to_bottom_field;// 					se(v)
	U32	num_ref_frames_in_pic_order_cnt_cycle;// 			ue(v)	
	S32	offset_for_ref_frame_array[16];//						se(v)
	
};

struct sps_rbsp_p3
{
	U32      num_ref_frames;// 									ue(v)
	U8	gaps_in_frame_num_value_allowed_flag;//				u(1)
	U32	pic_width_in_mbs_minus1;//							ue(v)
	U32	pic_height_in_map_units_minus1;//						u(1)

	U8	frame_mbs_only_flag;  	//							0 u(1) 
  	U8	mb_adaptive_frame_field_flag;// 						 0 u(1) 
  	U8	direct_8x8_inference_flag;//  							0 u(1) 
  	U8	frame_cropping_flag;//	u(1)

	U32	frame_crop_left_offset;//							ue(v)
	U32	frame_crop_right_offset;//							ue(v)
	U32	frame_crop_top_offset;//							ue(v)
	U32	frame_crop_bottom_offset;	//					ue(v)
	U8      vui_parameters_present_flag;//						u(1)
};
/*
vui_parameters( ) {  C Descriptor 
 aspect_ratio_info_present_flag  0 u(1) 
  if( aspect_ratio_info_present_flag ) {   
  aspect_ratio_idc  0 u(8) 
   if( aspect_ratio_idc  = =  Extended_SAR ) {    
   sar_width  0 u(16) 
   sar_height  0 u(16) 
  }   
 }    
  overscan_info_present_flag  0 u(1) 
 if( overscan_info_present_flag )   
   overscan_appropriate_flag  0 u(1) 
 video_signal_type_present_flag  0 u(1) 
 if( video_signal_type_present_flag ) {   
  video_format  0 u(3) 
   video_full_range_flag  0 u(1) 
   colour_description_present_flag  0 u(1) 
  if( colour_description_present_flag ) {    
    colour_primaries  0 u(8) 
    transfer_characteristics  0 u(8) 
   matrix_coefficients  0 u(8) 
   }   
  }    
 chroma_loc_info_present_flag  0 u(1) 
  if( chroma_loc_info_present_flag ) {   
  chroma_sample_loc_type_top_field  0 ue(v) 
  chroma_sample_loc_type_bottom_field  0 ue(v) 
  } 
timing_info_present_flag  0 u(1) 
  if( timing_info_present_flag ) {   
  num_units_in_tick  0 u(32) 
  time_scale  0 u(32) 
  fixed_frame_rate_flag  0 u(1) 
  }    
 nal_hrd_parameters_present_flag  0 u(1) 
 if( nal_hrd_parameters_present_flag )  

*/
#define Extended_SAR 255
struct vui_parameters_p1
{
    U8  aspect_ratio_info_present_flag; //  0 u(1) 
    U8  aspect_ratio_idc; //  0 u(8) 
    U16 sar_width; //  0 u(16) 
    U16 sar_height; //  0 u(16) 
    U8  overscan_info_present_flag; //  0 u(1) 
    U8  overscan_appropriate_flag; //  0 u(1) 
    U8  video_signal_type_present_flag; //  0 u(1) 
    U8  video_format; //  0 u(3) 
    U8  video_full_range_flag; //  0 u(1) 
    U8  colour_description_present_flag; //  0 u(1) 
    U8  colour_primaries; //  0 u(8) 
    U8  transfer_characteristics; //  0 u(8) 
    U8  matrix_coefficients; //  0 u(8) 
    U8  chroma_loc_info_present_flag; //  0 u(1) 
    U32 chroma_sample_loc_type_top_field; //  0 ue(v) 
    U32 chroma_sample_loc_type_bottom_field; //  0 ue(v) 
    U8  timing_info_present_flag; //  0 u(1) 
    U32 num_units_in_tick; //  0 u(32) 
    U32 time_scale; //  0 u(32) 
    U8  fixed_frame_rate_flag; //  0 u(1) 
    
};

class sps
{
private:
    U8 * str;
    U32 len;
    U32 bitp;
    struct sps_rbsp_p1 * pbsp1;
    struct sps_rbsp_p2 bsp2;
    struct sps_rbsp_sp1 sbsp1;
    struct sps_rbsp_p3 bsp3;
    struct vui_parameters_p1 vpp1;
public:
    sps(U8 * s,U32 l);
    int sps_parse();
    U32 get_num_units_in_tick();
    U32 get_time_scale();
	U32 get_log2_max_pic_order_cnt_lsb_minus4();
	U32 get_log2_max_frame_num_minus4();
	U8 get_frame_mbs_only_flag();

	U32 get_separate_colour_plane_flag();
	U32 get_pic_order_cnt_type();
	U32 get_pic_width_in_mbs_minus1();
	U32 get_pic_height_in_map_units_minus1();
	
};

#endif
