/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * =======================================================================
 *
 * Upper layer of the keyboard implementation. This file processes all
 * keyboard events which are generated by the low level keyboard layer.
 * Remeber, that the mouse is handled by the refresher and not by the
 * client!
 *
 * =======================================================================
 */

#include "header/client.h"

static cvar_t *cfg_unbindall;

/*
 * key up events are sent even if in console mode
 */

char key_lines[NUM_KEY_LINES][MAXCMDLINE];
int key_linepos;
int anykeydown;

int edit_line = 0;
int history_line = 0;

int key_waiting;
char *keybindings[K_LAST];
qboolean consolekeys[K_LAST]; /* if true, can't be rebound while in console */
qboolean menubound[K_LAST]; /* if true, can't be rebound while in menu */
int key_repeats[K_LAST]; /* if > 1, it is autorepeating */
qboolean keydown[K_LAST];

qboolean Cmd_IsComplete(char *cmd);

typedef struct
{
	char *name;
	int keynum;
} keyname_t;

/* Translates internal key representations
 * into human readable strings. */
keyname_t keynames[] = {
	{"TAB", K_TAB},
	{"ENTER", K_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},
	{"BACKSPACE", K_BACKSPACE},

	{"COMMAND", K_COMMAND},
	{"CAPSLOCK", K_CAPSLOCK},
	{"POWER", K_POWER},
	{"PAUSE", K_PAUSE},

	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},

	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},

	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},

	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGDN", K_PGDN},
	{"PGUP", K_PGUP},
	{"HOME", K_HOME},
	{"END", K_END},

	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},
	{"MOUSE4", K_MOUSE4},
	{"MOUSE5", K_MOUSE5},

	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},
	{"JOY5", K_JOY5},
	{"JOY6", K_JOY6},
	{"JOY7", K_JOY7},
	{"JOY8", K_JOY8},
	{"JOY9", K_JOY9},
	{"JOY10", K_JOY10},
	{"JOY11", K_JOY11},
	{"JOY12", K_JOY12},
	{"JOY13", K_JOY13},
	{"JOY14", K_JOY14},
	{"JOY15", K_JOY15},
	{"JOY16", K_JOY16},
	{"JOY17", K_JOY17},
	{"JOY18", K_JOY18},
	{"JOY19", K_JOY19},
	{"JOY20", K_JOY20},
	{"JOY21", K_JOY21},
	{"JOY22", K_JOY22},
	{"JOY23", K_JOY23},
	{"JOY24", K_JOY24},
	{"JOY25", K_JOY25},
	{"JOY26", K_JOY26},
	{"JOY27", K_JOY27},
	{"JOY28", K_JOY28},
	{"JOY29", K_JOY29},
	{"JOY30", K_JOY30},
	{"JOY31", K_JOY31},
	{"JOY32", K_JOY32},

	{"HAT_UP", K_HAT_UP},
	{"HAT_RIGHT", K_HAT_RIGHT},
	{"HAT_DOWN", K_HAT_DOWN},
	{"HAT_LEFT", K_HAT_LEFT},

	{"TRIG_LEFT", K_TRIG_LEFT},
	{"TRIG_RIGHT", K_TRIG_RIGHT},

	// virtual keys you get by pressing the corresponding normal joy key
	// and the altselector key
	{"JOY1_ALT", K_JOY1_ALT},
	{"JOY2_ALT", K_JOY2_ALT},
	{"JOY3_ALT", K_JOY3_ALT},
	{"JOY4_ALT", K_JOY4_ALT},
	{"JOY5_ALT", K_JOY5_ALT},
	{"JOY6_ALT", K_JOY6_ALT},
	{"JOY7_ALT", K_JOY7_ALT},
	{"JOY8_ALT", K_JOY8_ALT},
	{"JOY9_ALT", K_JOY9_ALT},
	{"JOY10_ALT", K_JOY10_ALT},
	{"JOY11_ALT", K_JOY11_ALT},
	{"JOY12_ALT", K_JOY12_ALT},
	{"JOY13_ALT", K_JOY13_ALT},
	{"JOY14_ALT", K_JOY14_ALT},
	{"JOY15_ALT", K_JOY15_ALT},
	{"JOY16_ALT", K_JOY16_ALT},
	{"JOY17_ALT", K_JOY17_ALT},
	{"JOY18_ALT", K_JOY18_ALT},
	{"JOY19_ALT", K_JOY19_ALT},
	{"JOY20_ALT", K_JOY20_ALT},
	{"JOY21_ALT", K_JOY21_ALT},
	{"JOY22_ALT", K_JOY22_ALT},
	{"JOY23_ALT", K_JOY23_ALT},
	{"JOY24_ALT", K_JOY24_ALT},
	{"JOY25_ALT", K_JOY25_ALT},
	{"JOY26_ALT", K_JOY26_ALT},
	{"JOY27_ALT", K_JOY27_ALT},
	{"JOY28_ALT", K_JOY28_ALT},
	{"JOY29_ALT", K_JOY29_ALT},
	{"JOY30_ALT", K_JOY30_ALT},
	{"JOY31_ALT", K_JOY31_ALT},
	{"JOY32_ALT", K_JOY32_ALT},

	{"HAT_UP_ALT", K_HAT_UP_ALT},
	{"HAT_RIGHT_ALT", K_HAT_RIGHT_ALT},
	{"HAT_DOWN_ALT", K_HAT_DOWN_ALT},
	{"HAT_LEFT_ALT", K_HAT_LEFT_ALT},

	{"TRIG_LEFT", K_TRIG_LEFT_ALT},
	{"TRIG_RIGHT", K_TRIG_RIGHT_ALT},

	{"JOY_BACK", K_JOY_BACK},

	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"AUX16", K_AUX16},
	{"AUX17", K_AUX17},
	{"AUX18", K_AUX18},
	{"AUX19", K_AUX19},
	{"AUX20", K_AUX20},
	{"AUX21", K_AUX21},
	{"AUX22", K_AUX22},
	{"AUX23", K_AUX23},
	{"AUX24", K_AUX24},
	{"AUX25", K_AUX25},
	{"AUX26", K_AUX26},
	{"AUX27", K_AUX27},
	{"AUX28", K_AUX28},
	{"AUX29", K_AUX29},
	{"AUX30", K_AUX30},
	{"AUX31", K_AUX31},
	{"AUX32", K_AUX32},

	{"KP_HOME", K_KP_HOME},
	{"KP_UPARROW", K_KP_UPARROW},
	{"KP_PGUP", K_KP_PGUP},
	{"KP_LEFTARROW", K_KP_LEFTARROW},
	{"KP_5", K_KP_5},
	{"KP_RIGHTARROW", K_KP_RIGHTARROW},
	{"KP_END", K_KP_END},
	{"KP_DOWNARROW", K_KP_DOWNARROW},
	{"KP_PGDN", K_KP_PGDN},
	{"KP_ENTER", K_KP_ENTER},
	{"KP_INS", K_KP_INS},
	{"KP_DEL", K_KP_DEL},
	{"KP_SLASH", K_KP_SLASH},
	{"KP_MINUS", K_KP_MINUS},
	{"KP_PLUS", K_KP_PLUS},

	{"MWHEELUP", K_MWHEELUP},
	{"MWHEELDOWN", K_MWHEELDOWN},

	{"PAUSE", K_PAUSE},

	{"SEMICOLON", ';'}, /* because a raw semicolon seperates commands */

	{NULL, 0}
};

/* ------------------------------------------------------------------ */

void
CompleteCommand(void)
{
	char *cmd, *s;

	s = key_lines[edit_line] + 1;

	if ((*s == '\\') || (*s == '/'))
	{
		s++;
	}

	cmd = Cmd_CompleteCommand(s);

	if (cmd)
	{
		key_lines[edit_line][1] = '/';
		strcpy(key_lines[edit_line] + 2, cmd);
		key_linepos = strlen(cmd) + 2;

		if (Cmd_IsComplete(cmd))
		{
			key_lines[edit_line][key_linepos] = ' ';
			key_linepos++;
			key_lines[edit_line][key_linepos] = 0;
		}
		else
		{
			key_lines[edit_line][key_linepos] = 0;
		}

		return;
	}
}

/*
 * Interactive line editing and console scrollback
 */
void
Key_Console(int key)
{
	/*
	 * Ignore keypad in console to prevent duplicate
	 * entries through key presses processed as a
	 * normal char event and additionally as key
	 * event.
	 */
	switch (key)
	{
		case K_KP_SLASH:
		case K_KP_MINUS:
		case K_KP_PLUS:
		case K_KP_HOME:
		case K_KP_UPARROW:
		case K_KP_PGUP:
		case K_KP_LEFTARROW:
		case K_KP_5:
		case K_KP_RIGHTARROW:
		case K_KP_END:
		case K_KP_DOWNARROW:
		case K_KP_PGDN:
		case K_KP_INS:
		case K_KP_DEL:
			return;
			break;

		default:
			break;
	}

	if (key == 'l')
	{
		if (keydown[K_CTRL])
		{
			Cbuf_AddText("clear\n");
			return;
		}
	}

	if ((key == K_ENTER) || (key == K_KP_ENTER))
	{
		/* slash text are commands, else chat */
		if ((key_lines[edit_line][1] == '\\') ||
			(key_lines[edit_line][1] == '/'))
		{
			Cbuf_AddText(key_lines[edit_line] + 2); /* skip the > */
		}
		else
		{
			Cbuf_AddText(key_lines[edit_line] + 1); /* valid command */
		}

		Cbuf_AddText("\n");
		Com_Printf("%s\n", key_lines[edit_line]);
		edit_line = (edit_line + 1) & (NUM_KEY_LINES-1);
		history_line = edit_line;
		key_lines[edit_line][0] = ']';
		key_linepos = 1;

		if (cls.state == ca_disconnected)
		{
			SCR_UpdateForEye(0);  /* force an update, because the command
								   	may take some time */
		}

		return;
	}

	if (key == K_TAB)
	{
		/* command completion */
		CompleteCommand();
		return;
	}

	if ((key == K_BACKSPACE) || (key == K_LEFTARROW) ||
		(key == K_KP_LEFTARROW) ||
		((key == 'h') && (keydown[K_CTRL])))
	{
		if (key_linepos > 1)
		{
			key_linepos--;
		}

		return;
	}

	if (key == K_DEL)
	{
		memmove(key_lines[edit_line] + key_linepos,
				key_lines[edit_line] + key_linepos + 1,
				sizeof(key_lines[edit_line]) - key_linepos - 1);
		return;
	}

	if ((key == K_UPARROW) || (key == K_KP_UPARROW) ||
		((key == 'p') && keydown[K_CTRL]))
	{
		do
		{
			history_line = (history_line - 1) & (NUM_KEY_LINES-1);
		}
		while (history_line != edit_line &&
			   !key_lines[history_line][1]);

		if (history_line == edit_line)
		{
			history_line = (edit_line + 1) & (NUM_KEY_LINES-1);
		}

		strcpy(key_lines[edit_line], key_lines[history_line]);
		key_linepos = (int)strlen(key_lines[edit_line]);
		return;
	}

	if ((key == K_DOWNARROW) || (key == K_KP_DOWNARROW) ||
		((key == 'n') && keydown[K_CTRL]))
	{
		if (history_line == edit_line)
		{
			return;
		}

		do
		{
			history_line = (history_line + 1) & (NUM_KEY_LINES-1);
		}
		while (history_line != edit_line &&
			   !key_lines[history_line][1]);

		if (history_line == edit_line)
		{
			key_lines[edit_line][0] = ']';
			key_linepos = 1;
		}
		else
		{
			strcpy(key_lines[edit_line], key_lines[history_line]);
			key_linepos = (int)strlen(key_lines[edit_line]);
		}

		return;
	}

	if ((key == K_PGUP) || (key == K_KP_PGUP) || (key == K_MWHEELUP) ||
		(key == K_MOUSE4))
	{
		con.display -= 2;
		return;
	}

	if ((key == K_PGDN) || (key == K_KP_PGDN) || (key == K_MWHEELDOWN) ||
		(key == K_MOUSE5))
	{
		con.display += 2;

		if (con.display > con.current)
		{
			con.display = con.current;
		}

		return;
	}

	if ((key == K_HOME) || (key == K_KP_HOME))
	{
		if (keydown[K_CTRL])
		{
			con.display = con.current - con.totallines + 10;
		}

		else
		{
			key_linepos = 1;
		}

		return;
	}

	if ((key == K_END) || (key == K_KP_END))
	{
		if (keydown[K_CTRL])
		{
			con.display = con.current;
		}

		else
		{
			key_linepos = (int)strlen(key_lines[edit_line]);
		}

		return;
	}

	if ((key < 32) || (key > 127))
	{
		return; /* non printable character */
	}

	if (key_linepos < MAXCMDLINE - 1)
	{
		int last;
		int length;

		length = strlen(key_lines[edit_line]);

		if (length >= MAXCMDLINE - 1)
		{
			return;
		}

		last = key_lines[edit_line][key_linepos];

		memmove(key_lines[edit_line] + key_linepos + 1,
				key_lines[edit_line] + key_linepos,
				length - key_linepos);

		key_lines[edit_line][key_linepos] = key;
		key_linepos++;

		if (!last)
		{
			key_lines[edit_line][key_linepos] = 0;
		}
	}
}

qboolean chat_team;
char chat_buffer[MAXCMDLINE];
int chat_bufferlen = 0;
int chat_cursorpos = 0;

void
Key_Message(int key)
{
	char last;

	if ((key == K_ENTER) || (key == K_KP_ENTER))
	{
		if (chat_team)
		{
			Cbuf_AddText("say_team \"");
		}

		else
		{
			Cbuf_AddText("say \"");
		}

		Cbuf_AddText(chat_buffer);
		Cbuf_AddText("\"\n");

		cls.key_dest = key_game;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		chat_cursorpos = 0;
		return;
	}

	if (key == K_ESCAPE)
	{
		cls.key_dest = key_game;
		chat_cursorpos = 0;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		return;
	}

	if (key == K_BACKSPACE)
	{
		if (chat_cursorpos)
		{
			memmove(chat_buffer + chat_cursorpos - 1,
					chat_buffer + chat_cursorpos,
					chat_bufferlen - chat_cursorpos + 1);
			chat_cursorpos--;
			chat_bufferlen--;
		}

		return;
	}

	if (key == K_DEL)
	{
		if (chat_bufferlen && (chat_cursorpos != chat_bufferlen))
		{
			memmove(chat_buffer + chat_cursorpos,
					chat_buffer + chat_cursorpos + 1,
					chat_bufferlen - chat_cursorpos + 1);
			chat_bufferlen--;
		}

		return;
	}

	if (key == K_LEFTARROW)
	{
		if (chat_cursorpos > 0)
		{
			chat_cursorpos--;
		}

		return;
	}

	if (key == K_HOME)
	{
		chat_cursorpos = 0;
		return;
	}

	if (key == K_END)
	{
		chat_cursorpos = chat_bufferlen;
		return;
	}

	if (key == K_RIGHTARROW)
	{
		if (chat_buffer[chat_cursorpos])
		{
			chat_cursorpos++;
		}

		return;
	}

	if ((key < 32) || (key > 127))
	{
		return; /* non printable charcter */
	}

	if (chat_bufferlen == sizeof(chat_buffer) - 1)
	{
		return; /* all full, this should never happen on modern systems */
	}

	memmove(chat_buffer + chat_cursorpos + 1,
			chat_buffer + chat_cursorpos,
			chat_bufferlen - chat_cursorpos + 1);

	last = chat_buffer[chat_cursorpos];

	chat_buffer[chat_cursorpos] = key;

	chat_bufferlen++;
	chat_cursorpos++;

	if (!last)
	{
		chat_buffer[chat_cursorpos] = 0;
	}
}

/*
 * Returns a key number to be used to index
 * keybindings[] by looking at the given string.
 * Single ascii characters return themselves, while
 * the K_* names are matched up.
 */
int
Key_StringToKeynum(char *str)
{
	keyname_t *kn;

	if (!str || !str[0])
	{
		return -1;
	}

	if (!str[1])
	{
		return str[0];
	}

	for (kn = keynames; kn->name; kn++)
	{
		if (!Q_stricmp(str, kn->name))
		{
			return kn->keynum;
		}
	}

	return -1;
}

/*
 * Returns a string (either a single ascii char,
 * or a K_* name) for the given keynum.
 */
char *
Key_KeynumToString(int keynum)
{
	keyname_t *kn;
	static char tinystr[2] = {0};

	if (keynum == -1)
	{
		return "<KEY NOT FOUND>";
	}

	if ((keynum > 32) && (keynum < 127))
	{
		/* printable ascii */
		tinystr[0] = keynum;
		return tinystr;
	}

	for (kn = keynames; kn->name; kn++)
	{
		if (keynum == kn->keynum)
		{
			return kn->name;
		}
	}

	return "<UNKNOWN KEYNUM>";
}

void
Key_SetBinding(int keynum, char *binding)
{
	char *new;
	int l;

	if (keynum == -1)
	{
		return;
	}

	/* free old bindings */
	if (keybindings[keynum])
	{
		Z_Free(keybindings[keynum]);
		keybindings[keynum] = NULL;
	}

	/* allocate memory for new binding */
	l = strlen(binding);
	new = Z_Malloc(l + 1);
	strcpy(new, binding);
	new[l] = 0;
	keybindings[keynum] = new;
}

void
Key_Unbind_f(void)
{
	int b;

	if (Cmd_Argc() != 2)
	{
		Com_Printf("unbind <key> : remove commands from a key\n");
		return;
	}

	b = Key_StringToKeynum(Cmd_Argv(1));

	if (b == -1)
	{
		Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding(b, "");
}

void
Key_Unbindall_f(void)
{
	int i;

	for (i = 0; i < K_LAST; i++)
	{
		if (keybindings[i])
		{
			Key_SetBinding(i, "");
		}
	}
}

/* ugly hack, set in Cmd_ExecuteString() when yq2.cfg is executed
 * (=> default.cfg is done) */
extern qboolean doneWithDefaultCfg;

void
Key_Bind_f(void)
{
	int i, c, b;
	char cmd[1024];

	c = Cmd_Argc();

	if (c < 2)
	{
		Com_Printf("bind <key> [command] : attach a command to a key\n");
		return;
	}

	b = Key_StringToKeynum(Cmd_Argv(1));

	if (b == -1)
	{
		Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	/* don't allow binding escape or the special console keys */
	if(b == K_ESCAPE || b == '^' || b == '`' || b == '~' || b == K_JOY_BACK)
	{
		if(doneWithDefaultCfg)
		{
			/* don't warn about this when it's from default.cfg, we can't change that anyway */
			Com_Printf("You can't bind the special key \"%s\"!\n", Cmd_Argv(1));
		}
		return;
	}

	if (c == 2)
	{
		if (keybindings[b])
		{
			Com_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), keybindings[b]);
		}

		else
		{
			Com_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
		}

		return;
	}

	/* copy the rest of the command line */
	cmd[0] = 0; /* start out with a null string */

	for (i = 2; i < c; i++)
	{
		strcat(cmd, Cmd_Argv(i));

		if (i != (c - 1))
		{
			strcat(cmd, " ");
		}
	}

	Key_SetBinding(b, cmd);
}

/*
 * Writes lines containing "bind key value"
 */
void
Key_WriteBindings(FILE *f)
{
	int i;

	if (cfg_unbindall->value)
	{
		fprintf(f, "unbindall\n");
	}

	for (i = 0; i < K_LAST; i++)
	{
		if (keybindings[i] && keybindings[i][0])
		{
			fprintf(f, "bind %s \"%s\"\n",
					Key_KeynumToString(i), keybindings[i]);
		}
	}
}

void
Key_WriteConsoleHistory()
{
	int i;
	char path[MAX_OSPATH];

	if (is_portable)
	{
		Com_sprintf(path, sizeof(path), "%sconsole_history.txt", Sys_GetBinaryDir());
	}
	else
	{
		Com_sprintf(path, sizeof(path), "%sconsole_history.txt", Sys_GetHomeDir());
	}

	FILE* f = Q_fopen(path, "w");

	if(f==NULL)
	{
		Com_Printf("Opening console history %s for writing failed!\n", path);
		return;
	}

	// save the oldest lines first by starting at edit_line
	// and going forward (and wrapping around)
	const char* lastWrittenLine = "";
	for(i=0; i<NUM_KEY_LINES; ++i)
	{
		int lineIdx = (edit_line+i) & (NUM_KEY_LINES-1);
		const char* line = key_lines[lineIdx];

		if(line[1] != '\0' && strcmp(lastWrittenLine, line ) != 0)
		{
			// if the line actually contains something besides the ] prompt,
			// and is not identical to the last written line, write it to the file
			fputs(line, f);
			fputc('\n', f);

			lastWrittenLine = line;
		}
	}

	fclose(f);
}

/* initializes key_lines from history file, if available */
void
Key_ReadConsoleHistory()
{
	int i;

	char path[MAX_OSPATH];

	if (is_portable)
	{
		Com_sprintf(path, sizeof(path), "%sconsole_history.txt", Sys_GetBinaryDir());
	}
	else
	{
		Com_sprintf(path, sizeof(path), "%sconsole_history.txt", Sys_GetHomeDir());
	}

	FILE* f = Q_fopen(path, "r");
	if(f==NULL)
	{
		Com_DPrintf("Opening console history %s for reading failed!\n", path);
		return;
	}

	for (i = 0; i < NUM_KEY_LINES; i++)
	{
		if(fgets(key_lines[i], MAXCMDLINE, f) == NULL)
		{
			// probably EOF.. adjust edit_line and history_line and we're done here
			edit_line = i;
			history_line = i;
			break;
		}
		// remove trailing newlines
		int lastCharIdx = strlen(key_lines[i])-1;
		while((key_lines[i][lastCharIdx] == '\n' || key_lines[i][lastCharIdx] == '\r') && lastCharIdx >= 0)
		{
			key_lines[i][lastCharIdx] = '\0';
			--lastCharIdx;
		}
	}

	fclose(f);
}

void
Key_Bindlist_f(void)
{
	int i;

	for (i = 0; i < K_LAST; i++)
	{
		if (keybindings[i] && keybindings[i][0])
		{
			Com_Printf("%s \"%s\"\n", Key_KeynumToString(i), keybindings[i]);
		}
	}
}

void
Key_Init(void)
{
	int i;
	for (i = 0; i < NUM_KEY_LINES; i++)
	{
		key_lines[i][0] = ']';
		key_lines[i][1] = 0;
	}
	// can't call Key_ReadConsoleHistory() here because FS_Gamedir() isn't set yet

	key_linepos = 1;

	/* init 128 bit ascii characters in console mode */
	for (i = 32; i < 128; i++)
	{
		consolekeys[i] = true;
	}

	consolekeys[K_ENTER] = true;
	consolekeys[K_KP_ENTER] = true;
	consolekeys[K_TAB] = true;
	consolekeys[K_LEFTARROW] = true;
	consolekeys[K_KP_LEFTARROW] = true;
	consolekeys[K_RIGHTARROW] = true;
	consolekeys[K_KP_RIGHTARROW] = true;
	consolekeys[K_UPARROW] = true;
	consolekeys[K_KP_UPARROW] = true;
	consolekeys[K_DOWNARROW] = true;
	consolekeys[K_KP_DOWNARROW] = true;
	consolekeys[K_BACKSPACE] = true;
	consolekeys[K_HOME] = true;
	consolekeys[K_KP_HOME] = true;
	consolekeys[K_END] = true;
	consolekeys[K_KP_END] = true;
	consolekeys[K_PGUP] = true;
	consolekeys[K_KP_PGUP] = true;
	consolekeys[K_PGDN] = true;
	consolekeys[K_KP_PGDN] = true;
	consolekeys[K_SHIFT] = true;
	consolekeys[K_INS] = true;
	consolekeys[K_KP_INS] = true;
	consolekeys[K_KP_DEL] = true;
	consolekeys[K_KP_SLASH] = true;
	consolekeys[K_KP_PLUS] = true;
	consolekeys[K_KP_MINUS] = true;
	consolekeys[K_KP_5] = true;

	consolekeys['`'] = false;
	consolekeys['~'] = false;
	consolekeys['^'] = false;

	menubound[K_ESCAPE] = true;

	for (i = 0; i < 12; i++)
	{
		menubound[K_F1 + i] = true;
	}

	/* register our variables */
	cfg_unbindall = Cvar_Get("cfg_unbindall", "1", CVAR_ARCHIVE);

	/* register our functions */
	Cmd_AddCommand("bind", Key_Bind_f);
	Cmd_AddCommand("unbind", Key_Unbind_f);
	Cmd_AddCommand("unbindall", Key_Unbindall_f);
	Cmd_AddCommand("bindlist", Key_Bindlist_f);

#ifdef __ANDROID__
    anykeydown = 0;
#endif
}

/*
 * Called every frame for every detected keypress.
 * ASCII input for the console, the menu and the
 * chat window are handled by this function.
 * Anything else is handled by Key_Event().
 */
void
Char_Event(int key)
{
	/* console key is hardcoded, so the user can never unbind it */
	if ((key == '^') || (key == '~') || (key == '`'))
	{
		Con_ToggleConsole_f();
		return;
	}

	switch (cls.key_dest)
	{
		/* Chat */
		case key_message:
			Key_Message(key);
			break;

		/* Menu */
		case key_menu:
			M_Keydown(key);
			break;

		/* Console */
		case key_console:
			Key_Console(key);
			break;

		/* Console is really open but key_dest is game anyway (not connected) */
		case key_game:
			if(cls.state == ca_disconnected || cls.state == ca_connecting)
				Key_Console(key);

			break;

		default:
			break;
	}
}

/*
 * Called every frame for every detected keypress.
 * This is only for movement and special characters,
 * anything else is handled by Char_Event().
 */
void
Key_Event(int key, qboolean down, qboolean special)
{
	char cmd[1024];
	char *kb;
	cvar_t *fullscreen;
	unsigned int time = Sys_Milliseconds();

	// evil hack for the joystick key altselector, which turns K_JOYx into K_JOYx_ALT
	if(joy_altselector_pressed && key >= K_JOY1 && key <= K_JOY_LAST_REGULAR)
	{
		// make sure key is not the altselector itself (which we won't turn into *_ALT)
		if(keybindings[key] == NULL || strcmp(keybindings[key], "+joyaltselector") != 0)
		{
			int altkey = key + (K_JOY1_ALT - K_JOY1);
			// allow fallback to binding with non-alt key
			if(keybindings[altkey] != NULL || keybindings[key] == NULL)
				key = altkey;
		}
	}

	/* Track if key is down */
	keydown[key] = down;

	/* Ignore most autorepeats */
	if (down)
	{
		key_repeats[key]++;

		if ((key != K_BACKSPACE) &&
			(key != K_PAUSE) &&
			(key != K_PGUP) &&
			(key != K_KP_PGUP) &&
			(key != K_PGDN) &&
			(key != K_KP_PGDN) &&
			(key_repeats[key] > 1))
		{
			return;
		}
	}
	else
	{
		key_repeats[key] = 0;
	}

	/* Fullscreen switch through Alt + Return */
	if (down && keydown[K_ALT] && key == K_ENTER)
	{
		fullscreen = Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);

		if (!fullscreen->value)
		{
			Cvar_Set("vid_fullscreen", "1");
			fullscreen->modified = true;
		}
		else
		{
			Cvar_Set("vid_fullscreen", "0");
			fullscreen->modified = true;
		}

		return;
	}

	/* Toogle console though Shift + Escape */
	if (down && keydown[K_SHIFT] && key == K_ESCAPE)
	{
		Con_ToggleConsole_f();
		return;
	}

	/* Key is unbound */
	if ((key >= K_MOUSE1 && key != K_JOY_BACK) && !keybindings[key] && (cls.key_dest != key_console))
	{
		Com_Printf("%s is unbound, hit F4 to set.\n", Key_KeynumToString(key));
	}

	/* While in attract loop all keys besides F1 to F12 (to
	   allow quick load and the like) are treated like escape. */
	if (cl.attractloop && (cls.key_dest != key_menu) &&
		!((key >= K_F1) && (key <= K_F12)))
	{
		key = K_ESCAPE;
	}

	/* Escape has a special meaning. Depending on the situation it
	   - pauses the game and breaks into the menu
	   - stops the attract loop and breaks into the menu
	   - closes the console and breaks into the menu
	   - moves one menu level up
	   - closes the menu
	   - closes the help computer
	   - closes the chat window
	   Fully same logic for K_JOY_BACK */
	if (!cls.disable_screen)
	{
		if (key == K_ESCAPE || key == K_JOY_BACK)
		{
			if (!down)
			{
				return;
			}

			/* Close the help computer */
			if (cl.frame.playerstate.stats[STAT_LAYOUTS] &&
				(cls.key_dest == key_game))
			{
				Cbuf_AddText("cmd putaway\n");
				return;
			}

			switch (cls.key_dest)
			{
				/* Close chat window */
				case key_message:
					Key_Message(key);
					break;

				/* Close menu or one layer up */
				case key_menu:
					M_Keydown(key);
					break;

				/* Pause game and / or leave console,
				   break into the menu. */
				case key_game:
				case key_console:
					M_Menu_Main_f();
					break;
			}

			return;
		}
	}

	/* This is one of the most ugly constructs I've
	   found so far in Quake II. When the game is in
	   the intermission, the player can press any key
	   to end it and advance into the next level. It
	   should be easy to figure out at server level if
	   a button is pressed. But somehow the developers
	   decided, that they'll need special move state
	   BUTTON_ANY to solve this problem. So there's
	   this global variable anykeydown. If it's not
	   0, CL_FinishMove() encodes BUTTON_ANY into the
	   button state. The server reads this value and
	   sends it to gi->ClientThink() where it's used
	   to determine if the intermission shall end.
	   Needless to say that this is the only consumer
	   of BUTTON_ANY.

	   Since we cannot alter the network protocol nor
	   the server <-> game API, I'll leave things alone
	   and try to forget. */
	if (down)
	{
		if (key_repeats[key] == 1)
		{
			anykeydown++;
		}
	}
	else
	{
		anykeydown--;

		if (anykeydown < 0)
		{
			anykeydown = 0;
		}
	}

	/* key up events only generate commands if the game key binding
	   is a button command (leading+ sign). These will occur even in
	   console mode, to keep the character from continuing an action
	   started before a console switch. Button commands include the
	   kenum as a parameter, so multiple downs can be matched with ups */
	if (!down)
	{
		kb = keybindings[key];

		if (kb && (kb[0] == '+'))
		{
			Com_sprintf(cmd, sizeof(cmd), "-%s %i %i\n", kb + 1, key, time);
			Cbuf_AddText(cmd);
		}

		return;
	}
	else if (((cls.key_dest == key_menu) && menubound[key]) ||
			((cls.key_dest == key_console) && !consolekeys[key]) ||
			((cls.key_dest == key_game) && ((cls.state == ca_active) ||
			  !consolekeys[key])))
	{
		kb = keybindings[key];

		if (kb)
		{
			if (kb[0] == '+')
			{
				/* button commands add keynum and time as a parm */
				Com_sprintf(cmd, sizeof(cmd), "%s %i %i\n", kb, key, time);
				Cbuf_AddText(cmd);
			}
			else
			{
				Cbuf_AddText(kb);
				Cbuf_AddText("\n");
			}
		}

		return;
	}

	/* All input subsystems handled after this
	   point only care for key down events. */
	if (!down)
	{
		return;
	}

	/* Everything that's not a special char
	   is processed by Char_Event(). */
	if (!special)
	{
		return;
	}

	/* Send key to the active input subsystem */
	switch (cls.key_dest)
	{
		/* Chat */
		case key_message:
			Key_Message(key);
			break;

		/* Menu */
		case key_menu:
			M_Keydown(key);
			break;

		/* Console */
		case key_game:
		case key_console:
			Key_Console(key);
			break;
	}
}

/*
 * Marks all keys as "up"
 */
void
Key_MarkAllUp(void)
{
	int key;

	for (key = 0; key < K_LAST; key++)
	{
		key_repeats[key] = 0;
		keydown[key] = 0;
	}
}
