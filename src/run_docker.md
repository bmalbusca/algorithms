# To build a image
docker build -t ci:0.1 .

../AED/numeric_trails_v1/
	|
	| - src
	| - test
#Run docker
1.
docker run -ti --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v $PWD:/AED/numeric_trails_v1/src/  ci:0.1 bash -c "cd /AED/numeric_trails_v1/src; make docker; make heap_docker"


