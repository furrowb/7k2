/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 * Copyright 2010 Unavowed <unavowed@vexillium.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENAL_AUDIO_H
#define OPENAL_AUDIO_H

#include <map>

#include OPENAL_AL_H
#include OPENAL_ALC_H

#include <audio_base.h>
#include <audio_stream.h>
#include <input_stream.h>

class OpenALAudio: public AudioBase
{
private:
   class StreamContext
   {
   public:
      AudioStream *stream;
      ALuint source;

      /* frames played since fade started */
      size_t fade_frames_played;

      /*
       * Number of frames over which volume fades to zero.  A value of
       * 0 means not fading.
       */
      size_t fade_frames;

      bool looping;

      /* where to restart playing after reaching the end */
      size_t loop_start_frame;

      bool streaming;

   public:
      StreamContext();
      ~StreamContext();
      bool init(AudioStream *as);
      bool stream_data(int new_buffer_count = 0);
      void stop();
      void apply_fading(void *buffer, size_t frames);

   private:
      /* forbid copying */
      StreamContext(const StreamContext &) {}
   };

   typedef std::map<int, StreamContext *> StreamMap;

public:
   OpenALAudio();
   ~OpenALAudio();

   int	init();
   void	deinit();

   void	yield(); // called by sys every some time

   int	play_mid(char*);

   // functions on short wave
   int	play_wav(char*, const DsVolume &);
   int	play_wav(short resIdx, const DsVolume &);
   int	play_resided_wav(char *, const DsVolume &);
   int	get_free_wav_ch();
   int	stop_wav(int);
   int	is_wav_playing(int);

   // functions on long wave
   int	play_long_wav(const char*, const DsVolume &);
   int	stop_long_wav(int);
   int	is_long_wav_playing(int);
   void	volume_long_wav(int ch, const DsVolume &);

   // functions on loop wave

   // return channel no.
   int	play_loop_wav(const char *, int repeatOffset, const DsVolume &);
   void	stop_loop_wav(int ch);
   void	volume_loop_wav(int ch, const DsVolume &);
   void	fade_out_loop_wav(int ch, int fadeRate);
   DsVolume get_loop_wav_volume(int ch);
   int	is_loop_wav_fading(int ch);

   int	play_cd(int, int retVolume);

   void	stop_mid();
   void	stop_wav();             // and stop also long wav
   void	stop_cd();

   int	is_mid_playing();
   int	is_wav_playing();
   int	is_cd_playing();

   void	toggle_mid(int);
   void	toggle_wav(int);
   void	toggle_cd(int);

   void	set_mid_volume(int);
   void	set_wav_volume(int);    // 0 to 100
   void	set_cd_volume(int);

   int	get_wav_volume() const; // 0 to 100

private:
   ALCdevice  *al_device;
   ALCcontext *al_context;

   StreamMap streams;

   int	max_sources;
   int	wav_volume; // -10000 to 0

private:
   int	init_mid();
   int	init_wav();
   int	init_cd();

   void	deinit_mid();
   void	deinit_wav();
   void	deinit_cd();

   int	play_long_wav(InputStream *, const DsVolume &);
};

typedef OpenALAudio Audio;

#endif
