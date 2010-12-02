-- name: Demo 1
-- author: Rich Lane
-- version: 1.0
-- description: 
-- min_teams: 0
-- max_teams: 0
dofile(data_dir .. "/scenarios/lib.lua")

local teams = { "denim", "ruby", "sushi" }

for i, t in ipairs(teams) do
	team(t, data_dir .. "/examples/demo1.lua", colors[t])
end

local M = 32
for i = 1,M do
	local r = 10
	local a = i*(2*math.pi)/M
	local x = r * math.cos(a)
	local y = r * math.sin(a)
	ship("fighter", teams[1 + (i % #teams)], x, y)
end

local N = #teams
for i = 1,N do
	local r = 3
	local a = i*(2*math.pi)/N
	local x = r * math.cos(a)
	local y = r * math.sin(a)
	ship("mothership", teams[1 + (i % #teams)], x, y)
end
