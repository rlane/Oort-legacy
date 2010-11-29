-- name: Furball
-- author: Rich Lane
-- version: 1.0
-- description: 
-- min_teams: 1
-- max_teams: 4
dofile(data_dir .. "/scenarios/lib.lua")

M = 32
W = 30
H = 20

if N >= num_colors then
	error("too many teams")
end

for ti = 1,N do
	local name = indexed_colors[ti]
	team(name, AI[ti-1], colors[name])
	print("team " .. name .. " is " .. AI[ti-1])
	for i = 1,M do
		ship("fighter", name, R(-W,W), R(-H,H))
	end
end
