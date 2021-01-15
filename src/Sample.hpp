/* B.Oops
 * Glitch effect sequencer LV2 plugin
 *
 * Copyright (C) 2020 by Sven Jähnichen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef SAMPLE_HPP_
#define SAMPLE_HPP_

#include "sndfile.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <stdexcept>

#ifndef SF_FORMAT_MP3
#ifndef MINIMP3_FLOAT_OUTPUT
#define MINIMP3_FLOAT_OUTPUT
#endif
#include "minimp3_ex.h"
#endif /* SF_FORMAT_MP3 */


struct Sample
{
        SF_INFO         info;      // Info about sample from sndfile
        float*          data;      // Sample data in float
        char*           path;      // Path of file
        bool            loop;      // Loop playing mode
        sf_count_t      start;     // Start frame
        sf_count_t      end;       // End frame

        Sample () : info {0, 0, 0, 0, 0, 0}, data (nullptr), path (nullptr) {}

        Sample (const char* samplepath) :
                info {0, 0, 0, 0, 0, 0}, data (nullptr), path (nullptr),
                loop (false), start (0), end (0)
        {
                if (!samplepath) return;

        	int len = strlen (samplepath);
                path = (char*) malloc (len + 1);
                if (!path) throw std::bad_alloc();
                memcpy (path, samplepath, len + 1);
                if (!len) return;

                // Extract file extension
                char* extptr = strrchr (path, '.');
                const int extsz = (extptr ? strlen (extptr) + 1 : 1);
                char* ext = (char*) malloc (extsz);
                if (!ext) throw std::bad_alloc();
                ext[0] = 0;
                if (extsz > 1) memcpy (ext, extptr, extsz);
                for (char* s = ext; *s; ++s) *s = tolower ((unsigned char)*s);


                // Check for known non-sndfiles
#ifndef SF_FORMAT_MP3
                if (!strcmp (ext, ".mp3"))
                {
                        mp3dec_t mp3dec;
                        mp3dec_file_info_t mp3info;
                        if (mp3dec_load (&mp3dec, path, &mp3info, NULL, NULL)) throw std::invalid_argument ("Can't open " + std::string (path) + ".");

                        info.samplerate = mp3info.hz;
                        info.channels = mp3info.channels;
                        info.frames = mp3info.samples / mp3info.channels;

                        data = (float*) malloc (sizeof(float) * info.frames * info.channels);
                        if (!data) throw std::bad_alloc();

                        memcpy (data, mp3info.buffer, sizeof(float) * info.frames * info.channels);
                }

                else
#endif /* !SF_FORMAT_MP3 */

        	{
                        SNDFILE* sndfile = sf_open (samplepath, SFM_READ, &info);

                        if ((!sndfile) || (!info.frames)) throw std::invalid_argument ("Can't open " + std::string (path) + ".");

                        // Read & render data
                        data = (float*) malloc (sizeof(float) * info.frames * info.channels);
                        if (!data)
                	{
                                sf_close (sndfile);
                		throw std::bad_alloc();
                	}

                        sf_seek (sndfile, 0, SEEK_SET);
                        sf_read_float (sndfile, data, info.frames * info.channels);
                        sf_close (sndfile);
                }

                end = info.frames;
        }

        Sample (const Sample& that) :
                info (that.info), data (nullptr), path (nullptr),
                loop (that.loop), start (that.start), end (that.end)
        {
                if (that.data)
                {
                        data = (float*) malloc (sizeof(float) * info.frames * info.channels);
                        if (!data) throw std::bad_alloc();
                        memcpy (data, that.data, sizeof(float) * info.frames * info.channels);
                }

                if (that.path)
                {
                        int len = strlen (that.path);
                        path = (char*) malloc (len + 1);
                        if (!path) throw std::bad_alloc();
                        memcpy (path, that.path, len + 1);
                }
        }

        ~Sample()
        {
                if (data) free (data);
        	if (path) free (path);
        }

        Sample& operator= (const Sample& that)
        {
                if (data) free (data);
        	if (path) free (path);

                info = that.info;
                data = nullptr;
                path = nullptr;
                loop = that.loop;
                start = that.start;
                end = that.end;

                if (that.data)
                {
                        data = (float*) malloc (sizeof(float) * info.frames * info.channels);
                        if (!data) throw std::bad_alloc();
                        memcpy (data, that.data, sizeof(float) * info.frames * info.channels);
                }

                if (that.path)
                {
                        int len = strlen (that.path);
                        path = (char*) malloc (len + 1);
                        if (!path) throw std::bad_alloc();
                        memcpy (path, that.path, len + 1);
                }

                return *this;
        }

        float get (const sf_count_t frame, const int channel, const int rate)
        {
        	if (!data) return 0.0f;

        	// Direct access if same frame rate
        	if (info.samplerate == rate)
        	{
        		if (frame >= info.frames) return 0.0f;
        		else return data[frame * info.channels + channel];
        	}

        	// Linear rendering if frame rates differ
        	const double f = (frame * info.samplerate) / rate;
                const double frac = fmod (f, 1.0);
        	sf_count_t f1 = f;

        	if (f1 >= info.frames) return 0.0f;

        	if (frac == 0.0) return data[f1 * info.channels + channel];

        	float data1 = data[f1 * info.channels + channel];
        	float data2 = (f1 + 1 < info.frames ? data[(f1 + 1) * info.channels + channel] : data1);
        	return (1.0 - frac) * data1 + frac * data2;
        }
};

#endif /* SAMPLE_HPP_ */
