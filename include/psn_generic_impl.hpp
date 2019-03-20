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

#ifndef PSN_GENERIC_IMPL_HPP
#define PSN_GENERIC_IMPL_HPP

#include <psn_generic.hpp>

namespace psn 
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_chunk::
psn_chunk( uint16_t id , psn_chunk_data * data /*= 0*/ , const ::std::string & name /*= ""*/ )
	: parent_( 0 )
	, id_( id )
	, name_( name ) 
	, data_( data )
{
	header_.bits_ = 0 ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_chunk::
~psn_chunk( void )
{
	// delete data
	if ( data_ != 0 )
	{
		delete data_ ;
		data_ = 0 ;
	}

	// Remove from parent
	if ( parent_ != 0 )
	{
		parent_->remove_child( this , false ) ;
	}

	// delete children
	remove_all_children( true ) ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
uint16_t 
psn_chunk::
get_id( void ) const 
{ 
	return id_ ; 
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void
psn_chunk::
set_name( const ::std::string & name ) 
{ 
	name_ = name ; 
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::std::string 
psn_chunk::
get_name( void ) const 
{
	return name_ ; 
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
size_t
psn_chunk::
get_chunk_size( void ) const
{
	size_t chunk_size = 0 ;

	chunk_size += SIZE_OF_PSN_CHUNK_HEADER ;

	if ( data_ != 0 )
		chunk_size += data_->get_data_size() ;

	for ( size_t i = 0 ; i < children_.size() ; ++i )
	{
		chunk_size += children_[ i ]->get_chunk_size() ;
	}

	return chunk_size ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_chunk_data *
psn_chunk::
get_data( void ) const
{
	return data_ ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
encode( void * buffer , size_t buffer_size ) 
{
	size_t data_size = get_chunk_size() ;

	if ( buffer_size < data_size )
		return false ;

	// encode header
	header_.id_ = id_ ;
	header_.data_len_ = data_size - SIZE_OF_PSN_CHUNK_HEADER ;
	header_.has_subchunks_ = ( children_.empty() ? 0 : 1 ) ;

	memcpy( buffer , &header_ , SIZE_OF_PSN_CHUNK_HEADER ) ;

	// seek through buffer 
	buffer = (char *)buffer + SIZE_OF_PSN_CHUNK_HEADER ;
	buffer_size -= SIZE_OF_PSN_CHUNK_HEADER ;

	// encode data only if we don't have children
	if ( data_ != 0 && children_.empty() )
	{
		data_size = data_->get_data_size() ;

		if ( buffer_size < data_size )
			return false ;

		if ( data_size > 0  )
		{
			data_->write_buffer( buffer ) ;

			// seek through buffer 
			buffer = (char *)buffer + data_size ;
			buffer_size -= data_size ;
		}
	}

	// encode children
	for ( size_t i = 0 ; i < children_.size() ; ++i )
	{
		data_size = children_[ i ]->get_chunk_size() ;

		if ( buffer_size < data_size )
			return false ;

		if ( !children_[ i ]->encode( buffer , buffer_size ) )
			return false ;		

		// seek through buffer 
		buffer = (char *)buffer + data_size ;
		buffer_size -= data_size ;
	}

	return true ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
decode( void * buffer , size_t buffer_size ) 
{
	if ( buffer_size < SIZE_OF_PSN_CHUNK_HEADER )
		return false ;

	// decode header
	memcpy( &header_ , buffer , SIZE_OF_PSN_CHUNK_HEADER ) ;

	if ( header_.id_ != id_ || buffer_size < header_.data_len_ )
		return false ;

	// seek through buffer 
	buffer = (char *)buffer + SIZE_OF_PSN_CHUNK_HEADER ;
	buffer_size -= SIZE_OF_PSN_CHUNK_HEADER ;

	// decode children or data
	bool result = true ;

	if ( header_.has_subchunks_ )
	{
		while ( buffer_size >= SIZE_OF_PSN_CHUNK_HEADER && result )
		{
			psn_chunk_header child_header ;

			// decode child header
			memcpy( &child_header , buffer , SIZE_OF_PSN_CHUNK_HEADER ) ;

			// decode child
			result = decode_child( child_header.id_ , buffer , buffer_size ) ;

			// seek through buffer 
			if ( result )
			{
				buffer = (char *)buffer + SIZE_OF_PSN_CHUNK_HEADER + child_header.data_len_ ;
				buffer_size -= SIZE_OF_PSN_CHUNK_HEADER + child_header.data_len_ ;
			}
		}
	}
	else
	{
		// Decode chunk data
		if ( data_ != 0 )
			data_->read_buffer( buffer , header_.data_len_ ) ;
	}

	return result ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
decode_child( uint16_t child_id , void * buffer , size_t buffer_size )
{
	psn_chunk * child = get_child_by_id( child_id ) ;

	if ( child != 0 )
	{
		return child->decode( buffer , buffer_size ) ;
	}

	return true ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_chunk * 
psn_chunk::
get_parent( void ) const
{
	return parent_ ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void
psn_chunk::
add_child( psn_chunk * child ) 
{
	if ( child == 0 )
		return ;

	// If it's already a child, do not add it again
	// Also check for infinite loop
	if ( !find_child( child ) && !this->find_parent( child ) ) 
	{
		if ( child->parent_ != 0 )
			child->parent_->remove_child( child , false ) ;

		child->parent_ = this ;
		children_.push_back( child ) ;
	}
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void
psn_chunk::
insert_child( int index , psn_chunk * child ) 
{
	if ( child == 0 )
		return ;

	// If it's already a child, do not add it again
	// Also check for infinite loop
	if ( !find_child( child ) && !this->find_parent( child ) ) 
	{
		if ( child->parent_ != 0 )
			child->parent_->remove_child( child , false ) ;

		child->parent_ = this ;

		if ( index >= 0 && index < (int)children_.size() )
			children_.insert( children_.begin() + index , child ) ;
		else
			children_.push_back( child ) ;
	}
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
remove_child_by_index( int index , bool auto_delete /*= true*/ ) 
{
	if ( index >= 0 && index < (int)children_.size() )
	{
		if ( auto_delete )
			delete children_[ index ] ;
		else 
		{
			children_[ index ]->parent_ = 0 ;
			children_.erase( children_.begin() + index ) ;
		}
		return true ;
	}

	return false ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
remove_child_by_id( uint16_t id , bool auto_delete /*= true*/ ) 
{
	for ( size_t i = 0 ; i < children_.size() ; ++i )
	{
		if ( children_[ i ]->get_id() == id )
		{
			if ( auto_delete )
				delete children_[ i ] ;
			else 
			{
				children_[ i ]->parent_ = 0 ;
				children_.erase( children_.begin() + i ) ;
			}
			return true ;
		}
	}

	return false ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
remove_child( psn_chunk * child , bool auto_delete /*= true*/ ) 
{
	for ( size_t i = 0 ; i < children_.size() ; ++i )
	{
		if ( children_[ i ] == child )
		{
			if ( auto_delete )
				delete children_[ i ] ;
			else 
			{
				children_[ i ]->parent_ = 0 ;
				children_.erase( children_.begin() + i ) ;
			}
			return true ;
		}
	}

	return false ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void
psn_chunk::
remove_all_children(  bool auto_delete /*= true*/ ) 
{
	if ( auto_delete )
	{
		while ( !children_.empty() )
		{
			delete children_[ 0 ] ;
		}
	}

	for ( size_t i = 0 ; i < children_.size() ; ++i )
	{
		children_[ i ]->parent_ = 0 ;
	}

	children_.clear() ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_chunk * 
psn_chunk::
get_child_by_id( uint16_t id )
{
	for ( size_t i = 0 ; i < children_.size() ; ++i )
	{
		if ( children_[ i ]->get_id() == id )
		{
			return children_[ i ] ;
		}
	}

	return 0 ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::std::vector< psn_chunk * >
psn_chunk::
get_children( void )
{ 
	return children_ ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
find_child( psn_chunk * child ) 
{
	if ( child == 0 )
		return false ;

	for ( size_t i = 0 ; i < children_.size() ; ++i )
	{
		if ( child == children_[ i ] )
			return true ;

		if ( children_[ i ]->find_child( child ) )
			return true ;
	}

	return false ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_chunk::
find_parent( psn_chunk * parent ) 
{
	if ( parent == 0 )
		return false ;

	psn_chunk * next_parent = get_parent() ;

	while ( next_parent != 0 )
	{
		if ( next_parent == parent )
			return true ;

		next_parent = next_parent->get_parent() ;
	}

	return false ;
}

} // namespace psn

#endif