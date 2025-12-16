CC = gcc
CFLAGS = -Wall -std=c99 -g
SRCDIR = src
INCDIR = include
OBJ = $(SRCDIR)/main.o $(SRCDIR)/cli.o $(SRCDIR)/vocab.o $(SRCDIR)/quiz.o $(SRCDIR)/score.o $(SRCDIR)/store_fs.o $(SRCDIR)/leaderboard.o

all: techvocab

techvocab: $(OBJ)
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $(OBJ)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o techvocab
