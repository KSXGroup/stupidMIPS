cc=g++
prom = code
deps = main.cpp
$(prom):$(deps)
	g++ -o $(prom) $(deps) -O3 -std=c++14

clean:
	rm -rf $(prom)
