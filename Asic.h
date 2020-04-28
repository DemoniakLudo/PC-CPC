#ifndef ASIC_H
#define ASIC_H


extern BOOL AsicOn;

extern UBYTE MemASIC[ 0x4000 ];


void SetAsicColor( int Pent, int Val );

void DelockAsic( int Val );

void ResetAsic( void );

void SyncAsic( void );



#endif
