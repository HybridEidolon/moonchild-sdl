#include "frm_int.hpp"

POINT   cp={0,0};
u32     windx,windy;
u32     doneone=0;
whRECT  rects1[256];
whRECT  rects2[256];
whRECT  merged[256];
u32     lastnum=0;
whRECT* lastframe=rects1;
whRECT* curframe=rects2;
BYTE    pallo[768];
UINT16  dithcnt = 0;

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
    NSLog(@"trying to play movie: %s", filename);
    if(strcmp(filename, "intro.smk")==0)
    {
        this->videoFilename = "intro";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, "bumper12.smk")==0)
    {
        this->videoFilename = "bumper12";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, "bumper23.smk")==0)
    {
        this->videoFilename = "bumper23";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, "bumper34.smk")==0)
    {
        this->videoFilename = "bumper34";
        this->videoReady = false;
        return (Smack *)1;
    }
    if(strcmp(filename, "extro.smk")==0)
    {
        this->videoFilename = "extro";
        this->videoReady = false;
        return (Smack *)1;
    }
    
    NSLog(@"Trying to play unknown moviefile!");
    
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



