dofile("scenarios/lib.lua")

M = 32
W = 30
H = 20

if N >= num_colors then
	error("too many teams")
end

for ti = 1,N do
	local name = indexed_colors[ti]
	team(name, colors[name])
	print("team " .. name .. " is " .. AI[ti-1])
	for i = 1,M do
		ship("fighter", AI[ti-1], name, R(-W,W), R(-H,H))
	end
end
