APP = xroe-app

# Add any other object files to this list below
APP_OBJS = xroe-app.o ip.o ecpri.o stats.o client.o comms.o parser.o enable.o disable.o restart.o radio_ctrl.o framing.o ecpri_proto.o xroe_api.o
CFLAGS += -g -I. -Werror -Wall

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)

clean:
	-rm -f $(APP_OBJS)
	-rm -f xroe-app
