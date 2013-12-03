/*
 * Copyright (C) 2003-2013 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MPD_CUE_PARSER_HXX
#define MPD_CUE_PARSER_HXX

#include "check.h"
#include "Compiler.h"

#include <string>

struct Song;
struct Tag;

class CueParser {
	enum {
		/**
		 * Parsing the CUE header.
		 */
		HEADER,

		/**
		 * Parsing a "FILE ... WAVE".
		 */
		WAVE,

		/**
		 * Ignore everything until the next "FILE".
		 */
		IGNORE_FILE,

		/**
		 * Parsing a "TRACK ... AUDIO".
		 */
		TRACK,

		/**
		 * Ignore everything until the next "TRACK".
		 */
		IGNORE_TRACK,
	} state;

	/**
	 * Tags read from the CUE header.
	 */
	Tag *header_tag;

	std::string filename;

	/**
	 * The song currently being edited.
	 */
	Song *current;

	/**
	 * The previous song.  It is remembered because its end_time
	 * will be set to the current song's start time.
	 */
	Song *previous;

	/**
	 * A song that is completely finished and can be returned to
	 * the caller via cue_parser_get().
	 */
	Song *finished;

	/**
	 * Set to true after previous.end_time has been updated to the
	 * start time of the current song.
	 */
	bool last_updated;

	/**
	 * Tracks whether cue_parser_finish() has been called.  If
	 * true, then all remaining (partial) results will be
	 * delivered by cue_parser_get().
	 */
	bool end;

public:
	CueParser();
	~CueParser();

	/**
	 * Feed a text line from the CUE file into the parser.  Call
	 * cue_parser_get() after this to see if a song has been finished.
	 */
	void Feed(const char *line);

	/**
	 * Tell the parser that the end of the file has been reached.  Call
	 * cue_parser_get() after this to see if a song has been finished.
	 * This procedure must be done twice!
	 */
	void Finish();

	/**
	 * Check if a song was finished by the last cue_parser_feed() or
	 * cue_parser_finish() call.
	 *
	 * @return a song object that must be freed by the caller, or NULL if
	 * no song was finished at this time
	 */
	Song *Get();

private:
	gcc_pure
	Tag *GetCurrentTag();

	/**
	 * Commit the current song.  It will be moved to "previous",
	 * so the next song may soon edit its end time (using the next
	 * song's start time).
	 */
	void Commit();

	void Feed2(char *p);
};

#endif
