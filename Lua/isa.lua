--[[
	Wireshark Lua ISAMail protocol dissector

	isa.lua v1.0 – 07.10.2021
	isa.lua v2.0 – 23.10.2021
	
	Author: Ondřej Sloup (xsloup02)
--]]

--[[
	Function which parses the words from recieved package
	It is rewritten code from requestmsg.cpp from C to Lua
	Inputs: 
		string str
		bool checkEnd
	Outputs:
		Function either outputs end of the message if the 
		checkEnd was set to true or returns an array of parsed values
--]]
local function parseMessage(str, checkEnd)
	local temp = ''
	flagMark = false -- signs if the string is in quotation marks since i++
	skipflag = false -- since i++ doesn't work it is done via flag
	local chunks = {}

	if checkEnd then
		local endStr = str:gsub('"(.-)[^\\]"', ''):sub(-1) -- take last char
		return endStr
	else
		for i = 1, #str do
			if skipflag then
				skipflag = false
				goto continue
			end
			local c = str:sub(i,i)
			if c == '"' then
				if flagMark then
					table.insert(chunks, temp)
					temp = '';
					flagMark = false
				else
					flagMark = true
				end
				goto continue
			elseif c == '\\' then
				temp = temp .. c
				skipflag = true
			end
			if flagMark then
				temp = temp .. c
			end
			::continue::
		end
		return chunks
	end
end

--[[
	ISA mail dissector which handles everything and adds 
	structure to wireshark
	Checks via lua patterns for every possibility how packet can look
--]]
local function LaunchISAMaildissector()
	-- Define all values
	local protocolName = 'ISAMail'
	local protocolSuccess = 'ok' -- 6f 6b
	local protocolError = 'err' -- 65 72 72

	-- Definition of the protocol structure
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

	-- Launch the dissector
	function isamailproto.dissector(buffer, pinfo, tree)
		local protoName = isamailproto.name
		
		local bufferDataLength = buffer:len()
		
		-- We expect at least 7 bytes and full lenght of msg, otherwise we don't dissect
		if bufferDataLength < 7 or bufferDataLength ~= buffer:reported_len() then
			return
		end

		local lastChar = parseMessage(buffer():string(), true)

		-- Reassembly
		if (lastChar ~= ')') then
			pinfo.desegment_len = DESEGMENT_ONE_MORE_SEGMENT
			return
		end
	
		-- DEBUG: Print whole msg
		-- print('Data recieved: ' .. buffer():string())
		  
		local parsedStatusMsg = buffer():string():match('%S+'):sub(2, -1)
		local statusMsgLength = buffer():string():match('%S+'):len()
		local clintOrServerParsed = buffer():string():sub(statusMsgLength + 2, -2) -- Add 2 to remove index and space

		pinfo.cols.protocol = protoName

		local treeRoot = tree:add(isamailproto, buffer(), protoName .. ' Protocol Data')
		
		-- If message don't have 'err' or 'ok' infront of the message, it came from the client
		if parsedStatusMsg ~= protocolError and parsedStatusMsg ~= protocolSuccess then 
			treeRoot:add(isamailproto.fields.request, parsedStatusMsg)

			if parsedStatusMsg == 'register' or parsedStatusMsg == 'login' then
				local chunks = parseMessage(clintOrServerParsed, false)
				
				treeRoot:add(isamailproto.fields.login, chunks[1])
				treeRoot:add(isamailproto.fields.password, chunks[2])
				if parsedStatusMsg == 'register' then
					pinfo.cols.info = 'Register request – ' .. chunks[1]
				else
					pinfo.cols.info = 'Login request – ' .. chunks[1]
				end
			elseif parsedStatusMsg == 'list' or parsedStatusMsg == 'logout' then
				treeRoot:add(isamailproto.fields.token, clintOrServerParsed:sub(1, -2):sub(2))
				local chunks = parseMessage(clintOrServerParsed, false)
				
				if parsedStatusMsg == 'list' then
					pinfo.cols.info = 'List request – ' .. chunks[1]
				else
					pinfo.cols.info = 'Logout request – ' .. chunks[1]
				end

			elseif parsedStatusMsg == 'fetch' then
				local chunks = parseMessage(clintOrServerParsed, false)

				treeRoot:add(isamailproto.fields.token, chunks[1]:sub(1, -2):sub(2)) -- Removing " "
				treeRoot:add(isamailproto.fields.id, clintOrServerParsed:sub(clintOrServerParsed:len()))
				pinfo.cols.info = 'Fetch request – ' .. clintOrServerParsed:sub(clintOrServerParsed:len())

			elseif parsedStatusMsg == 'send' then
				local chunks = parseMessage(clintOrServerParsed, false)

				treeRoot:add(isamailproto.fields.token, chunks[1])
				treeRoot:add(isamailproto.fields.recipient, chunks[2])
				treeRoot:add(isamailproto.fields.subject, chunks[3])
				treeRoot:add(isamailproto.fields.msg, chunks[4])
				pinfo.cols.info = 'Send request –> ' .. chunks[2] .. ', ' .. chunks[3]
			end
		elseif parsedStatusMsg == protocolError then -- ERROR -> Print msg
			pinfo.cols.info = 'Response Error'
			treeRoot:add(isamailproto.fields.status, parsedStatusMsg)
			treeRoot:add(isamailproto.fields.msg, clintOrServerParsed)
		elseif parsedStatusMsg == protocolSuccess then -- OK -> Parse msg from server
			if clintOrServerParsed:match('^"registered user ') then
				pinfo.cols.info = 'Response register – ' .. string.sub(clintOrServerParsed, 17, -2) -- Removing " "
				treeRoot:add(isamailproto.fields.login, string.sub(clintOrServerParsed, 17, -2)) -- Removing " "

			elseif clintOrServerParsed:match('^"user logged in" ') then
				pinfo.cols.info = 'Response login – ' .. string.sub(clintOrServerParsed, 19, -2) -- Removing " "
				treeRoot:add(isamailproto.fields.token, string.sub(clintOrServerParsed, 19, -2)) -- Removing " "

			elseif clintOrServerParsed:match('"message sent"') then
				pinfo.cols.info = 'Response send – ' .. string.sub(clintOrServerParsed, 2, -2) -- Removing " "

			elseif clintOrServerParsed:match('"logged out"') then
				pinfo.cols.info = 'Response logged out'
			
			elseif clintOrServerParsed:match('^%(%)') or clintOrServerParsed:match('^%(%(') then
				
				if clintOrServerParsed == "()" then
					pinfo.cols.info = 'Response list – No message'
				else
					local chunksMsg = parseMessage(clintOrServerParsed, false)
					pinfo.cols.info = 'Response list – ' .. #(chunksMsg) .. ' messages'
					for i = 1,#(chunksMsg),1 
					do 
						local treeMsg = treeRoot:add(isamailproto.fields.msg, i)
						treeMsg:add(isamailproto.fields.recipient, chunksMsg[1])
						treeMsg:add(isamailproto.fields.subject, chunksMsg[2])	
						
					end
				end
			elseif clintOrServerParsed:match('^%("(.*)"%s"(.*)"%s"(.*)"%)$') then
				local chunks = parseMessage(clintOrServerParsed, false)
				pinfo.cols.info = 'Response fetch – ' .. chunks[1] .. ', ' .. chunks[2]
				treeRoot:add(isamailproto.fields.recipient, chunks[1])
				treeRoot:add(isamailproto.fields.subject, chunks[2])
				treeRoot:add(isamailproto.fields.msg, chunks[3])
			else
				pinfo.cols.info = 'Unknown packet'
			end
			treeRoot:add(isamailproto.fields.status, parsedStatusMsg)
			treeRoot:add(isamailproto.fields.msg, clintOrServerParsed)
		else
			return
		end
		treeRoot:add(isamailproto.fields.length, bufferDataLength)
	end
	DissectorTable.get('tcp.port'):add(32323, isamailproto)
end

--[[
	Main function which launches whole process
--]]
local function Main()
	LaunchISAMaildissector()
end

Main()