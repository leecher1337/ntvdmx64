struct cvid_interface {
	struct VideoVector *Video;

	void (__stdcall *SetBiosWrites) (IUH markset);
	void (__stdcall *setReadPointers) (IUH readset);
	void (__stdcall *setWritePointers) ();
	void (__stdcall *setMarkPointers) (IUH markset);

	void (__stdcall *write_byte_ev_glue) (IU32 eaOff, IU8 eaVal);
	void (__stdcall *write_word_ev_glue) (IU32 eaOff, IU16 eaVal);
	void (__stdcall *write_dword_ev_glue) (IU32 eaOff, IU32 eaVal);
	void (__stdcall *fill_byte_ev_glue) (IU32 eaOff, IU8 eaVal, IU32 count);
	void (__stdcall *fill_word_ev_glue) (IU32 eaOff, IU16 eaVal, IU32 count);
	void (__stdcall *fill_dword_ev_glue) (IU32 eaOff, IU32 eaVal, IU32 count);
	void (__stdcall *move_byte_fwd_ev_glue) (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
	void (__stdcall *move_byte_bwd_ev_glue) (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
	void (__stdcall *move_word_fwd_ev_glue) (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
	void (__stdcall *move_word_bwd_ev_glue) (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
	void (__stdcall *move_dword_fwd_ev_glue) (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
	void (__stdcall *move_dword_bwd_ev_glue) (IU32 eaOff, IHPE fromOff, IU32 count, IBOOL srcInRAM);
	IU32 (__stdcall *read_byte_ev_glue) (IU32 eaOff);
	IU32 (__stdcall *read_word_ev_glue) (IU32 eaOff);
	IU32 (__stdcall *read_dword_ev_glue) (IU32 eaOff);
	void (__stdcall *read_str_fwd_ev_glue) (IU8 * dest, IU32 eaOff, IU32 count);
	void (__stdcall *read_str_bwd_ev_glue) (IU8 * dest, IU32 eaOff, IU32 count);

	EVID_WRT_POINTERS *simple_evid;
	EVID_WRT_POINTERS *gricvid_evid;
	EVID_WRT_POINTERS *dith_evid;
	EVID_WRT_POINTERS *chain2_evid;
	EVID_WRT_POINTERS *unchained_evid;
	EVID_WRT_POINTERS *chain4_evid;
	EVID_READ_POINTERS *simple_read_evid;
	EVID_READ_POINTERS *ram_dsbld_read_evid;
	EVID_READ_POINTERS *read_mode0_evid;
	EVID_READ_POINTERS *read_mode1_evid;
	EVID_MARK_POINTERS *simple_mark_evid;
	EVID_MARK_POINTERS *cga_mark_evid;
	EVID_MARK_POINTERS *unchained_mark_evid;
	EVID_MARK_POINTERS *chain4_mark_evid;
};

