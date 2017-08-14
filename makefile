DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CPPFLAGS += -Wall -Iboost_1_63_0

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

%.o : %.cpp
%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

TARGET := eec
.PHONY: default all clean
default: $(TARGET)
all: default
clean:
	-rm -f *.o
	-rm -f *.d
	-rm -f $(TARGET)

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))
$(TARGET): $(OBJECTS)
	$(CXX) $^ -Wall -o $@

SRCS := $(ls -1 *.cpp)
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
