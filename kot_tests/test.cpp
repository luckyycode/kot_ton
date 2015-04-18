//
//  main.cpp
//  kot_tests
//
//  Created by Neko Code on 4/18/15.
//  Copyright (c) 2015 nekocode. All rights reserved.
//

#define CATCH_CONFIG_MAIN  
#include "catch.hpp"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <queue>
#include <map>
#include <locale>
#include <wchar.h>

// wchar_t - utf-8s
const wchar_t EN_FAL = L'a';    // English.
const wchar_t EN_LAL = L'z';

const wchar_t RU_FAL = L'а';    // Russian.
const wchar_t RU_LAL = L'я';


TEST_CASE( "Main application" ) {
    
    std::locale rus( "ru_RU.UTF-8" ); // Independ ( rus/eng ).
    
    INFO( "Checking ru_RU/UTF8 support.. " );
    
    // Check locales.
    REQUIRE( !strcmp( setlocale( LC_ALL, "ru_RU.UTF-8"), "ru_RU.UTF-8" ) );
    
    std::wcout.imbue( rus ); // Wide cout locale.

    // Test character codes.
    CHECK( (int)RU_FAL == 1072 );
    CHECK( (int)RU_LAL == 1103 );
    
    // - -----
    std::wifstream wide_stream_words; // UTF-8.
    std::wifstream wide_stream_dict;
    std::unordered_set<std::wstring> dict;
    std::queue<std::wstring> word_data;
    std::map<std::wstring, bool> last_seen;
    
    std::wstring last_str, prev_str, start, end;
    std::string word_file, dict_file;
    
    int ar_size = 1, total_words = /* start */ 1, /* temp = */ is_eng = 0;
    wchar_t prev, preferred_fal = 0, preferred_lal = 0;
    // - -----
    
    //
    // Language chooser.
    INFO( "User input testing.. " );
    
    std::cout << "Русский(0) или Английский(1)?" << "\n";
    
    REQUIRE( fscanf( stdin, "%i", &is_eng ) );
    
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
    
    INFO( "Getting file names.. " );
    std::cout << "Введите путь и имя файла который содержит исходное и конечное слово: " << "\n";
    std::cin >> word_file;
    std::cout << "Введите путь и имя файла который содержит словарь: " << "\n";
    std::cin >> dict_file;
    
    REQUIRE( word_file.c_str() ); // != 0  ( strlen )
    REQUIRE( dict_file.c_str() ); // != 0  ( strlen )

    //
    // Words.
    printf( "Dictionary: %s\n", word_file.c_str() );
    printf( "Words: %s\n", dict_file.c_str() );

    wide_stream_words.open( word_file.c_str() );
    REQUIRE( wide_stream_words.is_open() );

    wide_stream_dict.open( dict_file.c_str() );
    REQUIRE( wide_stream_dict.is_open() );
    
    //
    // UTF-8 text data.
    wide_stream_words.imbue( rus );
    wide_stream_dict.imbue( rus );
    
    INFO( "Checking file stream locale setup..\n ");
    REQUIRE( wide_stream_dict.getloc() == rus );
    REQUIRE( wide_stream_words.getloc() == rus );
    
    //
    // Read data.
    std::wstring wbuff;
    while( getline( wide_stream_dict, wbuff ) ) { // td: free
        dict.insert( wbuff );
        //std::wcout << wbuff << "\n";
    }
    
    INFO( "Checking dictionary size..\n" );
    REQUIRE( dict.size() > 0 );
    
    wide_stream_dict.close();
    
    INFO( "Reading words from file.." );
    SECTION( "Reading source file words" ) {
        CHECK( getline( wide_stream_words, start ) );
        CHECK( getline( wide_stream_words, end ) );
    }
    
    wide_stream_words.close();
    
    INFO( "Checking file lengths.." );
    
    REQUIRE( start.size() != 0 );
    REQUIRE( end.size() != 0 );
    
    REQUIRE( start.size() == end.size() );

    word_data.push( start );
    last_seen[start] = true; // Already checked.
    
    // Notify.
    std::wcout << "\nBegin: " << start << "\tDestination: " << end << "\n"; //wprintf( L"Begin: %ls\tDestination: %ls\n", start.c_str(), end.c_str() );
    printf( "- --\n" );
    
    //SECTION("main stuff" ) {
    do {
        while( total_words > 0 ) {
            
            --total_words;
            
            last_str = word_data.front();
            prev_str = last_str;
            
            //REQUIRE( last_str.c_str() );

            word_data.pop();
            
            for( int i = 0; i < last_str.size(); ++i ) {
                prev = last_str[i];
                
                for( auto o = preferred_fal; o <= preferred_lal; ++o ) {
                    
                    if( o == prev )
                        continue;
                    
                    last_str[i] = o;
                    
                    if( last_str == end ) {
                        INFO( "Last stage.." );
                        REQUIRE( prev_str.c_str() );
                        
                        std::wcout << prev_str << "\n";
                        std::wcout << end << "\n\n\n";
                        printf( "TOTAL CHARACTER CHANGES: %i\n", ar_size + 1 );

                        return;
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
        
        CHECK( total_words == (int)word_data.size() );
        
        ++ar_size;
        
        std::wcout << last_str << "\n"; //wprintf( L"%ls\n", last_str.c_str() );

    } while ( total_words > 0 );
    //}
    
    std::wcout << end << "\n";

}