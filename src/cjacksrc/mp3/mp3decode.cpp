/*
 * mp3decode.cpp -- the modern build's MP3 decoder in SndDrv.dll (MODERN.md
 * EN-50); see mp3decode.h. #included once, at the end of s_dx8.cpp, in a
 * CJACK_MODERN block.
 *
 * minimp3 (github.com/lieff/minimp3, CC0: minimp3/LICENSE) is compiled in
 * here, without its SSE code (Visual C++ .NET 2003 has no <intrin.h>; the
 * speech, mono at 64 kbit/s, is cheap to decode either way).
 *
 * Streaming. minimp3 decodes one frame from a buffer that must hold the
 * whole frame and the next one's header; out of sync (the start, after a
 * seek) it also checks the frames that follow the one it finds. So a
 * stream holds up to MP3_IN_SIZE compressed bytes and decodes only while it
 * has MP3_IN_MIN of them -- more than the largest layer III frame (1441
 * bytes) several times over -- or the file has no more; the frame's PCM is
 * held until it is read. ACM's MP3 codec did the same buffering inside
 * acmStreamConvert.
 *
 * Modern build only (CJACK_MODERN).
 */
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_NO_SIMD
#include "minimp3/minimp3.h"

#define MP3_IN_SIZE		16384
#define MP3_IN_MIN		8192

struct CJackMp3Stream
{
	mp3dec_t		m_Decoder;
	unsigned int	m_nChannels;

	unsigned char	m_In[MP3_IN_SIZE];		// compressed bytes not decoded yet
	unsigned int	m_nIn;

	short			m_PCM[MINIMP3_MAX_SAMPLES_PER_FRAME];	// one frame, in m_nChannels
	unsigned int	m_nPCMBytes;
	unsigned int	m_nPCMRead;
};

// One frame's samples (nFrameChannels, interleaved) as nChannels; returns
// the samples written (per channel, times nChannels).
static unsigned int Mp3_ToChannels( const short* pIn, int nSamples, int nFrameChannels,
	short* pOut, unsigned int nChannels )
{
	int i;

	if( nSamples <= 0 )
		return 0;
	if( ( unsigned int )nFrameChannels == nChannels )
	{
		memcpy( pOut, pIn, nSamples * nChannels * sizeof( short ));
	}
	else if( nFrameChannels == 1 )		// mono into stereo
	{
		for( i = 0; i < nSamples; ++i )
			pOut[2*i] = pOut[2*i + 1] = pIn[i];
	}
	else								// stereo into mono
	{
		for( i = 0; i < nSamples; ++i )
			pOut[i] = ( short )(( pIn[2*i] + pIn[2*i + 1] ) / 2 );
	}
	return nSamples * nChannels;
}

void Mp3_PCMFormat( const WAVEFORMATEX* pFormat, WAVEFORMATEX* pPCMFormat )
{
	memset( pPCMFormat, 0, sizeof( WAVEFORMATEX ));
	pPCMFormat->wFormatTag = WAVE_FORMAT_PCM;
	pPCMFormat->nChannels = ( pFormat->nChannels >= 2 ) ? 2 : 1;
	pPCMFormat->nSamplesPerSec = pFormat->nSamplesPerSec;
	pPCMFormat->wBitsPerSample = 16;
	pPCMFormat->nBlockAlign = ( WORD )( pPCMFormat->nChannels * 2 );
	pPCMFormat->nAvgBytesPerSec = pPCMFormat->nSamplesPerSec * pPCMFormat->nBlockAlign;
}

bool Mp3_DecodeAll( const WAVEFORMATEX* pFormat, const void* pData, unsigned int uiDataSize,
	WAVEFORMATEX* pPCMFormat, unsigned char** ppPCM, unsigned int* puiPCMSize )
{
	mp3dec_t Decoder;
	mp3dec_frame_info_t Info;
	const unsigned char* pIn;
	unsigned int nIn, nSamples, nMax, nOut;
	short* pOut;
	short Frame[MINIMP3_MAX_SAMPLES_PER_FRAME];

	*ppPCM = NULL;
	*puiPCMSize = 0;
	Mp3_PCMFormat( pFormat, pPCMFormat );

	// The samples, counted from the frames' headers, for the buffer's size.
	mp3dec_init( &Decoder );
	pIn = ( const unsigned char* )pData;
	nIn = uiDataSize;
	nSamples = 0;
	while( nIn > 0 )
	{
		int n = mp3dec_decode_frame( &Decoder, pIn, nIn, NULL, &Info );
		if( Info.frame_bytes <= 0 )
			break;
		nSamples += n;
		pIn += Info.frame_bytes;
		nIn -= Info.frame_bytes;
	}
	if( nSamples == 0 )
		return false;

	nMax = nSamples * pPCMFormat->nChannels;
	LT_MEM_TRACK_ALLOC( *ppPCM = new unsigned char[ nMax * sizeof( short ) ], LT_MEM_TYPE_SOUND );
	if( !*ppPCM )
		return false;
	pOut = ( short* )*ppPCM;

	mp3dec_init( &Decoder );
	pIn = ( const unsigned char* )pData;
	nIn = uiDataSize;
	nOut = 0;
	while( nIn > 0 )
	{
		int n = mp3dec_decode_frame( &Decoder, pIn, nIn, Frame, &Info );
		if( Info.frame_bytes <= 0 )
			break;
		pIn += Info.frame_bytes;
		nIn -= Info.frame_bytes;
		if( n > 0 && nOut + ( unsigned int )n * pPCMFormat->nChannels <= nMax )
			nOut += Mp3_ToChannels( Frame, n, Info.channels, pOut + nOut, pPCMFormat->nChannels );
	}
	*puiPCMSize = nOut * sizeof( short );
	return true;
}

CJackMp3Stream* Mp3Stream_Create( unsigned int nChannels )
{
	CJackMp3Stream* pStream;

	LT_MEM_TRACK_ALLOC( pStream = new CJackMp3Stream, LT_MEM_TYPE_SOUND );
	if( !pStream )
		return NULL;
	pStream->m_nChannels = ( nChannels >= 2 ) ? 2 : 1;
	Mp3Stream_Reset( pStream );
	return pStream;
}

void Mp3Stream_Destroy( CJackMp3Stream* pStream )
{
	delete pStream;
}

void Mp3Stream_Reset( CJackMp3Stream* pStream )
{
	mp3dec_init( &pStream->m_Decoder );
	pStream->m_nIn = 0;
	pStream->m_nPCMBytes = 0;
	pStream->m_nPCMRead = 0;
}

unsigned int Mp3Stream_InputRoom( CJackMp3Stream* pStream )
{
	return MP3_IN_SIZE - pStream->m_nIn;
}

unsigned char* Mp3Stream_InputEnd( CJackMp3Stream* pStream )
{
	return pStream->m_In + pStream->m_nIn;
}

void Mp3Stream_InputAdded( CJackMp3Stream* pStream, unsigned int nBytes )
{
	pStream->m_nIn += nBytes;
}

unsigned int Mp3Stream_Read( CJackMp3Stream* pStream, unsigned char* pDest, unsigned int nBytes,
	bool bEnd, unsigned int* puiDecoded )
{
	unsigned int nOut = 0;

	while( nOut < nBytes )
	{
		mp3dec_frame_info_t Info;
		short Frame[MINIMP3_MAX_SAMPLES_PER_FRAME];
		int n;
		unsigned int nUsed;

		// the decoded frame first
		if( pStream->m_nPCMRead < pStream->m_nPCMBytes )
		{
			unsigned int nCopy = pStream->m_nPCMBytes - pStream->m_nPCMRead;
			if( nCopy > nBytes - nOut )
				nCopy = nBytes - nOut;
			if( pDest )
				memcpy( pDest + nOut, ( unsigned char* )pStream->m_PCM + pStream->m_nPCMRead, nCopy );
			pStream->m_nPCMRead += nCopy;
			nOut += nCopy;
			continue;
		}

		// then the next frame, when there is enough to find it
		if( pStream->m_nIn == 0 || ( !bEnd && pStream->m_nIn < MP3_IN_MIN ))
			break;

		n = mp3dec_decode_frame( &pStream->m_Decoder, pStream->m_In, pStream->m_nIn, Frame, &Info );
		nUsed = ( Info.frame_bytes > 0 ) ? ( unsigned int )Info.frame_bytes : 0;
		if( nUsed == 0 || nUsed > pStream->m_nIn )
			nUsed = pStream->m_nIn;		// nothing more to find in it
		memmove( pStream->m_In, pStream->m_In + nUsed, pStream->m_nIn - nUsed );
		pStream->m_nIn -= nUsed;

		pStream->m_nPCMRead = 0;
		pStream->m_nPCMBytes = Mp3_ToChannels( Frame, n, Info.channels, pStream->m_PCM,
			pStream->m_nChannels ) * sizeof( short );
		*puiDecoded += pStream->m_nPCMBytes;
	}
	return nOut;
}
