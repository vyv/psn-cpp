//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/*
The MIT License (MIT)

Copyright (c) 2014 VYV Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef PSN_DEFS_HPP
#define PSN_DEFS_HPP

#include <stdint.h>
#include <map>
#include <vector>
#include <string>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace psn
{

// PSN VERSION
#define PSN_HIGH_VERSION				2
#define PSN_LOW_VERSION					0

// DEFAULT UDP PORT and MULTICAST ADDRESS
#define PSN_DEFAULT_UDP_PORT			56565
#define PSN_DEFAULT_UDP_MULTICAST_ADDR	"236.10.10.10"

// MAX UDP PACKET SIZE
#define PSN_MAX_UDP_PACKET_SIZE			1500

// - PSN INFO -
#define PSN_INFO_PACKET					0x6756
#define PSN_INFO_PACKET_HEADER				0x0000
#define PSN_INFO_SYSTEM_NAME				0x0001
#define PSN_INFO_TRACKER_LIST				0x0002
//#define PSN_INFO_TRACKER						tracker_id
#define PSN_INFO_TRACKER_NAME						0x0000

// - PSN DATA -
#define PSN_DATA_PACKET					0x6755
#define PSN_DATA_PACKET_HEADER				0x0000
#define PSN_DATA_TRACKER_LIST				0x0001
//#define PSN_DATA_TRACKER						tracker_id
#define PSN_DATA_TRACKER_POS						0x0000
#define PSN_DATA_TRACKER_SPEED						0x0001
#define PSN_DATA_TRACKER_ORI						0x0002
#define PSN_DATA_TRACKER_STATUS						0x0003

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
struct float3
{
	float3( float px = 0 , float py = 0 , float pz = 0 ) 
		: x( px ) , y( py ) , z( pz )
	{}

	float x , y , z ;
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
struct psn_tracker
{
	psn_tracker( uint16_t id = 0 , 
				 const ::std::string & name = "" , 
				 float3 pos = float3() , 
				 float3 speed = float3() , 
				 float3 ori = float3() , 
				 float validity = 0.f )
		: id_( id )
		, name_( name )
		, pos_( pos )
		, speed_( speed ) 
		, ori_( ori )
		, validity_( validity )
	{}

	uint16_t id_ ;

	::std::string name_ ;

	float3 pos_ ;
	float3 speed_ ;
	float3 ori_ ;

	float validity_ ;
} ;

typedef ::std::map< uint16_t , psn_tracker > psn_tracker_array ; // map< id , psn_tracker >

} // namespace psn

#endif
