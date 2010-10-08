function R(a,b)
	d = b - a
	return a + math.random()*d
end

colors = {
	fern = 0x59BA7900,
	husk = 0x9E9B4B00,
	amethyst = 0xA265D300,
	mattise = 0x3A627A00,
	emerald = 0x60C97E00,
	royalblue = 0x4350E000,
	sushi = 0x92A53100,
	pueblo = 0x752F2300,
	zucchini = 0x0C260B00,
	turquoise = 0x44E5BD00,
	sapphire = 0x0D1E7C00,
	denim = 0x1653CE00,
	ruby = 0xD6176600,
	eggplant = 0x7C005D00,
}

indexed_colors = {}
local i = 1
for name, color in pairs(colors) do
	indexed_colors[i] = name
	i = i + 1
end

num_colors = table.getn(indexed_colors)
