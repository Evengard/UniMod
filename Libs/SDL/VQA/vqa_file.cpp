//#include "stdafx.h"

#define XCC
#include "vqa_file.h"

//#include <vfw.h>
//#include "image_file.h"
//#include "pcx_decode.h"
//#include "string_conversion.h"
#include "vqa_decode.h"
//#include "wav_structures.h"
//#include "xcc_log.h"


Cvqa_file::Cvqa_file(string filename)
{
	fileStream.open(filename, ios::in | ios::binary);
	bool success = fileStream.good();
	fileStream.seekg(0, ios::end);
	success = fileStream.good();
	fileSize = fileStream.tellg();
	success = fileStream.good();
	fileStream.seekg(0, ios::beg);
	success = fileStream.good();
	currentPosition = fileStream.tellg();
	success = fileStream.good();
}

bool Cvqa_file::is_valid()
{
	int size = get_size(); // Filesize
	/*
	if (get_data())
	memcpy(&m_header, get_data(), sizeof(t_vqa_header));
	*/
	return !(sizeof(t_vqa_header) > size ||
		m_header.file_header.id != vqa_file_id ||
		m_header.id != vqa_form_id);
}

Cvqa_file::~Cvqa_file()
{
	fileStream.close();
	fileStream.clear();
}

int Cvqa_file::post_open()
{
	int error = read(&m_header, sizeof(t_vqa_header)); // Read header
	return error ? error : read_chunk_header();
}

struct t_list_entry
{
	int c_samples;
	short* audio;
};

int Cvqa_file::extract_both(const string& name)
{
	int error = 0;
	Cvqa_decode vqa_d;
	vqa_d.start_decode(header());
	int cx = get_cx();
	int cy = get_cy();
	DDPIXELFORMAT_VQA pf;
	pf.dwRGBAlphaBitMask = 0;
	pf.dwRBitMask = 0x0000ff;
	pf.dwGBitMask = 0x00ff00;
	pf.dwBBitMask = 0xff0000;
	vqa_d.set_pf(pf, 3);
	int cs_remaining = 0;

	ofstream audio;
	audio.open(name + ".wav", ios::binary | ios::trunc);
	t_wav_header header;
	memset(&header, 0, sizeof(t_wav_header));
	audio.write((char*)&header, sizeof(t_wav_header));

	
	byte* frame = new byte[3 * cx * cy];

	int currentFrame = 0;

	while (currentFrame < get_c_frames())
	{
		if (get_chunk_id() == vqa_vqfl_id)
		{
			byte* data = new byte[get_chunk_size()];
			read_chunk(data);
			vqa_d.decode_vqfl_chunk(data, get_chunk_size());
		}
		else if (is_video_chunk())
		{
			byte* data = new byte[get_chunk_size()];
			read_chunk(data);
			vqa_d.decode_vqfr_chunk(data, frame, NULL);
			delete[] data;

			string filename = name + "_" + to_string(currentFrame) + ".bmp";
			bitmap_image bmp(cx, cy);
			byte* red = new byte[cx*cy];
			byte* green = new byte[cx*cy];
			byte* blue = new byte[cx*cy];
			for (int j = 0; j < cx * cy; j++)
			{
				red[j] = frame[j * 3];
				green[j] = frame[j * 3 + 1];
				blue[j] = frame[j * 3 + 2];
			}
			bmp.import_rgb(red, green, blue);
			bmp.save_image(filename);
			delete[] red;
			delete[] green;
			delete[] blue;
			// Raw frame in here (frame)

			currentFrame++;
		}
		else if (is_audio_chunk())
		{
			t_list_entry e;
			int size = get_chunk_size();
			if (get_chunk_id() >> 24 == '0')
			{
				e.c_samples = size >> 1;
				e.audio = new short[size / 2];
				read_chunk(e.audio);
				size /= 4;
			}
			else
			{
				e.c_samples = size << 1;
				e.audio = new short[2 * size];
				byte* data = new byte[size];
				read_chunk(data);
				vqa_d.decode_snd2_chunk(data, size, e.audio);
			}
			cs_remaining += e.c_samples;
			audio.write((char*)e.audio, 2 * e.c_samples);
		}
		else
		{
			skip_chunk();
		}
	}
	delete[] frame;

	audio.seekp(0, ios::beg);
	header.file_header.id = wav_file_id;
	header.file_header.size = sizeof(header) - sizeof(header.file_header) + cs_remaining;
	header.form_type = wav_form_id;
	header.format_chunk.header.id = wav_format_id;
	header.format_chunk.header.size = sizeof(header.format_chunk) - sizeof(header.format_chunk.header);
	header.format_chunk.formattag = 1;
	header.format_chunk.c_channels = get_c_channels();
	header.format_chunk.samplerate = get_samplerate();
	header.format_chunk.byterate = 2 * get_c_channels() * get_samplerate();
	header.format_chunk.blockalign = 2 * get_c_channels();
	header.format_chunk.cbits_sample = 16;
	header.data_chunk_header.id = wav_data_id;
	header.data_chunk_header.size = cs_remaining << 1;
	audio.write((char*)&header, sizeof(t_wav_header));

	audio.close();


	return error;
}

int Cvqa_file::extract_as_pcx(const string& name)
{
	int error = 0;
	Cvqa_decode vqa_d;
	vqa_d.start_decode(header());
	int cx = get_cx();
	int cy = get_cy();
	if (get_cbits_pixel() == 8)
	{
		t_palet palet;
		byte* frame = new byte[cx * cy];
		for (int i = 0; i < get_c_frames(); i++)
		{
			while (!is_video_chunk())
				skip_chunk();

			byte* data = new byte[get_chunk_size()];
			read_chunk(data);
			vqa_d.decode_vqfr_chunk(data, frame, palet);
			delete[] data;
		}
		delete[] frame;
	}
	else
	{
		DDPIXELFORMAT_VQA pf;
		pf.dwRGBAlphaBitMask = 0;
		pf.dwRBitMask = 0x0000ff;
		pf.dwGBitMask = 0x00ff00;
		pf.dwBBitMask = 0xff0000;
		vqa_d.set_pf(pf, 3);
		byte* frame = new byte[3 * cx * cy];

		for (int i = 0; i < get_c_frames(); i++)
		{
			if (get_chunk_id() == vqa_vqfl_id)
			{
				byte* data = new byte[get_chunk_size()];
				read_chunk(data);
				vqa_d.decode_vqfl_chunk(data, get_chunk_size());
			}
			while (!is_video_chunk())
				skip_chunk();
			byte* data = new byte[get_chunk_size()];
			read_chunk(data);
			vqa_d.decode_vqfr_chunk(data, frame, NULL);

			string filename = name + "_" + to_string(i) + ".bmp";
			bitmap_image bmp(cx, cy);
			byte* red = new byte[cx*cy];
			byte* green = new byte[cx*cy];
			byte* blue = new byte[cx*cy];
			for (int j = 0; j < cx * cy; j++)
			{
				red[j] = frame[j * 3];
				green[j] = frame[j * 3 + 1];
				blue[j] = frame[j * 3 + 2];
			}
			bmp.import_rgb(red, green, blue);
			bmp.save_image(filename);
			delete[] red;
			delete[] green;
			delete[] blue;
			// Raw frame in here (frame)


			delete[] data;
		}
		delete[] frame;
	}
	return error;
}

int Cvqa_file::extract_as_wav(const string& name)
{
	int error = 0;
	typedef vector<t_list_entry> t_list;
	t_list list;
	int cs_remaining = 0;	
	Cvqa_decode vqa_d;
	vqa_d.start_decode(header());
	for (int i = 0; i < get_c_frames(); i++)
	{
		while (1)
		{
			if (is_audio_chunk())
			{
				t_list_entry e;
				int size = get_chunk_size();
				if (get_chunk_id() >> 24 == '0')
				{
					e.c_samples = size >> 1;
					e.audio = new short[size / 2];
					read_chunk(e.audio);
					size /= 4;
				}
				else
				{
					e.c_samples = size << 1;
					e.audio = new short[2 * size];
					byte* data = new byte[size];
					read_chunk(data);
					vqa_d.decode_snd2_chunk(data, size, e.audio);
				}
				cs_remaining += e.c_samples;
				list.push_back(e);				
			}
			else if (is_video_chunk())
				break;				
			else
				skip_chunk();
		}
		skip_chunk();
	}

	// We've got RAW PCM here in list - one chunk per list item.

	ofstream file;
	file.open(name, ios::binary | ios::trunc);
	t_wav_header header;
	memset(&header, 0, sizeof(t_wav_header));
	header.file_header.id = wav_file_id;
	header.file_header.size = sizeof(header) - sizeof(header.file_header) + cs_remaining;
	header.form_type = wav_form_id;
	header.format_chunk.header.id = wav_format_id;
	header.format_chunk.header.size = sizeof(header.format_chunk) - sizeof(header.format_chunk.header);
	header.format_chunk.formattag = 1;
	header.format_chunk.c_channels = get_c_channels();
	header.format_chunk.samplerate = get_samplerate();
	header.format_chunk.byterate = 2 * get_c_channels() * get_samplerate();
	header.format_chunk.blockalign = 2 * get_c_channels();
	header.format_chunk.cbits_sample = 16;
	header.data_chunk_header.id = wav_data_id;
	header.data_chunk_header.size = cs_remaining << 1;
	file.write((char*)&header, sizeof(t_wav_header));
	for (auto& i : list)
	{
		file.write((char*)i.audio, 2 * i.c_samples);
		delete[] i.audio;
	}
	file.close();


	/*Cfile32 f;
	error = f.open(name, GENERIC_WRITE);
	if (!error)
	{
		t_wav_header header;
		memset(&header, 0, sizeof(t_wav_header));
		header.file_header.id = wav_file_id;
		header.file_header.size = sizeof(header) - sizeof(header.file_header) + cs_remaining;
		header.form_type = wav_form_id;
		header.format_chunk.header.id = wav_format_id;
		header.format_chunk.header.size = sizeof(header.format_chunk) - sizeof(header.format_chunk.header);
		header.format_chunk.formattag = 1;
		header.format_chunk.c_channels = get_c_channels();
		header.format_chunk.samplerate = get_samplerate();
		header.format_chunk.byterate = 2 * get_c_channels() * get_samplerate();
		header.format_chunk.blockalign = 2 * get_c_channels();
		header.format_chunk.cbits_sample = 16;
		header.data_chunk_header.id = wav_data_id;
		header.data_chunk_header.size = cs_remaining << 1;
		error = f.write(&header, sizeof(t_wav_header));
		if (!error)
		{
			for (auto& i : list)
			{
				f.write(i.audio, 2 * i.c_samples);
				delete[] i.audio;
			}
		}
		f.close();
	}*/
	return error;
}

int Cvqa_file::read_chunk_header()
{
	if (get_p() & 1) // Get position (seek)
		skip(1); // Skip 1 byte
	int error = read(&m_chunk_header, sizeof(t_vqa_chunk_header));
	m_chunk_header.size = reverse(m_chunk_header.size);
	return error;
}

int Cvqa_file::read_chunk(void* data)
{
	int error = read(data, get_chunk_size());
	return error ? error : read_chunk_header();
}

/*Cvirtual_binary Cvqa_file::read_chunk()
{
	Cvirtual_binary d;
	return read_chunk(d.write_start(get_chunk_size())) ? Cvirtual_binary() : d;
}*/

void Cvqa_file::set_empty_chunk()
{
	m_chunk_header.size = 0;
}

int Cvqa_file::skip_chunk()
{
	skip(get_chunk_size()); // Skip seek
	return read_chunk_header();	
}