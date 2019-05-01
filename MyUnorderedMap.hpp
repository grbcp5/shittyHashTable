#include "MyUnorderedMap.h"
#include <algorithm>
#include <typeinfo>


void get_identity(std::string &my_id) {
  my_id = "ewp6xc";
}


template <typename K, typename V>
struct Tombstone: public MyPair<K, V> {
};


template <typename K, typename V>
struct EmptyPair: public MyPair<K, V> {
};

template <typename K, typename V>
bool isTombstone(const MyPair<K, V> &myPair) {
  
  try {
    dynamic_cast< Tombstone<K, V> >(myPair);
    return true;
  } catch( std::bad_cast &bc) {
    return false;
  }
}



template <typename K, typename V>
bool isEmptyPair(const MyPair<K, V> &myPair) {
  
  try {
    dynamic_cast< EmptyPair<K, V> >(myPair);
    return true;
  } catch( std::bad_cast &bc) {
    return false;
  }
}


template <typename K, typename V>
int getIndex( const MyPair<K, V> *arr, int arrSize, int home, int key ) {

  int i = home;
  
  while( isTombstone( arr[i] )
          ||
           ( !isEmptyPair( arr[i] )
              && arr[ i ].first != key
            )
       ) {
    i = (i + 1) % arrSize;
  }
  
  return i;
}

template <typename K, typename V>
int getFirstOpenForFind( const MyPair<K, V> *arr, int arrSize, int home, int key ) {
  
  int i = home;
  
  while( !isEmptyPair( arr[ i ] ) && !isTombstone( arr[ i ] ) ) {
    i = (i + 1) % arrSize;
  }
  
  return i;
}

template <typename K, typename V>
int MyUnorderedMap<K,V>::hash(const K &key) const {

    int hash = 7;

    for (int i = 0; i < key.length(); i++) {
        hash = ( hash * 31 + key[i] ) % this->reserved_size;
    }

    return hash;
}

template< typename K, typename V >
MyUnorderedMap< K, V >::MyUnorderedMap() {

  this->data_size = 0;
  this->reserved_size = 0;
  this->m_data = nullptr;
  
}

template< typename K, typename V >
MyUnorderedMap< K, V >::~MyUnorderedMap() {
  
  delete m_data;
  this->data_size = 0;
  this->reserved_size = 0;
  
}

template< typename K, typename V >
MyUnorderedMap< K, V >::MyUnorderedMap( const MyUnorderedMap< K, V > &source ) {
  
  this->m_data = new MyPair<K, V>[ source.reserved_size ];
  this->reserved_size = source.reserved_size;
  this->data_size = source.data_size;
  
  for ( int i = 0; i < source.reserved_size; ++i ) {
    this->m_data[i] = source.m_data[i];
  }
}

template< typename K, typename V >
MyUnorderedMap< K, V > &
MyUnorderedMap< K, V >::operator=( const MyUnorderedMap< K, V > &source ) {
  
  if ( this == &source ) {
    return *this;
  }
  
  delete this->m_data;
  
  this->m_data = new MyPair<K, V>[ source.reserved_size ];
  this->reserved_size = source.reserved_size;
  this->data_size = source.data_size;
  
  for ( int i = 0; i < source.reserved_size; ++i ) {
    this->m_data[i] = source.m_data[i];
  }
  
  return *this;
}

template< typename K, typename V >
V &MyUnorderedMap< K, V >::at( const K &key ) {
  
  int i = getIndex( this->m_data, reserved_size, this->hash( key ), key );
  
  if( isEmptyPair( this->m_data[ i ] ) ) {
    
    i = getFirstOpenForFind(
      this->m_data,
      reserved_size,
      this->hash( key ),
      key
    );
    this->m_data[ i ] = MyPair<K, V>(key);
    
    this->data_size++;
    // todo: reserve
  }
  
  return this->m_data[ i ].second;
}

template< typename K, typename V >
V &MyUnorderedMap< K, V >::operator[]( const K &key ) {
  return this->at( key );
}

template< typename K, typename V >
bool MyUnorderedMap< K, V >::empty() const {
  return this->data_size == 0;
}

template< typename K, typename V >
int MyUnorderedMap< K, V >::size() const {
  return this->data_size;
}

template< typename K, typename V >
void MyUnorderedMap< K, V >::clear() {
  
//  for ( int i = 0; i < reserved_size; ++i ) {
//    this->m_data[i] = EmptyPair<K, V>();
//  }
//  data_size = 0;
//   todo: reserve?
  
  // ^
  // Either do that up there or...
  delete this->m_data;
  this->reserved_size = 0;
  this->data_size = 0;
}

template< typename K, typename V >
void MyUnorderedMap< K, V >::insert( const MyPair< K, V > &init_pair ) {
  
  int i = getFirstOpenForFind(
    this->m_data,
    reserved_size,
    this->hash( init_pair.first ),
    init_pair.first
  );
  
  this->m_data[ i ] = MyPair<K, V>( init_pair.first, init_pair.second );
  this->data_size++;
  
  // todo: reserve
}

template< typename K, typename V >
void MyUnorderedMap< K, V >::erase( const K &key ) {
  
  int i = getIndex( this->m_data, reserved_size, this->hash( key ), key );
  
  if( isEmptyPair( this->m_data[ i ] ) ) {
    return;
  }
  
  this->m_data[ i ] = Tombstone<K, V>();
  this->data_size--;
  
  // todo: reserve
}

template< typename K, typename V >
MyPair< K, V > *MyUnorderedMap< K, V >::find( const K &key ) const {

  int i = getIndex(this->m_data, reserved_size, this->hash( key ), key );
  
  if ( isEmptyPair( this->m_data[ i ] ) ) {
    return nullptr;
  } else {
    return &this->m_data[ i ];
  }
}

template< typename K, typename V >
void MyUnorderedMap< K, V >::print() const {

  cout << "(";
  
  int lastNonEmptyIdx = -1;
  int firstNonEmptyIdx = -1;
  
  for ( int i = 0; i < this->reserved_size; ++i ) {
    
    if( firstNonEmptyIdx == -1 && !isEmptyPair( this->m_data[ i ] ) &&
                                  !isTombstone( this->m_data[ i ] )) {
      firstNonEmptyIdx = i;
    }
    
    
    if ( !isEmptyPair( this->m_data[ i ] ) &&
         !isTombstone( this->m_data[ i ] )) {
      lastNonEmptyIdx = i;
    }
  }
  
  if ( lastNonEmptyIdx == -1 ) {
    cout << ")" << endl;
    return;
  }
  
  for ( int i = 0; i < this->reserved_size; ++i ) {
    
    if( !isEmptyPair( this->m_data[ i ] ) && !isTombstone( this->m_data[ i ] ) ) {
      
      if( i != firstNonEmptyIdx) {
        cout << " ";
      }
      
      cout << "[" << this->m_data->first << "]=" << this->m_data[ i ].second;
      
      if ( lastNonEmptyIdx == i ) {
        cout << ")" << endl;
        return;
      } else {
        cout << "," << endl;
      }
      
    }
  }
}

template< typename K, typename V >
int MyUnorderedMap< K, V >::count( const K &key ) const {

  int i = getIndex( this->m_data, reserved_size, this->hash( key ), key );
  
  return ( isEmptyPair( this->m_data[i] ) ? 0 : 1 );
}

template< typename K, typename V >
void MyUnorderedMap< K, V >::reserve( int new_cap ) {

  MyPair<K, V> *oldTable = this->m_data;
  
  m_data = new EmptyPair<K, V>[new_cap];
  
  for ( int i = 0; i < this->reserved_size; ++i ) {
    if( !isEmptyPair( oldTable[ i ] )
        && !isTombstone( oldTable[ i ] ) ) {
      this->insert( oldTable[ i ] );
    }
  }
  
  delete( oldTable );
  
  this->reserved_size = new_cap;
}
