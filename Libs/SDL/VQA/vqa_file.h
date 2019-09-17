#pragma once

// To avoid excessive includes in runtime. That's dirty, but who cares.

#include "CommonHeader.h"

#include <fstream>
#include <iostream>
#include <string>

#pragma pack(push, 1)

const int wav_file_id = *(int*)"RIFF";
const int wav_form_id = *(int*)"WAVE";
const int wav_format_id = *(int*)"fmt ";
const int wav_fact_id = *(int*)"fact";
const int wav_data_id = *(int*)"data";

struct t_wav_chunk_header
{
	int id;
	int size;
};

struct t_wav_format_chunk
{
	t_wav_chunk_header header;
	__int16 formattag;
	__int16 c_channels;
	__int32 samplerate;
	__int32 byterate;
	__int16 blockalign;
	__int16 cbits_sample;
};

struct t_wav_header
{
	t_wav_chunk_header file_header;
	int form_type;
	t_wav_format_chunk format_chunk;
	t_wav_chunk_header data_chunk_header;
};

struct t_palet_entry
{
	byte r;
	byte g;
	byte b;
};

struct t_palet24rgb_entry
{
	byte r, g, b;
};

struct t_palet24bgr_entry
{
	byte b, g, r;
};

union t_palet32entry
{
	struct
	{
		byte r, g, b, a;
	};
	unsigned __int32 v;
};

union t_palet32bgr_entry
{
	struct
	{
		byte b, g, r, a;
	};
	unsigned __int32 v;
};

typedef t_palet_entry t_palet[256];

enum t_game
{
	game_ts,
	game_td,
};

struct t_vqa_chunk_header
{
	__int32 id;
	__int32 size;
};

struct t_vqa_header
{
	t_vqa_chunk_header file_header;
	// 'FORM'
	__int64 id;
	// 'WVQAVQHD'
	__int32 startpos;
	__int16 version;
	__int16 video_flags;
	__int16 c_frames;
	__int16 cx;
	__int16 cy;
	__int8 cx_block;
	__int8 cy_block;
	__int16 fps;
	__int16 c_colors;
	__int16 cb_max_cbp_chunk;
	__int32 unknown4;
	__int16 unknown5;
	unsigned __int16 samplerate;
	__int8 c_channels;
	__int8 cbits_sample;
	__int8 unknown6[14];
};



const __int32 vqa_c_mask = 0xff000000;
const __int32 vqa_t_mask = 0x00ffffff;
const __int32 vqa_file_id = *(__int32*)"FORM";
const __int64 vqa_form_id = { *(__int64*)"WVQAVQHD" };
const __int32 vqa_cbf_id = *(__int32*)"CBF\0";
const __int32 vqa_cbp_id = *(__int32*)"CBP\0";
const __int32 vqa_cpl_id = *(__int32*)"CPL\0";
const __int32 vqa_finf_id = *(__int32*)"FINF";
const __int32 vqa_sn2j_id = *(__int32*)"SN2J";
const __int32 vqa_snd_id = *(__int32*)"SND\0";
const __int32 vqa_vpt_id = *(__int32*)"VPT\0";
const __int32 vqa_vpr_id = *(__int32*)"VPR\0";
const __int32 vqa_vqfl_id = *(__int32*)"VQFL";
const __int32 vqa_vqfr_id = *(__int32*)"VQFR";

#pragma pack(pop)

inline __int32 reverse(__int32 v)
{
    __int32 v2 = v;
    byte* vp = (byte*)& v;
    byte* v2p = (byte*)& v2;
    vp[0] = v2p[3];
    vp[1] = v2p[2];
    vp[2] = v2p[1];
    vp[3] = v2p[0];
	return v;
}

// To avoid excessive includes in runtime. That's dirty, but who cares.
#ifdef XCC
#include "bitmap_image.hpp"
#include "vqa_decode.h"
#endif

#include <Windows.h>
#include <InitGuid.h>
#include <Guiddef.h>
#include <dsound.h>

class Cvqa_file
{
public:
	Cvqa_file(string filename);
	~Cvqa_file();
	int post_open();
	int extract_as_pcx(const string& name);
	int extract_as_wav(const string& name);
	int extract_both();
	int read_chunk_header();
	int read_chunk(void* data);
	void set_empty_chunk();
	int skip_chunk();
	bool is_valid();

	void register_decode(int(*cb)(byte* frame, dword cx, dword cy))
	{
		decodeCallback = cb;
	}
	
	void register_dsound(LPDIRECTSOUND* dsound)
	{
		extDsoundObj = dsound;
	}

	size_t get_size()
	{
		return fileSize;
	}

	size_t get_p()
	{
		return currentPosition;
	}

	void skip(size_t amount)
	{
		size_t finalpos = currentPosition + amount;
		
		fileStream.seekg(finalpos);
		currentPosition = fileStream.tellg();
	}

	int read(void* out, size_t size)
	{
		fileStream.read((char*)out, size);
		currentPosition = (int)currentPosition + size;
		if ((size_t)fileStream.tellg() != currentPosition)
		{
			return 1;
		}
		return 0;
	}

	int get_c_channels() const
	{
		return m_header.c_channels;
	}

	int get_c_frames() const
	{
		return m_header.c_frames;
	}

	int get_chunk_id() const
	{
		return m_chunk_header.id;
	}

	int get_chunk_size() const
	{
		return m_chunk_header.size;
	}

	int get_cx() const
	{
		return m_header.cx;
	}

	int get_cy() const
	{
		return m_header.cy;
	}

	int get_cx_block() const
	{
		return m_header.cx_block;
	}

	int get_cy_block() const
	{
		return m_header.cy_block;
	}

	const t_vqa_header& header() const
	{
		return m_header;
	}

	int get_samplerate() const
	{
		return m_header.samplerate;
	}

	bool is_audio_chunk() const
	{
		return (get_chunk_id() & vqa_t_mask) == vqa_snd_id;
	}

	bool is_video_chunk() const
	{
		return (get_chunk_id()) == vqa_vqfr_id;
	}

	int get_cbits_pixel() const
	{
		return m_header.video_flags & 0x10 ? 16 : 8;
	}

	int get_cbits_sample() const
	{
		return m_header.cbits_sample;
	}
private:
	t_vqa_chunk_header m_chunk_header;
	t_vqa_header m_header;
	ifstream fileStream;
	size_t fileSize;
	size_t currentPosition;
	LPDIRECTSOUND* extDsoundObj;
	int(*decodeCallback)(byte* frame, dword cx, dword cy);
};