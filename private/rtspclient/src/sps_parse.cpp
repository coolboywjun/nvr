#include "sps_parse.h"
#include <stdlib.h>
#include <stdio.h>
#include "exp_golomb.h"
#include <string.h>
    sps::sps(U8 * s,U32 l)
    {
        str =s;
        len =l;
        bitp = 0;
		memset(&vpp1,0,sizeof(vui_parameters_p1));
    }
    
    int sps::sps_parse()
    {
        	pbsp1 = (struct sps_rbsp_p1 *)str;
		U8 * curpos = str + sizeof(sps_rbsp_p1);
		U32 curlen = len -sizeof(sps_rbsp_p1);
		
        
        	ExpGolomb b(curpos,0,curlen*8);

		bsp2.seq_parameter_set_id = b.parse_ue();
		U32 profile_idc = pbsp1->profile_idc;
		if( (profile_idc == 100) || (profile_idc == 110) || (profile_idc == 122) || (profile_idc == 244)
			|| (profile_idc  == 44) || (profile_idc == 83) || (profile_idc  == 86) || (profile_idc == 118) ||\
			(profile_idc == 128))
		{ 
		  bsp2.chroma_format_idc = b.parse_ue(); 
		   if( bsp2.chroma_format_idc  == 3)
			   bsp2.separate_colour_plane_flag =b.read_1bit();

		  bsp2.bit_depth_luma_minus8 = b.parse_ue();
		  bsp2.bit_depth_chroma_minus8 = b.parse_ue(); 
		  bsp2.qpprime_y_zero_transform_bypass_flag = b.read_1bit(); 
		  bsp2.seq_scaling_matrix_present_flag = b.read_1bit();
		  if( bsp2.seq_scaling_matrix_present_flag )
		  {
				
			  for( int i = 0; i < ((bsp2.chroma_format_idc != 3)?8:12 );i++)
			  {     
					S32 lastScale = 8;
					S32 nextScale = 8;
					S32 delta_scale;
					bsp2.seq_scaling_list_present_flag[i] = b.read_1bit();
					if( bsp2.seq_scaling_list_present_flag[i] )
					{
						if( i < 6 )
						{
							for(int j =0;j<16;j++)
							{
								if(nextScale != 0)
								{
									delta_scale = b.parse_se();
									nextScale = (lastScale + delta_scale + 256)%256;
									bsp2.UseDefaultScalingMatrix4x4Flag[i] = ((j == 0) && (nextScale == 0));
								}
								bsp2.ScalingList4x4[i][j] = (nextScale == 0)?lastScale:nextScale;
								lastScale = bsp2.ScalingList4x4[i][j];
							}
							
						}
						else
						{
							int ii = i-6;
							nextScale = 8;
							lastScale = 8;
							for(int j =0;j<16;j++)
							{
								if(nextScale != 0)
								{
									delta_scale = b.parse_se();
									nextScale = (lastScale + delta_scale + 256)%256;
									bsp2.UseDefaultScalingMatrix8x8Flag[ii] = ((j == 0) && (nextScale == 0));
								}
								bsp2.ScalingList8x8[ii][j] = (nextScale == 0)?lastScale:nextScale;
								lastScale = bsp2.ScalingList8x8[ii][j];
							}
						}
					}
			  }
		  }
		}		

		bsp2.log2_max_frame_num_minus4 = b.parse_ue();
		bsp2.pic_order_cnt_type = b.parse_ue();
		if(bsp2.pic_order_cnt_type == 0)
			bsp2.log2_max_pic_order_cnt_lsb_minus4 = b.parse_ue();
		else if(bsp2.pic_order_cnt_type == 1)
		{
			sbsp1.delta_pic_order_always_zero_flag = b.read_1bit();
			sbsp1.offset_for_non_ref_pic = b.parse_se();
			sbsp1.offset_for_top_to_bottom_field = b.parse_se();
			sbsp1.num_ref_frames_in_pic_order_cnt_cycle = b.parse_ue();
			
			for(unsigned int i=0;i<sbsp1.num_ref_frames_in_pic_order_cnt_cycle;i++)
			{
				sbsp1.offset_for_ref_frame_array[i] = b.parse_se();
			}
		}
		bsp3.num_ref_frames = b.parse_ue();
		bsp3.gaps_in_frame_num_value_allowed_flag = b.read_1bit();
		bsp3.pic_width_in_mbs_minus1 = b.parse_ue();
		bsp3.pic_height_in_map_units_minus1 = b.parse_ue();
		bsp3.frame_mbs_only_flag = b.read_1bit();
		if(!bsp3.frame_mbs_only_flag)
			bsp3.mb_adaptive_frame_field_flag = b.read_1bit();
		bsp3.direct_8x8_inference_flag = b.read_1bit();
		bsp3.frame_cropping_flag = b.read_1bit();
		if(bsp3.frame_cropping_flag)
		{
			bsp3.frame_crop_left_offset = b.parse_ue();
			bsp3.frame_crop_right_offset = b.parse_ue();
			bsp3.frame_crop_top_offset = b.parse_ue();
			bsp3.frame_crop_bottom_offset = b.parse_ue();
		}

		bsp3.vui_parameters_present_flag = b.read_1bit();
		if(bsp3.vui_parameters_present_flag)
		{
			vpp1.aspect_ratio_info_present_flag = b.read_1bit();
			if(vpp1.aspect_ratio_info_present_flag)
			{
				vpp1.aspect_ratio_idc = b.read_bits(8);
				if(vpp1.aspect_ratio_idc == Extended_SAR)
				{
					vpp1.sar_width = b.read_bits(16);
					vpp1.sar_height = b.read_bits(16);
				}
			}
			vpp1.overscan_info_present_flag = b.read_1bit();
			if(vpp1.overscan_info_present_flag)
			{
				vpp1.overscan_appropriate_flag = b.read_1bit();
			}
			vpp1.video_signal_type_present_flag = b.read_1bit();
			if(vpp1.video_signal_type_present_flag)
			{
				vpp1.video_format = b.read_bits(3);
				vpp1.video_full_range_flag = b.read_1bit();
				vpp1.colour_description_present_flag = b.read_1bit();
				if(vpp1.colour_description_present_flag)
				{
					vpp1.colour_primaries = b.read_bits(8);
					vpp1.transfer_characteristics = b.read_bits(8);
					vpp1.matrix_coefficients = b.read_bits(8);
				}
			}
			vpp1.chroma_loc_info_present_flag = b.read_1bit();
			if(vpp1.chroma_loc_info_present_flag)
			{
				vpp1.chroma_sample_loc_type_top_field = b.parse_ue();
				vpp1.chroma_sample_loc_type_bottom_field = b.parse_ue();
			}
			vpp1.timing_info_present_flag = b.read_1bit();
			if(vpp1.timing_info_present_flag)
			{
				vpp1.num_units_in_tick = b.read_bits(32);
				vpp1.time_scale = b.read_bits(32);
				vpp1.fixed_frame_rate_flag = b.read_1bit();
			}
			
			
		}
        return 0;
    }
    
    U32 sps::get_num_units_in_tick()
    {
        return vpp1.num_units_in_tick;
    }
    
    U32 sps::get_time_scale()
    {
        return vpp1.time_scale;
    }

	U32 sps::get_log2_max_pic_order_cnt_lsb_minus4()
	{
		return bsp2.log2_max_pic_order_cnt_lsb_minus4;
	}
	
	U32 sps::get_log2_max_frame_num_minus4()
	{
		return bsp2.log2_max_frame_num_minus4;
	}

	U8 sps::get_frame_mbs_only_flag()
	{
		return bsp3.frame_mbs_only_flag;
	}

	U32 sps::get_pic_order_cnt_type()
	{
		return bsp2.pic_order_cnt_type;
	}

	U32 sps::get_separate_colour_plane_flag()
	{
		return bsp2.separate_colour_plane_flag;
	}

	U32 sps::get_pic_width_in_mbs_minus1()
	{
		return bsp3.pic_width_in_mbs_minus1;
	}

	U32 sps::get_pic_height_in_map_units_minus1()
	{
		return bsp3.pic_height_in_map_units_minus1;
	}
		