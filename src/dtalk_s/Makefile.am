bin_PROGRAMS = dtalk_s

dtalk_s_SOURCES = dtalk_s.c	\
                  ../lib/debug.c  \
                  ../dtalk_s_lib/daemon.c \
                  ../lib/library.c \
                  ../lib/utils.c \
                  ../lib/packet.c \
                  ../lib/message.c \
                  ../dtalk_s_lib/online_users.c \
                  ../dtalk_s_lib/online_rooms.c \
                  ../lib/database/database.c \
                  ../lib/load_conf.c 

dtalk_s_CPPFLAGS = -I../lib \
                   -I../lib/database \
                   -I../dtalk_s_lib \
									 -DCONF_DIR=\"$(datadir)/@PACKAGE@\" \
									 -DDB_DIR=\"$(datadir)/@PACKAGE@\"

dtalk_sdir = $(datadir)/@PACKAGE@

dtalk_s_DATA = dtalk_s.conf
