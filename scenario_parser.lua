metadata = {}

local file = io.open(filename, 'r')
for line in file:lines() do
	key, value = string.match(line, "^-- ([%w_]+):%s+(.*)$")
	if not key then	break end
	metadata[key] = value
	print(key, value)
end

return metadata
