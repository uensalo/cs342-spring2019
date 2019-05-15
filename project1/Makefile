all: bilshell producer consumer bin
bilshell: bilshell.c
	gcc -Wall -g -o bilshell bilshell.c
producer: producer.c
	gcc -Wall -g -o producer producer.c
consumer: consumer.c
	gcc -Wall -g -o consumer consumer.c
bin: 
	@sudo cp ./bilshell /usr/bin
	@sudo cp ./producer /usr/bin
	@sudo cp ./consumer /usr/bin
