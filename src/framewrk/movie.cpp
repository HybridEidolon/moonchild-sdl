#include "frm_int.hpp"

// videos of Moon Child were in a proprietary format called Smackplayer'. However that doesn't exist anymore so videos not supported.
// hover I leave this class here so you'll see the point where the movie get's called... It just doesn't play anything now


Cmovie::Cmovie(Caudio *audio)
{
    this->videoFilename = NULL;
}


Cmovie::Cmovie( void )
{
    this->videoFilename = NULL;
}

Cmovie::~Cmovie(void)
{
}


Smack *Cmovie::open(char *filename)
{
    printf("trying to play movie: %s", filename);
    if(strcmp(filename, (char *)"intro.smk")==0)
    {
        this->videoFilename = "intro";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, (char *)"bumper12.smk")==0)
    {
        this->videoFilename = "bumper12";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, (char *)"bumper23.smk")==0)
    {
        this->videoFilename = "bumper23";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, (char *)"bumper34.smk")==0)
    {
        this->videoFilename = "bumper34";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, (char *)"extro.smk")==0)
    {
        this->videoFilename = "extro";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, (char *)"gameover.smk")==0)
    {
        this->videoFilename = "gameover.mp4";
        this->videoReady = false;
        return (Smack *)1;
    }
    
    printf("Trying to play unknown moviefile!");
    
	return 0;
}


void   Cmovie::close(Smack *smk)
{
}

void   Cmovie::playtovideo(Smack *smk, Cvideo *video, Cblitbuf *hulpbuf, UINT16 zoomfactor)
{
}

UINT16 Cmovie::stillplaying(void)
{
	return !videoReady;
}

void   Cmovie::movieplay(void)
{
}

void Cmovie::returnpal(BYTE *destpal)
{
}




void Cmovie::ClearBack(u32 flipafter)
{
}

void Cmovie::dopal(void)
{
}


void Cmovie::blitrect( u32 x, u32 y, u32 w, u32 h)
{
}

void Cmovie::DoPaint()
{
}


void Cmovie::mergeinterrect(whRECT* r1, whRECT* r2, whRECT* m, whRECT* i)
{
}

void Cmovie::mergerects(whRECT* r1, u32 r1num, whRECT* r2, u32 r2num, whRECT** o, u32* onum)
{
}


void Cmovie::DoAdvance( )
{
}


void Cmovie::InitPal( void )
{
}



