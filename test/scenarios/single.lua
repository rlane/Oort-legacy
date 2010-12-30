if N ~= 1 then
	error("this scenario only supports 1 team")
end

team("green", AI[0], 0x00FF0000)
ship("fighter", "green", 0, 0)
