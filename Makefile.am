NULL = 

AUTOMAKE_OPTIONS = foreign

SUBDIRS = \
	sim \
	ui/headless \
	ui/sdl \
	$(NULL)

lint:
	find sim ui gl -iname '*.cc' -o -iname '*.h' | xargs python cpplint.py --filter=-whitespace/tab,-build/header_guard,-\whitespace/blank_line,-whitespace/labels
