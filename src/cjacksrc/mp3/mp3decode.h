/*
 * mp3decode.h -- the modern build's MP3 decoder in SndDrv.dll (MODERN.md
 * EN-50).
 *
 * The characters' speech is MP3 in WAV files (WAVE_FORMAT_MPEGLAYER3, 0x55:
 * 661 of Sound.rez's 686 VOICE/ files). Retail's SndDrv.dll decodes it
 * through Windows' Audio Compression Manager, with whatever MP3 codec the
 * system has; under Wine that is winemp3.acm, which plays the streamed
 * lines as a drone of their first samples. The modern build decodes MP3
 * itself, with minimp3 (minimp3/, CC0), so no codec is needed. s_dx8.cpp
 * calls these from CJACK_MODERN blocks, in place of its acmStream* calls
 * for 0x55; IMA ADPCM still goes through ACM, as in retail.
 *
 * The output is what retail asked ACM for: 16-bit PCM at the file's rate,
 * with the file's channels (a frame with other channels is mixed down or
 * doubled to them).
 *
 * mp3decode.cpp, the implementation, is #included at the end of
 * s_dx8.cpp (SndDrv's project file is the retail one, so a modern source
 * of its own would have no place in it; and minimp3's macros stay out of
 * the retail code above it).
 *
 * Modern build only (CJACK_MODERN).
 */
#ifndef CJACK_MP3DECODE_H
#define CJACK_MP3DECODE_H

// A sound loaded whole: the MP3 data of a WAV file (its format and its
// data chunk) decoded at once. On success *ppPCM is the PCM, allocated with
// new unsigned char[] (as retail's ACM buffer was: the callers delete[]
// it), *puiPCMSize its size in bytes and *pPCMFormat its format.
bool Mp3_DecodeAll( const WAVEFORMATEX* pFormat, const void* pData, unsigned int uiDataSize,
	WAVEFORMATEX* pPCMFormat, unsigned char** ppPCM, unsigned int* puiPCMSize );

// The PCM format Mp3_DecodeAll and a stream give for an MP3 format.
void Mp3_PCMFormat( const WAVEFORMATEX* pFormat, WAVEFORMATEX* pPCMFormat );

// A streamed sound's decoder (WaveFile::ReadCompressed): the file's
// compressed bytes go in as it reads them, PCM comes out.
struct CJackMp3Stream;

CJackMp3Stream* Mp3Stream_Create( unsigned int nChannels );
void Mp3Stream_Destroy( CJackMp3Stream* pStream );
// Back to the start of a file (WaveFile::Cue), or a new place in it: the
// bytes held, compressed or decoded, are dropped.
void Mp3Stream_Reset( CJackMp3Stream* pStream );
// Room for compressed bytes: write up to Mp3Stream_InputRoom bytes at
// Mp3Stream_InputEnd, then say how many with Mp3Stream_InputAdded.
unsigned int Mp3Stream_InputRoom( CJackMp3Stream* pStream );
unsigned char* Mp3Stream_InputEnd( CJackMp3Stream* pStream );
void Mp3Stream_InputAdded( CJackMp3Stream* pStream, unsigned int nBytes );
// Up to nBytes of PCM into pDest (NULL: dropped). Decodes only while it
// holds enough compressed bytes for a frame to be found and checked, unless
// bEnd says the file has no more; returns the bytes given, which is less
// than nBytes when it wants more input (or, with bEnd, at the end). Adds
// the bytes it decodes to *puiDecoded.
unsigned int Mp3Stream_Read( CJackMp3Stream* pStream, unsigned char* pDest, unsigned int nBytes,
	bool bEnd, unsigned int* puiDecoded );

#endif
