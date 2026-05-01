PROTO_DIR := .
GEN_DIR   := ../generate
PROTOC    := protoc

PROTO_FILES := $(shell find $(PROTO_DIR) -type f -name '*.proto')

CPP_FILES  := $(foreach f,$(PROTO_FILES),$(GEN_DIR)/$(f:$(PROTO_DIR)/%.proto=%.pb.cc))
HEADER_FILES := $(foreach f,$(PROTO_FILES),$(GEN_DIR)/$(f:$(PROTO_DIR)/%.proto=%.pb.h))
DEP_FILES  := $(foreach f,$(PROTO_FILES),$(GEN_DIR)/$(f:$(PROTO_DIR)/%.proto=%.d))

all: $(CPP_FILES) $(HEADER_FILES)

clean:
	rm -f $(CPP_FILES) $(HEADER_FILES) $(DEP_FILES)

$(GEN_DIR):
	mkdir -p $(GEN_DIR)

$(GEN_DIR)/%.pb.cc: $(PROTO_DIR)/%.proto | $(GEN_DIR)
	$(PROTOC) --proto_path=$(PROTO_DIR) \
	          --cpp_out=$(GEN_DIR) \
	          --dependency_out=$(GEN_DIR)/$*.d $<
	@touch $(subst .pb.cc,.pb.h,$@)

$(GEN_DIR)/%.pb.h: $(GEN_DIR)/%.pb.cc
	@true

-include $(DEP_FILES)

.PHONY: all clean