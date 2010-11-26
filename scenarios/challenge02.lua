-- name: Challenge 2
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
team("eggplant", colors.eggplant)

ship("fighter", AI[0], "green", 0, 0)
ship("fighter", "examples/challenge02.lua", "eggplant", R(-2,2), R(-2,2))
