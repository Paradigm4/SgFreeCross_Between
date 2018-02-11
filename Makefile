ifeq ($(SCIDB),)
  X := $(shell which scidb 2>/dev/null)
  ifneq ($(X),)
    X := $(shell dirname ${X})
    SCIDB := $(shell dirname ${X})
  endif
endif

# A way to set the 3rdparty prefix path that is convenient
# for SciDB developers.
ifeq ($(SCIDB_VER),)
   SCIDB_3RDPARTY = $(SCIDB)
else
   SCIDB_3RDPARTY = /opt/scidb/$(SCIDB_VER)
endif

# A better way to set the 3rdparty prefix path that does
# not assume an absolute path. You can still use the above
# # method if you prefer.
ifeq ($(SCIDB_THIRDPARTY_PREFIX),)
  SCIDB_THIRDPARTY_PREFIX := $(SCIDB_3RDPARTY)
endif
#
#   # Debug:
#CFLAGS=-pedantic -W -Wextra -Wall -Wno-variadic-macros -Wno-strict-aliasing -Wno-long-long -Wno-unused-parameter -fPIC -D_STDC_FORMAT_MACROS -Wno-system-headers -g -ggdb3  -D_STDC_LIMIT_MACROS
CFLAGS=-pedantic -W -Wextra -Wall -Wno-unused-parameter -Wno-variadic-macros -Wno-strict-aliasing -Wno-long-long -Wno-unused -fPIC -D_STDC_FORMAT_MACROS -Wno-system-headers -O3 -DNDEBUG -D_STDC_LIMIT_MACROS
INC=-I. -I./asof/ -I./extern -DPROJECT_ROOT="\"$(SCIDB)\"" -I"$(SCIDB_THIRDPARTY_PREFIX)/3rdparty/boost/include/" -I"$(SCIDB)/include"
LIBS=-shared -Wl,-soname,libaxial_aggregate.so -L. -L"$(SCIDB_THIRDPARTY_PREFIX)/3rdparty/boost/lib" -L"$(SCIDB)/lib" -Wl,-rpath,$(SCIDB)/lib:$(RPATH) -lm

# Compiler settings for SciDB version >= 15.7
ifneq ("$(wildcard /usr/bin/g++-4.9)","")
 CC := "/usr/bin/gcc-4.9"
 CXX := "/usr/bin/g++-4.9"
 CFLAGS+=-std=c++14 -DCPP11
else
 ifneq ("$(wildcard /opt/rh/devtoolset-3/root/usr/bin/gcc)","")
  CC := "/opt/rh/devtoolset-3/root/usr/bin/gcc"
  CXX := "/opt/rh/devtoolset-3/root/usr/bin/g++"
  CFLAGS+=-std=c++14 -DCPP11
 endif
endif

libsg_free_cross_between.so: plugin.cpp.o LogicalCrossBetween.cpp.o PhysicalCrossBetween.cpp.o
	@if test ! -d "$(SCIDB)"; then echo  "Error. Try:\n\nmake SCIDB=<PATH TO SCIDB INSTALL PATH>"; exit 1; fi
	  
	$(CXX) $(CFLAGS) $(INC) -o libsg_free_cross_between.so plugin.cpp.o LogicalCrossBetween.cpp.o PhysicalCrossBetween.cpp.o $(LIBS)
	@echo "Now copy *.so to your SciDB lib/scidb/plugins directory"
	@echo "Remember to copy the plugin to all your nodes in the cluster."
	@echo "Now run..."
	@echo "iquery -aq \"load_library('sgf_cross_between')\" # to load the plugin."
	@echo
	@echo "Re-start SciDB if the plugin was already loaded previously."

plugin.cpp.o:
	$(CXX) $(CFLAGS) $(INC) -o plugin.cpp.o -c plugin.cpp

LogicalCrossBetween.cpp.o:
	$(CXX) $(CFLAGS) $(INC) -o LogicalCrossBetween.cpp.o -c LogicalCrossBetween.cpp

PhysicalCrossBetween.cpp.o:
	$(CXX) $(CFLAGS) $(INC) -o PhysicalCrossBetween.cpp.o -c PhysicalCrossBetween.cpp

clean:
	rm -f *.so *.o
