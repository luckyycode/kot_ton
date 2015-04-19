//
//  main.cpp
//  kot_ton
//
//  Created by Neko Code on 4/17/15.
//  Copyright (c) 2015 nekocode. All rights reserved.
//

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
/*static*/ const wchar_t EN_FAL = L'a';    // English.
/*static*/ const wchar_t EN_LAL = L'z';

/*static*/ const wchar_t RU_FAL = L'а';    // Russian.
/*static*/ const wchar_t RU_LAL = L'я';

// Dictionary file names.
/*static*/ const char * DEF_DICT_RUSSIAN = "ru_dict.txt";
/*static*/ const char * DEF_DICT_ENGLISH = "en_dict.txt";

/*static*/ const char * DEF_SRC_RUSSIAN = "ru_source.txt";
/*static*/ const char * DEF_SRC_ENGLISH = "en_source.txt";

/*static*/ const char * LOC_CHOOSE_DICT = "Введите путь и имя файла который содержит словарь: ";
/*static*/ const char * LOC_CHOOSE_WORDS = "Введите путь и имя файла который содержит исходное и конечное слово: ";
/*static*/ const char * LOC_CHOOSE_LANGUAGE = "Русский(0) или Английский(1)?";
/*static*/ const char * LOC_AVAILABLE_WORDS = "Words found in the dictionary:";

/*static*/ const char * LOCALIZATION = "ru_RU.UTF-8";

/*static*/ const char * LOC_LANGUAGE_RUSSIAN = "Язык: Русский.";
/*static*/ const char * LOC_LANGUAGE_ENGLISH = "Language: English.";

//
//  Error.
void com_error( const char * err );

//
//  Wait for user input.
void com_pause();

//
//  String to wide string.
std::wstring to_wstr( std::string & s );

int main( int argc, char ** argv ) {

    //
    // Set locale.
    std::locale rus( LOCALIZATION ); // Independ ( rus/eng ).
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
    
    int ar_size = 1, total_words = /* start */ 1, /* temp = */ is_eng = 0;
    wchar_t prev, preferred_fal = 0, preferred_lal = 0;
    
    //
    // Language chooser.
    std::cout << LOC_CHOOSE_LANGUAGE << "\n";
    fscanf( stdin, "%i", &is_eng );
    if( is_eng == 0 ) {
        std::cout << LOC_LANGUAGE_RUSSIAN << "\n";
        preferred_fal = RU_FAL;
        preferred_lal = RU_LAL;
    }
    else {
        std::cout << LOC_LANGUAGE_ENGLISH << "\n";
        preferred_fal = EN_FAL;
        preferred_lal = EN_LAL;
    }
    
    std::cout << LOC_CHOOSE_WORDS << "\n";
    std::cin >> word_file;
    std::cout << LOC_CHOOSE_DICT << "\n";
    std::cin >> dict_file;
    
    //dict.clear();
    
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
    // Set UTF-8 text data.
    wide_stream_words.imbue( rus );
    wide_stream_dict.imbue( rus );
    
    //
    // Read data.
    std::wstring wbuff;
    
    std::cout << LOC_AVAILABLE_WORDS << "\n";
    while( getline( wide_stream_dict, wbuff ) ) { // td: free
        dict.insert( wbuff );
        
        std::wcout << wbuff << "\n";
    }
    
    if( dict.size() <= 0 ) {
        com_error( "No words found in the dictionary." );
        return -1;
    }
    
    wide_stream_dict.close();
    
    // td: portable
    if( !getline( wide_stream_words, start ) ) { // td: free
        com_error( "Couldn't get begin word from source file.." );
        return -1;
    }
    
    if( !getline( wide_stream_words, end ) ) { // td: free
        com_error( "Couldn't get end word from source file..\n" );
        return -1;
    }
    
    wide_stream_words.close();
    
    if( start.size() != end.size() ) { // strlen
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
    
    do { // Do not finish because we need last word.
        while( total_words > 0 ) {
            
            --total_words;
            
            last_str = word_data.front();
            prev_str = last_str;
          
            // if( total_words <= 2  )
            word_data.pop();
            
            for( int i = 0; i < last_str.size(); ++i ) {
            //for( wchar_t i : last_str ) {
                prev = last_str[i];
                // wchar_t
                for( auto o = preferred_fal; o <= preferred_lal; ++o ) {
                    
                    //wcout << o << " | " << prev << endl;
                    if( o == prev ) {
                        //wcout << "Skipping.. ( " << o << " = " << prev << " ) " << endl;
                        continue;
                    }
                    
                    last_str[i] = o;

                    // Our string is a last string.
                    if( last_str == end ) {

                        std::wcout << prev_str << "\n";
                        std::wcout << end << "\n\n\n";
                        
                        printf( "TOTAL CHARACTER CHANGES: %i\n", ar_size + 1 );
                        
                        com_pause();
                        
                        return ar_size + 1;
                    }
                    
                    // Skip found words.
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
    std::cout << err << "\n";

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
