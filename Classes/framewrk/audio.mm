#include "frm_int.hpp"

#import <AVFoundation/AVFoundation.h>
#import "FISoundEngine.h"
#import "FIFactory.h"
#import "FISound.h"

FIFactory *soundFactory;
FISoundEngine *engine;
AVAudioPlayer *audioPlayer;   //for playing background music


float calcVolume(INT32 volume);
float calcPan(INT32 pan);

Caudio::Caudio(void)
{
    soundFactory = [[FIFactory alloc] init];
    
    engine = [[soundFactory buildSoundEngine] retain];
    BOOL rc = [engine activateAudioSessionWithCategory:AVAudioSessionCategoryPlayback];
    rc = [engine openAudioDevice];
    if(rc) NSLog(@"Audio initted");
    
    
}

Caudio::~Caudio(void)
{
}

UINT16 Caudio::play_cd(UINT16 tracknr)
{
    NSString *track = nil;
    float volume = 1.0f;
    switch(tracknr)
    {
        case 2:
            track = @"title.mp3";
            volume = 0.8f;
            break;
        case 3:
            track = @"world1.mp3";
            volume = 0.5f;
            break;
        case 4:
            track = @"world2.mp3";
            volume = 0.5f;
            break;
        case 5:
            track = @"world3.mp3";
            volume = 0.5f;
            break;
        case 6:
            track = @"world4.mp3";
            volume = 0.5f;
            break;
        case 7:
            track = @"gameover.mp3";
            volume = 1.0f;
            break;
        default:
            NSLog(@"audi track %d requested", tracknr);
            break;
    }
    
    if(nil == track) return 0;

    NSURL *url = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@/%@", [[NSBundle mainBundle] resourcePath], track]];
    
	NSError *error;
	audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
	audioPlayer.numberOfLoops = -1;
    audioPlayer.volume = volume;
    
	if (audioPlayer == nil)
		NSLog([error description]);
	else
		[audioPlayer play];    

    [audioPlayer retain];
 
	return 0;
}

void Caudio::checkVolume()
{
    BOOL musicEnabled= [[NSUserDefaults standardUserDefaults] boolForKey:@"music_preference"];
    
    audioPlayer.volume = (musicEnabled) ? 1.0f : 0.0f;
}


void Caudio::play_cd_cb(UINT16 tracknr)
{
}


void Caudio::stop_cd(void)
{
    [audioPlayer stop];
    [audioPlayer release];
    audioPlayer = nil;
}

char *soundFilenames[] = 
{
    "", //0
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",  //15
    "mcskid.wav",  //16
    "spring.wav", //17
    "mcwall.wav", //18
    "mcdrain.wav", //19
    "mcfall.wav", //20
    "blub.wav", //21
    "door.wav",  //22
    "switch.wav", //23
    "bonus.wav", //24
    "warp.wav", //25
    "shoot.wav",  //26
    "rumble.wav", //27
    "waterval.wav", //28
    "mcloop1.wav", //29
    "mcloop2.wav", //30
    "sapstap1.wav", //31
    "sapstap2.wav", //32
    "helmlp1.wav", //33
    "helmlp2.wav", //34
    "houtpunt.wav", //35
    "ball.wav", //36
    "segmshot.wav", //37
    "segmexpl.wav", //38
    "segmhit.wav", //39
    "vlamwerp.wav",  //40
    "tanden.wav", //41
    "tandenm.wav", //42
    "camstart.wav",  //43
    "camstop.wav",  //44
    "cammove.wav",  //45
    "madeit.wav", //46
    "heks.wav", //47
    "graspod.wav", //48
    "bat.wav", //49
    "vogel.wav", //50
    "vuur.wav", //51
    "drilboor.wav", //52
    "loopband.wav", //53
    "ventltor.wav", //54
    "bee.wav", //55
    "bee2.wav", //56
    "ptoei.wav", //57
    "schuif.wav", //58
    "smexp.wav", //59
    "backpak.wav", //60
    "restart.wav", //61
    "bigexp.wav", //62
    "stroom.wav", //63
    "cannon.wav", //64
    "gewicht.wav", //65
    "wheel.wav", //66
    "appel.wav", //67
    "mcdood.wav", //68
    "mcfart.wav", //69
    "chemo.wav", //70
    "tik.wav", //71
    "tak.wav",//72
    "raket.wav", //73
    "chemo2.wav", //74
    "helmdood.wav", //75
    "ketting.wav", //76
    "dimndsht.wav", //77
    "glasblok.wav", //78
    "hyprlift.wav", //79
    "lightwav.wav", //80
    "morphsht.wav", //81
    "mushup.wav", //82
    "mushdwn.wav", //83
    "plntlft.wav", //84
    "pltfdwn.wav", //85
    "pltfup.wav", //86
    "qbert1.wav", //87
    "qbert2.wav", //88
    "roltnlp.wav", //89
    "slowlift.wav", //90
    "tangjmp.wav", //91
    "tangclos.wav", //92
    "woeiwoei.wav", //93
    "heatskr.wav", //94
    "demo.wav", //95
    "explo.wav",
    "mcdrainold.wav"
};

HSNDOBJ Caudio::create_sound(int SoundID, int nrof_simult)
{
    NSLog(@"createsound(%d): %s", SoundID, soundFilenames[SoundID]);
    NSString *soundName = [NSString stringWithCString:soundFilenames[SoundID] encoding:NSASCIIStringEncoding];
    FISound *soundA = [[soundFactory loadSoundNamed:soundName  maxPolyphony:nrof_simult] retain];   

	return (long)soundA;
}


void Caudio::destroy_sound(HSNDOBJ sound)
{
    FISound *soundA = (FISound *) sound;
    [soundA release];
}

void Caudio::reset_audio()
{
}

void Caudio::stop_sound(HSNDOBJ sound)
{
    FISound *soundA = (FISound *) sound;
    [soundA stop];
}


void Caudio::stop_cursound(HSNDOBJ sound)
{
    FISound *soundA = (FISound *) sound;
    [soundA stop];
}


void Caudio::play_sound_1shot(HSNDOBJ sound, INT32 volume, INT32 pan)
{
    BOOL soundEnabled= [[NSUserDefaults standardUserDefaults] boolForKey:@"sound_preference"];
    if(!soundEnabled) return;
    
    float panf = calcPan(pan);
    float gain = calcVolume(volume);
    
    
    FISound *soundA = (FISound *) sound;
    soundA.loop = NO;
    soundA.gain = gain;
    soundA.pan = panf;
    [soundA play];
}


void Caudio::play_sound_loop(HSNDOBJ sound, INT32 volume, INT32 pan)
{
    BOOL soundEnabled= [[NSUserDefaults standardUserDefaults] boolForKey:@"sound_preference"];
    if(!soundEnabled) return;

    float panf = calcPan(pan);
    float gain = calcVolume(volume);
    
    FISound *soundA = (FISound *) sound;
    soundA.loop = YES;
    soundA.gain = gain;
    soundA.pan = panf;
    [soundA play];
}


float calcVolume(INT32 volume)
{
    if(volume < -4000)volume = -4000;
    if(volume > 0) volume = 0;
    float gain = (volume+4000) / 4000.0f;
    return gain;
    
    /*
    //komt binnen als -4000 tot 0
    if(volume<-0.001f)
    {
        if(volume<0) volume=-volume;
        volume = 4000 / volume;
    }
    else
    {
        volume = 0;
    }
    
    
    if(volume<0)volume =0;
    if(volume>=4000) volume = 4000;
    float gain = volume / 4000.0f;
    return gain;
     */
}

float calcPan(INT32 pan)
{
    if(pan < -1000)pan = -1000;
    if(pan >= 1000) pan = 1000;
    float panf = pan / 1000.0f;
    return panf;
}

void Caudio::sound_volume(HSNDOBJ sound, INT32 volume)
{
    float gain = calcVolume(volume);
    FISound *soundA = (FISound *) sound;
    soundA.gain = gain;
}


void Caudio::sound_pan(HSNDOBJ sound, INT32 pan)
{
    float panf = calcPan(pan);
    FISound *soundA = (FISound *) sound;
    soundA.pan = panf;
}

int Caudio::get_dsound(void)
{
	return 1;
}

  
