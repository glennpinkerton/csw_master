
/*
 * This file has the implementation of methods in the TagReplacer class.
 * See the comments in tags.h for more info.
 */

#include <ctime>
#include <stdlib.h>
#include <string>

#include "tags.h"

using namespace logger;

/*
 * This is the only constructor.  It initializes the private std::string
 * objects for tag names and the tag values that can be retrieved from 
 * environment variables.
 */
TagReplacer::TagReplacer ()
{
    home_dir_tag = HOME_DIR_TAG;
    home_dir_tag_len = home_dir_tag.size ();

    logger_envar_tag = LOGGER_ENVAR_TAG;
    logger_envar_tag_len = logger_envar_tag.size ();

    time_tag = TIME_TAG;
    time_tag_len = time_tag.size ();

    date_tag = DATE_TAG;
    date_tag_len = date_tag.size ();

    char *envar;

    envar = getenv ("HOME");
    if (envar == NULL) {
        envar = getenv ("home");
    }
    if (envar == NULL) {
        home_dir_string = "";
    }
    else {
        home_dir_string = envar;
    }
    home_dir_string_len = home_dir_string.size ();
    
    envar = getenv ("LOGGER_FILE");
    if (envar == NULL) {
        envar = getenv ("logger_file");
    }
    if (envar == NULL) {
        logger_envar_string = "logger.out";
    }
    else {
        logger_envar_string = envar;
    }
    logger_envar_string_len = logger_envar_string.size ();
    
}


/*
 * This is the metod og TagReplacer that actually does the work.  Pass this
 * method a string (could be a format string or a destination string) and 
 * it will return a new string object that has the defined tags replaced 
 * with their current values.  For example, pass the following string.
 *
 *      %home_dir%/logfiles/%date%.log
 *
 * If the HOME environment variable is set to /home/glenn and today is
 * November 8, 2008, the return string would be:
 *
 *      /home/glenn/logfiles/11-08-2008.log
 *
 * The input string is not changed by this method.  A copy is made and the
 * copy is modified and returned.
 *
 */

std::string TagReplacer::ReplaceTags (std::string str)
{

    std::string   swork = str;

    ReplaceHomeDirTags (swork);
    ReplaceLoggerEnvarTags (swork);
    ReplaceTimeTags (swork);
    ReplaceDateTags (swork);

    return swork;
}


/*
 * Private method to replace the %home_dir% tag with its value.
 */
void TagReplacer::ReplaceHomeDirTags (std::string &swork)
{

    std::string::size_type    index;

    index = swork.find (home_dir_tag);

    while (index != std::string::npos) {
        swork.replace (index, home_dir_tag_len, home_dir_string);
        index = swork.find (home_dir_tag, index + home_dir_string_len); 
    }

}


/*
 * Private method to replace the %logger_envar_file% tag with its value.
 */
void TagReplacer::ReplaceLoggerEnvarTags (std::string &swork)
{

    std::string::size_type    index;

    index = swork.find (logger_envar_tag);

    while (index != std::string::npos) {
        swork.replace (index, logger_envar_tag_len, logger_envar_string);
        index = swork.find (home_dir_tag, index + logger_envar_string_len); 
    }

}


/*
 * Private method to replace the %time% tag with its value.
 */
void TagReplacer::ReplaceTimeTags (std::string &swork)
{

    std::string::size_type    index;
    bool         first = true;
    std::string  time_string;
    int          time_string_len;

    index = swork.find (time_tag);

    while (index != std::string::npos) {
        if (first) {
            time_string = getCurrentTimeString ();
            time_string_len = time_string.size ();
            first = false;
        }
        swork.replace (index, time_tag_len, time_string);
        index = swork.find (time_tag, index + time_string_len); 
    }

}


/*
 * Private method to replace the %date% tag with its value.
 */
void TagReplacer::ReplaceDateTags (std::string &swork)
{

    std::string::size_type    index;
    bool         first = true;
    std::string  date_string;
    int          date_string_len;

    index = swork.find (date_tag);

    while (index != std::string::npos) {
        if (first) {
            date_string = getCurrentDateString ();
            date_string_len = date_string.size ();
            first = false;
        }
        swork.replace (index, date_tag_len, date_string);
        index = swork.find (date_tag, index + date_string_len); 
    }

}


/*
 * Use system calls to get the current time and format it into a string
 * with hh:mm:ss format.  If any error occurs the return value is 
 * "Invalid_Time".
 */
std::string TagReplacer::getCurrentTimeString ()
{
    std::string  stime;

    time_t       rawtime;
    struct tm    *timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    if (timeinfo == NULL) {
        stime = "Invalid_Time";
        return stime;
    }

    char work[100];

    sprintf (work, "%02d:%02d:%02d",
             timeinfo->tm_hour,
             timeinfo->tm_min,
             timeinfo->tm_sec);
    stime = work;

    return stime; 

}


/*
 * Use system calls to get the current date and format it into a string
 * with yyyy-mm-dd format.  If any error occurs the return value is 
 * "Invalid_Date".
 */
std::string TagReplacer::getCurrentDateString ()
{
    std::string sdate;

    time_t       rawtime;
    struct tm    *timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    if (timeinfo == NULL) {
        sdate = "Invalid_Date";
        return sdate;
    }

    char work[100];

    sprintf (work, "%4d:%02d:%02d",
             timeinfo->tm_year,
             timeinfo->tm_mon,
             timeinfo->tm_mday);
    sdate = work;

    return sdate; 

}
