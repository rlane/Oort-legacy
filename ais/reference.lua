while true do
	print(sys_position())
	print(sys_velocity())
	print(sys_heading())
	print(sys_angular_velocity())
	coroutine.yield()
end
