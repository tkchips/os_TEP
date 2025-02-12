BINARIES = barrier fork-join mutex-nostarve reader-writer-nostarve reader-writer rendezvous
HEADERS = common_threads.h

all: $(BINARIES)

clean:
	rm -f $(BINARIES)

barrier: barrier.c $(HEADERS)
	gcc -o barrier barrier.c -Wall -pthread

fork-join: fork-join.c $(HEADERS)
	gcc -o fork-join fork-join.c -Wall -pthread

rendezvous: rendezvous.c $(HEADERS)
	gcc -o rendezvous rendezvous.c -Wall -pthread

mutex-nostarve: mutex-nostarve.c $(HEADERS)
	gcc -o mutex-nostarve  mutex-nostarve.c -Wall -pthread

reader-writer-nostarve: reader-writer-nostarve.c $(HEADERS)
	gcc -o reader-writer-nostarve reader-writer-nostarve.c -Wall -pthread

reader-writer: reader-writer.c $(HEADERS)
	gcc -o reader-writer reader-writer.c -Wall -pthread




