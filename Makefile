cc=g++
prom = code
deps = main.cpp
$(prom):$(deps)
	g++ -o $(prom) $(deps) -O2 -std=c++14

clean:
	rm -rf $(prom)
