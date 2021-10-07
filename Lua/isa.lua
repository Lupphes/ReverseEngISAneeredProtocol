--[[
	07.10.2021
	isa.lua v1.0
	Wireshark Lua ISAMail protocol dissector
--]]

local function parseMessageBySpace(removeFL, string)
	local chunks = {}
	for substring in string:gmatch("%S+") do
		if removeFL then
			table.insert(chunks, substring:sub(1, -2):sub(2))
		else
			table.insert(chunks, substring)
		end
	end
	return chunks
end



local function LaunchISAMaildissector()
	-- Define all values
	local protocolName = 'ISAMail'
	local protocolSuccess = 'ok' -- 6f 6b
	local protocolError = 'err' -- 65 72 72

	-- Define protocol
	local isamailproto = Proto(protocolName, protocolName .. ' Protocol')


	isamailproto.fields.status = ProtoField.string(protocolName .. '.status', 'Status', 'The status of message')
	isamailproto.fields.request = ProtoField.string(protocolName .. '.request', 'Request', 'The name of the request')

	isamailproto.fields.login = ProtoField.string(protocolName .. '.login', 'Login', 'The login of the user')
	isamailproto.fields.password = ProtoField.string(protocolName .. '.password', 'Password', 'The password of the user')
	isamailproto.fields.token = ProtoField.string(protocolName .. '.token', 'Token', 'Recieved token')
	isamailproto.fields.id = ProtoField.string(protocolName .. '.id', 'ID', 'User\'s id')

	isamailproto.fields.recipient = ProtoField.string(protocolName .. '.recipient', 'Recipient', 'The recipient of the message')
	isamailproto.fields.subject = ProtoField.string(protocolName .. '.subject', 'Subject', 'The subject of the message')
	isamailproto.fields.msg = ProtoField.string(protocolName .. '.msg', 'Message', 'The message which was sent')

	isamailproto.fields.length = ProtoField.uint8(protocolName .. '.length', 'Length', base.DEC, nil, nil, 'The length of sent data from server')


	function isamailproto.dissector(buffer, pinfo, tree)
		local protoName = isamailproto.name
		
		local bufferDataLength = buffer:len()

		-- We expect at least 7 bytes and full lenght of msg, otherwise we don't dissect
		if bufferDataLength < 7 or bufferDataLength ~= buffer:reported_len() then
			return
		end

		local parsedDataFromBuffer = ''
		
		for i = 1, bufferDataLength - 2, 1
		do
			newchar = buffer(i,1):string()
			if newchar == nil then
				return
			else
				parsedDataFromBuffer = parsedDataFromBuffer .. newchar
			end
		end

		print('Data recieved: ' .. parsedDataFromBuffer)
		  
		local parsedStatusMsg = parsedDataFromBuffer:match('%S+')
		local statusMsgLength = parsedDataFromBuffer:match('%S+'):len()
		local clintOrServerParsed = parsedDataFromBuffer:sub(statusMsgLength + 2, -1) -- Add 2 to remove index and space

		pinfo.cols.protocol = protoName

		local treeRoot = tree:add(isamailproto, buffer(), protoName .. ' Protocol Data')
		
		-- If message don't have 'err' or 'ok' infront of the message, it came from the client
		if parsedStatusMsg ~= protocolError and parsedStatusMsg ~= protocolSuccess then 
			treeRoot:add(isamailproto.fields.request, parsedStatusMsg)

			if parsedStatusMsg == 'register' or parsedStatusMsg == 'login' then
				local chunks = parseMessageBySpace(true, clintOrServerParsed)
				
				treeRoot:add(isamailproto.fields.login, chunks[1])
				treeRoot:add(isamailproto.fields.password, chunks[2])

			elseif parsedStatusMsg == 'list' or parsedStatusMsg == 'logout' then
				treeRoot:add(isamailproto.fields.token, clintOrServerParsed:sub(1, -2):sub(2))

			elseif parsedStatusMsg == 'fetch' then
				local chunks = parseMessageBySpace(false, clintOrServerParsed)

				treeRoot:add(isamailproto.fields.token, chunks[1]:sub(1, -2):sub(2))
				treeRoot:add(isamailproto.fields.id, chunks[2])

			elseif parsedStatusMsg == 'send' then

				local chunks = parseMessageBySpace(true, clintOrServerParsed)

				treeRoot:add(isamailproto.fields.token, chunks[1])
				treeRoot:add(isamailproto.fields.recipient, chunks[2])
				treeRoot:add(isamailproto.fields.subject, chunks[3])
				treeRoot:add(isamailproto.fields.msg, chunks[4])
			end
			
		elseif parsedStatusMsg == protocolError then
			treeRoot:add(isamailproto.fields.status, parsedStatusMsg)
			treeRoot:add(isamailproto.fields.msg, clintOrServerParsed)
		elseif parsedStatusMsg == protocolSuccess then
			-- maybe will add later
			treeRoot:add(isamailproto.fields.status, parsedStatusMsg)
			treeRoot:add(isamailproto.fields.msg, clintOrServerParsed)
		else
			return
		end

		treeRoot:add(isamailproto.fields.length, bufferDataLength)

	end


	DissectorTable.get('tcp.port'):add(69, isamailproto)
end

local function Main()
	LaunchISAMaildissector()
end

Main()