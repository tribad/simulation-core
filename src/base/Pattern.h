// **************************************************************************
//
// Modul-Name        : Pattern.h
//  Copyrights by Hans-Juergen Lange. All rights reserved.
//
// **************************************************************************
#ifndef __PATTERN_INC__
#define __PATTERN_INC__
#include <string>
// **************************************************************************
//                 F u n c t i o n   P r o t o t y p e s
// **************************************************************************
bool PatternMatch(const std::string& aPattern, const std::string& aString);
bool PatternMatch(const char* aPattern, const std::string& aString);
bool PatternMatch(const std::string& aPattern, const char* aString);
bool PatternMatch(const char* aPattern, const char* aString);

#endif // __Util_INC__
