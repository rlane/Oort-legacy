while true do
	local tx, ty = math.random(-1000,1000), math.random(-1000,1000)
	for i = 1,256 do
		drive_towards(tx,ty,1000)
		yield()
	end
end
