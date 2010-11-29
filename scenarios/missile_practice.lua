-- name: Missile Practice
-- author: Rich Lane
-- version: 1.0
-- description: 
-- min_teams: 1
-- max_teams: 1
if N ~= 1 then
	error("this scenario only supports 1 team")
end

team("green", AI[0], 0x00FF0000)
team("blue", data_dir .. "/examples/target.lua", 0x0000FF00)

ship("mothership", "green", -10, 0)
ship("mothership", "blue", 10, 0)
