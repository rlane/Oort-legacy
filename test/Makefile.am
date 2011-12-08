NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(LUA_CFLAGS) \
	$(BOX2D_CFLAGS) \
	$(NULL)

all: move.so

%.so: %.cc
	$(CXX) -shared -fPIC $(CXXFLAGS) $(AM_CXXFLAGS) $^ -o $@
