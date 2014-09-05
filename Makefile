LLVM_BUILDDIR=$(shell llvm-config --prefix)
LLVM_INCDIR=$(shell llvm-config --includedir)

CXX=g++
CXXFLAGS=$(shell llvm-config --cxxflags)
CXXFLAGS+=-I./include
CXXFLAGS+=-I$(LLVM_BUILDDIR)/tools/clang/include
CXXFLAGS+=-I$(LLVM_INCDIR)/../tools/clang/include

BUILDDIR=obj

LIB_CXX=$(shell find lib -name *.cxx)
LIB_O=$(foreach obj,$(LIB_CXX),$(addprefix $(BUILDDIR)/,$(subst cxx,o,$(obj))))

.PHONY: all
all: $(LIB_O)
	@echo $(LIB_TARGETS)

$(LIB_O): $(BUILDDIR)/%.o: %.cxx
	@test -d $(shell dirname $@) || mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*
