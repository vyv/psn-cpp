//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 VYV Corporation
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
**/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// This simple OpenGL/Glut PSN client example display's solar system simulation, 
// data is received from the PSN server example.
// This program is in no way realistic or representative of the solar system and 
// is simply used to demonstrate the PSN protocol usage and implementation
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#include "psn_lib.hpp"
#include "utils/udp_socket.h"
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define PI_DEF   3.1415926535897932384626433832795f

::psn::float3 eye_position ;
int screen_width ;
int screen_height ;
int mouse_x ;
int mouse_y ;

wsa_session session ; // Init Microsoft Windows Winsock library 
udp_socket socket_client ; // simple UDP Socket, used for communication
::psn::psn_decoder psn_decoder ; // PSN library to handle message decoding
::psn::tracker_map recv_trackers ; // PSN received trackers

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void glut_string( GLfloat x , 
                  GLfloat y , 
                  const ::std::string & text , 
                  void * glut_font = GLUT_BITMAP_HELVETICA_12 )
{
    ::glTranslatef( x , y , 0 ) ;
    ::glRasterPos2f( 0.f , 0.f ) ;

    for ( size_t pos =  0 ; pos < text.length() ; ++pos )
        ::glutBitmapCharacter( glut_font , text[ pos ] ) ;

    ::glTranslatef( -x , -y , 0 ) ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
double glut_string_width( const ::std::string & text , 
                          void * glut_font = GLUT_BITMAP_HELVETICA_12 )
{
    return ::glutBitmapLength( glut_font , 
                               (const unsigned char*)text.c_str() ) ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void 
init( void )
{
    socket_client.bind( 56565 ) ;
    socket_client.join_multicast_group( "236.10.10.10" ) ;

    ::glClearColor( 0.0 , 0.0 , 0.0 , 0.0 ) ;
    ::glPointSize( 3 ) ;
    ::glEnable( GL_POINT_SMOOTH ) ; 
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void 
resize( int w , int h )
{
    if ( h == 0 ) 
        h = 1 ;

    glViewport( 0 , 0 , w , h ) ;

    screen_width = w ;
    screen_height = h ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void
set_view( void )
{
    float eye_dolly = 10 ; 
    float eye_x = ( ( mouse_x / (float)screen_width ) - 0.5f ) * 2 * PI_DEF ; 
    float eye_y = ( ( mouse_y / (float)screen_height ) - 0.5f ) * 2 * PI_DEF ; 

    eye_position.x = eye_dolly * ::std::cosf( eye_x ) * ::std::cosf( eye_y ) ;
    eye_position.y = eye_dolly * ::std::sinf( eye_y ) ; 
    eye_position.z = eye_dolly * ::std::sinf( eye_x ) * ::std::cosf( eye_y ) ;

    ::glMatrixMode( GL_PROJECTION ) ;
    ::glLoadIdentity() ;
    
    ::gluPerspective( 60.0f , 
                      screen_width / (float)screen_height , 
                      0.1f , 
                      100.0f ) ;
    ::glMatrixMode( GL_MODELVIEW ) ;
    ::glLoadIdentity() ;    
    ::gluLookAt( eye_position.x , eye_position.y , eye_position.z , 
                 0 , 0 , 0 ,
                 0 , 1 , 0 ) ;

}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void 
display( void )
{
    ::glClear( GL_COLOR_BUFFER_BIT | 
               GL_DEPTH_BUFFER_BIT ) ;
    ::glEnable( GL_DEPTH_TEST ) ;
    set_view() ;

    // draw trackers (planets in the provided example)
    ::glColor3f( 1 , 1 , 1 ) ; 

    for ( auto it = recv_trackers.begin() ; it != recv_trackers.end() ; ++it )
    {
        const ::psn::tracker & tracker = it->second ;
        const ::std::string & name = tracker.get_name() ;

        if ( name == "Sun" ) 
            glColor3f( 1 , 1 , .3 ) ; 
        else if ( name == "Mercury" ) 
            glColor3f( .8 , .2 , .2 ) ; 
        else if ( name == "Venus" ) 
            glColor3f( .2 , .8 , .2 ) ; 
        else if ( name == "Earth" ) 
            glColor3f( .2 , .3 , .8 ) ; 
        else if ( name == "Mars" ) 
            glColor3f( .8 , .3 , .1 ) ; 
        else if ( name == "Jupiter" ) 
            glColor3f( .8 , .5 , .1 ) ; 
        else if ( name == "Saturn" ) 
            glColor3f( .8 , .6 , .4 ) ; 
        else if ( name == "Uranus" ) 
            glColor3f( .2 , .7 , .8 ) ; 
        else if ( name == "Neptune" ) 
            glColor3f( .2 , .2 , .9 ) ; 
        else if ( name == "Pluto" ) 
            glColor3f( .6 , .6 , .6 ) ; 

        ::glTranslatef( tracker.get_pos().x , 
                        tracker.get_pos().y , 
                        tracker.get_pos().z ) ;
        ::glutSolidSphere( 0.1 , 10 , 10 ) ; 
        ::glTranslatef( -tracker.get_pos().x , 
                        -tracker.get_pos().y , 
                        -tracker.get_pos().z ) ;
    }

    // draw trackers labels
    {
        double modelview_matrix[ 16 ] ;
        double projection_matrix[ 16 ] ;
        int    viewport[ 4 ] ;
        ::glGetIntegerv( GL_VIEWPORT , 
                         viewport ) ; 
        ::glGetDoublev( GL_MODELVIEW_MATRIX ,
                        modelview_matrix ) ;
        ::glGetDoublev( GL_PROJECTION_MATRIX ,
                        projection_matrix ) ;

        ::glMatrixMode( GL_PROJECTION ) ;
        ::glLoadIdentity() ; 
        ::glOrtho( 0 , screen_width , 
                   0 , screen_height , 
                   0 , 1 ) ; 

        ::glMatrixMode( GL_MODELVIEW ) ;
        ::glLoadIdentity() ; 

        double pos_x , pos_y , pos_z ; 
        for ( auto it = recv_trackers.begin() ; it != recv_trackers.end() ; ++it )
        {
            const ::psn::tracker & tracker = it->second ;

            ::gluProject( (double)tracker.get_pos().x , 
                          (double)tracker.get_pos().y , 
                          (double)tracker.get_pos().z , 

                          modelview_matrix ,
                          projection_matrix ,
                          viewport ,

                          &pos_x , 
                          &pos_y , 
                          &pos_z ) ; 

            if ( pos_z > 0.1 ) 
            {
                glut_string( (float)pos_x + 10 , 
                             (float)pos_y + 10 , 
                             tracker.get_name() ,
                             GLUT_BITMAP_HELVETICA_18 ) ;
            }
        }
    }

    ::glutSwapBuffers() ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void 
idle( void )
{
    // Update PSN client
    {
        ::std::string msg ;
        if ( socket_client.receive_message( msg , ::psn::MAX_UDP_PACKET_SIZE ) )
        {
            psn_decoder.decode( msg.data() , msg.size() ) ;

            static uint8_t last_frame_id = 0 ;
            if ( psn_decoder.get_data().header.frame_id != last_frame_id )
            {
                last_frame_id = psn_decoder.get_data().header.frame_id ;
                recv_trackers = psn_decoder.get_data().trackers ;
            }
        }
    }

    // Periodically print PSN info in the console for debugging
    static int skip_cout = 0 ; 
    if ( skip_cout++ % 300 == 0 )
    {
        ::std::cout << "\n--------------------PSN CLIENT-----------------" ;
        ::std::cout << "\nPSN Server : " << psn_decoder.get_info().system_name ; 
        ::std::cout << "\nFrame Id : " << (int)psn_decoder.get_data().header.frame_id ; 
        ::std::cout << "\nTrackers : " << recv_trackers.size() ; 
        ::std::cout << "\n-----------------------------------------------" << ::std::endl ;
    }

    Sleep( 10 ) ; 

    // schedule GL update
    glutPostRedisplay() ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void 
key( unsigned char k , int x , int y )
{
    if( k == 27 || k == 'q' ) 
        exit( 0 ) ;

    glutPostRedisplay() ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void 
mouse_passive_motion( int x , int y ) 
{
    mouse_x = x ;
    mouse_y = y ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void 
special( int k , int x , int y )
{
    glutPostRedisplay() ;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
int 
main( int argc , char **argv )
{
    glutInit( &argc , argv ) ;
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB ) ;

    screen_width = 640 ;
    screen_height = 480 ;
    glutInitWindowSize( screen_width , 
                        screen_height ) ;
    glutCreateWindow( "PSN Viewer example - Copyright VYV Corporation 2014" ) ;

    glutReshapeFunc( resize ) ;
    glutDisplayFunc( display ) ;
    glutKeyboardFunc( key ) ;
    glutPassiveMotionFunc( mouse_passive_motion ) ; 
    
    glutIdleFunc( idle ) ;

    init() ;

    glutMainLoop() ;

    return 0 ;
}
