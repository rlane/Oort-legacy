while true do
	local p = position()
	print(p[1], p[2])
	coroutine.yield()
end
