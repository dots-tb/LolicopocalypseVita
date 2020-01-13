
// -DICUMFROMGL for OpenGL 
//#define ICUMFROMGL
//

#ifdef ICUMFROMGL
#include "GL/gl.h"
#include "SDL/SDL_opengl.h"
#endif

#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"


//...
#include "gfx.h"


SDL_Surface *scr=NULL;
SDL_Joystick *gmp=NULL;
Mix_Music *mus=NULL;
Mix_Chunk *snd[32];

//https://github.com/rsn8887/SDLPoP-Vita/blob/vita/src/config.h
#define VITA_BTN_TRIANGLE 0
#define VITA_BTN_CIRCLE 1
#define VITA_BTN_CROSS 2
#define VITA_BTN_SQUARE 3
#define VITA_BTN_LTRIGGER 4
#define VITA_BTN_RTRIGGER 5
#define VITA_BTN_DOWN 6
#define VITA_BTN_LEFT 7
#define VITA_BTN_UP 8
#define VITA_BTN_RIGHT 9
#define VITA_BTN_SELECT 10
#define VITA_BTN_START 11

int screen_res_w = 960;
int screen_res_h = 544;

SDLKey jpd[8]={VITA_BTN_RIGHT,VITA_BTN_UP,VITA_BTN_LEFT,VITA_BTN_DOWN,VITA_BTN_CROSS,VITA_BTN_CIRCLE,VITA_BTN_SQUARE,VITA_BTN_START};
SDLKey kbk[8]={SDLK_RIGHT,SDLK_UP,SDLK_LEFT,SDLK_DOWN,SDLK_SPACE,SDLK_LCTRL,SDLK_RETURN,SDLK_ESCAPE};


unsigned char scrb[80*60],scfx[80*60];
unsigned char a[65536];
unsigned long pal[16];
unsigned short pal16[32];
// 0-gameloop enabled
// 21 loadedgame
// 22 seenevil
//
// 128-16511 -first
// 16512-32895 second evol
// 32896 ... block ?

signed short b[64];
// 0 camx
// 1 camy
// 2 px
// 3 py

unsigned long count=0;

char spth[128];

void prepblock(void)
{
unsigned long i,x,y;
for (i=0;i<1024*1024;i++)
if (header_data[i]>7)
{
x=(i%1024)/8;
y=(i/1024/8);
a[32896+x+y*128]=1;
}

}


#ifdef ICUMFROMGL
GLfloat tx=80.0/128.0,ty=60.0/64.0;
GLushort stex[128*64];//hail thy buffer

void magicscreen(void)
{
glBegin(GL_QUADS);
glTexCoord2f(0.0, ty);glVertex2f(-1.0,-1.0);
glTexCoord2f( tx, ty);glVertex2f(1.0,-1.0);
glTexCoord2f( tx,0.0);glVertex2f(1.0,1.0);
glTexCoord2f(0.0,0.0);glVertex2f(-1.0,1.0);
glEnd();
}
#endif
#ifndef ICUMFROMGL
unsigned short scrbuf[320*240];
#endif


void renderscreen()
{
unsigned long x,y;

#ifdef ICUMFROMGL

for (y=0;y<60;y++) for (x=0;x<80;x++)
{
//scrb[x+y*60]=(count+x)%16;
stex[x+((y)<<7)]=pal16[scrb[x+y*80]];	
}
glEnable(GL_TEXTURE_2D);

glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,128,64,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,stex);
glPushMatrix();

glEnable(GL_BLEND);
glColor4f(0.9,0.8,0.7,1.0);magicscreen();
glScalef(1.01,1.01,1.0);
glColor4f(0.0,1.0,0.0,0.2);magicscreen();
glScalef(1.02,1.02,1.0);
glColor4f(1.0,0.0,0.0,0.2);magicscreen();
// =/
glDisable(GL_BLEND);
glDisable(GL_TEXTURE_2D);
glPopMatrix();

SDL_GL_SwapBuffers();
#endif

#ifndef ICUMFROMGL
unsigned short c,cdii;
unsigned long i=0,ii=0;
unsigned char ci;
for (y=0;y<60;y++)
{
ii=320*y*4;
i=y*80;
for (x=0;x<80;x++)
{
ci=scrb[i+x];
c=pal16[ci];
cdii=pal16[((ci)&15)+16];

scrbuf[ii+960]=cdii;
scrbuf[ii+961]=cdii;
scrbuf[ii+962]=cdii;
scrbuf[ii+963]=cdii;

scrbuf[ii+320]=c;
scrbuf[ii+321]=c;
scrbuf[ii+322]=c;
scrbuf[ii+323]=cdii;

scrbuf[ii+640]=c;
scrbuf[ii+641]=c;
scrbuf[ii+642]=c;
scrbuf[ii+643]=cdii;

scrbuf[ii++]=c;
scrbuf[ii++]=c;
scrbuf[ii++]=c;
scrbuf[ii++]=cdii;
}

}

SDL_LockSurface(scr);
memcpy(scr->pixels,scrbuf,320*240*2);
SDL_UnlockSurface(scr);
SDL_Flip(scr);

#endif

}

void render(void)
{
signed short x,y,cx,cy,ix,iy;
unsigned char c,anim=count/10%2;
cx=40+b[0];
cy=30+b[1];
//unoptimized quick solution ... I wanna rape thine cpu

for (iy=0;iy<60;iy++)
for (ix=0;ix<80;ix++)
{
scrb[ix+iy*80]=15;
x=ix-40+cx;
y=iy-30+cy;

if (x>=0)
if (x<1024)
if (y>=0)
if (y<1024)
{
c=a[128+((x)>>3)+((y)>>3)*128];

if (c>5) a[22]=10;

if (c!=0) c=c*2+anim;

if (c==0) scrb[ix+iy*80]=header_data[x+y*1024];
else scrb[ix+iy*80]=header_data[c*8-16+(x&7)+(8+(y&7))*1024];
}
}

}


void mutaterender(void)
{
unsigned char x,y,i,r,g,b;
unsigned short ii;
x=rand()%80;
y=rand()%60;
i=rand()%16;
scfx[x+y*80]=i;

i=rand()&32;
for (x=0;x<80;x++)
for (y=0;y<60;y++)
{
ii=x+y*80;

if (scfx[ii]==8) {if (i==0) scrb[ii]=15;}
else
if (scfx[ii]==1) {scrb[ii]+=16;}
else
if (scfx[ii]==10) {if (x>0) scrb[ii]=scrb[ii-1];}
else
if (scfx[ii]==11) {if (y>0) scrb[ii]=scrb[ii-80];}
else
if (scfx[ii]==12) {if (x<79) scrb[ii]=scrb[ii+1];}
else
if (scfx[ii]==13) {if (y<59) scrb[ii]=scrb[ii+80];}


}

i=rand()%16;

r=pal16[i]&31;
g=((pal16[i])>>5)&63;
b=((pal16[i])>>11)&31;

x=rand()%7;

switch(x)
{
case 0:if (r>0) r--; else r++; break;
case 1:if (g>0) g--; else g++; break;
case 2:if (b>0) b--; else b++; break;
case 3:if (r<31) r++; else r--; break;
case 4:if (g<63) g++; else g--; break;
case 5:if (b<31) b++; else b--; break;


}

pal16[i]=r+((g)<<5)+((b)<<11);

pal16[16+i]=r/2+((g/2)<<5)+((b/2)<<11);
}

void loadpal(void)
{
unsigned char i;
pal[0]=0x000000;
pal[1]=0x000080;
pal[2]=0x008000;
pal[3]=0x008080;
pal[4]=0x800000;
pal[5]=0x800080;
pal[6]=0x808000;
pal[7]=0x808080;
pal[8]=0xc0c0c0;
pal[9]=0x0000ff;
pal[10]=0x00ff00;
pal[11]=0x00ffff;
pal[12]=0xff0000;
pal[13]=0xff00ff;
pal[14]=0xffff00;
pal[15]=0xffffff;


for (i=0;i<16;i++)
pal16[i]=((((pal[i])>>3)&31))+((((pal[i])>>10)&63)<<5)+((((pal[i])>>19)&31)<<11);
}

void clearscfx(void)
{
unsigned short i;
for (i=0;i<80*60;i++) scfx[i]=0;

}

void newgame(void)
{
unsigned long i;
for (i=0;i<32896;i++) a[i]=0;
for (i=0;i<128;i++) b[i]=0;
loadpal();
clearscfx();
a[0]=1;
a[11]=0;

a[21]=1;

b[0]=0;
b[1]=1024-60;
b[4]=0;
b[5]=1024-60;


b[2]=74/8;
b[3]=172/8;

a[128+b[2]+b[3]*128]=1;


a[128+53+12*128]=5;
a[128+5+23*128]=6;


count=0;
}
void newturn(int aid)
{
unsigned char i,die=0,k;

unsigned long x,y,ii;

// 128-16511 -first
// 32896 ... block ?


if (a[22]) {a[11]=0;a[22]--;} else {a[11]=1;}

for (y=1;y<127;y++)
for (x=1;x<127;x++)
{
a[16512+x+y*128]=0;
}
for (y=1;y<127;y++)
for (x=1;x<127;x++)
{
ii=x+y*128;
i=a[128+ii];


if (a[16512+ii]==0) a[16512+ii]=a[128+ii];

if (i==6) a[16512+ii]=7;else
if (i==7) a[16512+ii]=8;else
if (i==8) a[16512+ii]=9;else
if (i==9) 
{//grow
if (a[128+ii-128]<6) if (a[32896+ii-128]==0) a[16512+ii-128]=6;
if (a[128+ii+128]<6) if (a[32896+ii+128]==0) a[16512+ii+128]=6;
if (a[128+ii-1]<6) if (a[32896+ii-1]==0) a[16512+ii-1]=6;
if (a[128+ii+1]<6) if (a[32896+ii+1]==0) a[16512+ii+1]=6;
}
else 
if (i==5) // bobble
{
k=(rand())%20;
if (k==7) a[16512+ii]=0;
if (k==1) {if (a[128+ii-128]==0) if (a[32896+ii-128]==0) a[16512+ii-128]=5;}
if (k==2) {if (a[128+ii+128]==0) if (a[32896+ii+128]==0) a[16512+ii+128]=5;}
if (k==3) {if (a[128+ii-1]==0) if (a[32896+ii-1]==0) a[16512+ii-1]=5;}
if (k==4) {if (a[128+ii+1]==0) if (a[32896+ii+1]==0) a[16512+ii+1]=5;}
}


}
for (y=1;y<127;y++)
for (x=1;x<127;x++)
{
ii=x+y*128;
a[128+ii]=a[16512+ii];
}


if (aid>0)
a[128+b[2]+b[3]*128]=0;

b[6]=b[2];
b[7]=b[3];

if (aid==1) b[6]=b[2]+1;
if (aid==2) b[7]=b[3]-1;
if (aid==3) b[6]=b[2]-1;
if (aid==4) b[7]=b[3]+1;

if (a[32896+b[6]+b[7]*128]==0) 
{b[2]=b[6];b[3]=b[7];}


i=(a[128+b[2]+b[3]*128]);
if (i>4)
{
if (i==5)//bobble
{
if ((rand()%2)==0) {loadpal();} else {clearscfx();}
}
if (i>5) a[21]=0;
}
if (i<6)
if (aid>0) a[128+b[2]+b[3]*128]=aid;

 b[4]=b[2]*8-40+4;
 b[5]=b[3]*8-30+4;

}

void procgame(void)
{
if (count>40)
if (a[13]!=a[10])//newkeywtf
{
//gamemodeeffect
if (a[20]==0)
{
if (a[10]>8) 
 {
 a[20]=1;a[11]=1;
 b[0]=b[2]*8-40;
 b[1]=b[3]*8-30;
 b[4]=b[2]*8-40;
 b[5]=b[3]*8-30;
 }
}//^splash
else
if (a[20]==1)
{
if (a[10]&16) newturn(0);else
if (a[10]&1) newturn(1);else
if (a[10]&2) newturn(2);else
if (a[10]&4) newturn(3);else
if (a[10]&8) newturn(4);
}//^gameplay

}

a[13]=a[10];

if (a[20]>0)
{
if (b[0]>b[4]) b[0]--; else if (b[0]<b[4]) b[0]++;
if (b[1]>b[5]) b[1]--; else if (b[1]<b[5]) b[1]++;
}

}

int main(void)
{
unsigned char i,ii;
newgame();
prepblock();
a[12]=255;
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
SDL_JoystickOpen(0);
Mix_OpenAudio(44100,AUDIO_S16,2,1024);

srand(time(NULL));
#ifdef ICUMFROMGL
SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);//why???
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

scr=SDL_SetVideoMode(640,480,32,SDL_OPENGL);

SDL_WM_SetCaption("Lolicopocalypse:Evolution",NULL);
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
glEnable(GL_TEXTURE_2D);
#endif
#ifndef ICUMFROMGL
scr=SDL_SetVideoMode(320,240,16,SDL_HWSURFACE | SDL_DOUBLEBUF);
SDL_SetVideoModeScaling(120, 0, 720, 540);
#endif
SDL_ShowCursor(0);
SDL_ShowCursor(0);//This fear comes from DirectX 6.0

a[0]=1;

while (a[0])
{

if (a[11]!=a[12])
 {
 a[12]=a[11];
 Mix_HaltMusic();
 Mix_FreeMusic(mus);
 #ifdef VITA
 sprintf(spth,"app0:%i.ogg",a[11]);
 #else
 sprintf(spth,"%i.ogg",a[11]);
 #endif

 mus=Mix_LoadMUS(spth);
 Mix_PlayMusic(mus,100);
 }


SDL_Event ev;
while (SDL_PollEvent(&ev)) switch(ev.type)
 {
 case SDL_QUIT		   :a[0]=0;break;
 case SDL_KEYDOWN	   :for (i=0;i<8;i++) if(ev.key.keysym.sym==kbk[i]) a[2+i]=1;break;
 case SDL_KEYUP  	   :for (i=0;i<8;i++) if(ev.key.keysym.sym==kbk[i]) a[2+i]=0;break;
 case SDL_JOYBUTTONDOWN:for (i=0;i<8;i++) if(ev.jbutton.button==jpd[i]) a[2+i]=1;break;
 case SDL_JOYBUTTONUP  :for (i=0;i<8;i++) if(ev.jbutton.button==jpd[i]) a[2+i]=0;break;

 }//event control
ii=1;




a[10]=0;
for (i=0;i<8;i++)
{
if (a[2+i]!=0) a[10]=a[10]|ii;
ii=ii*2;
}
procgame();
render();
mutaterender();
renderscreen();
SDL_Delay(50);
count++;
if (a[10]==128) a[0]=0;

if (a[21]==0) newgame();
}

Mix_CloseAudio();
SDL_Quit();
return 0;
}



