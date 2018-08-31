

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */
#define	BIAS		(0x84)		/* Bias for linear code. */
#define CLIP         8159

#ifndef NULL
#define NULL             0L
#endif

typedef enum {
	PCM_ALAW=0,
	PCM_ULAW,
	MPEG2_AUDIO,
	MPEG4_AUDIO
}AUD_ENC_MODE;

#define AUDIOALARM_THRESHOLD	(1)



/********************************G711 A/U编码函数********************/
static short seg_aend[8] = {0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF};
static short seg_uend[8] = {0x3F, 0x7F, 0xFF, 0x1FF,0x3FF, 0x7FF, 0xFFF, 0x1FFF};

static short HH_AENC_SEARCH(short val, short *table, short size)
{
	unsigned short	i = 0;

	for (i = 0; i < size; i++) 
	{
		if (val <= *table++)
			return (i);
	}
	return (size);
}

unsigned char HH_AENC_LINEAR2ALAW(short pcm_val)	/* 2's complement (16-bit range) */
{
	short		mask;
	short		seg;
	unsigned char	aval;

	pcm_val = pcm_val >> 3;

	if (pcm_val >= 0) 
	{
		mask = 0xD5;		/* sign (7th) bit = 1 */
	} 
	else
	{
		mask = 0x55;		/* sign bit = 0 */
		pcm_val = -pcm_val - 1;
	}

	/* Convert the scaled magnitude to segment number. */
	seg = HH_AENC_SEARCH(pcm_val, seg_aend, 8);

	/* Combine the sign, segment, and quantization bits. */

	if (seg >= 8)		/* out of range, return maximum value. */
		return (unsigned char) (0x7F ^ mask);
	else 
	{
		aval = (unsigned char) seg << SEG_SHIFT;
		if (seg < 2)
			aval |= (pcm_val >> 1) & QUANT_MASK;
		else
			aval |= (pcm_val >> seg) & QUANT_MASK;
		return (aval ^ mask);
	}
	return 0;
}

int HH_AENC_G711AEncode(char * pInData, int nInLen, char * pOutData, int *pOutLen)
{
	if ( pInData == NULL || nInLen%2 != 0 || nInLen <= 0 || pOutData == NULL || pOutLen == NULL)
	{
		return -1 ;
	}

	short *		pRawData	=	(short*) pInData ;
	int			nRawLen		=	nInLen/2 ;
	int         i = 0;
	for ( i = 0 ; i < nRawLen ; i++ )
	{
		pOutData [i] = HH_AENC_LINEAR2ALAW(pRawData[i]) ;		
	}

	*pOutLen = nRawLen ;

	return nRawLen ;
}

unsigned char HH_AENC_LINEAR2ULAW(short	pcm_val)	/* 2's complement (16-bit range) */
{
	short		mask;
	short		seg;
	unsigned char	uval;

	/* Get the sign and the magnitude of the value. */
	pcm_val = pcm_val >> 2;
	if (pcm_val < 0) 
	{
		pcm_val = -pcm_val;
		mask = 0x7F;
	} 
	else
	{
		mask = 0xFF;
	}

	if ( pcm_val > CLIP ) 
		pcm_val = CLIP;		/* clip the magnitude */
	pcm_val += (BIAS >> 2);

	/* Convert the scaled magnitude to segment number. */
	seg = HH_AENC_SEARCH(pcm_val, seg_uend, 8);
	/*
	 * Combine the sign, segment, quantization bits;
	 * and complement the code word.
	 */
	if (seg >= 8)		/* out of range, return maximum value. */
		return (unsigned char) (0x7F ^ mask);
	else 
	{
		uval = (unsigned char) (seg << 4) | ((pcm_val >> (seg + 1)) & 0xF);
		return (uval ^ mask);
	}
	return 0; //should never go here;
}

int HH_AENC_G711UEncode(char * pInData, int nInLen, char * pOutData,int *pOutLen)
{
	if ( pInData == NULL || nInLen%2 != 0 || nInLen <= 0 || pOutData == NULL || pOutLen == NULL)
	{
		return -1 ;
	}

	short *		pRawData	=	(short*) pInData ;
	int			nRawLen		=	nInLen/2 ;
	int         i           =   0;
	
	for (i = 0 ; i < nRawLen ; i++ )
	{
		pOutData [i] = HH_AENC_LINEAR2ULAW(pRawData [i]) ;		
	}

	*pOutLen = nRawLen ;

	return nRawLen ;
}


/**********************************G711 解码函数 *************************/
short HH_ADEC_ALAW2LINERA(unsigned char	a_val)
{
	short		t;
	short		seg;

	a_val ^= 0x55;

	t = (a_val & QUANT_MASK) << 4;
	seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
	
	switch (seg) 
	{
		case 0:
			t += 8;
			break;
		case 1:
			t += 0x108;
			break;
		default:
			t += 0x108;
			t <<= seg - 1;
	}
	return ((a_val & SIGN_BIT) ? t : -t);
}
/** 对 G711a 数据解码为线性 PCM 数据
*  @param[in] pInData		G711a 数据
*  @param[in] nInLen		G711a 数据长度
*  @param[out] pOutData		解码后的 PCM 数据
*  @param[out] nOutLen		解码后的 PCM 数据的长度
*  @return 返回结果
*		-1					失败
*		> 0					解码后的数据长度
*/
int HH_ADEC_G711ADecode(char * pInData, int nInLen, char * pOutData, int *pOutLen)
{
	if ( pInData == NULL || nInLen <= 0 || pOutData == NULL || pOutLen == NULL)
	{
		return -1 ;
	}

	short *pDecodeData = (short*) pOutData ;
	int i = 0;
	
	for (i = 0 ; i < nInLen ; i++ )
	{
		pDecodeData [i]	= HH_ADEC_ALAW2LINERA(pInData [i]) ;
	}

	*pOutLen = 2 * nInLen ;

	return *pOutLen ;
}

short HH_ADEC_UALAW2LINERA(unsigned char	u_val)
{
	short		t;
	/* Complement to obtain normal u-law value. */
	u_val = ~u_val;

	/*
	 * Extract and bias the quantization bits. Then
	 * shift up by the segment number and subtract out the bias.
	 */
	t = ((u_val & QUANT_MASK) << 3) + BIAS;
	t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

	return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}

/** 对 G711u 数据解码为线性 PCM 数据
*  @param[in] pInData		G711u 数据
*  @param[in] nInLen		G711u 数据长度
*  @param[out] pOutData		解码后的 PCM 数据
*  @param[out] nOutLen		解码后的 PCM 数据的长度
*  @return 返回结果
*		-1					失败
*		> 0					解码后的数据长度
*/
int HH_ADEC_G711UDecode(char * pInData, int nInLen, char * pOutData, int *nOutLen)
{
	if ( pInData == NULL || nInLen <= 0 || pOutData == NULL || nOutLen == NULL)
	{
		return -1 ;
	}
	
	short *		pDecodeData = (short*) pOutData ;
 	int i = 0;
	
	for (; i < nInLen ; i++ )
	{
		pDecodeData [i]	= HH_ADEC_UALAW2LINERA( pInData [i] ) ;
	}

	*nOutLen = 2 * nInLen ;

	return *nOutLen ;	
}


int ALG_audAlarmCheck(unsigned char *inpBuf, int numSamples, int alarmLevel)
{
    unsigned short i;
	int audAlarm = 0;
	int sampLimit = (alarmLevel*32767)/100;
	short *src = (short *)inpBuf;
    short data, alarmcount=0;
    unsigned short isNegative;

	//sampLimit = (sampLimit*50)/100;

    for (i = 0; i < numSamples; i++)
    {
        data = src[i];
        isNegative = (data < 0) ? 1 : 0;
        if (isNegative)
            data = -data;

		if(data>=sampLimit)
			alarmcount++;
	}

#ifdef ALG_AUD_ENC_DEBUG
	OSA_printf("AUDIO ALARM DEBUG MESSAGE: samples(%d) level(%d) samplimit(%d) count(%d)\n",
					numSamples, alarmLevel, sampLimit, alarmcount);
#endif

	if(alarmcount>=AUDIOALARM_THRESHOLD)
		audAlarm = 1;

	return audAlarm;
}



