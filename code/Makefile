SOURCEDIR     = src
INCLUDEDIR    = include
TMPDIR        = tmp

CXX      = g++
CXXFLAGS = `xml2-config --cflags` -I$(INCLUDEDIR) -ansi -Wall -g
LIBS     = `xml2-config --libs` -lsfml-graphics -lsfml-window -lsfml-system

VPATH    = $(INCLUDEDIR):$(SOURCEDIR):$(TMPDIR)

target   = test
sources  = $(wildcard src/*.cpp)
objects  = $(notdir $(patsubst %.cpp, %.o, $(sources)))
objects += $(addsuffix .o, $(target))

$(target): $(objects) 
	@echo "Linking..."
	@$(CXX) $(CXXFLAGS) -o $(target) $(patsubst %.o,$(TMPDIR)/%.o,$(objects)) $(LIBS)

-include $(patsubst %.d,$(TMPDIR)/%.d,$(objects:.o=.d))
	
%.o: %.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -o $(TMPDIR)/$@ -c  $<

$(TMPDIR)/%.d: %.cpp
	@echo "Making dependencies for $<..."
	@$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$;\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

.PHONY: all clean documentation

all: $(target) documentation

clean:
	@echo "Cleaning up..."
	@/bin/rm -f $(TMPDIR)/*.o $(TMPDIR)/*.d $(target)

documentation:
	@echo "Generating documentation..."
	@doxygen .Doxyfile &> Doxygen.log

clean_documentation:
	@echo "Cleaning up documentation..."
	@/bin/rm -r -f Doxygen.log documentation/*

clean_all: clean clean_documentation	

