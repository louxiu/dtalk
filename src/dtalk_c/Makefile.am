bin_PROGRAMS = dtalk_c

dtalk_c_SOURCES = dtalk_c.c \
                  deal_reply.c \
                  deal_request.c \
                  dtalk_c_room.c \
                  color_print.c	\
                  ../lib/message.c \
                  ../lib/packet.c \
                  ../lib/utils.c \
                  ../lib/debug.c \
                  ../lib/load_conf.c

dtalk_c_CPPFLAGS = -I../lib \
									 -DCONF_DIR=\"$(datadir)/@PACKAGE@\"

dtalk_cdir = $(datadir)/@PACKAGE@

dtalk_c_DATA = dtalk_c.conf
