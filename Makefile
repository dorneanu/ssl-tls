# Configuration stuff
CC = gcc
RM = rm -rf
MV = mv

# Add version to binary

# Flags
CFLAGS = -W -Wall -O2
#LDFLAGS = ""

# Default (dir) locations
SRCDIR = src

# Required objects
OBJS =  $(SRCDIR)/main.o \
		$(SRCDIR)/args.o \
		$(SRCDIR)/connection.o \
		$(SRCDIR)/server.o \
		$(SRCDIR)/utils/base64.o

$(SRCDIR)%.o: %.c
	  $(CC) $(CFLAGS) -c -o $@ $< 
	
	  
ssl-tls: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


clean:
	@echo "Removing objects ..."
	find $(SRCDIR) -name "*.o" -exec $(RM) {} \;
	@echo "Removing binary ..."
	$(RM) ssl-tls
