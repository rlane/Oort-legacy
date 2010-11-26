-- name: Challenge 3
-- author: Rich Lane
-- version: 1.0
-- description: 
-- min_teams: 1
-- max_teams: 1
dofile("scenarios/lib.lua")

if N ~= 1 then
	error("this scenario only supports 1 team")
end

team("green", 0x00FF0000)
team("turquoise", colors.turquoise)

ship("fighter", AI[0], "green", 0, 0)
ship("fighter", "examples/challenge03.lua", "turquoise", 2, 0)
ship("fighter", "examples/challenge03.lua", "turquoise", -2, 0)
ship("fighter", "examples/challenge03.lua", "turquoise", 0, 2)
