CC = gcc
PROB_NAME = pp4.26
SOURCE_FILES = $(PROB_NAME).c
OUT_EXE = $(PROB_NAME).bin

all:
	$(CC) -o $(OUT_EXE) -lpthread $(SOURCE_FILES)
  # make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

build:
	$(CC) -o $(OUT_EXE) -lpthread $(SOURCE_FILES)

buildnosleep:
	$(CC) -o $(OUT_EXE) -lpthread $(SOURCE_FILES) -DSLEEP_TIME_DISABLED

buildusecs:
	$(CC) -o $(OUT_EXE) -lpthread $(SOURCE_FILES) -DMICRO_SECONDS_TIME_UNITS

clean:
	rm $(OUT_EXE)
  # make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
run:
	./$(OUT_EXE)

runpiped:
	./$(OUT_EXE) 2>&1 log_stdout.txt