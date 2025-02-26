/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkStringTools.h"

#include <algorithm> // std::for_each()
#include <cctype>    // std::toupper(), std::tolower()

namespace itk
{

/////////////////////////////////////////////////////////////////////////////
// helper functions for string manipulations
/////////////////////////////////////////////////////////////////////////////

/** Method to trim the spaces or user-specified characters on both ends of a string. */
std::string &
StringTools::Trim(std::string & str, const std::string & dislike)
{
  return TrimRight(TrimLeft(str, dislike), dislike);
}

/** Method to trim the spaces or user-specified characters on left end of a string. */
std::string &
StringTools::TrimLeft(std::string & str, const std::string & dislike)
{
  if (!str.empty())
  {
    const auto pos = str.find_first_not_of(dislike);
    if (pos != std::string::npos)
    {
      if (pos > 0)
      {
        str.erase(0, pos);
      }
    }
    else
    {
      str.clear();
    }
  }
  return str;
}

/** Method to trim the spaces or user-specified characters on right end of a string. */
std::string &
StringTools::TrimRight(std::string & str, const std::string & dislike)
{
  if (!str.empty())
  {
    auto pos = str.find_last_not_of(dislike);
    if (pos != std::string::npos)
    {
      if ((++pos) < str.size())
      {
        str.erase(pos);
      }
    }
    else
    {
      str.clear();
    }
  }

  return str;
}

/** Method to convert lower-case characters to upper cases in a string. */
std::string &
StringTools::ToUpperCase(std::string & str)
{
  // https://en.cppreference.com/w/cpp/string/byte/toupper with std algorithms
  std::for_each(str.begin(), str.end(), [](std::string::value_type & c) -> void {
    c = static_cast<std::string::value_type>(std::toupper(c));
  });
  return str;
}

/** Method to convert upper-case characters to lower cases in a string. */
std::string &
StringTools::ToLowerCase(std::string & str)
{
  // https://en.cppreference.com/w/cpp/string/byte/tolower with std algorithms
  std::for_each(str.begin(), str.end(), [](std::string::value_type & c) -> void {
    c = static_cast<std::string::value_type>(std::tolower(c));
  });
  return str;
}

/** Method to split a string into two parts with user-defined delimiters. */
void
StringTools::Split(const std::string & s, std::string & lpart, std::string & rpart, const std::string & delims)
{
  const std::string::size_type pos = s.find_first_of(delims);
  if (pos != std::string::npos)
  {
    lpart = s.substr(0, pos);
    Trim(lpart);
    rpart = s.substr(pos + 1);
    Trim(rpart);
  }
  else
  {
    lpart = s;
    Trim(lpart);
    rpart = "";
  }
}

/** Method to split a string into a sequence of strings with user-defined delimiters. */
void
StringTools::Split(const std::string & s, std::vector<std::string> & result, const std::string & delims)
{
  std::string str = s;
  while (!str.empty())
  {
    const std::string::size_type pos = str.find_first_of(delims);
    if (pos != std::string::npos)
    {
      std::string front = str.substr(0, pos);
      Trim(front);
      result.push_back(front);
      str = str.substr(pos + 1);
    }
    else
    {
      Trim(str);
      result.push_back(str);
      str = ""; // no more text to split
    }
  }
}

/**
 * Method to split a string into a sequence of sub-strings with user-defined delimiters,
 * then each sub-string is further split into a <key,value> pair with separators "=:".
 */
void
StringTools::Split(const std::string & s, std::map<std::string, std::string> & result, const std::string & delims)
{
  std::vector<std::string> items;
  Split(s, items, delims);

  for (const auto & item : items)
  {
    std::string value;
    std::string key;
    Split(item, key, value);
    result[key] = value;
  }
}

/** Method to test whether one string matches with another. */
bool
StringTools::MatchWith(const std::string & s1, const std::string & s2, bool ignoreCase)
{
  // compare ignoring case
  if (ignoreCase)
  {
    // s1 to lower case
    std::string ls1 = s1;
    ToLowerCase(ls1);
    // s2 to lower case
    std::string ls2 = s2;
    ToLowerCase(ls2);
    // compare the two
    if (ls1 == ls2)
    {
      return true;
    }
    return false;
  }

  // compare considering case
  if (s1 == s2)
  {
    return true;
  }
  return false;
}

/** Method to test whether a string starts with a user-given sub-string. */
bool
StringTools::StartWith(const std::string & s1, const std::string & s2, bool ignoreCase)
{
  // if case is not important
  if (ignoreCase)
  {
    // s1 to lower case
    std::string ls1 = s1;
    ToLowerCase(ls1);
    // s2 to lower case
    std::string ls2 = s2;
    ToLowerCase(ls2);
    // do the check
    if (ls1.find(ls2) == 0)
    {
      return true;
    }
    return false;
  }

  // if case must be considered
  if (s1.find(s2) == 0)
  {
    return true;
  }
  return false;
}

/** Method to test whether a string ends with a user-given sub-string. */
bool
StringTools::EndWith(const std::string & s1, const std::string & s2, bool ignoreCase)
{
  const std::string::size_type pos = s1.size() - s2.size();

  // if case is not important
  if (ignoreCase)
  {
    // s1 to lower case
    std::string ls1 = s1;
    ToLowerCase(ls1);
    // s2 to lower case
    std::string ls2 = s2;
    ToLowerCase(ls2);
    // do the check
    if (ls1.rfind(ls2) == pos)
    {
      return true;
    }
    return false;
  }

  // if case must be considered
  if (s1.rfind(s2) == pos)
  {
    return true;
  }
  return false;
}

/** Method to test whether a string contains a user-given sub-string. */
bool
StringTools::ContainSub(const std::string & s1, const std::string & s2, bool ignoreCase)
{
  // if case is not important
  if (ignoreCase)
  {
    // s1 to lower case
    std::string ls1 = s1;
    ToLowerCase(ls1);
    // s2 to lower case
    std::string ls2 = s2;
    ToLowerCase(ls2);
    // to the check
    if (ls1.find(ls2) != std::string::npos)
    {
      return true;
    }
    return false;
  }

  // if case must be considered
  if (s1.find(s2) != std::string::npos)
  {
    return true;
  }
  return false;
}

} // namespace itk
