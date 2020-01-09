CXX = clang++
CXXFLAGS = -std=c++11

all: transpose

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf *.o*
