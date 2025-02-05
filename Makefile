CXXFLAGS=-rdynamic $(shell llvm-config --cxxflags) -g -O0 -fPIC
LDFLAGS=$(shell llvm-config --ldflags)
LDLIBS=$(shell llvm-config --libs)

all: p34.so

%.so: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -dynamiclib -o $@ $^ $(LDLIBS)


clean:
	rm -f *.o *.so

# 결과 저장 디렉토리
RESULTS_DIR = results

# 결과 폴더 생성
$(RESULTS_DIR):
	mkdir -p $(RESULTS_DIR)

# 테스트 실행 (루트 파일 변경 없이 results 폴더 안에서 실행)
test: $(RESULTS_DIR) p34.so
	@echo "🔍 Running tests..."
	@for file in test*.c; do \
		name=$$(basename $$file .c); \
		clang -c -emit-llvm -fno-discard-value-names $$file -o $(RESULTS_DIR)/$$name.bc; \
		llvm-dis $(RESULTS_DIR)/$$name.bc -o $(RESULTS_DIR)/$$name.ll; \
		opt -load-pass-plugin ./p34.so --passes=def-pass $(RESULTS_DIR)/$$name.ll -o $(RESULTS_DIR)/$$name.bc 2> $(RESULTS_DIR)/$$name.def; \
		opt -load-pass-plugin ./p34.so --passes=fix-pass $(RESULTS_DIR)/$$name.ll -o $(RESULTS_DIR)/$$name_fix.bc; \
		lli $(RESULTS_DIR)/$$name_fix.bc > $(RESULTS_DIR)/$$name.out; \
	done
	@echo "All tests completed!"

# 정답 파일과 비교 (루트 디렉토리 내 정답 파일과 비교)
compare: test
	@echo "🔍 Comparing results with expected outputs..."
	@for file in $(RESULTS_DIR)/*.def; do \
		name=$$(basename $$file .def); \
		if diff -w $$file $$name.def > $(RESULTS_DIR)/$$name.diff; then \
			echo "$$name.def matches"; \
			rm -f $(RESULTS_DIR)/$$name.diff; \
		else \
			echo "❌ $$name.def differs! See $(RESULTS_DIR)/$$name.diff"; \
		fi; \
	done
	@for file in $(RESULTS_DIR)/*.out; do \
		name=$$(basename $$file .out); \
		if diff -w $$file $$name.out > $(RESULTS_DIR)/$$name.diff; then \
			echo "$$name.out matches"; \
			rm -f $(RESULTS_DIR)/$$name.diff; \
		else \
			echo "$$name.out differs! See $(RESULTS_DIR)/$$name.diff"; \
		fi; \
	done
	@echo "Comparison complete!"

# 정리 (루트 디렉토리를 변경하지 않고 results 내부 파일만 삭제)
cleanall:
	rm -f *.o *.so
	rm -rf $(RESULTS_DIR)
