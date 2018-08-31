#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define SUCCESS 								0
#define FAILURE								~0
#define RETRY								1		//return retry, value 1;
	
//////////////////////
/////////////////////////

typedef struct tag_bs_s
{
	uint8_t *p_start;
	uint8_t *p;
	uint8_t *p_end;
	int     i_left;			// i_count number of available bits
	int     i_bits_encoded; // RD only
} bs_t;


static int	_bs_eof( bs_t *s )
{
	return( s->p >= s->p_end ? 1: 0 );
}

static uint32_t _bs_read(bs_t *s, int i_count )
{
	static uint32_t i_mask[33] ={
		0x00,
		0x01,      0x03,      0x07,      0x0f,
		0x1f,      0x3f,      0x7f,      0xff,
		0x1ff,     0x3ff,     0x7ff,     0xfff,
		0x1fff,    0x3fff,    0x7fff,    0xffff,
		0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
		0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
		0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
		0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff
	};

	int      i_shr;
	uint32_t i_result = 0;

	while( i_count > 0 )
	{
		if( s->p >= s->p_end )
		{
			break;
		}

		if( ( i_shr = s->i_left - i_count ) >= 0 )
		{
			/* more in the buffer than requested */
			i_result |= ( *s->p >> i_shr )&i_mask[i_count];
			s->i_left -= i_count;
			if( s->i_left == 0 )
			{
				s->p++;
				s->i_left = 8;
			}
			return( i_result );
		}
		else
		{
			// less in the buffer than requested
			i_result |= (*s->p&i_mask[s->i_left]) << -i_shr;
			i_count  -= s->i_left;
			s->p++;
			s->i_left = 8;
		}
	}

	return( i_result );
}

static uint32_t _bs_read1( bs_t *s )
{

	if( s->p < s->p_end )
	{
		unsigned int i_result;

		s->i_left--;
		i_result = ( *s->p >> s->i_left )&0x01;
		if( s->i_left == 0 )
		{
			s->p++;
			s->i_left = 8;
		}
		return i_result;
	}

	return 0;
}

static void _bs_skip( bs_t *s, int i_count )
{
	s->i_left -= i_count;

	while( s->i_left <= 0 )
	{
		s->p++;
		s->i_left += 8;
	}
}

static int	_bs_read_ue( bs_t *s )
{
	int i = 0;

	while( _bs_read1( s ) == 0 && s->p < s->p_end && i < 32 )
	{
		i++;
	}

	return( ( 1 << i) - 1 + _bs_read( s, i ) );
}


static int	_bs_read_se( bs_t *s )
{
	int val = _bs_read_ue( s );

	return val & 0x01 ? (val + 1) / 2 : -(val / 2);
}


static void _scaling_list(bs_t *s,int scalingList,int sizeOfScalingList,int useDefaultScalingMatrixFlag ) 
{ 
	int lastScale = 8 ;
	int nextScale = 8 ;
	int	delta_scale;
	int	scalingListEx[64] = {0};	//这是一个伪数组,在白皮书中,这个数组不应该存在于这里.
	int j = 0;

	for(j = 0; j < sizeOfScalingList; j++ ) { 
		if( nextScale != 0 ) { 
			delta_scale =   _bs_read_se( s );
			nextScale = ( lastScale + delta_scale + 256 ) % 256 ;
			useDefaultScalingMatrixFlag = ( j  ==  0 && nextScale  ==  0 ) ;
		} 
		scalingListEx[ j ] = ( nextScale  ==  0 ) ? lastScale : nextScale ;
		lastScale = scalingListEx[ j ] ;
	} 
} 


static int _read_sps(bs_t *s, uint32_t *width, uint32_t *height)
{
	int i_profile_idc;
	int i_level_idc;

	int b_constraint_set0;
	int b_constraint_set1;
	int b_constraint_set2;
	int id;
	int i_log2_max_frame_num;
	int i_poc_type;
	int i_log2_max_poc_lsb;

	int i_num_ref_frames;
	int b_gaps_in_frame_num_value_allowed;
	int i_mb_width;
	int i_mb_height;

	unsigned char *p_data;

	p_data = s->p;

	if( (p_data[0] == 0x00) && (p_data[1] == 0x00) && (p_data[2] == 0x00) && (p_data[3] == 0x01) )
	{
		p_data += 4;
		s->p += 5;
	}
	else
	{
		if((p_data[0]<<8 | p_data[1]) <= 3)
		{
			return -1;
		}
		else 
		{
			//p_data += 2;
			//s->p += 3;
			s->p += 1;
		}
	}

	if((p_data[0] & 0x0F) != 0x07 )
	{
		return -2;
	}

	//s->p += 3;

	i_profile_idc     = _bs_read( s, 8 );
	b_constraint_set0 = _bs_read( s, 1 );
	b_constraint_set1 = _bs_read( s, 1 );
	b_constraint_set2 = _bs_read( s, 1 );

	_bs_skip( s, 5 );    // reserved
	i_level_idc       = _bs_read( s, 8 );

	


	id = _bs_read_ue( s );			//set_id

	//根据H264白皮书添加by:wuxl   xiao17174@126.com
	if (i_profile_idc == 100 || i_profile_idc == 110 || i_profile_idc == 122 || i_profile_idc == 144)
	{
		int chroma_format_idc;
		int bit_depth_luma_minus8;
		int bit_depth_chroma_minus8;
		int qpprime_y_zero_transform_bypass_flag;
		int seq_scaling_matrix_present_flag;

		chroma_format_idc = _bs_read_ue(s);
		if (chroma_format_idc == 3)
		{
			int residual_colour_transform_flag  = _bs_read( s, 1 );
			(void)residual_colour_transform_flag;
		}
		bit_depth_luma_minus8	= _bs_read_ue(s);
		bit_depth_chroma_minus8 = _bs_read_ue(s);
		qpprime_y_zero_transform_bypass_flag	= _bs_read( s, 1 );
		seq_scaling_matrix_present_flag			= _bs_read( s, 1 );
		if (seq_scaling_matrix_present_flag)
		{
			int seq_scaling_list_present_flag[8] = {0};
			int i = 0;
			for(i = 0; i < 8; i++ ) 
			{ 
				seq_scaling_list_present_flag[i] = _bs_read( s, 1 );
				if( seq_scaling_list_present_flag[i] ) 
				{
					if (i<6)
					{
						_scaling_list(s,0,16,0);
					}
					else
					{
						_scaling_list(s,0,64,0);
					}
				}
			}
		}

		(void)bit_depth_luma_minus8;
		(void)bit_depth_chroma_minus8;
		(void)qpprime_y_zero_transform_bypass_flag;

	}
	//if( _bs_eof( s ) || id >= 32 )
	//{
	//	// the sps is invalid, no need to corrupt sps_array[0]
	//	return -1;
	//}

	i_log2_max_frame_num = _bs_read_ue( s ) + 4;

	i_poc_type = _bs_read_ue( s );
	//i_poc_type = 1;
	if( i_poc_type == 0 )
	{
		i_log2_max_poc_lsb = _bs_read_ue( s ) + 4;
	}
	else if( i_poc_type == 1 )
	{
		int i;

		int b_delta_pic_order_always_zero;
		int i_offset_for_non_ref_pic;
		int i_offset_for_top_to_bottom_field;
		int i_num_ref_frames_in_poc_cycle;
		int i_offset_for_ref_frame [256];

		b_delta_pic_order_always_zero    = _bs_read( s, 1 );
		i_offset_for_non_ref_pic         = _bs_read_se( s );
		i_offset_for_top_to_bottom_field = _bs_read_se( s );
		i_num_ref_frames_in_poc_cycle    = _bs_read_ue( s );
		if( i_num_ref_frames_in_poc_cycle > 256 )
		{
			// FIXME what to do
			i_num_ref_frames_in_poc_cycle = 256;
		}

		for( i = 0; i < i_num_ref_frames_in_poc_cycle; i++ )
		{
			i_offset_for_ref_frame[i] = _bs_read_se( s );
		}

		(void)b_delta_pic_order_always_zero;
		(void)i_offset_for_non_ref_pic;
		(void)i_offset_for_top_to_bottom_field;
		(void)i_offset_for_ref_frame;
	}
	else if( i_poc_type > 2 )
	{
		goto error;
	}

	i_num_ref_frames				   = _bs_read_ue( s );
	b_gaps_in_frame_num_value_allowed = _bs_read( s, 1 );
	i_mb_width  = _bs_read_ue( s ) + 1;
	i_mb_height = _bs_read_ue( s ) + 1;

	*width  = i_mb_width  * 16;
	*height = i_mb_height * 16;

	(void)b_constraint_set0;
	(void)b_constraint_set1;
	(void)b_constraint_set2;
	(void)i_level_idc;
	(void)i_log2_max_frame_num;
	(void)i_log2_max_poc_lsb;
	(void)i_num_ref_frames;
	(void)b_gaps_in_frame_num_value_allowed;

	return id;

error:    
	return -1;
}

int sps_parse(char *sps, int sps_len, uint32_t *width, uint32_t *height)
{
	int ret;
	bs_t s;
	s.p			= (uint8_t *)sps;//在这里,_Resolution是经过BASE64解码后的SPS数据的指针
	s.p_start	= (uint8_t *)sps;
	s.p_end		= (uint8_t *)(sps + sps_len);//size是SPS数据的长度
	s.i_left	= 8;		//这个是固定的.是指SPS的对齐宽度
	ret = _read_sps(&s, width, height);
	
	return ret;
}


#if 0
int parame_prase(char *buf,int buflen)
{
	if(!buf || buflen<=0)
		return FAILURE;
	int post =0;
	printf("<><><><><><><><><><><>buflen=%d<><><><><><><><><><><><><><>\n",buflen);
	int test =0;
	for(test =post;test<buflen+post;test++)
	{
		printf("0x%02x ",buf[test]&0xff);
	}
	printf("\n");
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	
	//configurationVersion 
	unsigned char configurationVersion = buf[post++] & 0xff ;
	//AVCProfileIndication 
	unsigned char AVCProfileIndication = buf[post++] & 0xff ;
	//profile_compatibility
	unsigned char profile_compatibility = buf[post++] & 0xff ;
	//AVCLevelIndication 
	unsigned char AVCLevelIndication = buf[post++] & 0xff ;
	//lengthSizeMinusOne H.264 视频中 NALU 的长度，计算方法是 1 + (lengthSizeMinusOne & 3),实际测试时发现总为ff，计算结果为4，
	unsigned char lengthSizeMinusOne  = 1 + (buf[post++] & 3 );
	//numOfSequenceParameterSets ,SPS 的个数，计算方法是 numOfSequenceParameterSets & 0x1F,实际测试时发现总为E1，计算结果为1
	unsigned char numOfSequenceParameterSets = buf[post++] & 0x1f ;
	//sequenceParameterSetLength ,SPS 的长度
	unsigned char sequenceParameterSetLength = ((buf[post++] & 0xff)<<8|(buf[++post] & 0xff));

	//sequenceParameterSetNALUnits ,sps
	int sps_length = sequenceParameterSetLength;

	printf("<><><><><><><><><><><>sps  sps_length=%d<><><><><><><><><><><><><><>\n",sps_length);
	for(test =post;test<sps_length+post;test++)
	{
		printf("0x%02x ",buf[test]&0xff);
		
	}
	printf("\n");
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");
	char *sps = (char *)malloc(sps_length);
	memset(sps,0x00,sps_length);
	memcpy(sps,&buf[post],sps_length);
	post += sps_length;

	//解析sps
	sps_prase(sps,sps_length);
	
	//numOfPictureParameterSets ,PPS 的个数,计算方法是 numOfPictureParameterSets & 0x1F,实际测试时发现总为E1，计算结果为1
	unsigned char numOfPictureParameterSets = buf[post++] & 0x1f ;
	//pictureParameterSetLength ,PPS 的长度
	unsigned char pictureParameterSetLength = ((buf[post++] & 0xff)<<8|(buf[++post] & 0xff));
	//pps
	int pps_length = pictureParameterSetLength;
	
	char *pps = (char *)malloc(pps_length);
	memset(pps,0x00,pps_length);
	memcpy(pps,&buf[post],pps_length);
	printf("<><><><><><><><><><><>pps  pps_length=%d<><><><><><><><><><><><><><>\n",pps_length);
	for(test =post;test<pps_length+post;test++)
	{
		printf("0x%02x ",buf[test]&0xff);
		
	}
	printf("\n");
	printf("<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n");

	pps_prase(pps,pps_length);

	if(sps)
		free(sps);
	if(pps)
		free(pps);
}

int h264_prase(char *filename)
{
	if(!filename)
		return FAILURE;
	FILE *fin =NULL;
	fin = fopen(filename,"rb");
	if (!fin)
	{
		perror("file open error ");
		exit(1);
	}
	
	char *buf =NULL;
	buf =(char *)malloc(sizeof(char)*512);
	memset(buf,0,sizeof(char)*512);
	int nb_read = fread(buf, sizeof(char), 512, fin);    
	if(nb_read<=0)
		return FAILURE;
	parame_prase(buf,nb_read);

	if(buf)
		free(buf);
}
int main(int argc, char **argv)
{
	if(argc>1)
	{
		if(argv[1])
		{
			char *ptr =NULL;
			char fileExt[256];
			char filename[256];
			
			sprintf(filename,"%s",argv[1]);
			ptr = strrchr(filename+1,'.');
			if(!ptr)
			{
				printf("ptr  ==NULL\n");
				return FAILURE;
			}
			
			int pos = ptr-filename;//用指针相减 求得索引	
		      	substr(filename+1,pos,strlen(filename)-1,fileExt);
			if(!strcmp(fileExt,"h264")||!strcmp(fileExt,"H264"))
			{
				h264_prase(filename);
			}
			else
				return FAILURE;
		}
		else
		{
			return FAILURE;
		}
	}
	else
	{
		return FAILURE;
	}


}
#endif

