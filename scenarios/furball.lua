dofile("scenarios/lib.lua")

if N >= num_colors then
	error("too many teams")
end

for ti = 1,N do
	local name = indexed_colors[ti]
	team(name, colors[name])
	print("team " .. name .. " is " .. AI[ti-1])
	for i = 1,32 do
		ship("fighter", AI[ti-1], name, R(-30,30), R(-20,20))
	end
end
