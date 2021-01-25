# 	Build the image

This image is an C code testbench for checking memory leaks, heap usage and runtimes.
-	```docker build -t ci:0.1 . ```


#	Run docker

Project folder:
```
../AED/numeric_trails_v1/
	|
	| - src
	| - test
```

In order to provide the source code and execute the code via the terminal, use:
-	``` docker run -ti --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v $PWD:/AED/numeric_trails_v1/src/  ci:0.1 bash -c "cd /AED/numeric_trails_v1/src; make docker; make heap_docker"```


