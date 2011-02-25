{
	luaL_loadstring(L,
		"function chatMode() \
			if 0==bitAnd(gameFlags(),0x80) then \
				gameFlags(gameFlags()+0x80) \
			else \
				gameFlags(gameFlags()-0x80) \
			end \
		end"
	);
}
