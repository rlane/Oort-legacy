NULL = 

AM_CXXFLAGS = \
	-include $(CONFIG_HEADER) \
	-include testcase.h \
	-I $(top_srcdir) \
	$(GLM_CFLAGS) \
	$(LUA_CFLAGS) \
	$(BOX2D_CFLAGS) \
	$(NULL)

TEST_LIBS := $(patsubst %.cc,%.so,$(wildcard *.cc))

all: $(TEST_LIBS)

test: all
	echo $(TEST_LIBS) | xargs -n1 $(top_srcdir)/ui/headless/oort_headless

%.so: %.cc testcase.h
	$(CXX) -shared -fPIC $(CXXFLAGS) $(AM_CXXFLAGS) $< -o $@
