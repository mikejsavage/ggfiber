if #arg ~= 1 then
	print( "usage: " .. arg[ 0 ] .. " <file.bin>" )
	os.exit( 1 )
end

local f = assert( io.open( arg[ 1 ], "rb" ) )
local contents = assert( f:read( "*all" ) )
assert( f:close() )

local instruction_boundaries = {
	string.char( 0x48, 0x8d ), -- lea
	string.char( 0x48, 0x89 ), -- store64 REX.W
	string.char( 0x4c, 0x89 ), -- store64 REX.WR
	string.char( 0x4d, 0x89 ), -- store64 REX.??
	string.char( 0x0f, 0x29 ), -- store128
	string.char( 0x44, 0x0f, 0x29 ), -- store128 REX.??
	string.char( 0x65, 0x4c ), -- load64 gs rel
	string.char( 0x48, 0x8b ), -- load64 REX.W
	string.char( 0x4c, 0x8b ), -- load64 REX.WR
	string.char( 0x4d, 0x8b ), -- load64 REX.??
	string.char( 0x0f, 0x28 ), -- load128
	string.char( 0x44, 0x0f, 0x28 ), -- load128 REX.??
	string.char( 0xff ), -- jmp
	string.char( 0x41, 0xff ), -- jmp REX.??
	string.char( 0xc3 ), -- ret
}

function string.startswith( self, prefix )
	return self:sub( 1, #prefix ) == prefix
end

local bytes = { }
local function flush()
	if #bytes > 0 then
		print( table.concat( bytes, " " ) )
		bytes = { }
	end
end

local noflush = 0

for i = 1, #contents do
	if noflush == 0 then
		for _, boundary in ipairs( instruction_boundaries ) do
			if contents:sub( i ):startswith( boundary ) then
				flush()
				noflush = #boundary - 1
				break
			end
		end
	else
		noflush = noflush - 1
	end

	table.insert( bytes, string.format( "0x%02x,", contents:byte( i ) ) )
end

flush()

-- pad with int3
if #contents % 16 ~= 0 then
	print( string.rep( "0xcc, ", 16 - ( #contents % 16 ) ) )
end
