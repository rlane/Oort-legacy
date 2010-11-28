-- name: Challenge 1
-- author: Rich Lane
-- version: 1.0
-- description: 
-- min_teams: 1
-- max_teams: 1
if N ~= 1 then
	error("this scenario only supports 1 team")
end

team("blue",  0x0000FF00)
team("green", 0x00FF0000)

ship("mothership", AI[0], "green", -10, 0)
ship("mothership", data_dir .. "/examples/target.lua", "blue", 10, 0)
