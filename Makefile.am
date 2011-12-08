NULL = 

AUTOMAKE_OPTIONS = foreign

SUBDIRS = \
	common \
	sim \
	renderer \
	gl \
	ui/headless \
	ui/sdl \
	third_party \
	test \
	$(NULL)

lint:
	find sim renderer ui gl -iname '*.cc' -o -iname '*.h' | xargs python cpplint.py --filter=-whitespace/tab,-build/header_guard,-\whitespace/blank_line,-whitespace/labels
