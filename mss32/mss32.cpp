#include <windows.h>
#pragma pack(1)


HINSTANCE hLThis = 0;
HINSTANCE hL = 0;
HINSTANCE hUniMod = 0;
FARPROC p[316] = { 0 };

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		hLThis = hInst;
		hL = LoadLibrary(L".\\mss32_orig.dll");
		if (!hL) return false;


		p[0] = GetProcAddress(hL, "AIL_debug_printf");
		p[1] = GetProcAddress(hL, "AIL_sprintf");
		p[2] = GetProcAddress(hL, "DLSClose");
		p[3] = GetProcAddress(hL, "DLSCompactMemory");
		p[4] = GetProcAddress(hL, "DLSGetInfo");
		p[5] = GetProcAddress(hL, "DLSLoadFile");
		p[6] = GetProcAddress(hL, "DLSLoadMemFile");
		p[7] = GetProcAddress(hL, "DLSMSSOpen");
		p[8] = GetProcAddress(hL, "DLSSetAttribute");
		p[9] = GetProcAddress(hL, "DLSUnloadAll");
		p[10] = GetProcAddress(hL, "DLSUnloadFile");
		p[11] = GetProcAddress(hL, "RIB_alloc_provider_handle");
		p[12] = GetProcAddress(hL, "RIB_enumerate_interface");
		p[13] = GetProcAddress(hL, "RIB_error");
		p[14] = GetProcAddress(hL, "RIB_find_file_provider");
		p[15] = GetProcAddress(hL, "RIB_free_provider_handle");
		p[16] = GetProcAddress(hL, "RIB_free_provider_library");
		p[17] = GetProcAddress(hL, "RIB_load_provider_library");
		p[18] = GetProcAddress(hL, "RIB_register_interface");
		p[19] = GetProcAddress(hL, "RIB_request_interface");
		p[20] = GetProcAddress(hL, "RIB_request_interface_entry");
		p[21] = GetProcAddress(hL, "RIB_type_string");
		p[22] = GetProcAddress(hL, "RIB_unregister_interface");
		p[23] = GetProcAddress(hL, "_AIL_3D_auto_update_position@8");
		p[24] = GetProcAddress(hL, "_AIL_3D_orientation@28");
		p[25] = GetProcAddress(hL, "_AIL_3D_position@16");
		p[26] = GetProcAddress(hL, "_AIL_3D_provider_attribute@12");
		p[27] = GetProcAddress(hL, "_AIL_3D_sample_attribute@12");
		p[28] = GetProcAddress(hL, "_AIL_3D_sample_distances@20");
		p[29] = GetProcAddress(hL, "_AIL_3D_sample_float_distances@20");
		p[30] = GetProcAddress(hL, "_AIL_3D_sample_length@4");
		p[31] = GetProcAddress(hL, "_AIL_3D_sample_loop_count@4");
		p[32] = GetProcAddress(hL, "_AIL_3D_sample_offset@4");
		p[33] = GetProcAddress(hL, "_AIL_3D_sample_playback_rate@4");
		p[34] = GetProcAddress(hL, "_AIL_3D_sample_status@4");
		p[35] = GetProcAddress(hL, "_AIL_3D_sample_volume@4");
		p[36] = GetProcAddress(hL, "_AIL_3D_update_position@8");
		p[37] = GetProcAddress(hL, "_AIL_3D_user_data@8");
		p[38] = GetProcAddress(hL, "_AIL_3D_velocity@16");
		p[39] = GetProcAddress(hL, "_AIL_DLS_close@8");
		p[40] = GetProcAddress(hL, "_AIL_DLS_compact@4");
		p[41] = GetProcAddress(hL, "_AIL_DLS_get_info@12");
		p[42] = GetProcAddress(hL, "_AIL_DLS_get_reverb@16");
		p[43] = GetProcAddress(hL, "_AIL_DLS_load_file@12");
		p[44] = GetProcAddress(hL, "_AIL_DLS_load_memory@12");
		p[45] = GetProcAddress(hL, "_AIL_DLS_open@28");
		p[46] = GetProcAddress(hL, "_AIL_DLS_set_reverb@16");
		p[47] = GetProcAddress(hL, "_AIL_DLS_unload@8");
		p[48] = GetProcAddress(hL, "_AIL_HWND@0");
		p[49] = GetProcAddress(hL, "_AIL_MIDI_handle_reacquire@4");
		p[50] = GetProcAddress(hL, "_AIL_MIDI_handle_release@4");
		p[51] = GetProcAddress(hL, "_AIL_MIDI_to_XMI@20");
		p[52] = GetProcAddress(hL, "_AIL_MMX_available@0");
		p[53] = GetProcAddress(hL, "_AIL_WAV_file_write@20");
		p[54] = GetProcAddress(hL, "_AIL_WAV_info@8");
		p[55] = GetProcAddress(hL, "_AIL_XMIDI_master_volume@4");
		p[56] = GetProcAddress(hL, "_AIL_active_3D_sample_count@4");
		p[57] = GetProcAddress(hL, "_AIL_active_sample_count@4");
		p[58] = GetProcAddress(hL, "_AIL_active_sequence_count@4");
		p[59] = GetProcAddress(hL, "_AIL_allocate_3D_sample_handle@4");
		p[60] = GetProcAddress(hL, "_AIL_allocate_file_sample@12");
		p[61] = GetProcAddress(hL, "_AIL_allocate_sample_handle@4");
		p[62] = GetProcAddress(hL, "_AIL_allocate_sequence_handle@4");
		p[63] = GetProcAddress(hL, "_AIL_auto_service_stream@8");
		p[64] = GetProcAddress(hL, "_AIL_background@0");
		p[65] = GetProcAddress(hL, "_AIL_branch_index@8");
		p[66] = GetProcAddress(hL, "_AIL_channel_notes@8");
		p[67] = GetProcAddress(hL, "_AIL_close_3D_listener@4");
		p[68] = GetProcAddress(hL, "_AIL_close_3D_object@4");
		p[69] = GetProcAddress(hL, "_AIL_close_3D_provider@4");
		p[70] = GetProcAddress(hL, "_AIL_close_filter@4");
		p[71] = GetProcAddress(hL, "_AIL_close_stream@4");
		p[72] = GetProcAddress(hL, "_AIL_compress_ADPCM@12");
		p[73] = GetProcAddress(hL, "_AIL_compress_ASI@20");
		p[74] = GetProcAddress(hL, "_AIL_compress_DLS@20");
		p[75] = GetProcAddress(hL, "_AIL_controller_value@12");
		p[76] = GetProcAddress(hL, "_AIL_create_wave_synthesizer@16");
		p[77] = GetProcAddress(hL, "_AIL_decompress_ADPCM@12");
		p[78] = GetProcAddress(hL, "_AIL_decompress_ASI@24");
		p[79] = GetProcAddress(hL, "_AIL_delay@4");
		p[80] = GetProcAddress(hL, "_AIL_destroy_wave_synthesizer@4");
		p[81] = GetProcAddress(hL, "_AIL_digital_CPU_percent@4");
		p[82] = GetProcAddress(hL, "_AIL_digital_configuration@16");
		p[83] = GetProcAddress(hL, "_AIL_digital_handle_reacquire@4");
		p[84] = GetProcAddress(hL, "_AIL_digital_handle_release@4");
		p[85] = GetProcAddress(hL, "_AIL_digital_master_volume@4");
		p[86] = GetProcAddress(hL, "_AIL_end_3D_sample@4");
		p[87] = GetProcAddress(hL, "_AIL_end_sample@4");
		p[88] = GetProcAddress(hL, "_AIL_end_sequence@4");
		p[89] = GetProcAddress(hL, "_AIL_enumerate_3D_provider_attributes@12");
		p[90] = GetProcAddress(hL, "_AIL_enumerate_3D_providers@12");
		p[91] = GetProcAddress(hL, "_AIL_enumerate_3D_sample_attributes@12");
		p[92] = GetProcAddress(hL, "_AIL_enumerate_filter_attributes@12");
		p[93] = GetProcAddress(hL, "_AIL_enumerate_filter_sample_attributes@12");
		p[94] = GetProcAddress(hL, "_AIL_enumerate_filters@12");
		p[95] = GetProcAddress(hL, "_AIL_extract_DLS@28");
		p[96] = GetProcAddress(hL, "_AIL_file_error@0");
		p[97] = GetProcAddress(hL, "_AIL_file_read@8");
		p[98] = GetProcAddress(hL, "_AIL_file_size@4");
		p[99] = GetProcAddress(hL, "_AIL_file_type@8");
		p[100] = GetProcAddress(hL, "_AIL_file_write@12");
		p[101] = GetProcAddress(hL, "_AIL_filter_DLS_attribute@12");
		p[102] = GetProcAddress(hL, "_AIL_filter_DLS_with_XMI@24");
		p[103] = GetProcAddress(hL, "_AIL_filter_attribute@12");
		p[104] = GetProcAddress(hL, "_AIL_filter_sample_attribute@12");
		p[105] = GetProcAddress(hL, "_AIL_filter_stream_attribute@12");
		p[106] = GetProcAddress(hL, "_AIL_find_DLS@24");
		p[107] = GetProcAddress(hL, "_AIL_get_DirectSound_info@12");
		p[108] = GetProcAddress(hL, "_AIL_get_preference@4");
		p[109] = GetProcAddress(hL, "_AIL_get_timer_highest_delay@0");
		p[110] = GetProcAddress(hL, "_AIL_init_sample@4");
		p[111] = GetProcAddress(hL, "_AIL_init_sequence@12");
		p[112] = GetProcAddress(hL, "_AIL_input_close@4");
		p[113] = GetProcAddress(hL, "_AIL_input_info@4");
		p[114] = GetProcAddress(hL, "_AIL_input_open@12");
		p[115] = GetProcAddress(hL, "_AIL_last_error@0");
		p[116] = GetProcAddress(hL, "_AIL_list_DLS@20");
		p[117] = GetProcAddress(hL, "_AIL_list_MIDI@20");
		p[118] = GetProcAddress(hL, "_AIL_load_sample_buffer@16");
		p[119] = GetProcAddress(hL, "_AIL_lock@0");
		p[120] = GetProcAddress(hL, "_AIL_lock_channel@4");
		p[121] = GetProcAddress(hL, "_AIL_map_sequence_channel@12");
		p[122] = GetProcAddress(hL, "_AIL_mem_alloc_lock@4");
		p[123] = GetProcAddress(hL, "_AIL_mem_free_lock@4");
		p[124] = GetProcAddress(hL, "_AIL_merge_DLS_with_XMI@16");
		p[125] = GetProcAddress(hL, "_AIL_midiOutClose@4");
		p[126] = GetProcAddress(hL, "_AIL_midiOutOpen@12");
		p[127] = GetProcAddress(hL, "_AIL_minimum_sample_buffer_size@12");
		p[128] = GetProcAddress(hL, "_AIL_ms_count@0");
		p[129] = GetProcAddress(hL, "_AIL_open_3D_listener@4");
		p[130] = GetProcAddress(hL, "_AIL_open_3D_object@4");
		p[131] = GetProcAddress(hL, "_AIL_open_3D_provider@4");
		p[132] = GetProcAddress(hL, "_AIL_open_filter@8");
		p[133] = GetProcAddress(hL, "_AIL_open_stream@12");
		p[134] = GetProcAddress(hL, "_AIL_pause_stream@8");
		p[135] = GetProcAddress(hL, "_AIL_primary_digital_driver@4");
		p[136] = GetProcAddress(hL, "_AIL_process_digital_audio@24");
		p[137] = GetProcAddress(hL, "_AIL_quick_copy@4");
		p[138] = GetProcAddress(hL, "_AIL_quick_halt@4");
		p[139] = GetProcAddress(hL, "_AIL_quick_handles@12");
		p[140] = GetProcAddress(hL, "_AIL_quick_load@4");
		p[141] = GetProcAddress(hL, "_AIL_quick_load_and_play@12");
		p[142] = GetProcAddress(hL, "_AIL_quick_load_mem@8");
		p[143] = GetProcAddress(hL, "_AIL_quick_ms_length@4");
		p[144] = GetProcAddress(hL, "_AIL_quick_ms_position@4");
		p[145] = GetProcAddress(hL, "_AIL_quick_play@8");
		p[146] = GetProcAddress(hL, "_AIL_quick_set_ms_position@8");
		p[147] = GetProcAddress(hL, "_AIL_quick_set_reverb@16");
		p[148] = GetProcAddress(hL, "_AIL_quick_set_speed@8");
		p[149] = GetProcAddress(hL, "_AIL_quick_set_volume@12");
		p[150] = GetProcAddress(hL, "_AIL_quick_shutdown@0");
		p[151] = GetProcAddress(hL, "_AIL_quick_startup@20");
		p[152] = GetProcAddress(hL, "_AIL_quick_status@4");
		p[153] = GetProcAddress(hL, "_AIL_quick_type@4");
		p[154] = GetProcAddress(hL, "_AIL_quick_unload@4");
		p[155] = GetProcAddress(hL, "_AIL_redbook_close@4");
		p[156] = GetProcAddress(hL, "_AIL_redbook_eject@4");
		p[157] = GetProcAddress(hL, "_AIL_redbook_id@4");
		p[158] = GetProcAddress(hL, "_AIL_redbook_open@4");
		p[159] = GetProcAddress(hL, "_AIL_redbook_open_drive@4");
		p[160] = GetProcAddress(hL, "_AIL_redbook_pause@4");
		p[161] = GetProcAddress(hL, "_AIL_redbook_play@12");
		p[162] = GetProcAddress(hL, "_AIL_redbook_position@4");
		p[163] = GetProcAddress(hL, "_AIL_redbook_resume@4");
		p[164] = GetProcAddress(hL, "_AIL_redbook_retract@4");
		p[165] = GetProcAddress(hL, "_AIL_redbook_set_volume@8");
		p[166] = GetProcAddress(hL, "_AIL_redbook_status@4");
		p[167] = GetProcAddress(hL, "_AIL_redbook_stop@4");
		p[168] = GetProcAddress(hL, "_AIL_redbook_track@4");
		p[169] = GetProcAddress(hL, "_AIL_redbook_track_info@16");
		p[170] = GetProcAddress(hL, "_AIL_redbook_tracks@4");
		p[171] = GetProcAddress(hL, "_AIL_redbook_volume@4");
		p[172] = GetProcAddress(hL, "_AIL_register_EOB_callback@8");
		p[173] = GetProcAddress(hL, "_AIL_register_EOF_callback@8");
		p[174] = GetProcAddress(hL, "_AIL_register_EOS_callback@8");
		p[175] = GetProcAddress(hL, "_AIL_register_ICA_array@8");
		p[176] = GetProcAddress(hL, "_AIL_register_SOB_callback@8");
		p[177] = GetProcAddress(hL, "_AIL_register_beat_callback@8");
		p[178] = GetProcAddress(hL, "_AIL_register_event_callback@8");
		p[179] = GetProcAddress(hL, "_AIL_register_prefix_callback@8");
		p[180] = GetProcAddress(hL, "_AIL_register_sequence_callback@8");
		p[181] = GetProcAddress(hL, "_AIL_register_stream_callback@8");
		p[182] = GetProcAddress(hL, "_AIL_register_timbre_callback@8");
		p[183] = GetProcAddress(hL, "_AIL_register_timer@4");
		p[184] = GetProcAddress(hL, "_AIL_register_trigger_callback@8");
		p[185] = GetProcAddress(hL, "_AIL_release_3D_sample_handle@4");
		p[186] = GetProcAddress(hL, "_AIL_release_all_timers@0");
		p[187] = GetProcAddress(hL, "_AIL_release_channel@8");
		p[188] = GetProcAddress(hL, "_AIL_release_sample_handle@4");
		p[189] = GetProcAddress(hL, "_AIL_release_sequence_handle@4");
		p[190] = GetProcAddress(hL, "_AIL_release_timer_handle@4");
		p[191] = GetProcAddress(hL, "_AIL_resume_3D_sample@4");
		p[192] = GetProcAddress(hL, "_AIL_resume_sample@4");
		p[193] = GetProcAddress(hL, "_AIL_resume_sequence@4");
		p[194] = GetProcAddress(hL, "_AIL_sample_buffer_info@20");
		p[195] = GetProcAddress(hL, "_AIL_sample_buffer_ready@4");
		p[196] = GetProcAddress(hL, "_AIL_sample_granularity@4");
		p[197] = GetProcAddress(hL, "_AIL_sample_loop_count@4");
		p[198] = GetProcAddress(hL, "_AIL_sample_ms_position@12");
		p[199] = GetProcAddress(hL, "_AIL_sample_pan@4");
		p[200] = GetProcAddress(hL, "_AIL_sample_playback_rate@4");
		p[201] = GetProcAddress(hL, "_AIL_sample_position@4");
		p[202] = GetProcAddress(hL, "_AIL_sample_reverb@16");
		p[203] = GetProcAddress(hL, "_AIL_sample_status@4");
		p[204] = GetProcAddress(hL, "_AIL_sample_user_data@8");
		p[205] = GetProcAddress(hL, "_AIL_sample_volume@4");
		p[206] = GetProcAddress(hL, "_AIL_send_channel_voice_message@20");
		p[207] = GetProcAddress(hL, "_AIL_send_sysex_message@8");
		p[208] = GetProcAddress(hL, "_AIL_sequence_loop_count@4");
		p[209] = GetProcAddress(hL, "_AIL_sequence_ms_position@12");
		p[210] = GetProcAddress(hL, "_AIL_sequence_position@12");
		p[211] = GetProcAddress(hL, "_AIL_sequence_status@4");
		p[212] = GetProcAddress(hL, "_AIL_sequence_tempo@4");
		p[213] = GetProcAddress(hL, "_AIL_sequence_user_data@8");
		p[214] = GetProcAddress(hL, "_AIL_sequence_volume@4");
		p[215] = GetProcAddress(hL, "_AIL_serve@0");
		p[216] = GetProcAddress(hL, "_AIL_service_stream@8");
		p[217] = GetProcAddress(hL, "_AIL_set_3D_orientation@28");
		p[218] = GetProcAddress(hL, "_AIL_set_3D_position@16");
		p[219] = GetProcAddress(hL, "_AIL_set_3D_provider_preference@12");
		p[220] = GetProcAddress(hL, "_AIL_set_3D_sample_distances@20");
		p[221] = GetProcAddress(hL, "_AIL_set_3D_sample_file@8");
		p[222] = GetProcAddress(hL, "_AIL_set_3D_sample_float_distances@20");
		p[223] = GetProcAddress(hL, "_AIL_set_3D_sample_info@8");
		p[224] = GetProcAddress(hL, "_AIL_set_3D_sample_loop_block@12");
		p[225] = GetProcAddress(hL, "_AIL_set_3D_sample_loop_count@8");
		p[226] = GetProcAddress(hL, "_AIL_set_3D_sample_offset@8");
		p[227] = GetProcAddress(hL, "_AIL_set_3D_sample_playback_rate@8");
		p[228] = GetProcAddress(hL, "_AIL_set_3D_sample_preference@12");
		p[229] = GetProcAddress(hL, "_AIL_set_3D_sample_volume@8");
		p[230] = GetProcAddress(hL, "_AIL_set_3D_user_data@12");
		p[231] = GetProcAddress(hL, "_AIL_set_3D_velocity@20");
		p[232] = GetProcAddress(hL, "_AIL_set_3D_velocity_vector@16");
		p[233] = GetProcAddress(hL, "_AIL_set_DirectSound_HWND@8");
		p[234] = GetProcAddress(hL, "_AIL_set_XMIDI_master_volume@8");
		p[235] = GetProcAddress(hL, "_AIL_set_digital_driver_processor@12");
		p[236] = GetProcAddress(hL, "_AIL_set_digital_master_volume@8");
		p[237] = GetProcAddress(hL, "_AIL_set_direct_buffer_control@8");
		p[238] = GetProcAddress(hL, "_AIL_set_error@4");
		p[239] = GetProcAddress(hL, "_AIL_set_filter_DLS_preference@12");
		p[240] = GetProcAddress(hL, "_AIL_set_filter_preference@12");
		p[241] = GetProcAddress(hL, "_AIL_set_filter_sample_preference@12");
		p[242] = GetProcAddress(hL, "_AIL_set_filter_stream_preference@12");
		p[243] = GetProcAddress(hL, "_AIL_set_input_state@8");
		p[244] = GetProcAddress(hL, "_AIL_set_named_sample_file@20");
		p[245] = GetProcAddress(hL, "_AIL_set_preference@8");
		p[246] = GetProcAddress(hL, "_AIL_set_sample_address@12");
		p[247] = GetProcAddress(hL, "_AIL_set_sample_adpcm_block_size@8");
		p[248] = GetProcAddress(hL, "_AIL_set_sample_file@12");
		p[249] = GetProcAddress(hL, "_AIL_set_sample_loop_block@12");
		p[250] = GetProcAddress(hL, "_AIL_set_sample_loop_count@8");
		p[251] = GetProcAddress(hL, "_AIL_set_sample_ms_position@8");
		p[252] = GetProcAddress(hL, "_AIL_set_sample_pan@8");
		p[253] = GetProcAddress(hL, "_AIL_set_sample_playback_rate@8");
		p[254] = GetProcAddress(hL, "_AIL_set_sample_position@8");
		p[255] = GetProcAddress(hL, "_AIL_set_sample_processor@12");
		p[256] = GetProcAddress(hL, "_AIL_set_sample_reverb@16");
		p[257] = GetProcAddress(hL, "_AIL_set_sample_type@12");
		p[258] = GetProcAddress(hL, "_AIL_set_sample_user_data@12");
		p[259] = GetProcAddress(hL, "_AIL_set_sample_volume@8");
		p[260] = GetProcAddress(hL, "_AIL_set_sequence_loop_count@8");
		p[261] = GetProcAddress(hL, "_AIL_set_sequence_ms_position@8");
		p[262] = GetProcAddress(hL, "_AIL_set_sequence_tempo@12");
		p[263] = GetProcAddress(hL, "_AIL_set_sequence_user_data@12");
		p[264] = GetProcAddress(hL, "_AIL_set_sequence_volume@12");
		p[265] = GetProcAddress(hL, "_AIL_set_stream_loop_count@8");
		p[266] = GetProcAddress(hL, "_AIL_set_stream_ms_position@8");
		p[267] = GetProcAddress(hL, "_AIL_set_stream_pan@8");
		p[268] = GetProcAddress(hL, "_AIL_set_stream_playback_rate@8");
		p[269] = GetProcAddress(hL, "_AIL_set_stream_position@8");
		p[270] = GetProcAddress(hL, "_AIL_set_stream_reverb@16");
		p[271] = GetProcAddress(hL, "_AIL_set_stream_user_data@12");
		p[272] = GetProcAddress(hL, "_AIL_set_stream_volume@8");
		p[273] = GetProcAddress(hL, "_AIL_set_timer_divisor@8");
		p[274] = GetProcAddress(hL, "_AIL_set_timer_frequency@8");
		p[275] = GetProcAddress(hL, "_AIL_set_timer_period@8");
		p[276] = GetProcAddress(hL, "_AIL_set_timer_user@8");
		p[277] = GetProcAddress(hL, "_AIL_shutdown@0");
		p[278] = GetProcAddress(hL, "_AIL_size_processed_digital_audio@16");
		p[279] = GetProcAddress(hL, "_AIL_start_3D_sample@4");
		p[280] = GetProcAddress(hL, "_AIL_start_all_timers@0");
		p[281] = GetProcAddress(hL, "_AIL_start_sample@4");
		p[282] = GetProcAddress(hL, "_AIL_start_sequence@4");
		p[283] = GetProcAddress(hL, "_AIL_start_stream@4");
		p[284] = GetProcAddress(hL, "_AIL_start_timer@4");
		p[285] = GetProcAddress(hL, "_AIL_startup@0");
		p[286] = GetProcAddress(hL, "_AIL_stop_3D_sample@4");
		p[287] = GetProcAddress(hL, "_AIL_stop_all_timers@0");
		p[288] = GetProcAddress(hL, "_AIL_stop_sample@4");
		p[289] = GetProcAddress(hL, "_AIL_stop_sequence@4");
		p[290] = GetProcAddress(hL, "_AIL_stop_timer@4");
		p[291] = GetProcAddress(hL, "_AIL_stream_info@20");
		p[292] = GetProcAddress(hL, "_AIL_stream_loop_count@4");
		p[293] = GetProcAddress(hL, "_AIL_stream_ms_position@12");
		p[294] = GetProcAddress(hL, "_AIL_stream_pan@4");
		p[295] = GetProcAddress(hL, "_AIL_stream_playback_rate@4");
		p[296] = GetProcAddress(hL, "_AIL_stream_position@4");
		p[297] = GetProcAddress(hL, "_AIL_stream_reverb@16");
		p[298] = GetProcAddress(hL, "_AIL_stream_status@4");
		p[299] = GetProcAddress(hL, "_AIL_stream_user_data@8");
		p[300] = GetProcAddress(hL, "_AIL_stream_volume@4");
		p[301] = GetProcAddress(hL, "_AIL_true_sequence_channel@8");
		p[302] = GetProcAddress(hL, "_AIL_unlock@0");
		p[303] = GetProcAddress(hL, "_AIL_us_count@0");
		p[304] = GetProcAddress(hL, "_AIL_waveOutClose@4");
		p[305] = GetProcAddress(hL, "_AIL_waveOutOpen@16");
		p[306] = GetProcAddress(hL, "_DLSMSSGetCPU@4");
		p[307] = GetProcAddress(hL, "_DllMain@12");
		p[308] = GetProcAddress(hL, "_RIB_enumerate_providers@12");
		p[309] = GetProcAddress(hL, "_RIB_find_provider@12");
		p[310] = GetProcAddress(hL, "_RIB_load_application_providers@4");
		p[311] = GetProcAddress(hL, "_RIB_provider_library_handle@0");
		p[312] = GetProcAddress(hL, "_RIB_provider_system_data@8");
		p[313] = GetProcAddress(hL, "_RIB_provider_user_data@8");
		p[314] = GetProcAddress(hL, "_RIB_set_provider_system_data@12");
		p[315] = GetProcAddress(hL, "_RIB_set_provider_user_data@12");

		hUniMod = LoadLibrary(L".\\UniMod2.dll");

	}
	if (reason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(hL);
		if (!hUniMod) FreeLibrary(hUniMod);
	}

	return 1;
}

// AIL_debug_printf
extern "C" __declspec(naked) void __stdcall __E__0__()
{
	__asm
	{
		jmp p[0 * 4];
	}
}

// AIL_sprintf
extern "C" __declspec(naked) void __stdcall __E__1__()
{
	__asm
	{
		jmp p[1 * 4];
	}
}

// DLSClose
extern "C" __declspec(naked) void __stdcall __E__2__()
{
	__asm
	{
		jmp p[2 * 4];
	}
}

// DLSCompactMemory
extern "C" __declspec(naked) void __stdcall __E__3__()
{
	__asm
	{
		jmp p[3 * 4];
	}
}

// DLSGetInfo
extern "C" __declspec(naked) void __stdcall __E__4__()
{
	__asm
	{
		jmp p[4 * 4];
	}
}

// DLSLoadFile
extern "C" __declspec(naked) void __stdcall __E__5__()
{
	__asm
	{
		jmp p[5 * 4];
	}
}

// DLSLoadMemFile
extern "C" __declspec(naked) void __stdcall __E__6__()
{
	__asm
	{
		jmp p[6 * 4];
	}
}

// DLSMSSOpen
extern "C" __declspec(naked) void __stdcall __E__7__()
{
	__asm
	{
		jmp p[7 * 4];
	}
}

// DLSSetAttribute
extern "C" __declspec(naked) void __stdcall __E__8__()
{
	__asm
	{
		jmp p[8 * 4];
	}
}

// DLSUnloadAll
extern "C" __declspec(naked) void __stdcall __E__9__()
{
	__asm
	{
		jmp p[9 * 4];
	}
}

// DLSUnloadFile
extern "C" __declspec(naked) void __stdcall __E__10__()
{
	__asm
	{
		jmp p[10 * 4];
	}
}

// RIB_alloc_provider_handle
extern "C" __declspec(naked) void __stdcall __E__11__()
{
	__asm
	{
		jmp p[11 * 4];
	}
}

// RIB_enumerate_interface
extern "C" __declspec(naked) void __stdcall __E__12__()
{
	__asm
	{
		jmp p[12 * 4];
	}
}

// RIB_error
extern "C" __declspec(naked) void __stdcall __E__13__()
{
	__asm
	{
		jmp p[13 * 4];
	}
}

// RIB_find_file_provider
extern "C" __declspec(naked) void __stdcall __E__14__()
{
	__asm
	{
		jmp p[14 * 4];
	}
}

// RIB_free_provider_handle
extern "C" __declspec(naked) void __stdcall __E__15__()
{
	__asm
	{
		jmp p[15 * 4];
	}
}

// RIB_free_provider_library
extern "C" __declspec(naked) void __stdcall __E__16__()
{
	__asm
	{
		jmp p[16 * 4];
	}
}

// RIB_load_provider_library
extern "C" __declspec(naked) void __stdcall __E__17__()
{
	__asm
	{
		jmp p[17 * 4];
	}
}

// RIB_register_interface
extern "C" __declspec(naked) void __stdcall __E__18__()
{
	__asm
	{
		jmp p[18 * 4];
	}
}

// RIB_request_interface
extern "C" __declspec(naked) void __stdcall __E__19__()
{
	__asm
	{
		jmp p[19 * 4];
	}
}

// RIB_request_interface_entry
extern "C" __declspec(naked) void __stdcall __E__20__()
{
	__asm
	{
		jmp p[20 * 4];
	}
}

// RIB_type_string
extern "C" __declspec(naked) void __stdcall __E__21__()
{
	__asm
	{
		jmp p[21 * 4];
	}
}

// RIB_unregister_interface
extern "C" __declspec(naked) void __stdcall __E__22__()
{
	__asm
	{
		jmp p[22 * 4];
	}
}

// _AIL_3D_auto_update_position@8
extern "C" __declspec(naked) void __stdcall __E__23__(void)
{
	__asm
	{
		jmp p[23 * 4];
	}
}

// _AIL_3D_orientation@28
extern "C" __declspec(naked) void __stdcall __E__24__()
{
	__asm
	{
		jmp p[24 * 4];
	}
}

// _AIL_3D_position@16
extern "C" __declspec(naked) void __stdcall __E__25__()
{
	__asm
	{
		jmp p[25 * 4];
	}
}

// _AIL_3D_provider_attribute@12
extern "C" __declspec(naked) void __stdcall __E__26__()
{
	__asm
	{
		jmp p[26 * 4];
	}
}

// _AIL_3D_sample_attribute@12
extern "C" __declspec(naked) void __stdcall __E__27__()
{
	__asm
	{
		jmp p[27 * 4];
	}
}

// _AIL_3D_sample_distances@20
extern "C" __declspec(naked) void __stdcall __E__28__()
{
	__asm
	{
		jmp p[28 * 4];
	}
}

// _AIL_3D_sample_float_distances@20
extern "C" __declspec(naked) void __stdcall __E__29__()
{
	__asm
	{
		jmp p[29 * 4];
	}
}

// _AIL_3D_sample_length@4
extern "C" __declspec(naked) void __stdcall __E__30__()
{
	__asm
	{
		jmp p[30 * 4];
	}
}

// _AIL_3D_sample_loop_count@4
extern "C" __declspec(naked) void __stdcall __E__31__()
{
	__asm
	{
		jmp p[31 * 4];
	}
}

// _AIL_3D_sample_offset@4
extern "C" __declspec(naked) void __stdcall __E__32__()
{
	__asm
	{
		jmp p[32 * 4];
	}
}

// _AIL_3D_sample_playback_rate@4
extern "C" __declspec(naked) void __stdcall __E__33__()
{
	__asm
	{
		jmp p[33 * 4];
	}
}

// _AIL_3D_sample_status@4
extern "C" __declspec(naked) void __stdcall __E__34__()
{
	__asm
	{
		jmp p[34 * 4];
	}
}

// _AIL_3D_sample_volume@4
extern "C" __declspec(naked) void __stdcall __E__35__()
{
	__asm
	{
		jmp p[35 * 4];
	}
}

// _AIL_3D_update_position@8
extern "C" __declspec(naked) void __stdcall __E__36__()
{
	__asm
	{
		jmp p[36 * 4];
	}
}

// _AIL_3D_user_data@8
extern "C" __declspec(naked) void __stdcall __E__37__()
{
	__asm
	{
		jmp p[37 * 4];
	}
}

// _AIL_3D_velocity@16
extern "C" __declspec(naked) void __stdcall __E__38__()
{
	__asm
	{
		jmp p[38 * 4];
	}
}

// _AIL_DLS_close@8
extern "C" __declspec(naked) void __stdcall __E__39__()
{
	__asm
	{
		jmp p[39 * 4];
	}
}

// _AIL_DLS_compact@4
extern "C" __declspec(naked) void __stdcall __E__40__()
{
	__asm
	{
		jmp p[40 * 4];
	}
}

// _AIL_DLS_get_info@12
extern "C" __declspec(naked) void __stdcall __E__41__()
{
	__asm
	{
		jmp p[41 * 4];
	}
}

// _AIL_DLS_get_reverb@16
extern "C" __declspec(naked) void __stdcall __E__42__()
{
	__asm
	{
		jmp p[42 * 4];
	}
}

// _AIL_DLS_load_file@12
extern "C" __declspec(naked) void __stdcall __E__43__()
{
	__asm
	{
		jmp p[43 * 4];
	}
}

// _AIL_DLS_load_memory@12
extern "C" __declspec(naked) void __stdcall __E__44__()
{
	__asm
	{
		jmp p[44 * 4];
	}
}

// _AIL_DLS_open@28
extern "C" __declspec(naked) void __stdcall __E__45__()
{
	__asm
	{
		jmp p[45 * 4];
	}
}

// _AIL_DLS_set_reverb@16
extern "C" __declspec(naked) void __stdcall __E__46__()
{
	__asm
	{
		jmp p[46 * 4];
	}
}

// _AIL_DLS_unload@8
extern "C" __declspec(naked) void __stdcall __E__47__()
{
	__asm
	{
		jmp p[47 * 4];
	}
}

// _AIL_HWND@0
extern "C" __declspec(naked) void __stdcall __E__48__()
{
	__asm
	{
		jmp p[48 * 4];
	}
}

// _AIL_MIDI_handle_reacquire@4
extern "C" __declspec(naked) void __stdcall __E__49__()
{
	__asm
	{
		jmp p[49 * 4];
	}
}

// _AIL_MIDI_handle_release@4
extern "C" __declspec(naked) void __stdcall __E__50__()
{
	__asm
	{
		jmp p[50 * 4];
	}
}

// _AIL_MIDI_to_XMI@20
extern "C" __declspec(naked) void __stdcall __E__51__()
{
	__asm
	{
		jmp p[51 * 4];
	}
}

// _AIL_MMX_available@0
extern "C" __declspec(naked) void __stdcall __E__52__()
{
	__asm
	{
		jmp p[52 * 4];
	}
}

// _AIL_WAV_file_write@20
extern "C" __declspec(naked) void __stdcall __E__53__()
{
	__asm
	{
		jmp p[53 * 4];
	}
}

// _AIL_WAV_info@8
extern "C" __declspec(naked) void __stdcall __E__54__()
{
	__asm
	{
		jmp p[54 * 4];
	}
}

// _AIL_XMIDI_master_volume@4
extern "C" __declspec(naked) void __stdcall __E__55__()
{
	__asm
	{
		jmp p[55 * 4];
	}
}

// _AIL_active_3D_sample_count@4
extern "C" __declspec(naked) void __stdcall __E__56__()
{
	__asm
	{
		jmp p[56 * 4];
	}
}

// _AIL_active_sample_count@4
extern "C" __declspec(naked) void __stdcall __E__57__()
{
	__asm
	{
		jmp p[57 * 4];
	}
}

// _AIL_active_sequence_count@4
extern "C" __declspec(naked) void __stdcall __E__58__()
{
	__asm
	{
		jmp p[58 * 4];
	}
}

// _AIL_allocate_3D_sample_handle@4
extern "C" __declspec(naked) void __stdcall __E__59__()
{
	__asm
	{
		jmp p[59 * 4];
	}
}

// _AIL_allocate_file_sample@12
extern "C" __declspec(naked) void __stdcall __E__60__()
{
	__asm
	{
		jmp p[60 * 4];
	}
}

// _AIL_allocate_sample_handle@4
extern "C" __declspec(naked) void __stdcall __E__61__()
{
	__asm
	{
		jmp p[61 * 4];
	}
}

// _AIL_allocate_sequence_handle@4
extern "C" __declspec(naked) void __stdcall __E__62__()
{
	__asm
	{
		jmp p[62 * 4];
	}
}

// _AIL_auto_service_stream@8
extern "C" __declspec(naked) void __stdcall __E__63__()
{
	__asm
	{
		jmp p[63 * 4];
	}
}

// _AIL_background@0
extern "C" __declspec(naked) void __stdcall __E__64__()
{
	__asm
	{
		jmp p[64 * 4];
	}
}

// _AIL_branch_index@8
extern "C" __declspec(naked) void __stdcall __E__65__()
{
	__asm
	{
		jmp p[65 * 4];
	}
}

// _AIL_channel_notes@8
extern "C" __declspec(naked) void __stdcall __E__66__()
{
	__asm
	{
		jmp p[66 * 4];
	}
}

// _AIL_close_3D_listener@4
extern "C" __declspec(naked) void __stdcall __E__67__()
{
	__asm
	{
		jmp p[67 * 4];
	}
}

// _AIL_close_3D_object@4
extern "C" __declspec(naked) void __stdcall __E__68__()
{
	__asm
	{
		jmp p[68 * 4];
	}
}

// _AIL_close_3D_provider@4
extern "C" __declspec(naked) void __stdcall __E__69__()
{
	__asm
	{
		jmp p[69 * 4];
	}
}

// _AIL_close_filter@4
extern "C" __declspec(naked) void __stdcall __E__70__()
{
	__asm
	{
		jmp p[70 * 4];
	}
}

// _AIL_close_stream@4
extern "C" __declspec(naked) void __stdcall __E__71__()
{
	__asm
	{
		jmp p[71 * 4];
	}
}

// _AIL_compress_ADPCM@12
extern "C" __declspec(naked) void __stdcall __E__72__()
{
	__asm
	{
		jmp p[72 * 4];
	}
}

// _AIL_compress_ASI@20
extern "C" __declspec(naked) void __stdcall __E__73__()
{
	__asm
	{
		jmp p[73 * 4];
	}
}

// _AIL_compress_DLS@20
extern "C" __declspec(naked) void __stdcall __E__74__()
{
	__asm
	{
		jmp p[74 * 4];
	}
}

// _AIL_controller_value@12
extern "C" __declspec(naked) void __stdcall __E__75__()
{
	__asm
	{
		jmp p[75 * 4];
	}
}

// _AIL_create_wave_synthesizer@16
extern "C" __declspec(naked) void __stdcall __E__76__()
{
	__asm
	{
		jmp p[76 * 4];
	}
}

// _AIL_decompress_ADPCM@12
extern "C" __declspec(naked) void __stdcall __E__77__()
{
	__asm
	{
		jmp p[77 * 4];
	}
}

// _AIL_decompress_ASI@24
extern "C" __declspec(naked) void __stdcall __E__78__()
{
	__asm
	{
		jmp p[78 * 4];
	}
}

// _AIL_delay@4
extern "C" __declspec(naked) void __stdcall __E__79__()
{
	__asm
	{
		jmp p[79 * 4];
	}
}

// _AIL_destroy_wave_synthesizer@4
extern "C" __declspec(naked) void __stdcall __E__80__()
{
	__asm
	{
		jmp p[80 * 4];
	}
}

// _AIL_digital_CPU_percent@4
extern "C" __declspec(naked) void __stdcall __E__81__()
{
	__asm
	{
		jmp p[81 * 4];
	}
}

// _AIL_digital_configuration@16
extern "C" __declspec(naked) void __stdcall __E__82__()
{
	__asm
	{
		jmp p[82 * 4];
	}
}

// _AIL_digital_handle_reacquire@4
extern "C" __declspec(naked) void __stdcall __E__83__()
{
	__asm
	{
		jmp p[83 * 4];
	}
}

// _AIL_digital_handle_release@4
extern "C" __declspec(naked) void __stdcall __E__84__()
{
	__asm
	{
		jmp p[84 * 4];
	}
}

// _AIL_digital_master_volume@4
extern "C" __declspec(naked) void __stdcall __E__85__()
{
	__asm
	{
		jmp p[85 * 4];
	}
}

// _AIL_end_3D_sample@4
extern "C" __declspec(naked) void __stdcall __E__86__()
{
	__asm
	{
		jmp p[86 * 4];
	}
}

// _AIL_end_sample@4
extern "C" __declspec(naked) void __stdcall __E__87__()
{
	__asm
	{
		jmp p[87 * 4];
	}
}

// _AIL_end_sequence@4
extern "C" __declspec(naked) void __stdcall __E__88__()
{
	__asm
	{
		jmp p[88 * 4];
	}
}

// _AIL_enumerate_3D_provider_attributes@12
extern "C" __declspec(naked) void __stdcall __E__89__()
{
	__asm
	{
		jmp p[89 * 4];
	}
}

// _AIL_enumerate_3D_providers@12
extern "C" __declspec(naked) void __stdcall __E__90__()
{
	__asm
	{
		jmp p[90 * 4];
	}
}

// _AIL_enumerate_3D_sample_attributes@12
extern "C" __declspec(naked) void __stdcall __E__91__()
{
	__asm
	{
		jmp p[91 * 4];
	}
}

// _AIL_enumerate_filter_attributes@12
extern "C" __declspec(naked) void __stdcall __E__92__()
{
	__asm
	{
		jmp p[92 * 4];
	}
}

// _AIL_enumerate_filter_sample_attributes@12
extern "C" __declspec(naked) void __stdcall __E__93__()
{
	__asm
	{
		jmp p[93 * 4];
	}
}

// _AIL_enumerate_filters@12
extern "C" __declspec(naked) void __stdcall __E__94__()
{
	__asm
	{
		jmp p[94 * 4];
	}
}

// _AIL_extract_DLS@28
extern "C" __declspec(naked) void __stdcall __E__95__()
{
	__asm
	{
		jmp p[95 * 4];
	}
}

// _AIL_file_error@0
extern "C" __declspec(naked) void __stdcall __E__96__()
{
	__asm
	{
		jmp p[96 * 4];
	}
}

// _AIL_file_read@8
extern "C" __declspec(naked) void __stdcall __E__97__()
{
	__asm
	{
		jmp p[97 * 4];
	}
}

// _AIL_file_size@4
extern "C" __declspec(naked) void __stdcall __E__98__()
{
	__asm
	{
		jmp p[98 * 4];
	}
}

// _AIL_file_type@8
extern "C" __declspec(naked) void __stdcall __E__99__()
{
	__asm
	{
		jmp p[99 * 4];
	}
}

// _AIL_file_write@12
extern "C" __declspec(naked) void __stdcall __E__100__()
{
	__asm
	{
		jmp p[100 * 4];
	}
}

// _AIL_filter_DLS_attribute@12
extern "C" __declspec(naked) void __stdcall __E__101__()
{
	__asm
	{
		jmp p[101 * 4];
	}
}

// _AIL_filter_DLS_with_XMI@24
extern "C" __declspec(naked) void __stdcall __E__102__()
{
	__asm
	{
		jmp p[102 * 4];
	}
}

// _AIL_filter_attribute@12
extern "C" __declspec(naked) void __stdcall __E__103__()
{
	__asm
	{
		jmp p[103 * 4];
	}
}

// _AIL_filter_sample_attribute@12
extern "C" __declspec(naked) void __stdcall __E__104__()
{
	__asm
	{
		jmp p[104 * 4];
	}
}

// _AIL_filter_stream_attribute@12
extern "C" __declspec(naked) void __stdcall __E__105__()
{
	__asm
	{
		jmp p[105 * 4];
	}
}

// _AIL_find_DLS@24
extern "C" __declspec(naked) void __stdcall __E__106__()
{
	__asm
	{
		jmp p[106 * 4];
	}
}

// _AIL_get_DirectSound_info@12
extern "C" __declspec(naked) void __stdcall __E__107__()
{
	__asm
	{
		jmp p[107 * 4];
	}
}

// _AIL_get_preference@4
extern "C" __declspec(naked) void __stdcall __E__108__()
{
	__asm
	{
		jmp p[108 * 4];
	}
}

// _AIL_get_timer_highest_delay@0
extern "C" __declspec(naked) void __stdcall __E__109__()
{
	__asm
	{
		jmp p[109 * 4];
	}
}

// _AIL_init_sample@4
extern "C" __declspec(naked) void __stdcall __E__110__()
{
	__asm
	{
		jmp p[110 * 4];
	}
}

// _AIL_init_sequence@12
extern "C" __declspec(naked) void __stdcall __E__111__()
{
	__asm
	{
		jmp p[111 * 4];
	}
}

// _AIL_input_close@4
extern "C" __declspec(naked) void __stdcall __E__112__()
{
	__asm
	{
		jmp p[112 * 4];
	}
}

// _AIL_input_info@4
extern "C" __declspec(naked) void __stdcall __E__113__()
{
	__asm
	{
		jmp p[113 * 4];
	}
}

// _AIL_input_open@12
extern "C" __declspec(naked) void __stdcall __E__114__()
{
	__asm
	{
		jmp p[114 * 4];
	}
}

// _AIL_last_error@0
extern "C" __declspec(naked) void __stdcall __E__115__()
{
	__asm
	{
		jmp p[115 * 4];
	}
}

// _AIL_list_DLS@20
extern "C" __declspec(naked) void __stdcall __E__116__()
{
	__asm
	{
		jmp p[116 * 4];
	}
}

// _AIL_list_MIDI@20
extern "C" __declspec(naked) void __stdcall __E__117__()
{
	__asm
	{
		jmp p[117 * 4];
	}
}

// _AIL_load_sample_buffer@16
extern "C" __declspec(naked) void __stdcall __E__118__()
{
	__asm
	{
		jmp p[118 * 4];
	}
}

// _AIL_lock@0
extern "C" __declspec(naked) void __stdcall __E__119__()
{
	__asm
	{
		jmp p[119 * 4];
	}
}

// _AIL_lock_channel@4
extern "C" __declspec(naked) void __stdcall __E__120__()
{
	__asm
	{
		jmp p[120 * 4];
	}
}

// _AIL_map_sequence_channel@12
extern "C" __declspec(naked) void __stdcall __E__121__()
{
	__asm
	{
		jmp p[121 * 4];
	}
}

// _AIL_mem_alloc_lock@4
extern "C" __declspec(naked) void __stdcall __E__122__()
{
	__asm
	{
		jmp p[122 * 4];
	}
}

// _AIL_mem_free_lock@4
extern "C" __declspec(naked) void __stdcall __E__123__()
{
	__asm
	{
		jmp p[123 * 4];
	}
}

// _AIL_merge_DLS_with_XMI@16
extern "C" __declspec(naked) void __stdcall __E__124__()
{
	__asm
	{
		jmp p[124 * 4];
	}
}

// _AIL_midiOutClose@4
extern "C" __declspec(naked) void __stdcall __E__125__()
{
	__asm
	{
		jmp p[125 * 4];
	}
}

// _AIL_midiOutOpen@12
extern "C" __declspec(naked) void __stdcall __E__126__()
{
	__asm
	{
		jmp p[126 * 4];
	}
}

// _AIL_minimum_sample_buffer_size@12
extern "C" __declspec(naked) void __stdcall __E__127__()
{
	__asm
	{
		jmp p[127 * 4];
	}
}

// _AIL_ms_count@0
extern "C" __declspec(naked) void __stdcall __E__128__()
{
	__asm
	{
		jmp p[128 * 4];
	}
}

// _AIL_open_3D_listener@4
extern "C" __declspec(naked) void __stdcall __E__129__()
{
	__asm
	{
		jmp p[129 * 4];
	}
}

// _AIL_open_3D_object@4
extern "C" __declspec(naked) void __stdcall __E__130__()
{
	__asm
	{
		jmp p[130 * 4];
	}
}

// _AIL_open_3D_provider@4
extern "C" __declspec(naked) void __stdcall __E__131__()
{
	__asm
	{
		jmp p[131 * 4];
	}
}

// _AIL_open_filter@8
extern "C" __declspec(naked) void __stdcall __E__132__()
{
	__asm
	{
		jmp p[132 * 4];
	}
}

// _AIL_open_stream@12
extern "C" __declspec(naked) void __stdcall __E__133__()
{
	__asm
	{
		jmp p[133 * 4];
	}
}

// _AIL_pause_stream@8
extern "C" __declspec(naked) void __stdcall __E__134__()
{
	__asm
	{
		jmp p[134 * 4];
	}
}

// _AIL_primary_digital_driver@4
extern "C" __declspec(naked) void __stdcall __E__135__()
{
	__asm
	{
		jmp p[135 * 4];
	}
}

// _AIL_process_digital_audio@24
extern "C" __declspec(naked) void __stdcall __E__136__()
{
	__asm
	{
		jmp p[136 * 4];
	}
}

// _AIL_quick_copy@4
extern "C" __declspec(naked) void __stdcall __E__137__()
{
	__asm
	{
		jmp p[137 * 4];
	}
}

// _AIL_quick_halt@4
extern "C" __declspec(naked) void __stdcall __E__138__()
{
	__asm
	{
		jmp p[138 * 4];
	}
}

// _AIL_quick_handles@12
extern "C" __declspec(naked) void __stdcall __E__139__()
{
	__asm
	{
		jmp p[139 * 4];
	}
}

// _AIL_quick_load@4
extern "C" __declspec(naked) void __stdcall __E__140__()
{
	__asm
	{
		jmp p[140 * 4];
	}
}

// _AIL_quick_load_and_play@12
extern "C" __declspec(naked) void __stdcall __E__141__()
{
	__asm
	{
		jmp p[141 * 4];
	}
}

// _AIL_quick_load_mem@8
extern "C" __declspec(naked) void __stdcall __E__142__()
{
	__asm
	{
		jmp p[142 * 4];
	}
}

// _AIL_quick_ms_length@4
extern "C" __declspec(naked) void __stdcall __E__143__()
{
	__asm
	{
		jmp p[143 * 4];
	}
}

// _AIL_quick_ms_position@4
extern "C" __declspec(naked) void __stdcall __E__144__()
{
	__asm
	{
		jmp p[144 * 4];
	}
}

// _AIL_quick_play@8
extern "C" __declspec(naked) void __stdcall __E__145__()
{
	__asm
	{
		jmp p[145 * 4];
	}
}

// _AIL_quick_set_ms_position@8
extern "C" __declspec(naked) void __stdcall __E__146__()
{
	__asm
	{
		jmp p[146 * 4];
	}
}

// _AIL_quick_set_reverb@16
extern "C" __declspec(naked) void __stdcall __E__147__()
{
	__asm
	{
		jmp p[147 * 4];
	}
}

// _AIL_quick_set_speed@8
extern "C" __declspec(naked) void __stdcall __E__148__()
{
	__asm
	{
		jmp p[148 * 4];
	}
}

// _AIL_quick_set_volume@12
extern "C" __declspec(naked) void __stdcall __E__149__()
{
	__asm
	{
		jmp p[149 * 4];
	}
}

// _AIL_quick_shutdown@0
extern "C" __declspec(naked) void __stdcall __E__150__()
{
	__asm
	{
		jmp p[150 * 4];
	}
}

// _AIL_quick_startup@20
extern "C" __declspec(naked) void __stdcall __E__151__()
{
	__asm
	{
		jmp p[151 * 4];
	}
}

// _AIL_quick_status@4
extern "C" __declspec(naked) void __stdcall __E__152__()
{
	__asm
	{
		jmp p[152 * 4];
	}
}

// _AIL_quick_type@4
extern "C" __declspec(naked) void __stdcall __E__153__()
{
	__asm
	{
		jmp p[153 * 4];
	}
}

// _AIL_quick_unload@4
extern "C" __declspec(naked) void __stdcall __E__154__()
{
	__asm
	{
		jmp p[154 * 4];
	}
}

// _AIL_redbook_close@4
extern "C" __declspec(naked) void __stdcall __E__155__()
{
	__asm
	{
		jmp p[155 * 4];
	}
}

// _AIL_redbook_eject@4
extern "C" __declspec(naked) void __stdcall __E__156__()
{
	__asm
	{
		jmp p[156 * 4];
	}
}

// _AIL_redbook_id@4
extern "C" __declspec(naked) void __stdcall __E__157__()
{
	__asm
	{
		jmp p[157 * 4];
	}
}

// _AIL_redbook_open@4
extern "C" __declspec(naked) void __stdcall __E__158__()
{
	__asm
	{
		jmp p[158 * 4];
	}
}

// _AIL_redbook_open_drive@4
extern "C" __declspec(naked) void __stdcall __E__159__()
{
	__asm
	{
		jmp p[159 * 4];
	}
}

// _AIL_redbook_pause@4
extern "C" __declspec(naked) void __stdcall __E__160__()
{
	__asm
	{
		jmp p[160 * 4];
	}
}

// _AIL_redbook_play@12
extern "C" __declspec(naked) void __stdcall __E__161__()
{
	__asm
	{
		jmp p[161 * 4];
	}
}

// _AIL_redbook_position@4
extern "C" __declspec(naked) void __stdcall __E__162__()
{
	__asm
	{
		jmp p[162 * 4];
	}
}

// _AIL_redbook_resume@4
extern "C" __declspec(naked) void __stdcall __E__163__()
{
	__asm
	{
		jmp p[163 * 4];
	}
}

// _AIL_redbook_retract@4
extern "C" __declspec(naked) void __stdcall __E__164__()
{
	__asm
	{
		jmp p[164 * 4];
	}
}

// _AIL_redbook_set_volume@8
extern "C" __declspec(naked) void __stdcall __E__165__()
{
	__asm
	{
		jmp p[165 * 4];
	}
}

// _AIL_redbook_status@4
extern "C" __declspec(naked) void __stdcall __E__166__()
{
	__asm
	{
		jmp p[166 * 4];
	}
}

// _AIL_redbook_stop@4
extern "C" __declspec(naked) void __stdcall __E__167__()
{
	__asm
	{
		jmp p[167 * 4];
	}
}

// _AIL_redbook_track@4
extern "C" __declspec(naked) void __stdcall __E__168__()
{
	__asm
	{
		jmp p[168 * 4];
	}
}

// _AIL_redbook_track_info@16
extern "C" __declspec(naked) void __stdcall __E__169__()
{
	__asm
	{
		jmp p[169 * 4];
	}
}

// _AIL_redbook_tracks@4
extern "C" __declspec(naked) void __stdcall __E__170__()
{
	__asm
	{
		jmp p[170 * 4];
	}
}

// _AIL_redbook_volume@4
extern "C" __declspec(naked) void __stdcall __E__171__()
{
	__asm
	{
		jmp p[171 * 4];
	}
}

// _AIL_register_EOB_callback@8
extern "C" __declspec(naked) void __stdcall __E__172__()
{
	__asm
	{
		jmp p[172 * 4];
	}
}

// _AIL_register_EOF_callback@8
extern "C" __declspec(naked) void __stdcall __E__173__()
{
	__asm
	{
		jmp p[173 * 4];
	}
}

// _AIL_register_EOS_callback@8
extern "C" __declspec(naked) void __stdcall __E__174__()
{
	__asm
	{
		jmp p[174 * 4];
	}
}

// _AIL_register_ICA_array@8
extern "C" __declspec(naked) void __stdcall __E__175__()
{
	__asm
	{
		jmp p[175 * 4];
	}
}

// _AIL_register_SOB_callback@8
extern "C" __declspec(naked) void __stdcall __E__176__()
{
	__asm
	{
		jmp p[176 * 4];
	}
}

// _AIL_register_beat_callback@8
extern "C" __declspec(naked) void __stdcall __E__177__()
{
	__asm
	{
		jmp p[177 * 4];
	}
}

// _AIL_register_event_callback@8
extern "C" __declspec(naked) void __stdcall __E__178__()
{
	__asm
	{
		jmp p[178 * 4];
	}
}

// _AIL_register_prefix_callback@8
extern "C" __declspec(naked) void __stdcall __E__179__()
{
	__asm
	{
		jmp p[179 * 4];
	}
}

// _AIL_register_sequence_callback@8
extern "C" __declspec(naked) void __stdcall __E__180__()
{
	__asm
	{
		jmp p[180 * 4];
	}
}

// _AIL_register_stream_callback@8
extern "C" __declspec(naked) void __stdcall __E__181__()
{
	__asm
	{
		jmp p[181 * 4];
	}
}

// _AIL_register_timbre_callback@8
extern "C" __declspec(naked) void __stdcall __E__182__()
{
	__asm
	{
		jmp p[182 * 4];
	}
}

// _AIL_register_timer@4
extern "C" __declspec(naked) void __stdcall __E__183__()
{
	__asm
	{
		jmp p[183 * 4];
	}
}

// _AIL_register_trigger_callback@8
extern "C" __declspec(naked) void __stdcall __E__184__()
{
	__asm
	{
		jmp p[184 * 4];
	}
}

// _AIL_release_3D_sample_handle@4
extern "C" __declspec(naked) void __stdcall __E__185__()
{
	__asm
	{
		jmp p[185 * 4];
	}
}

// _AIL_release_all_timers@0
extern "C" __declspec(naked) void __stdcall __E__186__()
{
	__asm
	{
		jmp p[186 * 4];
	}
}

// _AIL_release_channel@8
extern "C" __declspec(naked) void __stdcall __E__187__()
{
	__asm
	{
		jmp p[187 * 4];
	}
}

// _AIL_release_sample_handle@4
extern "C" __declspec(naked) void __stdcall __E__188__()
{
	__asm
	{
		jmp p[188 * 4];
	}
}

// _AIL_release_sequence_handle@4
extern "C" __declspec(naked) void __stdcall __E__189__()
{
	__asm
	{
		jmp p[189 * 4];
	}
}

// _AIL_release_timer_handle@4
extern "C" __declspec(naked) void __stdcall __E__190__()
{
	__asm
	{
		jmp p[190 * 4];
	}
}

// _AIL_resume_3D_sample@4
extern "C" __declspec(naked) void __stdcall __E__191__()
{
	__asm
	{
		jmp p[191 * 4];
	}
}

// _AIL_resume_sample@4
extern "C" __declspec(naked) void __stdcall __E__192__()
{
	__asm
	{
		jmp p[192 * 4];
	}
}

// _AIL_resume_sequence@4
extern "C" __declspec(naked) void __stdcall __E__193__()
{
	__asm
	{
		jmp p[193 * 4];
	}
}

// _AIL_sample_buffer_info@20
extern "C" __declspec(naked) void __stdcall __E__194__()
{
	__asm
	{
		jmp p[194 * 4];
	}
}

// _AIL_sample_buffer_ready@4
extern "C" __declspec(naked) void __stdcall __E__195__()
{
	__asm
	{
		jmp p[195 * 4];
	}
}

// _AIL_sample_granularity@4
extern "C" __declspec(naked) void __stdcall __E__196__()
{
	__asm
	{
		jmp p[196 * 4];
	}
}

// _AIL_sample_loop_count@4
extern "C" __declspec(naked) void __stdcall __E__197__()
{
	__asm
	{
		jmp p[197 * 4];
	}
}

// _AIL_sample_ms_position@12
extern "C" __declspec(naked) void __stdcall __E__198__()
{
	__asm
	{
		jmp p[198 * 4];
	}
}

// _AIL_sample_pan@4
extern "C" __declspec(naked) void __stdcall __E__199__()
{
	__asm
	{
		jmp p[199 * 4];
	}
}

// _AIL_sample_playback_rate@4
extern "C" __declspec(naked) void __stdcall __E__200__()
{
	__asm
	{
		jmp p[200 * 4];
	}
}

// _AIL_sample_position@4
extern "C" __declspec(naked) void __stdcall __E__201__()
{
	__asm
	{
		jmp p[201 * 4];
	}
}

// _AIL_sample_reverb@16
extern "C" __declspec(naked) void __stdcall __E__202__()
{
	__asm
	{
		jmp p[202 * 4];
	}
}

// _AIL_sample_status@4
extern "C" __declspec(naked) void __stdcall __E__203__()
{
	__asm
	{
		jmp p[203 * 4];
	}
}

// _AIL_sample_user_data@8
extern "C" __declspec(naked) void __stdcall __E__204__()
{
	__asm
	{
		jmp p[204 * 4];
	}
}

// _AIL_sample_volume@4
extern "C" __declspec(naked) void __stdcall __E__205__()
{
	__asm
	{
		jmp p[205 * 4];
	}
}

// _AIL_send_channel_voice_message@20
extern "C" __declspec(naked) void __stdcall __E__206__()
{
	__asm
	{
		jmp p[206 * 4];
	}
}

// _AIL_send_sysex_message@8
extern "C" __declspec(naked) void __stdcall __E__207__()
{
	__asm
	{
		jmp p[207 * 4];
	}
}

// _AIL_sequence_loop_count@4
extern "C" __declspec(naked) void __stdcall __E__208__()
{
	__asm
	{
		jmp p[208 * 4];
	}
}

// _AIL_sequence_ms_position@12
extern "C" __declspec(naked) void __stdcall __E__209__()
{
	__asm
	{
		jmp p[209 * 4];
	}
}

// _AIL_sequence_position@12
extern "C" __declspec(naked) void __stdcall __E__210__()
{
	__asm
	{
		jmp p[210 * 4];
	}
}

// _AIL_sequence_status@4
extern "C" __declspec(naked) void __stdcall __E__211__()
{
	__asm
	{
		jmp p[211 * 4];
	}
}

// _AIL_sequence_tempo@4
extern "C" __declspec(naked) void __stdcall __E__212__()
{
	__asm
	{
		jmp p[212 * 4];
	}
}

// _AIL_sequence_user_data@8
extern "C" __declspec(naked) void __stdcall __E__213__()
{
	__asm
	{
		jmp p[213 * 4];
	}
}

// _AIL_sequence_volume@4
extern "C" __declspec(naked) void __stdcall __E__214__()
{
	__asm
	{
		jmp p[214 * 4];
	}
}

// _AIL_serve@0
extern "C" __declspec(naked) void __stdcall __E__215__()
{
	__asm
	{
		jmp p[215 * 4];
	}
}

// _AIL_service_stream@8
extern "C" __declspec(naked) void __stdcall __E__216__()
{
	__asm
	{
		jmp p[216 * 4];
	}
}

// _AIL_set_3D_orientation@28
extern "C" __declspec(naked) void __stdcall __E__217__()
{
	__asm
	{
		jmp p[217 * 4];
	}
}

// _AIL_set_3D_position@16
extern "C" __declspec(naked) void __stdcall __E__218__()
{
	__asm
	{
		jmp p[218 * 4];
	}
}

// _AIL_set_3D_provider_preference@12
extern "C" __declspec(naked) void __stdcall __E__219__()
{
	__asm
	{
		jmp p[219 * 4];
	}
}

// _AIL_set_3D_sample_distances@20
extern "C" __declspec(naked) void __stdcall __E__220__()
{
	__asm
	{
		jmp p[220 * 4];
	}
}

// _AIL_set_3D_sample_file@8
extern "C" __declspec(naked) void __stdcall __E__221__()
{
	__asm
	{
		jmp p[221 * 4];
	}
}

// _AIL_set_3D_sample_float_distances@20
extern "C" __declspec(naked) void __stdcall __E__222__()
{
	__asm
	{
		jmp p[222 * 4];
	}
}

// _AIL_set_3D_sample_info@8
extern "C" __declspec(naked) void __stdcall __E__223__()
{
	__asm
	{
		jmp p[223 * 4];
	}
}

// _AIL_set_3D_sample_loop_block@12
extern "C" __declspec(naked) void __stdcall __E__224__()
{
	__asm
	{
		jmp p[224 * 4];
	}
}

// _AIL_set_3D_sample_loop_count@8
extern "C" __declspec(naked) void __stdcall __E__225__()
{
	__asm
	{
		jmp p[225 * 4];
	}
}

// _AIL_set_3D_sample_offset@8
extern "C" __declspec(naked) void __stdcall __E__226__()
{
	__asm
	{
		jmp p[226 * 4];
	}
}

// _AIL_set_3D_sample_playback_rate@8
extern "C" __declspec(naked) void __stdcall __E__227__()
{
	__asm
	{
		jmp p[227 * 4];
	}
}

// _AIL_set_3D_sample_preference@12
extern "C" __declspec(naked) void __stdcall __E__228__()
{
	__asm
	{
		jmp p[228 * 4];
	}
}

// _AIL_set_3D_sample_volume@8
extern "C" __declspec(naked) void __stdcall __E__229__()
{
	__asm
	{
		jmp p[229 * 4];
	}
}

// _AIL_set_3D_user_data@12
extern "C" __declspec(naked) void __stdcall __E__230__()
{
	__asm
	{
		jmp p[230 * 4];
	}
}

// _AIL_set_3D_velocity@20
extern "C" __declspec(naked) void __stdcall __E__231__()
{
	__asm
	{
		jmp p[231 * 4];
	}
}

// _AIL_set_3D_velocity_vector@16
extern "C" __declspec(naked) void __stdcall __E__232__()
{
	__asm
	{
		jmp p[232 * 4];
	}
}

// _AIL_set_DirectSound_HWND@8
extern "C" __declspec(naked) void __stdcall __E__233__()
{
	__asm
	{
		jmp p[233 * 4];
	}
}

// _AIL_set_XMIDI_master_volume@8
extern "C" __declspec(naked) void __stdcall __E__234__()
{
	__asm
	{
		jmp p[234 * 4];
	}
}

// _AIL_set_digital_driver_processor@12
extern "C" __declspec(naked) void __stdcall __E__235__()
{
	__asm
	{
		jmp p[235 * 4];
	}
}

// _AIL_set_digital_master_volume@8
extern "C" __declspec(naked) void __stdcall __E__236__()
{
	__asm
	{
		jmp p[236 * 4];
	}
}

// _AIL_set_direct_buffer_control@8
extern "C" __declspec(naked) void __stdcall __E__237__()
{
	__asm
	{
		jmp p[237 * 4];
	}
}

// _AIL_set_error@4
extern "C" __declspec(naked) void __stdcall __E__238__()
{
	__asm
	{
		jmp p[238 * 4];
	}
}

// _AIL_set_filter_DLS_preference@12
extern "C" __declspec(naked) void __stdcall __E__239__()
{
	__asm
	{
		jmp p[239 * 4];
	}
}

// _AIL_set_filter_preference@12
extern "C" __declspec(naked) void __stdcall __E__240__()
{
	__asm
	{
		jmp p[240 * 4];
	}
}

// _AIL_set_filter_sample_preference@12
extern "C" __declspec(naked) void __stdcall __E__241__()
{
	__asm
	{
		jmp p[241 * 4];
	}
}

// _AIL_set_filter_stream_preference@12
extern "C" __declspec(naked) void __stdcall __E__242__()
{
	__asm
	{
		jmp p[242 * 4];
	}
}

// _AIL_set_input_state@8
extern "C" __declspec(naked) void __stdcall __E__243__()
{
	__asm
	{
		jmp p[243 * 4];
	}
}

// _AIL_set_named_sample_file@20
extern "C" __declspec(naked) void __stdcall __E__244__()
{
	__asm
	{
		jmp p[244 * 4];
	}
}

// _AIL_set_preference@8
extern "C" __declspec(naked) void __stdcall __E__245__()
{
	__asm
	{
		jmp p[245 * 4];
	}
}

// _AIL_set_sample_address@12
extern "C" __declspec(naked) void __stdcall __E__246__()
{
	__asm
	{
		jmp p[246 * 4];
	}
}

// _AIL_set_sample_adpcm_block_size@8
extern "C" __declspec(naked) void __stdcall __E__247__()
{
	__asm
	{
		jmp p[247 * 4];
	}
}

// _AIL_set_sample_file@12
extern "C" __declspec(naked) void __stdcall __E__248__()
{
	__asm
	{
		jmp p[248 * 4];
	}
}

// _AIL_set_sample_loop_block@12
extern "C" __declspec(naked) void __stdcall __E__249__()
{
	__asm
	{
		jmp p[249 * 4];
	}
}

// _AIL_set_sample_loop_count@8
extern "C" __declspec(naked) void __stdcall __E__250__()
{
	__asm
	{
		jmp p[250 * 4];
	}
}

// _AIL_set_sample_ms_position@8
extern "C" __declspec(naked) void __stdcall __E__251__()
{
	__asm
	{
		jmp p[251 * 4];
	}
}

// _AIL_set_sample_pan@8
extern "C" __declspec(naked) void __stdcall __E__252__()
{
	__asm
	{
		jmp p[252 * 4];
	}
}

// _AIL_set_sample_playback_rate@8
extern "C" __declspec(naked) void __stdcall __E__253__()
{
	__asm
	{
		jmp p[253 * 4];
	}
}

// _AIL_set_sample_position@8
extern "C" __declspec(naked) void __stdcall __E__254__()
{
	__asm
	{
		jmp p[254 * 4];
	}
}

// _AIL_set_sample_processor@12
extern "C" __declspec(naked) void __stdcall __E__255__()
{
	__asm
	{
		jmp p[255 * 4];
	}
}

// _AIL_set_sample_reverb@16
extern "C" __declspec(naked) void __stdcall __E__256__()
{
	__asm
	{
		jmp p[256 * 4];
	}
}

// _AIL_set_sample_type@12
extern "C" __declspec(naked) void __stdcall __E__257__()
{
	__asm
	{
		jmp p[257 * 4];
	}
}

// _AIL_set_sample_user_data@12
extern "C" __declspec(naked) void __stdcall __E__258__()
{
	__asm
	{
		jmp p[258 * 4];
	}
}

// _AIL_set_sample_volume@8
extern "C" __declspec(naked) void __stdcall __E__259__()
{
	__asm
	{
		jmp p[259 * 4];
	}
}

// _AIL_set_sequence_loop_count@8
extern "C" __declspec(naked) void __stdcall __E__260__()
{
	__asm
	{
		jmp p[260 * 4];
	}
}

// _AIL_set_sequence_ms_position@8
extern "C" __declspec(naked) void __stdcall __E__261__()
{
	__asm
	{
		jmp p[261 * 4];
	}
}

// _AIL_set_sequence_tempo@12
extern "C" __declspec(naked) void __stdcall __E__262__()
{
	__asm
	{
		jmp p[262 * 4];
	}
}

// _AIL_set_sequence_user_data@12
extern "C" __declspec(naked) void __stdcall __E__263__()
{
	__asm
	{
		jmp p[263 * 4];
	}
}

// _AIL_set_sequence_volume@12
extern "C" __declspec(naked) void __stdcall __E__264__()
{
	__asm
	{
		jmp p[264 * 4];
	}
}

// _AIL_set_stream_loop_count@8
extern "C" __declspec(naked) void __stdcall __E__265__()
{
	__asm
	{
		jmp p[265 * 4];
	}
}

// _AIL_set_stream_ms_position@8
extern "C" __declspec(naked) void __stdcall __E__266__()
{
	__asm
	{
		jmp p[266 * 4];
	}
}

// _AIL_set_stream_pan@8
extern "C" __declspec(naked) void __stdcall __E__267__()
{
	__asm
	{
		jmp p[267 * 4];
	}
}

// _AIL_set_stream_playback_rate@8
extern "C" __declspec(naked) void __stdcall __E__268__()
{
	__asm
	{
		jmp p[268 * 4];
	}
}

// _AIL_set_stream_position@8
extern "C" __declspec(naked) void __stdcall __E__269__()
{
	__asm
	{
		jmp p[269 * 4];
	}
}

// _AIL_set_stream_reverb@16
extern "C" __declspec(naked) void __stdcall __E__270__()
{
	__asm
	{
		jmp p[270 * 4];
	}
}

// _AIL_set_stream_user_data@12
extern "C" __declspec(naked) void __stdcall __E__271__()
{
	__asm
	{
		jmp p[271 * 4];
	}
}

// _AIL_set_stream_volume@8
extern "C" __declspec(naked) void __stdcall __E__272__()
{
	__asm
	{
		jmp p[272 * 4];
	}
}

// _AIL_set_timer_divisor@8
extern "C" __declspec(naked) void __stdcall __E__273__()
{
	__asm
	{
		jmp p[273 * 4];
	}
}

// _AIL_set_timer_frequency@8
extern "C" __declspec(naked) void __stdcall __E__274__()
{
	__asm
	{
		jmp p[274 * 4];
	}
}

// _AIL_set_timer_period@8
extern "C" __declspec(naked) void __stdcall __E__275__()
{
	__asm
	{
		jmp p[275 * 4];
	}
}

// _AIL_set_timer_user@8
extern "C" __declspec(naked) void __stdcall __E__276__()
{
	__asm
	{
		jmp p[276 * 4];
	}
}

// _AIL_shutdown@0
extern "C" __declspec(naked) void __stdcall __E__277__()
{
	__asm
	{
		jmp p[277 * 4];
	}
}

// _AIL_size_processed_digital_audio@16
extern "C" __declspec(naked) void __stdcall __E__278__()
{
	__asm
	{
		jmp p[278 * 4];
	}
}

// _AIL_start_3D_sample@4
extern "C" __declspec(naked) void __stdcall __E__279__()
{
	__asm
	{
		jmp p[279 * 4];
	}
}

// _AIL_start_all_timers@0
extern "C" __declspec(naked) void __stdcall __E__280__()
{
	__asm
	{
		jmp p[280 * 4];
	}
}

// _AIL_start_sample@4
extern "C" __declspec(naked) void __stdcall __E__281__()
{
	__asm
	{
		jmp p[281 * 4];
	}
}

// _AIL_start_sequence@4
extern "C" __declspec(naked) void __stdcall __E__282__()
{
	__asm
	{
		jmp p[282 * 4];
	}
}

// _AIL_start_stream@4
extern "C" __declspec(naked) void __stdcall __E__283__()
{
	__asm
	{
		jmp p[283 * 4];
	}
}

// _AIL_start_timer@4
extern "C" __declspec(naked) void __stdcall __E__284__()
{
	__asm
	{
		jmp p[284 * 4];
	}
}

// _AIL_startup@0
extern "C" __declspec(naked) void __stdcall __E__285__()
{
	__asm
	{
		jmp p[285 * 4];
	}
}

// _AIL_stop_3D_sample@4
extern "C" __declspec(naked) void __stdcall __E__286__()
{
	__asm
	{
		jmp p[286 * 4];
	}
}

// _AIL_stop_all_timers@0
extern "C" __declspec(naked) void __stdcall __E__287__()
{
	__asm
	{
		jmp p[287 * 4];
	}
}

// _AIL_stop_sample@4
extern "C" __declspec(naked) void __stdcall __E__288__()
{
	__asm
	{
		jmp p[288 * 4];
	}
}

// _AIL_stop_sequence@4
extern "C" __declspec(naked) void __stdcall __E__289__()
{
	__asm
	{
		jmp p[289 * 4];
	}
}

// _AIL_stop_timer@4
extern "C" __declspec(naked) void __stdcall __E__290__()
{
	__asm
	{
		jmp p[290 * 4];
	}
}

// _AIL_stream_info@20
extern "C" __declspec(naked) void __stdcall __E__291__()
{
	__asm
	{
		jmp p[291 * 4];
	}
}

// _AIL_stream_loop_count@4
extern "C" __declspec(naked) void __stdcall __E__292__()
{
	__asm
	{
		jmp p[292 * 4];
	}
}

// _AIL_stream_ms_position@12
extern "C" __declspec(naked) void __stdcall __E__293__()
{
	__asm
	{
		jmp p[293 * 4];
	}
}

// _AIL_stream_pan@4
extern "C" __declspec(naked) void __stdcall __E__294__()
{
	__asm
	{
		jmp p[294 * 4];
	}
}

// _AIL_stream_playback_rate@4
extern "C" __declspec(naked) void __stdcall __E__295__()
{
	__asm
	{
		jmp p[295 * 4];
	}
}

// _AIL_stream_position@4
extern "C" __declspec(naked) void __stdcall __E__296__()
{
	__asm
	{
		jmp p[296 * 4];
	}
}

// _AIL_stream_reverb@16
extern "C" __declspec(naked) void __stdcall __E__297__()
{
	__asm
	{
		jmp p[297 * 4];
	}
}

// _AIL_stream_status@4
extern "C" __declspec(naked) void __stdcall __E__298__()
{
	__asm
	{
		jmp p[298 * 4];
	}
}

// _AIL_stream_user_data@8
extern "C" __declspec(naked) void __stdcall __E__299__()
{
	__asm
	{
		jmp p[299 * 4];
	}
}

// _AIL_stream_volume@4
extern "C" __declspec(naked) void __stdcall __E__300__()
{
	__asm
	{
		jmp p[300 * 4];
	}
}

// _AIL_true_sequence_channel@8
extern "C" __declspec(naked) void __stdcall __E__301__()
{
	__asm
	{
		jmp p[301 * 4];
	}
}

// _AIL_unlock@0
extern "C" __declspec(naked) void __stdcall __E__302__()
{
	__asm
	{
		jmp p[302 * 4];
	}
}

// _AIL_us_count@0
extern "C" __declspec(naked) void __stdcall __E__303__()
{
	__asm
	{
		jmp p[303 * 4];
	}
}

// _AIL_waveOutClose@4
extern "C" __declspec(naked) void __stdcall __E__304__()
{
	__asm
	{
		jmp p[304 * 4];
	}
}

// _AIL_waveOutOpen@16
extern "C" __declspec(naked) void __stdcall __E__305__()
{
	__asm
	{
		jmp p[305 * 4];
	}
}

// _DLSMSSGetCPU@4
extern "C" __declspec(naked) void __stdcall __E__306__()
{
	__asm
	{
		jmp p[306 * 4];
	}
}

// _DllMain@12
extern "C" __declspec(naked) void __stdcall __E__307__()
{
	__asm
	{
		jmp p[307 * 4];
	}
}

// _RIB_enumerate_providers@12
extern "C" __declspec(naked) void __stdcall __E__308__()
{
	__asm
	{
		jmp p[308 * 4];
	}
}

// _RIB_find_provider@12
extern "C" __declspec(naked) void __stdcall __E__309__()
{
	__asm
	{
		jmp p[309 * 4];
	}
}

// _RIB_load_application_providers@4
extern "C" __declspec(naked) void __stdcall __E__310__()
{
	__asm
	{
		jmp p[310 * 4];
	}
}

// _RIB_provider_library_handle@0
extern "C" __declspec(naked) void __stdcall __E__311__()
{
	__asm
	{
		jmp p[311 * 4];
	}
}

// _RIB_provider_system_data@8
extern "C" __declspec(naked) void __stdcall __E__312__()
{
	__asm
	{
		jmp p[312 * 4];
	}
}

// _RIB_provider_user_data@8
extern "C" __declspec(naked) void __stdcall __E__313__()
{
	__asm
	{
		jmp p[313 * 4];
	}
}

// _RIB_set_provider_system_data@12
extern "C" __declspec(naked) void __stdcall __E__314__()
{
	__asm
	{
		jmp p[314 * 4];
	}
}

// _RIB_set_provider_user_data@12
extern "C" __declspec(naked) void __stdcall __E__315__()
{
	__asm
	{
		jmp p[315 * 4];
	}
}

