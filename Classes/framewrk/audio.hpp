/* The audio classes */

#ifndef _AUDIO_HPP
#define _AUDIO_HPP

typedef long HSNDOBJ;


class Caudio {

public:
  Caudio(void);
  ~Caudio(void);

  void	  reset_audio();

  void    checkVolume();

  UINT16  play_cd(UINT16 tracknr);
  void    play_cd_cb(UINT16 tracknr);
  void    stop_cd(void);
  HSNDOBJ create_sound(int SoundID, int nrof_simult);
  void    destroy_sound(HSNDOBJ sound);
  void    play_sound_1shot(HSNDOBJ sound, INT32 volume, INT32 pan);
  void    play_sound_loop (HSNDOBJ sound, INT32 volume, INT32 pan);  // only works if nr of simult was 1 !
  void    stop_sound(HSNDOBJ sound);
  void    stop_cursound(HSNDOBJ sound);
  void    sound_volume(HSNDOBJ sound, INT32 volume);
  void    sound_pan(HSNDOBJ sound, INT32 pan);

  int	get_dsound(void);
  
  void    dump(FILE *fd);
  
private:
};


#endif
