local my_class = class
local my_ship = ships[my_class]

while true do
	thrust_main(my_ship.max_acc)
	sleep(32)
	thrust_main(-my_ship.max_acc)
	sleep(32)
	thrust_main(0)
	yield()

	thrust_lateral(my_ship.max_acc)
	sleep(32)
	thrust_lateral(-my_ship.max_acc)
	sleep(32)
	thrust_lateral(0)
	yield()

	thrust_angular(math.pi)
	sleep(32)
	thrust_angular(-math.pi)
	sleep(32)
	thrust_angular(0)

	sleep(10)
end
