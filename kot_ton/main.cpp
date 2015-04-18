//
//  main.cpp
//  kot_ton
//
//  Created by Neko Code on 4/17/15.
//  Copyright (c) 2015 nekocode. All rights reserved.
//

#include <stdlib.h>
#include <iostream> // wcout, locale
#include <fstream>  // wofstream ( wide )
#include <string>
#include <unordered_set>
#include <queue>
#include <map>
//#include <codecvt>  // utf16/utf8
#include <locale>   // ru_ru
#include <wchar.h>  // wchar_t for wide char type

// wchar_t - utf-8s
const wchar_t EN_FAL = L'a';    // English.
const wchar_t EN_LAL = L'z';

const wchar_t RU_FAL = L'а';    // Russian.
const wchar_t RU_LAL = L'я';

// Dictionary file names.
const char * DEF_DICT_RUSSIAN = "ru_dict.txt";
const char * DEF_DICT_ENGLISH = "en_dict.txt";

const char * DEF_SRC_RUSSIAN = "ru_source.txt";
const char * DEF_SRC_ENGLISH = "en_source.txt";

//
//  Error.
void com_error( const char * err );

//
//  Wait for user input.
void com_pause();

//
//  Unicode char to UTF8 string.
const wchar_t * unicode_to_utf8( wchar_t c );

//
//  String to wide string.
std::wstring to_wstr( std::string & s );

int main( int argc, char ** argv ) {

    //
    // Set locale.
    std::locale rus( "ru_RU.UTF-8" ); // Independ ( rus/eng ).
    std::wcout.imbue( rus ); // Wide cout locale.

    //const char *funnything = "а А";
    //const char *funnything2 = "бвг я Я";
    //wchar_t t1 = L'я';
    //wchar_t t2 = L'а';
    //printf( "\n%i %i\n", 'a', 'z' );
    
    //
    // Text data.
    //FILE * source_words = NULL;
    //FILE * source_dict = NULL;
    std::wifstream wide_stream_words; // UTF-8.
    std::wifstream wide_stream_dict;
    
    //
    // Default word dictionary data, read from file later.
    std::unordered_set<std::wstring> dict;
    std::queue<std::wstring> word_data;
    std::map<std::wstring, bool> last_seen;
    
    std::wstring last_str, prev_str, start, end;
    std::string word_file, dict_file;
    
    int ar_size = 1, ar_prev = 1, total_words = /* start */ 1, /* temp = */ is_eng = 0;
    wchar_t prev, preferred_fal = 0, preferred_lal = 0;
    
    //
    // Language chooser.
    std::cout << "Русский(0) или Английский(1)?" << "\n";
    fscanf( stdin, "%i", &is_eng );
    if( is_eng == 0 ) {
        std::cout << "Язык: Русский." << "\n";
        preferred_fal = RU_FAL;
        preferred_lal = RU_LAL;
    }
    else {
        std::cout << "Language: English." << "\n";
        preferred_fal = EN_FAL;
        preferred_lal = EN_LAL;
    }
    
    std::cout << "Введите путь и имя файла который содержит исходное и конечное слово: " << "\n";
    std::cin >> word_file;
    std::cout << "Введите путь и имя файла который содержит словарь: " << "\n";
    std::cin >> dict_file;
    
    dict.clear();
    
    //
    // Words.
    printf( "Dictionary: %s\n", word_file.c_str() );
    printf( "Words: %s\n", dict_file.c_str() );
    
    wide_stream_words.open( word_file.c_str() );
    if( !wide_stream_words.is_open() ) {
        com_error( "Couldn't find source word data.. \n" );
        return -1;
    }

    //
    // Fill our dictionary.
    wide_stream_dict.open( dict_file.c_str() );
    if( !wide_stream_dict.is_open() ) {
        com_error( "Couldn't find dictionary data...\n" );
        return -1;
    }
    
    //
    // UTF-8 text data.
    wide_stream_words.imbue( rus );
    wide_stream_dict.imbue( rus );
    
    //
    // Read data.
    std::wstring wbuff;
    while( getline( wide_stream_dict, wbuff ) ) {
        dict.insert( wbuff );
        std::wcout << wbuff << "\n";
    }
    
    wide_stream_dict.close();
    
    getline( wide_stream_words, start );
    getline( wide_stream_words, end );
    
    wide_stream_words.close();
    
    if( start.size() != end.size() ) {
        com_error( "Begin and destination words must be the same length.\n" );
        return -1;
    }
    
    //start = L"кот"; end = L"тон";

    // Begin(first) word.
    word_data.push( start );
    last_seen[start] = true; // Already checked.
    
    // Notify.
    std::wcout << "\nBegin: " << start << "\tDestination: " << end << "\n"; //wprintf( L"Begin: %ls\tDestination: %ls\n", start.c_str(), end.c_str() );
    printf( "- --\n" );
    
    do {
        while( total_words > 0 ) {
            
            --total_words;
            
            last_str = word_data.front();
            prev_str = last_str;
          
            // if( total_words <= 2  )
            word_data.pop();
            
            for( int i = 0; i < last_str.size(); ++i ) {
            //for( wchar_t i : last_str ) {
                prev = last_str[i];
 
                for( auto o = preferred_fal; o <= preferred_lal; ++o ) {
                    
                    //wcout << o << " | " << prev << endl;
                    if( o == prev ) {
                        //wcout << "Skipping.. ( " << o << " = " << prev << " ) " << endl;
                        continue;
                    }
                    
                    last_str[i] = o;

                    if( last_str == end ) {

                        std::wcout << prev_str << "\n"; //wprintf( L"%ls\n", last_str.c_str() );
                        std::wcout << end << "\n\n\n"; //printf( "- %ls \n", end.c_str() );
                        printf( "TOTAL CHARACTER CHANGES: %i\n", ar_size + 1 );
                        
                        com_pause();
                        return ar_size + 1;
                    }
                    
                    if( dict.find( last_str ) != dict.end() && last_seen.find( last_str ) == last_seen.end() ) {
                        word_data.push( last_str );
                        last_seen[last_str] = true;
                    }
                }
            
                last_str[i] = prev;
                prev_str = last_str;
            }
        }
        
        total_words = (int)word_data.size(); // td: make c++ cast
        ar_prev = ar_size;
        
        ++ar_size;
        
        std::wcout << last_str << "\n"; //wprintf( L"%ls\n", last_str.c_str() );

        
    } while ( total_words > 0 );

    std::wcout << end << "\n";
    //printf( "%s \n", end.c_str() );
    com_pause();
    
    return 0;
}

/**
 *  Error message.
 */
void com_error( const char * err ) {
    std::wcout << err << "\n";

    com_pause();
}

/**
 *  Pause.
 */
void com_pause() {
#ifdef _WIN32
    system( "pause" );
#elif __APPLE__
    system( "read -n 1 -s -p \"Press any key to continue...\n\"" );
#else 
    std::cin.ignore();
    std::cin.get();
#endif
    
    printf( "\n" );
}

/**
 *  Temp.
 */
std::wstring to_wstr( std::string & s ) {
    std::wstring temp( s.length(), L' ' );
    std::copy( s.begin(), s.end(), temp.begin() );
    return temp;
    
}