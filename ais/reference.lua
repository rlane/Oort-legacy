while true do
	print(sys_position())
	print(sys_velocity())
	print(sys_heading())
	print(sys_angular_velocity())
	sys_thrust_main(10)
	sys_thrust_lateral(5)
	sys_thrust_angular(0.1)
	coroutine.yield()
end
