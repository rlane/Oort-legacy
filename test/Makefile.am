NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-include testcase.h \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(LUA_CFLAGS) \
	$(BOX2D_CFLAGS) \
	$(NULL)

all: $(patsubst %.cc,%.so,$(wildcard *.cc))

%.so: %.cc
	$(CXX) -shared -fPIC $(CXXFLAGS) $(AM_CXXFLAGS) $^ -o $@
