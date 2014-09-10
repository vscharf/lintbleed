ifndef LLVM_BUILDDIR
  LLVM_BUILDDIR=$(shell llvm-config --prefix)
endif

ifndef LLVM_INCDIR
  LLVM_INCDIR=$(shell llvm-config --includedir)
endif

ifndef BOOSTINCDIR
  BOOSTINCDIR=/afsuser/vscharf/src/boost_1_56_0
endif


ifndef BUILDDIR
  BUILDDIR=obj
endif

ifndef BINDIR
  BINDIR=bin
endif

CXX=g++
LD=g++

#CXXFLAGS=$(shell llvm-config --cxxflags | sed 's/ \-fno\-rtti//' | sed 's/ \-fno\-exceptions//')
CXXFLAGS=$(shell llvm-config --cxxflags | sed 's/ \-fno\-exceptions//')
CXXFLAGS+=-I./include
CXXFLAGS+=-I$(LLVM_BUILDDIR)/tools/clang/include
CXXFLAGS+=-I$(LLVM_INCDIR)/../tools/clang/include
CXXFLAGS+=-I$(BOOSTINCDIR)

CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangASTMatchers \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangTooling \
	-Wl,--end-group

LDFLAGS=$(shell llvm-config --ldflags) $(CLANG_LIBS) $(shell llvm-config --libs --system-libs)

STATLIB=$(BUILDDIR)/lib.a

LIB_CXX=$(shell find lib -name '*.cxx')
TEST_CXX=$(shell find tests -name '*.cxx')

LIB_O=$(foreach obj,$(LIB_CXX),$(addprefix $(BUILDDIR)/,$(subst cxx,o,$(obj))))
TEST_BIN=$(foreach test,$(TEST_CXX),$(addprefix $(BINDIR)/,$(subst .cxx,,$(test))))
TEST_OBJ=$(foreach test,$(TEST_CXX),$(addprefix $(BUILDDIR)/,$(subst .cxx,.o,$(test))))

DEPFILES=$(foreach test,$(shell echo $(TEST_CXX) $(LIB_CXX)),$(addprefix $(BUILDDIR)/,$(subst .cxx,.d,$(test))))

.PHONY: all
all:
	@echo LD: $(LD)
	@echo LIB_CXX: $(LIB_CXX)
	@echo LIB_O: $(LIB_O)
	@echo DEPFILES: $(DEPFILES)
	@echo TEST_OBJ: $(TEST_OBJ)
	@echo TEST_BIN: $(TEST_BIN)
	@echo TEST_CXX: $(TEST_CXX)

.PHONY: tests
tests: $(TEST_BIN)

.PHONY: lib
lib: $(STATLIB)

$(STATLIB): $(LIB_O)
	@echo "Creating static library $@ ..."
	@ar rcs $@ $^

$(BINDIR)/%: obj/%.o $(STATLIB)
	@echo "Linking $< ..."
	@test -d $(shell dirname $@) || mkdir -p $(shell dirname $@)
	@$(LD) $< $(STATLIB) $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: %.cxx
	@echo "Compiling $< ..."
	@test -d $(shell dirname $@) || mkdir -p $(shell dirname $@)
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/%.d: %.cxx
	@echo "Making dependencies for $< ..."
	@test -d $(shell dirname $@) || mkdir -p $(shell dirname $@)
	@$(CXX) $(CXXFLAGS) -MM -MT $(BUILDDIR)/$(subst .cxx,.o,$<) $< > $@

.PHONY: clean
clean:
	@echo "Cleaning ..."
	@rm -rf ./$(BUILDDIR)/*
	@rm -rf ./$(BINDIR)/*

-include $(DEPFILES)
