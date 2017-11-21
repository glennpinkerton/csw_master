/*
 * This header file defines the TagReplacer class.  In a real world
 * logger implementation, the TagReplacer class would probably be the
 * most changeable class.  As new "dynamic tags" are defined, the code
 * to replace the tags with current values (e.g the %time% tag is replaced
 * with the current time in hours:minutes:seconds format) is confined
 * to this class.  Since this is intended to be farly changeable, I have
 * used separate source files for tags.h and tags.cc rather than putting
 * this class into the logger.h and logger.cc files.
 *
 * For the "homework" assignment, the TagReplacer class knows about four
 * tags:
 *
 *  %home_dir%   Replaced with the user's home directory, if defined by
 *               the environment variable HOME or if HOME is not defined,
 *               the home environment variable is checked.  If neither is
 *               defined, the empty string is used.
 *
 *  %logger_envar_file%   Replaced with the value of the LOGGER_FILE
 *               environment variable or the logger_file environment variable
 *               if the upper case version is not found.  Again, an empty
 *               string is used if neither is found.
 *
 *  %time%       Replaced with the current time in hours:minutes:seconds format.
 *
 *  %date%       Replaced with the current date in yyyy-dd-mm format.
 */

#ifndef TAGS_H
#define TAGS_H

#include <string>

#define HOME_DIR_TAG        "%home_dir%"
#define LOGGER_ENVAR_TAG    "%logger_envar_file%"
#define TIME_TAG            "%time%"
#define DATE_TAG            "%date%"

namespace logger
{

    class TagReplacer {
        
      public:

        TagReplacer ();

        std::string ReplaceTags (std::string str);

      private:

        void ReplaceHomeDirTags (std::string &swork);
        void ReplaceLoggerEnvarTags (std::string &swork);
        void ReplaceTimeTags (std::string &swork);
        void ReplaceDateTags (std::string &swork);


        std::string  home_dir_tag;
        int          home_dir_tag_len;

        std::string  logger_envar_tag;
        int          logger_envar_tag_len;

        std::string  time_tag;
        int          time_tag_len;
        std::string  getCurrentTimeString ();

        std::string  date_tag;
        int          date_tag_len;
        std::string  getCurrentDateString ();

        std::string  home_dir_string;
        int          home_dir_string_len;

        std::string  logger_envar_string;
        int          logger_envar_string_len;

    };  // end of TagReplacer class

}  // end of namespace logger

#endif
    
