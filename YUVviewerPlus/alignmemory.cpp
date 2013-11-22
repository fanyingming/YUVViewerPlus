#include "stdafx.h"
void *align_malloc( int i_size )
{
    unsigned char *buf;
    unsigned char *align_buf;
    buf = (unsigned char *) malloc( i_size + 15 + sizeof( void ** ) +
              sizeof( int ) );
    align_buf = buf + 15 + sizeof( void ** ) + sizeof( int );
    align_buf -= (long) align_buf & 15;
    *( (void **) ( align_buf - sizeof( void ** ) ) ) = buf;
    *( (int *) ( align_buf - sizeof( void ** ) - sizeof( int ) ) ) = i_size;
    return align_buf;
}

void align_free( void *p )
{
    if( p )
    {
        free( *( ( ( void **) p ) - 1 ) );
    }
}