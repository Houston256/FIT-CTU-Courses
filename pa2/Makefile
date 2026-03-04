# Author: Honza Matoušek
CXX	   = g++
CXXFLAGS  = -Wall -pedantic -O0 -Werror -std=c++17 -g3 -fno-omit-frame-pointer -fsanitize=address
LDFLAGS   = -g3 -fno-omit-frame-pointer -fsanitize=address
LD		= g++
LIBS	  = -lncurses -lform -lmenu #-lncursesw
OBJDIR	= obj
SRCDIR	= src
DEPDIR	= dep

test: CXXFLAGS += -DTEST

TEST_DIR := test
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_EXECUTABLES := $(patsubst $(TEST_DIR)/%.cpp, $(TEST_DIR)/%, $(TEST_SOURCES))


rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
HEADERS = $(call rwildcard,$(SRCDIR),*.h)
DEP = $(patsubst $(SRCDIR)/%.cpp,$(DEPDIR)/%.d,$(SRC))

all: compile doc

compile: $(OBJ)
	$(LD) -o krausri1 $^ $(LIBS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

doc: $(HEADERS) $(SRC) Doxyfile README.en.md
	doxygen


.PHONY: test # ensure nothing is cached

test: $(TEST_EXECUTABLES) # don't run on multiple threads (avoid using "make -jXX")
	@echo ""
	@echo "Finished running all tests"
	@rm -f $(TEST_EXECUTABLES)



$(TEST_EXECUTABLES): $(TEST_DIR)/%: $(TEST_DIR)/%.cpp $(OBJ)
	@$(CXX) -c -o $(OBJDIR)/%.o $< $(CXXFLAGS)
	@$(LD) -o $@ $(OBJ) $(OBJDIR)/%.o $(LIBS) $(LDFLAGS)
	@./$@; \
	EXIT_CODE=$$?; \
	if [ $$EXIT_CODE -eq 0 ]; then \
		echo "$@: PASSED"; \
	else \
		echo "$@: FAILED"; \
	fi

clean:
	rm -rf doc
	rm -rf $(DEPDIR)
	rm -rf $(OBJDIR)
	rm -f krausri1

run: compile
	./krausri1

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CXX) -MM $< -MT $(OBJDIR)/$*.o > $@

-include $(DEP)